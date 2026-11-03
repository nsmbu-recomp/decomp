# RDash add-on mount

USA 1.3.0 `RDashMgr` setup is `f_029D4818`.
It calls `f_029D4548`.

`f_029D4548` uses `nn_aoc` in this order:

1. `AOC_Initialize`.
2. `AOC_CalculateWorkBufferSize(1)`.
3. `AOC_ListTitle(&count, title, 1, work, workSize)`.
4. Stop when `count` is not 1.
5. `AOC_OpenTitle(path, title, work, workSize)`.

The title record is `0x61` bytes.
The path result is a C string.
The game appends the fixed text `/0010`.
That string is the root of the sead device named `rdash`.

`RDashMgr` byte `+0x10` stores the mount result.
Byte `+0x11` is `isNSLU`.
A mount result of 0 leaves the eShop prompt on the title.
