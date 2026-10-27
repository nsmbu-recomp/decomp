# World-map shadows and frame interpolation

`LightMapMgr` for course select (`initializeForCourseSelect` `0x024E51C8`, `setCobModelLightMapForCourseSelect` `0x024E5900`) feeds soft contact shading on map props.
It builds per-direction light-map names with `lmDifCSW%u%c` / `lmSpcCSW%u%c` (`0x100BB5E0` / `0x100BB600`) and applies the `lmDifCob` / `0x100BB5F0` and `lmSpcCob` / `0x100BB60C` maps through `setModelLightMapWithName_` (`0x024E5734`).

Depth shadows are a separate render-obj layer, not a per-frame clear.
`ShadowRenderObjLayer::draw_` is `0x024FF988`.
It sets polygon offset and stencil state and calls `RenderObjRenderMgr::drawShadowOpa` (`0x024FC3F0`), and it issues no `GX2Clear*` call.
The clears reachable from the draw path are the main target's: `0x024F9E24` calls `0x02A1E644` with target index `0` and colour+depth flags.
`LayerMgr::clearRenderMgr` (`0x024E0E50`) is a data reset (`RenderObjRenderMgr::clear` `0x024FBED0`), not a GPU clear.

Course select creates a `"DepthShadow_DRC"` layer (`0x10047098`) with layer id `0x17` in `CourseSelectLayerMgr::createLayers` (`0x021EE9E4`).
The factory `0x021EEFF8` allocates `0x1154` bytes and runs the `ShadowRenderObjLayer` ctor `0x024FF0E0`.
The TV `"DepthShadow"` string (`0x100470A8`) has no reference in `.text`, so its creation is unknown.

`ApplicationFramework::procCalc_` / `GameFrame::procCalc_` (`0x029C52CC`) runs `clearRenderMgr`, the task tree (including `LightMapMgr::calc` at `0x024E5008`), then `LayerMgr::calcDL` (`0x024E0F84`), then `calc_`.
Hold / in-between interpolation passes call only `calc_` (`0x029C5104`) and still run `procDraw_`.
Without the skipped calc prep the light-map state is one pass stale on the hold draw, and map shadows alternate on and off at 120/144 fps.
No `GX2Clear*` call exists in the depth-shadow layer's draw, so for those shadows the "clear and come back empty" mechanism is unconfirmed; the light-map path is the one the hold skip protects.
The runtime skips `procDraw_` on hold when `AreaTask` is not live (world map and menus).
Course play still redraws holds for camera and model blending.
See `frame-loop.md` for the full sead / ApplicationFramework timing model.
