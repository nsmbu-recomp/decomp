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

## Story-map notice

USA 1.3.0 `f_02632418` requests message `SaveData_Rdash_info_00`.
That message is index 0 in the common-window table at `0x102145B4`.
The function stores the index at window-task `+0x1A34` and sets byte `+0x1A3C`.
`f_025D065C` then opens the window.
`f_02632458` writes `1` to save header byte `+0x8` (`rdash_state`) after the player closes it.
Europe 1.3.0 has the same request at `0x026324BC`.
Base discs do not have this function.
