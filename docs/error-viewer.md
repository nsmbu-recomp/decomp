# Error viewer

`ErrorViewer::show` is `0x029CBBB4` in USA 1.3.0.
The function is 580 bytes.

Europe 1.3.0 compiles that function at `0x029CBC58`, still 580 bytes.
Both versions show the error viewer.

USA writes 1 to the kind field at offset 12 of the message struct it builds on the stack.
Europe writes 2 to that field.
The field is not an `ErrorViewer` member.
Display mode comes from `*(*(0x101D7D68) + 0x10)`.
USA: display mode 1 selects kind 2, display mode 2 selects kind 5, and any other mode selects kind 1.

Display modes 1 through 7 load a kind byte from `0x10189E27` indexed by the mode.
Those bytes are 2, 3, 4, 5, 8, 9, and 10.
A mode outside that range selects kind 1.
USA has no such table at `0x10189E27`.
