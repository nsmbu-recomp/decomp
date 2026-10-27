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

`BgScrollMgr::sInstance` is `0x101E7C54`.
The screen update is `0x029A6170`.
It writes the 1280-wide screen window: left at `+0x70`, center X at `+0x74`, and right at `+0x78`.
Background edges are `+0x88` and `+0x90`.
The title parade and other screen-follow placement read the screen window, so a wider left and right slide that parade toward the new right edge.
Background parts spawn from the background edges.
