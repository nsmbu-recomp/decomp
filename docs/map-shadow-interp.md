# World-map shadows and frame interpolation

`LightMapMgr` for course select (`initializeForCourseSelect` `0x024E51C8`, `setCobModelLightMapForCourseSelect` `0x024E5900`) feeds soft contact shading on map props.
It builds per-direction light-map names with `lmDifCSW%u%c` / `lmSpcCSW%u%c` (`0x100BB5E0` / `0x100BB600`) and applies the `lmDifCob` / `0x100BB5F0` and `lmSpcCob` / `0x100BB60C` maps through `setModelLightMapWithName_` (`0x024E5734`).

Depth shadows are a separate render-obj layer, not a per-frame clear.
`ShadowRenderObjLayer::draw_` is `0x024FF988`.
It sets polygon offset and stencil state and calls `RenderObjRenderMgr::drawShadowOpa` (`0x024FC3F0`), and it issues no `GX2Clear*` call.
`LayerMgr::clearRenderMgr` (`0x024E0E50`) is a data reset (`RenderObjRenderMgr::clear` `0x024FBED0`), not a GPU clear.

The clear reachable from the draw path belongs to the reflection layer, not the shadow layer.
`0x024F9E24` is a branch label, not a function entry: it is the `cRenderStep_Opa` arm of `ReflectionRenderObjLayer::render_` (`0x024F9C04`, ending `0x024FA280`).
That arm clears the layer's own render buffer at `0x024FA07C` with `agl::RenderBuffer::clear` (`0x02A1E644`): target index `0`, flags `3` (colour + depth), colour `(0, 0, 1, 0)`, depth `1.0f`, stencil `0`.
`ReflectionRenderObjLayer::render_` is stored as a delegate at `0x024F9860` by the ctor `0x024F9800`, so it is reached only through the layer render step, never by a static call.
The decompiled body is `src/graphics/ReflectionRenderObjLayer.cpp`.

Course select creates a `"DepthShadow_DRC"` layer (`0x10047098`) with layer id `0x17` in `CourseSelectLayerMgr::createLayers` (`0x021EE9E4`).
The factory `0x021EEFF8` allocates `0x1154` bytes and runs the `ShadowRenderObjLayer` ctor `0x024FF0E0`.
The TV `"DepthShadow"` layer is created as well: layer id `5` at `0x021EEAD8`, in the same `createLayers`, with the `"CourseSelect"` render mgr (`0x100470D8`) from `0x021EEA38`.
A second TV `"DepthShadow"` string at `0x101768C0` is created with layer id `5` at `0x02980F44`, in the game layer setup that starts at `0x02980E60`.
An earlier sweep reported "no reference" because the compiler emitted the address as `lis` + `addic` (`3c001004` / `300070a8`), and the sweep only matched `addi` and `ori`.
`src/course_select/CourseSelectLayerMgr.cpp` is the decompiled `createLayers`.

The `"Shadow Map"` render-target names are agl setup, not game passes.
`"Shadow Map"` (`0x101A94AC`) is used at `0x02A4DF84`, `"shadow_map_depth"` (`0x101A94B8`) at `0x02A4E018`, and `"vsm"` (`0x101A94D0`) at `0x02A4E3B0`.
All three are inside the agl render-target setup class whose ctor starts at `0x02A4DC00` (size `0xB38`, vtable `0x101A94E0`, dtor `0x02A4DD70`).
That class passes `{0x101A9478, "<name>"}` pairs to `0x02A5069C`, which is a `blr` no-op, so the names are recorded but no target is created there; creation is `0x02A528D0` through `*(0x101F8A50)`.
`"static_depth_shadow"` (`0x101A658C`) is unrelated: it is an entry in `agl::detail::ShaderHolder`'s `sShaders[]` at `.rodata 0x101A6000`, next to `light_pre_pass_quad`, `multi_filter_reduce`, and the `cubemap_*` entries.

`ApplicationFramework::procCalc_` / `GameFrame::procCalc_` (`0x029C52CC`) runs `clearRenderMgr`, the task tree (including `LightMapMgr::calc` at `0x024E5008`), then `LayerMgr::calcDL` (`0x024E0F84`), then `calc_`.
Hold / in-between interpolation passes call only `calc_` (`0x029C5104`) and still run `procDraw_`.
Without the skipped calc prep the light-map state is one pass stale on the hold draw, and map shadows alternate on and off at 120/144 fps.
No `GX2Clear*` call exists in the depth-shadow layer's draw, and the only clear on the draw path is the reflection layer's own target, so for the map shadows the "clear and come back empty" mechanism is still unconfirmed; the light-map path is the one the hold skip protects.
The runtime decides a hold before `procDraw_`.
Holds stay on for a settled map and for course play.
On a map hold, `procDraw_` is skipped so the light-map pass is not one step stale.
Course play still redraws holds for camera and model blending.
While the GPU is still finishing an earlier picture, that extra course draw is skipped and the last picture is shown again.
The present cap stays at the selected rate.
Map and course logic stay near 60 steps per second.
See `frame-loop.md` for the full sead / ApplicationFramework timing model.
