# Grass and flower quads

`Bg::sInstance` is `0x101DAB94`.
`0x0268B67C` runs on the area-task calc.
It calls `updateGrassAndFlowers` (`0x02686428`) with `Bg+0x3F90` and a zero init flag.
The same object draws from `0x0268B70C`.
That call is inside the logic step that records the display list.
A hold present does not call it.
The hold present copies the recorded picture from `calc_` (`0x029C5104`).
That draw copies `Bg+0x14` to `Bg+0x18`, then draws four `TexQuadDeco` objects.
The objects sit at `+0`, `+0xC78`, `+0x18F0`, and `+0x2568` inside `Bg+0x3F90`.
The draw skips `+0x2568` when the byte at `+0x3E4B` is zero.
Each draw goes through `0x02503AB8` into `0x02501D08`.

The byte at `TexQuadDeco+0xBC` selects the quad.
Kind 2 does not move the side edges.
Other kinds load a float pair from `+0x54`.
The count is the word at `+0x50`.
The draw also loads vec3 positions from `+0x44`.
The count is the word at `+0x40`.
The position step is 12 bytes.
Kind 3 adds the vec3 at `+0x4C` to the position.
The count is the word at `+0x48`.
The loop stops at `0x12C` quads.

The draw at `0x02501D08` loads edge width from the wave heap (`lfsx` / `+4` on `+0x54`), vec3 positions from `+0x44`, and sway from `TexQuadDeco+0x570`.
The instance tables at `Bg+0x38D4`, `+0x38D8`, and `+0x38DC` hold pointers and tags at stride `0x10`; the draw does not lerp those words.
Kind 2 (flowers) uses the first float in each wave pair for scale; other kinds use the second float with sway.

`updateGrassAndFlowers` writes those pairs on the per-frame path (`r4 == 0`).
The init path (`r4 != 0`) comes from `0x0268A038`.

The baked quads live on the draw object at `*(*0x101D2DB0 + 0x24)`.
`0x02509D90` flips `+0x394` between 0 and 1 and clears the counter at `+0x398`.
The vertex buffer at `+0x3B4` holds both halves.
A half starts at `span * (+0x3A8) * 0x88`, and each recorded index copies `(+0x3A4)` vertices of `0x88` bytes.
`+0x3A0` is the index limit.
`+0x30` is a side vec3 list.
`0x02508B1C` allocates one vec3 when `+0xD28` is zero, and `(+0x3A0)` vec3s otherwise.
`0x0250A554` skips the side store when `+0xD28` is zero.
`0x0250A8C0` walks the twelve objects at `parent+0x10` and calls virtual offset `0x6C`.
A zero word at the start of one of those objects faults at guest address `0x6C`.
