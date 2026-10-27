# Screen projections

`sead::OrthoProjection::setTBLR` is `0x02A06818`.
It stores top, bottom, left, and right at object offsets 156, 160, 164, and 168.

`sead::PerspectiveProjection` is constructed at `0x02A061AC`.
The arguments are near, far, field of view in radians, and aspect.
The per-frame setter at `0x02A062EC` uses that same argument order.

Field of view is stored at object offset 156, and aspect at offset 172.
`DistantViewMgr` keeps its projection at object offset 168.

`nw::lyt::Pane::CalculateMtx` is `0x02ABC054`.
`nw::lyt::Pane::Draw` is `0x02ABCAE8`.
`DrawSelf` is the virtual call at `0x02ABCB34`, and the child walk starts at `0x02ABCB38`.
`nw::lyt::DrawInfo::LoadProjectionMtx` is `0x02AB9BF0`.
`DrawInfo::LoadMtxModelView` is `0x02AB9C2C`.
The base pane vtable is `0x101B93D4`.
`CalculateMtx` is slot `0x84` and `Draw` is slot `0x8C`.
`DrawSelf` is slot `0x94`.
The base pane's `DrawSelf` is `0x02ABCB84` and does nothing.
Window panes such as `W_Base_00` use `DrawSelf` `0x02AC30A4`.
Text panes such as `T_Btn_00` use `DrawSelf` `0x02ABE3B8`.

The global matrix at pane `+0x48` is a 3×4.
The first eight floats are the X row and the Y row.
The fourth float of each row is that axis's translation.
`0x02ABE3B8` multiplies the projection stored at the start of `DrawInfo`.
Edge button pictures already use the widescreen projection.
That text multiply was still using the 16:9 projection, so the label sat past the button and was clipped at the screen edge.

`GameScene` is drawn to the TV and to the gamepad.
A gamepad draw must not clear the TV flag, or the next `CalculateMtx` rebuilds `N_ZankiAll_00` and `N_TimerScore_00` at their 16:9 translations and the counters jump.
Panes parked outside the 16:9 frame stay undrawn on both targets, or they show up in the side strips.
`N_Coin_00` is also the coin row in `PointGet`, so an edge pane has to sit on the screen edge and not only share that name.

`BgScrollMgr::sInstance` is `0x101E7C54`.
The screen update is `0x029A6170`.
It copies the screen box into the scalar window: left at `+0x70`, center X at `+0x74`, and right at `+0x78`.
The screen box is `+0x28` (min X), `+0x2C` (min Y), `+0x30` (max X), and `+0x34` (max Y).
`ActorCullUtil::screenCullCheck` (`0x02008918`) tests that box.
The padded check at `0x02008838` tests the zoom box at `+0x48`.
`Actor::drawCullCheck_` uses the screen box, so a 16:9 box hides actors that the wide camera can already see.
The previous-frame box is `+0x38`.
The fixed-size box is `+0x58`.
Background edges are `+0x88` and `+0x90`, and the previous background box is `+0x98`.
The same update can run again after it rewrites the screen box, so a box whose width/height is already past 16:9 stays as it is.
The first of those two updates leaves the scalar window at 16:9.
The rewrite then builds the game's camera, and the second update widens that camera.
The title parade and other screen-follow placement read the scalar window, so a wider left and right slide that parade toward the new right edge.
`CourseInfo::isTitle` is `0x024D3940`.
It is true when the course kind at `CourseInfo + 0x11` is `0x26`.
`CourseInfo::sInstance` is `0x101D18EC`.
A course widens the screen box, the zoom box, the scalar window, and the background box.
The widen pivots on the average X of players inside that 16:9 screen box.
Each player's fraction of the width stays the same as 16:9.
The scalar center X becomes the midpoint of the widened left and right.
With no player inside the box, the pivot stays the old center.
The title course does not widen the screen box, so the parade stays centered.
Its background box still widens around its own center.
Background parts spawn from the background edges, and those edges widen on the title course too.
