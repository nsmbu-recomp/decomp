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
