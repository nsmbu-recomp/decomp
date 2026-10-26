# Soft restart and HeapMgr TLS

The product path now uses a hard app relaunch for language/mod restart.

This note keeps the soft-restart investigation for guest TLS / AX / sound-info races.

`0x02A0D584` is `sead::HeapMgr::setCurrentHeap` (swap the thread’s current heap).
`0x02A0D554` is `getCurrentHeap`.
`CourseTask.h` once tagged `0x02A0D584` as `GamesceneFactory_release`; that name is a different helper whose real PC is still unknown.

Both HeapMgr helpers load a Cafe thread-specific slot id from `*(0x101E80A0) + 0x20`, then call `OSGetThreadSpecific`.
The sead `Thread` object keeps the current heap at `+0x6C`.

Many game `sInstance` pointers sit in MEM2 between RPX sections (about `0x101CB7C8`–`0x101E93BF`), including the HeapMgr TLS root at `0x101E80A0`.
Those addresses are not in any ELF PROGBITS/BSS section, so a section-only reload leaves the previous process image there.
Cold boot is fine because anonymous guest memory starts zeroed.

A soft restart must:

1. Stop every previous guest host thread before clearing `g_reboot_kill` (busy threads exit via `ppc_preempt`; Linux must not detach still-running threads).
2. Clear that MEM2 span (and MEM1 / FG) before the guest runs again.
3. Drop host-side Cafe sync mirrors (mutex / event / queue / sleep queue / rendezvous) and cancel armed alarms so callbacks do not run against the wiped image.
4. Any host wait that can outlive a preempt (frame-heap spin, GX2 fence `render_sync`) must observe the reboot flag and exit via `GuestExit` rather than `pthread_kill`/`pthread_exit`, which skips C++ unlock of `g_exec_mutex` and deadlocks the next boot.
5. Realign GX2 `g_flip_count` with `g_swap_count` (same `g_count_offset`) after a reboot.
   A soft restart that drains flips with a timeout can leave flip behind swap; the guest then waits forever for a flip that never catches up.
6. Stop and join the AX `frame_thread` before wiping MEM2.
   That host service thread is exempt from reboot `GuestExit` and can still `guest_call` into guest audio after game threads are killed.
   Clear AX app/aux/final-mix callbacks and set `g_running` false during prepare (do not join there: the frame thread may be waiting on a guest core the preparer still holds).
   Join after guest threads have exited, before MEM2 wipe / the next `AXInit`; never run two frame threads.
7. Tear down AX `make_service_cpu` HostThreads and do not reset the host bump allocator.
   `make_service_cpu` allocates the guest OSThread and stack with `host_alloc` and registers `service=true` in `g_threads`.
   Soft restart joins the AX `std::thread`, then that thread must `retire_service_cpu` (unregister and delete its HostThread).
   The host bump allocator must keep its high-water mark across soft restart so the long-lived alarm service HostThread is not overwritten when the next boot calls `host_alloc`.
8. SoundStartable sound-info table (`AudAudioPlayer+0x20` / SoundStartable `+0x1C`):
   - Layout: `sead::AudioPlayer` vtable at `+0`, `nw::snd::SoundArchivePlayer` / SoundStartable at `+4`.
   - Count at SoundStartable `+0x18` (`player+0x1C`); table pointer at `+0x1C` (`player+0x20`); entry stride 120 (`0x78`).
   - GetSoundInfo is `0x02ADB9C0`: `table + id*120`.
   - Ctor `0x02ADAA34` zeroes count/table; shutdown `0x02ADB714` clears them.
   - Setup carves the table from the player buffer in `0x02ADAE50` (`stwu` at `02ADAEC4` / count at `02ADAECC`), reached from SoundArchivePlayer Setup `0x02ADB338` ← `0x02ADB634` ← AudioPlayerCafe buffer setup `0x029E6C38` / `0x029E8324`.
   - `AudAudioPlayer::initialize` (`0x029B21A0`) is a no-op; `SndAudioMgr::loadData` / `startSound` do not install the table.
   - Runtime skip hooks on `0x029B3C5C` / `0x029B7448` only paper over a null table; they cannot cause Setup to fail, and skipping `startSound` cannot leave the table unset.
   - Soft restart with real audio can leave the table null through early boot while startSound / isSndPlaying / AX app-frame callbacks already run; cold boot and soft restart with `NSMBU_NO_AUDIO=1` (AX waits for Audio* threads to park before starting the frame thread) succeed.
   - Host fix: join AX frame thread, `retire_service_cpu`, keep `g_host_top` across soft restart.
   - Do not start the AX frame thread from `AXInit`; start it when SoundArchivePlayer Setup (`0x02ADB338`) returns. Starting AX earlier races callbacks into Setup (table pointer is stored before Setup finishes) and after soft restart can prevent Setup from running at all.
   - Gate sound-info walks / `startSoundImpl` on that Setup-done flag; defer the sound-info write (`0x029B7448`) until replayed after Setup; do not skip `SndAudioMgr::calc`.

Otherwise Prepare Thread can call `setCurrentHeap` while a leftover thread shares its OSThread (TLS wiped) and fault at guest `0x6C`, the next GX2 path stalls forever after a forced stop or a stale flip wait, or sound init after a language soft restart faults at `0x8EC` / later in `startSoundImpl` with a null player.
