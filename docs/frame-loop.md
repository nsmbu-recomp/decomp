# Frame loop and timing

NSMBU runs a fixed 60 Hz logic step tied to GX2 vsync.
It does not scale gameplay by wall-clock delta time.

`ApplicationFramework` (headers) and `GameFrame` (this tree's WIP rename) are the same RPX class.
Size is `0x398`.
Ctor is `0x029C4DC0`-family; it sets `mCalcPassCount = 1` and `mSkippedCalcCount = 0`.

## sead loop

`sead::GameFrameworkCafe::runImpl_` waits for the root task, stamps `mLastFrameBegin`, then `mainLoop_`.
`mainLoop_` forever calls `procFrame_`.

Each `procFrame_`:

1. Lock draw context.
2. `TaskMgr::afterCalc` for the previous frame.
3. `procDraw_` (TV then DRC method trees, or the game override).
4. `procCalc_` (task beforeCalc + DualScreen method-tree calc, or the game override).
5. `procReset_`.
6. `swapBuffer_` → `GX2SwapScanBuffers` when display state is Show, then `GX2DrawDone`.
7. Unlock.
8. Store `mFrameSpan = mLastFrameBegin.diffToNow()` and stamp `mLastFrameBegin` again.
9. `waitForNextFrame_`.

`waitForNextFrame_` runs only when `CreateArg::wait_vblank` is non-zero (default `1`).
It loops `GX2WaitForVsync` + `GX2GetSwapStatus` until `flipCount >= swapCount`.
`initializeGraphicsSystem` also calls `GX2SetSwapInterval(wait_vblank)`.
So one guest frame is one TV vsync by default (~60 Hz).

`mFlag` bit 0 is a half-frame mode in sead:
with bit 0 set, alternate `procFrame_` calls skip draw and skip swap (bit 1 toggles).
NSMBU leaves that flag clear in normal play.

`calcFps()` is `1s / mFrameSpan`.
Nothing in the game path feeds `mFrameSpan` into physics or animation.

## ApplicationFramework overrides

Addresses (USA v1.3.0):

- `procDraw_` `0x029C4FE4`
- `calc_` `0x029C5104`
- `procCalc_` `0x029C52CC`

`procCalc_` (`mCalcPassCount` at `+0x38C`, `mSkippedCalcCount` at `+0x390`):

- If `mCalcPassCount > 0`: clear render mgr, run `GameFrameworkCafe::procCalc_` once, then call the app calc method-tree root (`getRootMethodTreeNode(1)`) an extra `mCalcPassCount - 1` times, then `LayerMgr::calcDL` and `calc_`.
- If `mCalcPassCount <= 0`: count skips in `mSkippedCalcCount` until `skipped + 1 >= -mCalcPassCount`, then run one full calc and reset the skip counter.
  Default is `1` (one calc per frame).
  No in-tree writer of a non-default value was found yet; the fields are the stock multi-step / skip hooks.

`calc_` waits on `GX2DrawDone`, copies the TV/DRC framebuffers to display buffers, flushes, and runs `LayerMgr::calcParallelExec`.
Hold / in-between port passes call only this path so the task tree does not advance.

`procDraw_` draws TV and DRC method trees (or, in the GameFrame rewrite, the root task's draw nodes).
The port's 30 fps-style "no draw" comment on the hook refers to skip/hold paths that return before the original body, not to a native 30 Hz title rate.
NSMBU's native rate is 60 steps/s.

`swapBuffer_` adds first-swap TV/DRC enable and ProcUI / HBM handling on top of `GameFrameworkCafe::swapBuffer_`.

## Per-step animation, not delta time

`FrameCtrl::play` (`0x024DEF04`) does `mFrame += mRate` once per call.
Rates are authored in frames per logic step (often `1.0f`).
Player, enemy, and layout code assume one step equals one sixtieth of a second.

## Port hooks (reference)

The recomp port hooks `0x029C52CC` / `0x029C4FE4` / `0x029C5104` to insert draw-only holds between logic steps.
That is host presentation work; the guest engine still has no delta-time sim.

See also `map-shadow-interp.md` for why holds must skip `procDraw_` on the world map.
