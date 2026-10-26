# World-map shadows and frame interpolation

`LightMapMgr` for course select (`initializeForCourseSelect`, `setCobModelLightMapForCourseSelect`) feeds soft contact shading on map props (`lmDifCob` / `lmSpcCob`).
Depth shadows under trees and terrain also clear and redraw each `procDraw_`.

`GameFrame::procCalc_` (`0x029C52CC`) runs `clearRenderMgr`, the task tree (including `LightMapMgr::calc` at `0x024E5008`), then `LayerMgr::calcDL` (`0x024E0F84`), then `calc_`.
Hold / in-between interpolation passes call only `GameFrame::calc_` (`0x029C5104`) and still run `procDraw_`.
Without the skipped calc prep, light and shadow targets clear on the hold draw and come back empty until the next logic pass, so map shadows alternate on/off at 120/144 fps.

The runtime skips `procDraw_` on hold when `AreaTask` is not live (world map and menus).
Course play still redraws holds for camera and model blending.
