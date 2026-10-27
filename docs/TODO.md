# Decomp verification TODO

Open questions from the decomp notes, one claim per item.
Checked items are removed: the answer lives in the note it came from.
Check a claim against the USA v1.3.0 `red-pro2.rpx` unless the item says Europe.

Tools:

- `python3 tools/rpx/ppcdis.py ADDR [COUNT] [RPX]` disassembles `.text`.
  It annotates import calls (`gx2!GX2DrawDone`) and calls to a name in `tools/recomp/headers_names.txt`.
- `tools/recomp/headers_names.txt` gives the class and symbol for a game address.
- `game/active` selects the dump that `ppcdis.py` reads by default.

## frame-loop.md

- [ ] Name the remaining sead helpers in `procFrame_`.
  `0x02A07A54` / `0x02A07AF8` are the draw-context lock / unlock, `0x02A0183C` (`0x02A01730`) is the task-manager `afterCalc`, `0x02A02F74` is the GPU-time updater, and `0x029FF1D8` is the `+0x6C` slot guessed to be `procReset_`.
  The bodies are known; only the sead names are missing, and no tree here has the sead symbols.
- [ ] Confirm `0x029C5734` (`procDraw_` slot `+0xAC`) is the per-screen method-tree exec.
  It is called twice by the app `procDraw_`, once per screen, with `GX2SetContextState` on `*(0x101F8000)+0x134` between.
- [ ] Make the negative sweeps exhaustive.
  No reader of `mFrameSpan` (`+0x70`) was found in the sead framework or the app overrides, and no writer of `mFlag` bit 0 was found.
  A whole-`.text` sweep is not decisive for either, because `+0x70` and `+0x6C` are common field offsets.

## map-shadow-interp.md

- [ ] Find where the TV `"DepthShadow"` layer (`0x100470A8`) is created.
  Course select only creates the `"DepthShadow_DRC"` variant (`0x021EEC2C`), and the TV string has no `lis`/`addi` reference in `.text`.
- [ ] Show whether the course-select draw reaches `Renderer` `0x024F9E24`, the only clear pinned on the draw path.
  It clears target index `0` with colour+depth flags, and it is reached virtually, so static callers do not settle it.
- [ ] Identify the `"Shadow Map"` / `"shadow_map_depth"` (`0x101A94AC` / `0x101A94B8`) and `"vsm"` (`0x101A94D0`) users.
  Those strings exist next to `"static_depth_shadow"` (`0x101A658C`), but no pass that clears those targets has been located.

## soft-restart-tls.md

- [ ] Confirm `startSound`'s indirect call at `0x029B5460` is `startSoundImpl`.
  It is a `bctrl` through `*(this+0x3D0)+0x24`, so the interface table would need to be resolved.
- [ ] Resolve the bootstrap order between audio init and the first `startSound`.
  The init root `0x029E8324` has no static caller; it is reached through tables `.rodata 0x1017C1A4`, `0x1017C284`, `0x1018D9E0`, `0x1018DA04`, and no `.text` or data reloc points at them.
  The earliest `startSound` caller found is `SndAudioMgr::startSystemSe` (`0x029B54C0`) from gameplay, so the "sound can start before Setup" claim is unproven either way.
