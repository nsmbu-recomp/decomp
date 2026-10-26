# Screen projections

`sead::OrthoProjection::setTBLR` is `0x02A06818`.
It stores top, bottom, left, and right at object offsets 156, 160, 164, and 168.

`sead::PerspectiveProjection` is constructed at `0x02A061AC`.
The arguments are near, far, field of view in radians, and aspect.
The per-frame setter at `0x02A062EC` uses that same argument order.

Field of view is stored at object offset 156, and aspect at offset 172.
`DistantViewMgr` keeps its projection at object offset 168.
