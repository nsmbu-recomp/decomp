# Soft restart and HeapMgr TLS

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

Otherwise Prepare Thread can call `setCurrentHeap` while a leftover thread shares its OSThread (TLS wiped) and fault at guest `0x6C`, or the next GX2 path stalls forever after a forced stop or a stale flip wait.
