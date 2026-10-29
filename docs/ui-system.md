# UI system (layout / 2D)

NSMBU draws menus and HUD through Nintendo `nw::lyt` layouts wrapped in game-specific C++.
The port can hook or replace this stack once the guest symbols below are understood and mirrored in runtime code.

## Layers

```text
CommonWindowMgr (singleton 0x101D15E8)
  └── CommonWindow + PaCursor          modal yes/no and message windows

AttentionScreen, ChangePlayerNum, …    full-screen or overlay AnimLayout screens

AnimLayout<N> : AnimLayoutBase : Multi2D
  └── ArcResAccMulti, UtilCursorButtonMgr, FlexibleTextBox lists (build time)

Multi2D (0xE88 bytes)
  └── LayoutObj, LayoutAllocator, DrawInfo, anim maps, TagProcessor
```

`GamesceneBase` ties course gameplay actors and coin HUD text into the same layout world (`registerActor`, `setCoinNumText`).

## Multi2D responsibilities

Guest address block `0x026711C8`–`0x02672E98`.

- **`createLayoutObj` / `initializeLayoutObj_`**: load `.bflyt` / `.bflan` through `LayoutObj` and register anim resources.
- **Pane registration**: `NPaneRegister`, `PPaneRegister`, `TPaneRegister`, `BPaneRegister`, `WPaneRegister`, and `Pa*` variants cache pointers into `nw::lyt` panes by name (see `find*ByName_` helpers).
- **Animation**: `animResRegister`, `animStartSetup` family, `animPlay`, `setPlayerColor`, frame get/set.
- **Draw**: `draw(proj_mtx)` uses embedded `nw::lyt::DrawInfo` and the root pane from `getRootPane`.
- **Text**: `setTextBoxMessageFromTextID` resolves message IDs through the project text database; `TagProcessor` handles layout tag codes in text boxes.

Decomp: `vendor/decomp/include/layout_sys/AnimLayout.h`, `vendor/decomp/src/layout_sys/Multi2D.cpp`, `AnimLayoutBase.cpp`, `LayoutAllocator.cpp`, `TagProcessor.cpp`.

## Screen pattern (FState + AnimLayout)

Screens such as `AttentionScreen` and `ChangePlayerNum` follow the same shape:

1. Embedded `AnimLayout<N>` with a fixed anim buffer size.
2. `FStateMgr` drives `In` / gameplay / `Out` (or `CloseRequest`) states.
3. `create()` calls `readResource` + `build`, registers anim and pane names from rodata tables.
4. `execute()` runs the state machine and calls `Multi2D::animPlay` on layer `0xE` (same id as other UI).
5. `draw` / `drawDRC` gate on visibility and target display, then delegate to `Multi2D::draw`.

State IDs are guest globals (for example `AttentionScreen::StateID_In` at `0x1020B588`).

## Pause menu and CommonWindowMgr (modal UI)

`CommonWindowMgr` (singleton `0x101D15E8`, disposer `0x024BC444` / `0x024BC874`) owns one embedded `CommonWindow` (`0x1ABC` bytes).
The pause dialog uses `CommonWindow::cType_WinText_Pause` via `CommonWindowMgr::open`.
Message copy can differ through `CommonWindow::Type msg_text_type` while the window kind stays pause.

Guest flow:

1. `CommonWindowMgr::create` calls `CommonWindow::create`, which runs `Multi2D::createLayoutObj`, `AnimLayoutBase::readResource`, `build`, pane registration, and `PaCursor` setup on the window layout at `+0x24`.
2. `CommonWindowMgr::execute` runs `CommonWindow::execute` (state machine + `animPlay` on layer `0xE` when visible).
3. Pad handling: `CommonWindowMgr::processPadTrig_OneButton` / `processPadTrig_TwoButtons` delegate to `CommonWindow::processPadTrigOneButton` / `processPadTrigTwoButtons`, which install a handler from `lookupPadTrigFuncOneButton_` / `lookupPadTrigFuncTwoButtons_` (indexed by `CommonWindow::Type` at `+0x1AA0`) into `+0x1128`, then drive `PaCursor` and `FStateMgr` transitions.
4. Draw: `CommonWindowMgr::draw` / `drawDRC` / `drawDRC_System` call `CommonWindow::draw` / `drawDRC`, which gate on `mIsVisible`, `mIsFadeOut`, and DRC flags before `Multi2D::draw`.

Decomp headers: `vendor/decomp/include/game/CommonWindowMgr.h`, `vendor/decomp/include/layout/CommonWindow.h`.
Implementation: `vendor/decomp/src/layout/CommonWindowMgr.cpp`, `CommonWindow.cpp`, `PaCursor.cpp`.

## Mod-facing integration (future)

Nothing in the recomp binary exposes mod UI yet.
Plausible hooks once this decomp is stable:

1. **Native extension**: register a new `AnimLayout` subclass or standalone `Multi2D` derivative from mod code, call the same `createLayoutObj` / pane register / `animPlay` / `draw` sequence on the UI layer.
2. **Guest hooks**: intercept `CommonWindowMgr::open` or a screen's `create` to inject extra panes or swap `.bflyt` resource names (requires mod asset pipeline).
3. **Parallel overlay**: run a second layout with the same `DrawInfo` conventions without going through `CommonWindowMgr`, matching how course HUD uses `Multi2D` inside `PlayerCourseHud`.

Mod code must respect `LayoutAllocator` heap (`setHeap` on the embedded allocator in each `Multi2D`) and `UtilCursorButtonMgr` when building layouts that use cursor buttons.

## Related symbols still header-only or data-only

- Header shells for `LayoutObj`, `PartsLayoutObj`, `TextBox`, `ArcResAccMulti`, `FlexibleTextBox`, and `AnimText` live under `vendor/decomp/include/layout_sys/`; `.cpp` units are still partial (`Multi2D.cpp`, `AnimLayoutBase.cpp`).
- Static state ID objects (`ChangePlayerNum::StateID_CloseRequest`, etc.) live in `.data`, not `.text`.

## Tools

- Disassemble: `python3 tools/rpx/ppcdis.py 0x026711C8` (uses `game/active`).
- Names: `tools/recomp/headers_names.txt` for `Multi2D::`, `CommonWindowMgr::`, etc.
