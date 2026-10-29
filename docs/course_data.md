# Course data blocks (USA v1.3.0)

## Blocks and sizes

`CourseDataFile::registerBlocks` (`0x0269972C`) fills `mBlock`, `mBlockSize`, and `mBlockEntryNum` from the element size table at `0x100E5FCC`.
The table has one `u32` per block, because the file is read as one blob that is split on those sizes.

The confirmed table values per block index:

- `0` = `0x80`, environment
- `1` = `0x18`, options
- `2` = `0x1C`, scroll data
- `3` = `0x08`
- `4` = `0x1C`, distant view data (`sizeof(DistantViewData)`)
- `5` = `0x14`
- `6` = `0x18`, next goto (`sizeof(NextGoto)`)
- `7` = `0x18`, map actor (`sizeof(MapActorData)`)
- `8` = `0x04`, map actor load res
- `9` = `0x1C`, area data (`sizeof(AreaData)`)
- `10` = `0x0C`, location (`sizeof(Location)`)
- `11` = `0x18`
- `12` = `0x18`
- `13` = `0x0C`, rail info (`sizeof(RailInfo)`)
- `14` = `0x14`, rail point (`sizeof(RailPoint)`)

`registerBlocks` subtracts `4` from the block `7` size before computing the entry count, so the map actor block carries a `4` byte header in the file.
The header enum `CourseDataFile::Block` matches the table, so `cBlock_*` indices are correct.
The struct offsets in `include/map/CourseData.h` also match: `mBlock` `+0x04`, `mBlockSize` `+0x40`, `mBlockEntryNum` `+0x7C`, `_b8` `+0xB8`, `_1b8` `+0x1B8`, `_2b8` `+0x2B8`, total `0x3B8`.

## CourseData

`CourseData` is `0xEF0` bytes: a `SEAD_SINGLETON_DISPOSER` prefix of `0x10` bytes and `sead::SafeArray<CourseDataFile, 4> mFile` at `+0x10`.
The ctor (`0x0269A144`) builds the four files with the shared array helper `sub_02AFA350(mFile, 4, 0x3B8, 0x026A94FC)`.

`CourseData::getFile` (`0x0269A2DC`) clamps an out of range index to element `0` (`(u32)index >= 4`).
It returns `nullptr` when the file has no area data block (block `9`).
The header declares `getFileIfLoaded` at that address, but the rpx symbol map names `0x0269A2DC` `CourseData::getFile`.

## CourseDataFile getters

Each getter reads a block base pointer and its entry count from `CourseDataFile`:

- `getNextGoto` (`0x02699BD8`): base `+0x1C`, count `+0x94`, stride `0x18`, id byte at `+0x08`
- `getMapActor` (`0x02699C08`): base `+0x20`, count `+0x98`, stride `0x18`, type `u16` at `+0x00`; `p_start` resumes at `p_start + 0x18`
- `getLocation` (`0x0269A020`): base `+0x2C`, count `+0xA4`, stride `0x0C`, id byte at `+0x08`
- `getRailInfo` (`0x0269A0A8`): base `+0x38`, count `+0xB0`, stride `0x0C`, id byte at `+0x00`; returns `nullptr` when the block is not loaded
- `getRailPoint` (`0x0269A0F4`): `getRailInfo(rail_id)`, then `block 14 + rail->point.start * 0x14`

`getLocation` also calls the area box builder (`0x02699F70`) with a delta of `0.0f` when `p_box != nullptr`.
`0x02699F70` takes the same fields as `getAreaBox_` but takes separate x and y deltas, which are equal at every call site.

## World map model layout

`CourseSelectMap` is `0x40C8` bytes.
Nine model lists start at `+0x10`, each `0x10` bytes.
The list head is `+0x00`, the walk start is `+0x04`, the count is `+0x08`, and the model node offset is `+0x0C` (`0x72C`).
Byte `+0xA0` is `1` after the ctor, and `CourseSelectParallelExecuter` sits at `+0xA4`.

A list node is `(u8*)model + mOffset`.
`sub_029F1A50` (`0x029F1A50`) links a node at the head.
Each node keeps the list pointer at `+0x04` as the terminator, so iteration starts from the list `+0x04` field and follows node `+0x04`.

`CourseSelectMap::initialize` (`0x021F6C14`) runs the whole map model setup:

1. `mExecuter.initialize(nullptr)`
2. per world `0..8`, `loadModel(world, index)` until it returns null, linking each model and bumping the count
3. `sub_021F5D20` (`0x021F5D20`)
4. per world, per model, `sub_021FAC24(CourseSelectMgr::instance(), model, world, index)` (`0x021FAC24`), which walks the list from `+0x04`
5. `sub_021F62E0` (`0x021F62E0`)
6. per world, `sub_021F6AEC(this, world, MainGame::sInstance[0x2F8 + world*4], 1.0f, 1)` (`0x021F6AEC`)
7. `sub_021F6B3C` (`0x021F6B3C`)
8. `sub_0229B44C` (`0x0229B44C`) on the first node of list `1`
9. per world, per model, `sub_0229A4A8` (`0x0229A4A8`)

`CourseSelectMap::loadModel` (`0x021F56CC`) formats `CS_W<world+1>` and `course_select/<name>.szs`.
For index `0` it first checks the plain archive, then falls back to the suffixed name.
For higher indices it only checks the suffixed name `CS_W<world+1><a+index>`.
It then builds a `MapModel` (`0x022961AC`) with a null heap and calls `initialize(world, index)` (`0x02296B38`).

`CourseSelectMap::calc` (`0x021F6FE8`) returns early when `CourseSelectTask+0xDC9` is not `0`.
Otherwise it clears the executer, queues every list node (cap `0x1000`), runs `executeAll`, and calls `sub_0228A948(*(0x101CD0C0))`.

`CourseSelectMap::draw` (`0x021F7114`) sets a renderer layer, draws the lists in the order `8, 5, 6, 0, 1, 4, 3, 7, 2` (`0x10047808`), then resets the layer.

## Path tables

`CourseSelectMgr` keeps two path tables, each with `0x12` entries.

- `getPath` (`0x021F9FC8`) uses `+0x18`
- `getMapPath` (`0x021F9FEC`) uses `+0x60`

Both index with `(world << 1) + section` and fall back to entry `[0]` when the index is `0x12` or higher, so they never return null.
`CourseSelectPathNode` stride is `0x68` and `getNode` (`0x021C9464`) rejects indices `0x100` and higher.
Path node fields: name at `+0x00`, position at `+0x44/+0x48/+0x4C`, course slot at `+0x58`, node index at `+0x5C`.
