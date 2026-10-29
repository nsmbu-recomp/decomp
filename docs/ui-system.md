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

## CommonWindowMgr (modal UI)

Singleton disposer at `0x024BC444` / `0x024BC874`.
Holds one `CommonWindow` instance and routes pad input through `processPadTrig_OneButton` / `TwoButtons`.
`open` sets `CommonWindow::Type` (60 enumerated dialog kinds in `CommonWindow.h`) and optional message text type.
Draw paths split TV (`draw`), GamePad (`drawDRC`), and system DRC (`drawDRC_System`).

Decomp: `vendor/decomp/include/game/CommonWindowMgr.h`, `vendor/decomp/src/layout/CommonWindowMgr.cpp`, `CommonWindow.cpp`, `PaCursor.cpp`.

## Mod-facing integration (future)

Nothing in the recomp binary exposes mod UI yet.
Plausible hooks once this decomp is stable:

1. **Native extension**: register a new `AnimLayout` subclass or standalone `Multi2D` derivative from mod code, call the same `createLayoutObj` / pane register / `animPlay` / `draw` sequence on the UI layer.
2. **Guest hooks**: intercept `CommonWindowMgr::open` or a screen's `create` to inject extra panes or swap `.bflyt` resource names (requires mod asset pipeline).
3. **Parallel overlay**: run a second layout with the same `DrawInfo` conventions without going through `CommonWindowMgr`, matching how course HUD uses `Multi2D` inside `PlayerCourseHud`.

Mod code must respect `LayoutAllocator` heap (`setHeap` on the embedded allocator in each `Multi2D`) and `UtilCursorButtonMgr` when building layouts that use cursor buttons.

## Related symbols still header-only or data-only

- `LayoutObj`, `TextBox`, `ArcResAccMulti`, `FlexibleTextBox`, `AnimText` — referenced from `AnimLayoutBase::build` but not fully decompiled as units yet.
- Static state ID objects (`ChangePlayerNum::StateID_CloseRequest`, etc.) live in `.data`, not `.text`.

## Tools

- Disassemble: `python3 tools/rpx/ppcdis.py 0x026711C8` (uses `game/active`).
- Names: `tools/recomp/headers_names.txt` for `Multi2D::`, `CommonWindowMgr::`, etc.
