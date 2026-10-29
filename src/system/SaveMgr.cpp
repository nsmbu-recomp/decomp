#include <system/SaveMgr.h>

#include <game/FieldGame.h>
#include <game/SubjectMgr.h>
#include <game_info/CourseInfo.h>
#include <system/MainGame.h>
#include <player/PlayerEnum.h>
#include <scene/Scene.h>
#include <system/CommentMgr.h>
#include <system/MainGame.h>

#include <filedevice/seadFileDevice.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <prim/seadFormat.h>
#include <prim/seadSafeString.h>

#include <cstring>

namespace
{

constexpr u32 kSavedataMagic          = 0x52505344u;  // 'RPSD'
constexpr u32 kSavedataVersionMajor   = 1;
constexpr u32 kSavedataVersionMinor   = 0;
constexpr u32 kSavedataVersionPatch   = 2;
constexpr u32 kSavedataSize           = 0xB134;
constexpr u32 kSavedataRecordCount    = 6;
constexpr u32 kSavedataRecordSize     = 0x204;
constexpr u32 kSavedataMainRecordBase = 0x10;
constexpr u32 kSavedataBackupBase     = 0xC28;
constexpr u32 kSavedataPlayTimeBase   = 0xAE2C;
constexpr u32 kMaxPlaySeconds         = 999999999u;

constexpr uintptr_t kSaveFileDevice     = 0x101E7F18u;
constexpr uintptr_t kSaveTableBuiltFlag = 0x101E7EF8u;
constexpr uintptr_t kSaveHashTable      = 0x105F9A54u;
constexpr uintptr_t kSaveTrailerFnTable = 0x101929B8u;
constexpr uintptr_t kSaveTrailerFnIndex = 0x105FAA78u;

constexpr uintptr_t kCounterMgrInstance = 0x101D2CCCu;
constexpr uintptr_t kCounterMgrFlag     = 0x500u;
constexpr uintptr_t kSaveApplyMgr       = 0x101E7D9Cu;
constexpr uintptr_t kSaveApplyFlagOff   = 0x11u;
constexpr uintptr_t kNewGameFlag        = 0x101EBB86u;
constexpr uintptr_t kPlayLogSource      = 0x101D4D70u;
constexpr uintptr_t kPlayLogDeviceByte  = 0xA1772u;
constexpr uintptr_t kSceneInstance      = 0x101E7B28u;

constexpr u32 kCourseSelectTaskFactory = 0x0202B86Cu;
constexpr u32 kMenuSceneWrapperFactory = 0x0202B570u;

constexpr u32 kPersonalSavedataSize = 8;

typedef u32 (*SavedataTrailerFn)(u32);

// Address: 0x029F1960
void buildSavedataHashTable()
{
    u32* table = reinterpret_cast<u32*>(kSaveHashTable);

    for (u32 i = 0; i < 256; ++i)
    {
        u32 c = i;

        for (s32 k = 0; k < 8; ++k)
            c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;

        table[i] = c;
    }

    *reinterpret_cast<u8*>(kSaveTableBuiltFlag) = 1;
}

// Address: 0x029F19C4
u32 computeSavedataBlockHash(const void* data, u32 size)
{
    if (*reinterpret_cast<const u8*>(kSaveTableBuiltFlag) == 0)
        buildSavedataHashTable();

    const u32* table = reinterpret_cast<const u32*>(kSaveHashTable);
    const u8* p = static_cast<const u8*>(data);
    u32 crc = 0xFFFFFFFF;

    for (u32 i = 0; i < size; ++i)
        crc = ((crc << 8) & 0x00FFFFFFu) ^ table[(crc ^ *p++) & 0xFF];

    return ~crc;
}

SavedataTrailerFn savedataTrailerFn()
{
    const u32* table = reinterpret_cast<const u32*>(kSaveTrailerFnTable);
    return reinterpret_cast<SavedataTrailerFn>(table[*reinterpret_cast<const u32*>(kSaveTrailerFnIndex)]);
}

u8* savedataAt(SaveMgr* mgr, u32 offset)
{
    return reinterpret_cast<u8*>(const_cast<SaveData*>(mgr->mpSavedata)) + offset;
}

u8 saveApplyFlag()
{
    return *reinterpret_cast<const u8*>(*reinterpret_cast<const u32*>(kSaveApplyMgr) + kSaveApplyFlagOff);
}

sead::FileDevice* saveFileDevice()
{
    return *reinterpret_cast<sead::FileDevice**>(kSaveFileDevice);
}

// Address: 0x029CF580
s32 sub_029CF580(u8 value)
{
    if (value == 0xFF)
        return -1;

    return value;
}

// Address: 0x021F54D0
void sub_021F54D0(void* dst, const void* src)
{
    std::memcpy(dst, src, 0xC);
}

// Address: 0x024BBBD4
void sub_024BBBD4(void* course_flags);

// Address: 0x024BBC48
void sub_024BBC48(void* course_flags, const void* src);

// Address: 0x024BB344
u32 sub_024BB344(const void* course_flags);

// Address: 0x024BB3B0
void sub_024BB3B0(const void* course_flags, u8* out_bits);

// Address: 0x024CD624
void sub_024CD624(u8* course_bits, u32 index, bool value);

// Address: 0x024CD834
void sub_024CD834(void* course_bits, const void* src);

// Address: 0x024C4658
void sub_024C4658(void* stats, const void* src);

// Address: 0x024C41A8
void sub_024C41A8(void* stats);

// Address: 0x021D6540
void sub_021D6540(void* dst);

// Address: 0x021D6778
void sub_021D6778(void* dst, const void* src);

// Address: 0x029CDD48
void sub_029CDD48(void* main, const void* src);

// Address: 0x029CDD64
void sub_029CDD64(void* main, u32 value);

// Address: 0x029CDD8C
void sub_029CDD8C(void* main, u32 value);

// Address: 0x029CDEE4
void sub_029CDEE4(void* main, u8 arg_0, u8 arg_1);

// Address: 0x029CD22C
void sub_029CD22C(void* main);

// Address: 0x029CD1E8
void sub_029CD1E8(void* main);

// Address: 0x029CDF24
void sub_029CDF24(void* main);

// Address: 0x029CDF30
void sub_029CDF30(void* main);

// Address: 0x029CD290
void sub_029CD290(void* main);

// Address: 0x029C8134
void sub_029C8134(CommentMgr* mgr, u32 value);

// Address: 0x029C816C
void sub_029C816C(CommentMgr* mgr, u8 value);

// Address: 0x02A1A020
void sub_02A1A020(void* tick_time);

void signalSaveThread_(SaveMgr* mgr, u32 mode)
{
    u8* thread = reinterpret_cast<u8*>(mgr->mpDelegateThread);
    u8* delegate_ = *reinterpret_cast<u8**>(thread + 0xC);
    reinterpret_cast<void (*)(u8*, u32, u32)>(*reinterpret_cast<u32*>(delegate_ + 0x1C))(delegate_, mode, 1);
}

// Address: 0x029D6D80
void initRecord_(SaveMgr* mgr, u32 index)
{
    (void)mgr;
    (void)index;
}

// Address: 0x029D8FDC
void normalizeHeader_(SaveMgr* mgr)
{
    u8* savedata = savedataAt(mgr, 0);

    u8 last_file = savedata[0x07];
    if (last_file > 5)
        last_file = 5;
    savedata[0x07] = last_file;

    u8 rdash_state = savedata[0x08];
    if (rdash_state > 1)
        rdash_state = 1;
    savedata[0x08] = rdash_state;

    u8 _09 = savedata[0x09];
    if (_09 > 1)
        _09 = 1;
    savedata[0x09] = _09;
}

// Address: 0x029D9020
void normalizeMapRecord_(SaveMgr* mgr)
{
    u8* base = savedataAt(mgr, 0x1840);

    u8 flag = base[0];
    if (flag > 1)
        flag = 1;
    base[0] = flag;

    for (u32 i = 1; i <= 5; ++i)
    {
        u8 v = base[i];
        if (v > 3)
            v = 3;
        base[i] = v;
    }

    u8 flag6 = base[6];
    if (flag6 > 1)
        flag6 = 1;
    base[6] = flag6;

    for (u32 i = 0; i < 0x80; ++i)
    {
        u8* entry = savedataAt(mgr, 0x1848 + i * 0x6C);

        u8 a = entry[0];
        if (a > 5)
            a = 5;
        entry[0] = a;

        u8 b = entry[1];
        if (b > 5)
            b = 5;
        entry[1] = b;
    }
}

// Address: 0x029D90C4
void normalizeReplayRecord_(SaveMgr* mgr)
{
    u8* base = savedataAt(mgr, 0x4E4C);

    u8 flag = base[0];
    if (flag > 1)
        flag = 1;
    base[0] = flag;

    for (u32 i = 0; i < 0x20; ++i)
    {
        u8* byte = savedataAt(mgr, 0x4E4C + 0x84 + i);
        if (*byte > 2)
            *byte = 2;

        u32* counter = reinterpret_cast<u32*>(savedataAt(mgr, 0x4E4C + 0x04 + i * 4));
        if (*counter > 999999u)
            *counter = 999999u;
    }
}

// Address: 0x029D912C
void normalizeSaveCounter_(SaveMgr* mgr)
{
    u8* counter = savedataAt(mgr, 0xAEC0);
    if (*counter > 0x3Eu)
        *counter = 0x3Eu;
}

// Address: 0x024E87DC
u32 savedataCounter_(void* counter_mgr, u8* dst)
{
    u8* counter = reinterpret_cast<u8*>(counter_mgr);
    const u8* node = reinterpret_cast<const u8*>(*reinterpret_cast<const u32*>(counter + 0x14)) - 0xC;
    const u8* sentinel = counter + 4;

    *reinterpret_cast<u8*>(counter + 0x500) = 0;

    while (node != sentinel)
    {
        std::memcpy(dst, node, 0xA);
        node = reinterpret_cast<const u8*>(*reinterpret_cast<const u32*>(node + 0x10)) - 0xC;
        dst += 0xA;
    }

    return *reinterpret_cast<const u32*>(counter + 0x18);
}

// Address: 0x029F81B0-family helper: tick delta to elapsed seconds
u32 elapsedSeconds_(u64 now, u64 init)
{
    (void)now;
    (void)init;
    return 0;
}

// Address: 0x024D2904
void resetCourseInfo_(CourseInfo* course_info)
{
    u8* info = reinterpret_cast<u8*>(course_info);

    *reinterpret_cast<u32*>(info + 0x20) = 0;
    info[0x13] = 0xFF;
    *reinterpret_cast<u32*>(info + 0x18) = 0;
    info[0x1F] = 0xFF;
    info[0x10] = 0;
    info[0x12] = 0;
    info[0x11] = 0;
    info[0x14] = 0;
    info[0x1E] = 0;
    info[0x1C] = 0;
    info[0x1D] = 0;
}

// Address: 0x024CD7EC
void resetMainGameCourseSelectBits_(u8* course_bits)
{
    for (u32 index = 0; index < 0x52; ++index)
        sub_024CD624(course_bits, index, false);

    *reinterpret_cast<u32*>(course_bits) = 0;
}

// Address: 0x029CF2D0
void resetMainGameRunState_()
{
    u8* main = reinterpret_cast<u8*>(MainGame::instance());

    *reinterpret_cast<u32*>(main + 0x1A0) = 0;
    *reinterpret_cast<u32*>(main + 0x19C) = 0;
    *reinterpret_cast<u32*>(main + 0x1A4) = -1;
    *reinterpret_cast<u32*>(main + 0x1BC) = 0;

    static const s32 cNoMapPose[3] = { -1, -1, -1 };
    sub_021F54D0(main + 0x1D4, cNoMapPose);

    if (main[0x78] == 0)
        main[0x78] = 1;

    sub_024BBBD4(main + 0x31C);
    resetMainGameCourseSelectBits_(main + 0x34);

    const f32* default_scale = reinterpret_cast<const f32*>(0x105EA760);
    *reinterpret_cast<f32*>(main + 0x858) = default_scale[0];
    *reinterpret_cast<f32*>(main + 0x85C) = default_scale[1];
    *reinterpret_cast<f32*>(main + 0x860) = default_scale[2];

    for (s32 i = 0; i < 10; ++i)
        *reinterpret_cast<u32*>(main + 0x180B8 + i * 4) = 0xE;

    *reinterpret_cast<u16*>(main + 0x208) = 0;
    main[0x20A] = 0;
    std::memset(main + 0x64, 0, 0x10);

    sub_021D6540(main + 0x3D8);
    main[0x21DFC] = 0;
    sub_029CDD8C(main, 0);
    main[0x82D] = 0;
    main[0x82C] = 0;

    static const s32 cNoMapSection[3] = { -1, -1, -1 };
    sub_021F54D0(main + 0x1C8, cNoMapSection);

    main[0x21E00] = 0;
    main[0x21E01] = 0;
    main[0x21DFE] = 0xFF;
    main[0x21DFF] = 0xFF;

    for (s32 i = 0; i < 7; ++i)
        main[0x21E03 + i] = 0;

    sub_029CDEE4(main, 0, 0);
    *reinterpret_cast<u32*>(main + 0x218) = 0;
    main[0x21E0E] = 0;
    main[0x21E0F] = 0;

    CommentMgr* comment_mgr = CommentMgr::instance();
    sub_029C8134(comment_mgr, 0);
    sub_029C816C(comment_mgr, 0);
    comment_mgr->_d0 = 0;

    sub_029CD1E8(main);
    sub_024C41A8(&FieldGame::instance()->getGameData().getStatsData());
    sub_029CDF24(main);
    sub_029CDF30(main);
    main[0x180E0] = 0;
    sub_029CD290(main);

    main[0x214] = 0;
    main[0x1F9] = 0;
    main[0x21E14] = 0;
    *reinterpret_cast<u16*>(main + 0x21E10) = 0;
    *reinterpret_cast<u32*>(main + 0x24) = 0;
    *reinterpret_cast<u32*>(main + 0x20) = 1;
    *reinterpret_cast<u32*>(main + 0x1C0) = 0;
    *reinterpret_cast<u32*>(main + 0x218) = 0;
    sub_029CDD64(main, 0);

    main[0x82E] = 0;
    *reinterpret_cast<u32*>(main + 0x1A8) = -1;
    *reinterpret_cast<u32*>(main + 0x1F0) = 6;
    *reinterpret_cast<u32*>(main + 0x1AC) = -1;
    main[0x1FF] = 0;
    main[0x1FE] = 0;
    *reinterpret_cast<u32*>(main + 0x1B4) = -1;
    main[0x1FB] = 0;
    *reinterpret_cast<u32*>(main + 0x1B0) = -1;
    main[0x1FC] = 0;
    main[0x1F8] = 0;
    *reinterpret_cast<u32*>(main + 0x1B8) = -1;
    main[0x82F] = 0;
    main[0x200] = 0;
    main[0x830] = 0;
    main[0x1FD] = 0;

    static const s32 cNoMapNode[3] = { -1, -1, -1 };
    sub_021F54D0(main + 0x1E0, cNoMapNode);

    *reinterpret_cast<u32*>(main + 0x1EC) = 6;
    main[0x21DFD] = 0;
    sub_029CD22C(main);
    main[0x21E02] = 0;
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(comment_mgr) + 0xEC) = 0;
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(comment_mgr) + 0xE8) = 0;
    main[0x21DFB] = 0;
    main[0x21DFA] = 0;
    main[0x82F] = 0;
    *reinterpret_cast<u32*>(main + 0x7C) = 0;
    main[0x82E] = 0;
    *reinterpret_cast<u32*>(main + 0x18) = 0;

    SubjectMgr::instance()->clearSubject();
}

// Address: 0x029CF590
void copyPlayerSetupToMainGame_(const u8* record, u32 play_seconds)
{
    (void)play_seconds;

    MainGame* main_game = MainGame::instance();
    u8* main = reinterpret_cast<u8*>(main_game);

    *reinterpret_cast<u32*>(main + 0x19C) = record[4];
    *reinterpret_cast<u32*>(main + 0x1A0) = record[5];
    *reinterpret_cast<u32*>(main + 0x1A4) = sub_029CF580(record[6]);
    *reinterpret_cast<u32*>(main + 0x1BC) = record[4];

    s32 map_pose[3];
    if (record[7] == 0xFF)
    {
        map_pose[0] = -1;
        map_pose[1] = -1;
        map_pose[2] = -1;
    }
    else
    {
        map_pose[0] = record[7];
        map_pose[1] = record[8];
        map_pose[2] = sub_029CF580(record[9]);
    }
    sub_021F54D0(main + 0x1D4, map_pose);

    u8 otehon = record[3] & 1;
    if (main[0x78] != otehon)
        main[0x78] = main[0x78] ^ 1;

    sub_024BBC48(main + 0x31C, record + 0x6B);
    sub_024CD834(main + 0x34, record + 0x124);

    *reinterpret_cast<f32*>(main + 0x858) = *reinterpret_cast<const f32*>(record + 0x20);
    *reinterpret_cast<f32*>(main + 0x85C) = *reinterpret_cast<const f32*>(record + 0x24);
    *reinterpret_cast<f32*>(main + 0x860) = *reinterpret_cast<const f32*>(record + 0x28);

    for (s32 i = 0; i < 10; ++i)
        *reinterpret_cast<u32*>(main + 0x180B8 + i * 4) = record[0x14D + i];

    *reinterpret_cast<u16*>(main + 0x208) = *reinterpret_cast<const u16*>(record + 0x1C);
    main[0x20A] = record[0x58] != 0;

    sub_029CDD48(main, record + 0x34);
    sub_021D6778(main + 0x3D8, record + 0x157);

    main[0x21DFC] = record[0x1FC];
    sub_029CDD8C(main, *reinterpret_cast<const u32*>(record + 0x2C));

    main[0x82C] = record[0x1E];
    main[0x82D] = record[0x1F];

    s32 map_node[3];
    if (record[0x46] == 0xFF)
    {
        map_node[0] = -1;
        map_node[1] = -1;
        map_node[2] = -1;
    }
    else
    {
        map_node[0] = record[0x46];
        map_node[1] = record[0x47];
        map_node[2] = sub_029CF580(record[0x48]);
    }
    sub_021F54D0(main + 0x1C8, map_node);

    main[0x21DFE] = record[0x49];
    main[0x21DFF] = record[0x4A];
    main[0x21E00] = record[0x4B];
    main[0x21E01] = record[0x4C];

    for (s32 i = 0; i < 7; ++i)
        main[0x21E03 + i] = record[0x64 + i];

    sub_029CDEE4(main, record[0xA], record[0xB]);

    *reinterpret_cast<u32*>(main + 0x218) = record[0x4D];
    main[0x21E0E] = record[0x4F] & 1;
    main[0x21E0F] = (record[0x4F] >> 1) & 1;

    CommentMgr* comment_mgr = CommentMgr::instance();
    sub_029C8134(comment_mgr, record[0x50] & 0x7F);
    sub_029C816C(comment_mgr, (record[0x50] >> 7) & 1);
    comment_mgr->_d0 = record[0x51] != 0;

    main[0x20B] = record[0x52];
    main[0x20C] = record[0x53];
    main[0x180E0] = record[0x54] != 0;
    main[0x20D] = record[0x55];
    *reinterpret_cast<u32*>(main + 0x210) = record[0x56];
    main[0x20E] = record[0x5A];
    main[0x20F] = record[0x5B];
    main[0x214] = record[0x59] != 0;
    main[0x1F9] = record[0x57] != 0;

    for (s32 i = 0; i < 4; ++i)
    {
        main[0x824 + i] = record[0x5C + i];
        main[0x828 + i] = record[0x60 + i];
    }

    sub_024C4658(&FieldGame::instance()->getGameData().getStatsData(), record + 0x1C4);
    *reinterpret_cast<u16*>(main + 0x21E10) = *reinterpret_cast<const u16*>(record + 0x44);

    *reinterpret_cast<u32*>(main + 0x20) = 1;
    *reinterpret_cast<u32*>(main + 0x1C0) = 0;
    *reinterpret_cast<u32*>(main + 0x24) = 0;
    sub_029CDD64(main, 0);

    main[0x1FF] = 0;
    *reinterpret_cast<u32*>(main + 0x1B8) = -1;
    main[0x1FE] = 0;
    *reinterpret_cast<u32*>(main + 0x1F0) = 6;
    *reinterpret_cast<u32*>(main + 0x1B4) = -1;
    main[0x1FC] = 0;
    main[0x1FD] = 0;
    main[0x830] = 0;
    *reinterpret_cast<u32*>(main + 0x1B0) = -1;
    main[0x200] = 0;
    main[0x82F] = 0;
    *reinterpret_cast<u32*>(main + 0x1A8) = -1;
    main[0x82E] = 0;
    main[0x1FB] = 0;
    main[0x1F8] = 0;
    *reinterpret_cast<u32*>(main + 0x1AC) = -1;

    s32 current_pose[3];
    current_pose[0] = *reinterpret_cast<const u32*>(main + 0x19C);
    current_pose[1] = *reinterpret_cast<const u32*>(main + 0x1A0);
    current_pose[2] = *reinterpret_cast<const u32*>(main + 0x1A4);
    sub_021F54D0(main + 0x1E0, current_pose);

    *reinterpret_cast<u32*>(main + 0x1EC) = 6;
    main[0x21DFD] = record[0x1FC];
    sub_029CD22C(main);
    main[0x21E02] = 0;
    main[0x21DFB] = 0;
    main[0x21DFA] = 0;
    main[0x82E] = 0;
    main[0x82F] = 0;

    SubjectMgr::instance()->clearSubject();
}

// Address: 0x024C1FC4
void applyPlayerEntryFlags_(FieldGameData* game_data, u8 flag0, u8 flag1, u8 flag2, u8 flag3)
{
    const u32 characters[cPlayerNum] = { flag0, flag1, flag2, flag3 };

    game_data->orderEntriesByCharacter(characters);
}

// Address: 0x024C2230
void enterMapFromGameData_(FieldGameData* game_data, s32 mode)
{
    (void)mode;

    u8* data = reinterpret_cast<u8*>(game_data);
    CourseInfo* course_info = CourseInfo::instance();

    game_data->setCoinNum(0);
    *reinterpret_cast<u32*>(data + 0x138) = 0;
    game_data->resetEntriesUnk30();

    // Next world/course come from CourseInfo (set by course-out / menu).
    // Checkpoint itself is only filled by the save map-enter path (0x0250C76C).
    u8 next_world = course_info->getNextWorldNo();
    u8 next_course = course_info->getNextCourseNo();

    s32 checkpoint = game_data->checkEnterCourse(next_world, next_course);

    FieldGameMode::Mode game_mode = game_data->getGameMode().getMode();

    if (game_mode == FieldGameMode::cCoinBattle || game_mode == FieldGameMode::cCoinEdit
        || game_mode == FieldGameMode::cChallenge || checkpoint == -1)
    {
        game_data->clearCheckPoint();

        *reinterpret_cast<u8*>(data + 0x13D) = 0xFF;
        *reinterpret_cast<u8*>(data + 0x13E) = 0xFF;
        *reinterpret_cast<u8*>(data + 0x13F) = 0xFF;

        course_info->setNextFileNo(0);
        course_info->setNextNextGotoNo(0xFF);
        course_info->setNextSceneEnterType(1);
    }
    else
    {
        for (s32 i = 0; i < 3; ++i)
            *reinterpret_cast<u8*>(data + 0x13D + i) = *reinterpret_cast<u8*>(data + 0x10 + i);

        course_info->setNextFileNo(*reinterpret_cast<u8*>(data + 0x06));
        course_info->setNextNextGotoNo(*reinterpret_cast<u8*>(data + 0x07));
        course_info->setNextSceneEnterType(2);
    }

    for (s32 i = 0; i < 5; ++i)
        *reinterpret_cast<u32*>(data + 0x144 + i * 4) = 0xE;

    *reinterpret_cast<u8*>(data + 0x1A4) = 0;
    *reinterpret_cast<u32*>(data + 0x164) = 0xE;
}

// Address: 0x029D9E80
void updateStoryPlayText_(SaveMgr* mgr)
{
    sub_02A1A020(reinterpret_cast<u8*>(mgr) + 0x40);
}

} // namespace

// Address: 0x029E2924
bool checkSavedataBlockCrc(const void* block, u32 size)
{
    const u8* data = static_cast<u8 const*>(block);
    u32 computed = computeSavedataBlockHash(data, size - 4);
    u32 stored = *reinterpret_cast<const u32*>(data + size - 4);
    return computed == savedataTrailerFn()(stored);
}

// Address: 0x029E2994
void writeSavedataBlockCrc(void* block, u32 size)
{
    u8* data = static_cast<u8*>(block);
    u32 computed = computeSavedataBlockHash(data, size - 4);
    *reinterpret_cast<u32*>(data + size - 4) = savedataTrailerFn()(computed);
}

void SaveMgr::runState(u32 state)
{
    switch (state)
    {
    case 1:
        loadSavedata();
        break;
    case 2:
        saveSavedata();
        break;
    case 3:
        loadChallengeReplay();
        break;
    case 4:
        saveChallengeReplay();
        break;
    case 5:
        loadPersonalSavedata();
        break;
    case 6:
        savePersonalSavedata();
        break;
    }
}

// Address: 0x029D8270
void SaveMgr::startReadGame()
{
    if (mStatus != cStatus_Ready)
        return;

    mStatus = cStatus_Reading;
    signalSaveThread_(this, 1);
}

// Address: 0x029D82A0
void SaveMgr::startReadChallengeReplay(void* param)
{
    if (mStatus != cStatus_Ready)
        return;

    _20 = reinterpret_cast<s32>(param);
    mStatus = cStatus_Reading;
    signalSaveThread_(this, 3);
}

// Address: 0x029D82D4
void SaveMgr::startReadPersonalSavedata()
{
    if (mStatus != cStatus_Ready)
        return;

    mStatus = cStatus_Reading;
    signalSaveThread_(this, 5);
}

// Address: 0x029D8478
void SaveMgr::clearQuickAndWrite(u8 file)
{
    u8* quick = savedataAt(this, kSavedataBackupBase + file * kSavedataRecordSize);
    std::memset(quick, 0, kSavedataRecordSize);
    startWriteGame();
}

// Address: 0x029D8304
void SaveMgr::startWriteGame()
{
    if (mStatus != cStatus_Ready)
        return;

    if (*reinterpret_cast<const u8*>(kCounterMgrInstance + kCounterMgrFlag) != 0)
    {
        u8* counter_block = savedataAt(this, 0xAEC0);
        *counter_block = savedataCounter_(reinterpret_cast<u8*>(kCounterMgrInstance), counter_block + 1);
    }

    mStatus = cStatus_Writing;
    signalSaveThread_(this, 2);
}

// Address: 0x029D8E88
void SaveMgr::startWriteChallengeReplay(void* param)
{
    if (mStatus != cStatus_Ready)
        return;

    _20 = reinterpret_cast<s32>(param);
    mStatus = cStatus_Writing;
    _30 = -1;
    signalSaveThread_(this, 4);
}

// Address: 0x029D8394
void SaveMgr::startWritePersonalSavedata()
{
    if (mStatus != cStatus_Ready)
        return;

    mStatus = cStatus_Writing;
    signalSaveThread_(this, 6);
}

// Address: 0x029D6E8C
void SaveMgr::initSavedata()
{
    u8* savedata = savedataAt(this, 0);

    std::memset(savedata, 0, 0x10);
    *reinterpret_cast<u32*>(savedata) = kSavedataMagic;
    savedata[0x04] = kSavedataVersionMajor;
    savedata[0x05] = kSavedataVersionMinor;
    savedata[0x06] = kSavedataVersionPatch;

    for (u32 i = 0; i < kSavedataRecordCount; ++i)
        initRecord_(this, i);
}

// Address: 0x029D914C
void SaveMgr::loadSavedata()
{
    sead::FileHandle handle;

    if (!saveFileDevice()->tryOpen(&handle, "save://rp_savedata.dat", sead::FileDevice::cFileAccess_ReadOnly))
    {
        initSavedata();
        mReadError = cReadError_FileDeviceUnavailable;
        mStatus = cStatus_Ready;
        return;
    }

    u32 read_size = 0;
    bool ok = handle.tryRead(&read_size, mpSavedata, kSavedataSize);
    saveFileDevice()->tryClose(&handle);

    if (!ok || read_size != kSavedataSize)
    {
        if (read_size == 0)
        {
            initSavedata();
            mReadError = cReadError_FileUnavailable;
        }
        else
        {
            mReadError = cReadError_FileInvalid;
        }

        mStatus = cStatus_Ready;
        return;
    }

    mReadError = cReadError_OK;

    u32 invalid = checkSavedataBlockCrc(mpSavedata, 0x10) == 0;

    if (invalid == 0)
    {
        u32 record_flags = 0;

        for (u32 i = 0; i < kSavedataRecordCount; ++i)
        {
            u32 off = i * kSavedataRecordSize;
            if (!checkSavedataBlockCrc(savedataAt(this, kSavedataMainRecordBase + off), kSavedataRecordSize) ||
                !checkSavedataBlockCrc(savedataAt(this, kSavedataBackupBase + off), kSavedataRecordSize))
                record_flags |= 1 << i;
        }

        invalid |= checkSavedataBlockCrc(savedataAt(this, 0x1840), 0x360C) == 0;
        invalid |= checkSavedataBlockCrc(savedataAt(this, 0x4E4C), 0xA8) == 0;
        invalid |= checkSavedataBlockCrc(savedataAt(this, 0x4EF4), 0x5F38) == 0;
        invalid |= checkSavedataBlockCrc(savedataAt(this, kSavedataPlayTimeBase), 0x94) == 0;
        invalid |= checkSavedataBlockCrc(savedataAt(this, 0xAEC0), 0x274) == 0;
    }

    u8* savedata = savedataAt(this, 0);

    if (savedata[0x04] != kSavedataVersionMajor || savedata[0x05] != kSavedataVersionMinor)
        invalid = 1;

    normalizeHeader_(this);
    normalizeMapRecord_(this);
    normalizeReplayRecord_(this);
    normalizeSaveCounter_(this);

    if (*reinterpret_cast<u32*>(savedata) != kSavedataMagic)
        invalid = 1;

    if (invalid != 0)
    {
        initSavedata();
        mReadError = cReadError_FileInvalid;
    }

    mStatus = cStatus_Ready;
}

// Address: 0x029D94C4
void SaveMgr::updatePlayTime()
{
    u64 now = sead::Tick::currentTick();

    u32 played = *reinterpret_cast<const u32*>(savedataAt(this, kSavedataPlayTimeBase));
    u64 total = static_cast<u64>(played) + elapsedSeconds_(now, mInitTime);
    if (total > kMaxPlaySeconds)
        total = kMaxPlaySeconds;

    *reinterpret_cast<u32*>(savedataAt(this, kSavedataPlayTimeBase)) = static_cast<u32>(total);
    mInitTime = now;
}

// Address: 0x029D9560
void SaveMgr::saveSavedata()
{
    sead::FileHandle handle;

    u8* savedata = savedataAt(this, 0);
    savedata[0x04] = kSavedataVersionMajor;
    savedata[0x05] = kSavedataVersionMinor;
    savedata[0x06] = kSavedataVersionPatch;

    writeSavedataBlockCrc(mpSavedata, 0x10);

    for (u32 i = 0; i < kSavedataRecordCount; ++i)
    {
        u32 off = i * kSavedataRecordSize;
        writeSavedataBlockCrc(savedataAt(this, kSavedataMainRecordBase + off), kSavedataRecordSize);
        writeSavedataBlockCrc(savedataAt(this, kSavedataBackupBase + off), kSavedataRecordSize);
    }

    writeSavedataBlockCrc(savedataAt(this, 0x1840), 0x360C);
    writeSavedataBlockCrc(savedataAt(this, 0x4E4C), 0xA8);
    writeSavedataBlockCrc(savedataAt(this, 0x4EF4), 0x5F38);
    writeSavedataBlockCrc(savedataAt(this, kSavedataPlayTimeBase), 0x94);
    writeSavedataBlockCrc(savedataAt(this, 0xAEC0), 0x274);

    if (!saveFileDevice()->tryOpen(&handle, "save://rp_savedata.dat", sead::FileDevice::cFileAccess_WriteOnly))
    {
        mWriteError = cWriteError_FileDeviceUnavailable;
        mStatus = cStatus_Ready;
        return;
    }

    u32 written = 0;
    bool ok = handle.tryWrite(&written, mpSavedata, kSavedataSize);
    saveFileDevice()->tryClose(&handle);

    if (!ok || written != kSavedataSize)
        mWriteError = cWriteError_FileWriteFailed;
    else
        mWriteError = cWriteError_OK;

    if (_30 != -1)
        startWriteChallengeReplay(this, reinterpret_cast<void*>(_30));

    mButtonLayoutChanged = 0;
    _2d = 0;
    _2e = 0;
    _2f = 0;
    mStatus = cStatus_Ready;
}

// Address: 0x029D97BC
void SaveMgr::loadChallengeReplay()
{
    sead::FileHandle handle;
    sead::FormatFixedSafeString<64> path;
    path.format("save://rp_challenge_replay%02d.dat", _20);

    u8* buffer = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(MainGame::instance()) + 0x80);
    u32 max_size = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(MainGame::instance()) + 0x84);

    if (!saveFileDevice()->tryOpen(&handle, path, sead::FileDevice::cFileAccess_ReadOnly))
    {
        mReadError = cReadError_FileUnavailable;
        std::memset(buffer, 0, max_size);
        *reinterpret_cast<u32*>(reinterpret_cast<u8*>(MainGame::instance()) + 0x84) = 0;
        mStatus = cStatus_Ready;
        return;
    }

    u32 read_size = 0;
    bool ok = handle.tryRead(&read_size, buffer, kSavedataSize);
    saveFileDevice()->tryClose(&handle);

    if (!ok || read_size < 4 || checkSavedataBlockCrc(buffer, read_size) == 0)
    {
        mReadError = cReadError_FileUnavailable;
        std::memset(buffer, 0, max_size);
        *reinterpret_cast<u32*>(reinterpret_cast<u8*>(MainGame::instance()) + 0x84) = 0;
        mStatus = cStatus_Ready;
        return;
    }

    mReadError = cReadError_OK;
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(MainGame::instance()) + 0x84) = read_size;
    mStatus = cStatus_Ready;
}

// Address: 0x029D994C
void SaveMgr::saveChallengeReplay()
{
    sead::FileHandle handle;
    sead::FormatFixedSafeString<64> path;
    path.format("save://rp_challenge_replay%02d.dat", _20);

    u8* buffer = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(MainGame::instance()) + 0x80);
    u32 size = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(MainGame::instance()) + 0x84);

    writeSavedataBlockCrc(buffer, size);

    if (!saveFileDevice()->tryOpen(&handle, path, sead::FileDevice::cFileAccess_WriteOnly))
    {
        mWriteError = cWriteError_FileDeviceUnavailable;
        mStatus = cStatus_Ready;
        return;
    }

    u32 written = 0;
    bool ok = handle.tryWrite(&written, buffer, size);
    saveFileDevice()->tryClose(&handle);

    if (!ok || written != size)
        mWriteError = cWriteError_FileWriteFailed;
    else
        mWriteError = cWriteError_OK;

    mStatus = cStatus_Ready;
}

// Address: 0x029D9AD8
void SaveMgr::loadPersonalSavedata()
{
    sead::FileHandle handle;

    if (!saveFileDevice()->tryOpen(&handle, "save_act://rp_personal_savedata.dat", sead::FileDevice::cFileAccess_ReadOnly))
    {
        mReadError = cReadError_FileUnavailable;
        mStatus = cStatus_Ready;
        return;
    }

    u32 read_size = 0;
    bool ok = handle.tryRead(&read_size, mpPersonalSavedata, kPersonalSavedataSize);
    saveFileDevice()->tryClose(&handle);

    if (!ok || read_size != kPersonalSavedataSize || checkSavedataBlockCrc(mpPersonalSavedata, kPersonalSavedataSize) == 0)
    {
        std::memset(mpPersonalSavedata, 0, kPersonalSavedataSize);
        mReadError = cReadError_FileUnavailable;
        mStatus = cStatus_Ready;
        return;
    }

    mReadError = cReadError_OK;
    mStatus = cStatus_Ready;
}

// Address: 0x029D9CBC
void SaveMgr::savePersonalSavedata()
{
    sead::FileHandle handle;

    if (!saveFileDevice()->tryOpen(&handle, "save_act://rp_personal_savedata.dat", sead::FileDevice::cFileAccess_WriteOnly))
    {
        mWriteError = cWriteError_FileDeviceUnavailable;
        mStatus = cStatus_Ready;
        return;
    }

    u32 written = 0;
    bool ok = handle.tryWrite(&written, mpPersonalSavedata, kPersonalSavedataSize);
    saveFileDevice()->tryClose(&handle);

    if (!ok || written != kPersonalSavedataSize)
        mWriteError = cWriteError_FileWriteFailed;
    else
        mWriteError = cWriteError_OK;

    mStatus = cStatus_Ready;
}

// Address: 0x029D7200
void writeDefaultPlayerTable_(SaveMgr* mgr, u8* game_data)
{
    (void)mgr;

    static const u32 characters[cPlayerNum] = { 1, 2, 3, 4 };
    static const u32 colors[cPlayerNum] = { 1, 2, 3, 4 };

    for (u32 i = 0; i < cPlayerNum; ++i)
    {
        u8* entry = game_data + 0x14 + i * 0x48;
        *reinterpret_cast<u32*>(entry + 0x0C) = characters[i];
        *reinterpret_cast<u32*>(entry + 0x10) = colors[i];
    }
}

// The inline copy in the apply path: entry character ids 0..3
void writeIndexPlayerTable_(u8* game_data)
{
    static const u32 characters[cPlayerNum] = { 0, 1, 2, 3 };

    for (u32 i = 0; i < cPlayerNum; ++i)
    {
        u8* entry = game_data + 0x14 + i * 0x48;
        *reinterpret_cast<u32*>(entry + 0x0C) = characters[i];
    }
}

// Address: 0x029D727C
void sanitizeRecord_(SaveMgr* mgr, u8* record)
{
    (void)mgr;

    u8 used = record[0];
    if (used > 1)
        used = 1;
    record[0] = used;

    if (used == 0)
        return;

    u8 mode = record[1];
    if (mode == 0)
        mode = 1;
    else if (mode > 4)
        mode = 4;
    record[1] = mode;

    if (record[2] > 100)
        record[2] = 100;

    record[3] &= 1;

    if (record[4] > 8)
        record[4] = 8;

    if (record[5] > 1)
        record[5] = 1;

    record[0xA] &= 0x0F;

    if (record[0xB] > 2)
        record[0xB] = 2;

    for (u32 i = 0; i < 4; ++i)
    {
        if (record[0x0C + i] > 99)
            record[0x0C + i] = 99;

        if (record[0x10 + i] > 4)
            record[0x10 + i] = 4;

        record[0x14 + i] &= 0x8F;

        if (record[0x18 + i] > 99)
            record[0x18 + i] = 99;
    }

    record[0x1E] &= 0x1F;
    record[0x1F] &= 0x1F;

    *reinterpret_cast<u16*>(record + 0x1C) &= 0x1FF;
    *reinterpret_cast<u32*>(record + 0x2C) &= 0x3FFFF;

    if (*reinterpret_cast<u32*>(record + 0x30) > kMaxPlaySeconds)
        *reinterpret_cast<u32*>(record + 0x30) = kMaxPlaySeconds;

    if (*reinterpret_cast<u16*>(record + 0x44) > 9999)
        *reinterpret_cast<u16*>(record + 0x44) = 9999;

    s32 world = static_cast<s8>(record[0x49]);
    if (world < -1)
        world = -1;
    else if (world > 6)
        world = 6;
    record[0x49] = static_cast<u8>(world);

    s32 course = static_cast<s8>(record[0x4A]);
    if (course < -1)
        course = -1;
    else if (course > 6)
        course = 6;
    record[0x4A] = static_cast<u8>(course);

    if (record[0x4B] > 5)
        record[0x4B] = 5;

    if (record[0x4C] > 3)
        record[0x4C] = 3;

    if (record[0x4D] > 4)
        record[0x4D] = 4;

    if (record[0x4E] > 0xE)
        record[0x4E] = 0xE;

    record[0x4F] &= 3;

    if (record[0x51] > 1)
        record[0x51] = 1;

    if (record[0x52] > 99)
        record[0x52] = 99;

    if (record[0x53] > 99)
        record[0x53] = 99;

    if (record[0x54] > 1)
        record[0x54] = 1;

    if (record[0x56] > 4)
        record[0x56] = 4;

    if (record[0x57] > 1)
        record[0x57] = 1;

    if (record[0x58] > 1)
        record[0x58] = 1;

    if (record[0x59] > 1)
        record[0x59] = 1;

    if (record[0x5A] > 5)
        record[0x5A] = 5;

    if (record[0x5B] > 5)
        record[0x5B] = 5;

    for (u32 i = 0; i < 4; ++i)
    {
        if (record[0x5C + i] > 0x13)
            record[0x5C + i] = 0x13;

        if (record[0x60 + i] > 7)
            record[0x60 + i] = 7;
    }

    for (u32 i = 0; i < 7; ++i)
    {
        if (record[0x64 + i] > 3)
            record[0x64 + i] = 3;
    }

    for (u32 i = 0; i < 10; ++i)
    {
        if (record[0x14D + i] > 0xE)
            record[0x14D + i] = 0xE;
    }

    record[0x1FC] &= 0x7F;
}

// Address: 0x029D7184
// File-select course flags. Slot builders 0x02631B50, 0x0263AD40, and 0x02641974
// call this with the file index. The copy source is the main record
// (savedata+0x10+file*0x204): 123 course-flag bytes at record+0x6B and the
// 0x3E death counters after them, into *(0x101E7D7C)+0x31C. Those builders
// already take lives from the quick record at +0xC28 when its exists byte is
// set, and show 5 when that lives byte is 0. They do not read the
// world/section/node triple. The slot's map picture is these flags.
// After the call, lbz rd, 0x2E(savedata+file*0x204) reads main record+0x1E
// and the popcount is what the slot keeps. 0x02641974 pane index 6 stays on
// the main record even when quick exists. That pane is the file-copy preview:
// 0x029D83FC copies the main record and clears the destination quick record.
u32 copyFileSelectCourseFlags_(SaveMgr* mgr, u32 file, u8* out_bits)
{
    u8* course_flags = reinterpret_cast<u8*>(MainGame::instance()) + 0x31C;
    const u8* record = savedataAt(mgr, kSavedataMainRecordBase + file * kSavedataRecordSize);

    sub_024BBC48(course_flags, record + 0x6B);

    if (out_bits != nullptr)
        sub_024BB3B0(course_flags, out_bits);

    u32 count = sub_024BB344(course_flags);
    if (count > 99)
        count = 99;

    return count;
}

// Address: 0x024BBD24
void mergeCourseFlagBytes_(u8* dst, const u8* src0, const u8* src1, const u8* src2)
{
    for (u32 index = 0; index < 0x7B; ++index)
        dst[index] = src0[index] | src1[index] | src2[index];
}

// Address: 0x029CFB54
void syncCourseFlagsFromMainRecords_(u8* record0, u8* record1, u8* record2)
{
    MainGame* main_game = MainGame::instance();
    u8* main = reinterpret_cast<u8*>(main_game);

    mergeCourseFlagBytes_(main + 0x31C, record0 + 0x6B, record1 + 0x6B, record2 + 0x6B);
    resetMainGameCourseSelectBits_(main + 0x34);

    main[0x21DFA] = 0;
    main[0x21DFB] = 0;
    main[0x82E] = 0;
    main[0x82F] = 0;

    SubjectMgr::instance()->clearSubject();
}

// Address: 0x029D7CC8
void writeDefaultGameDataToRecords_(SaveMgr* mgr)
{
    u8* record0 = savedataAt(mgr, kSavedataMainRecordBase);
    u8* record1 = savedataAt(mgr, kSavedataMainRecordBase + kSavedataRecordSize);
    u8* record2 = savedataAt(mgr, kSavedataMainRecordBase + 2 * kSavedataRecordSize);

    FieldGame* field_game = FieldGame::instance();
    FieldGameData& game_data = field_game->getGameData();
    new (&game_data) FieldGameData();

    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(field_game) + 0x16C) = 4;

    resetCourseInfo_(CourseInfo::instance());
    syncCourseFlagsFromMainRecords_(record0, record1, record2);
}

// Address: 0x029D7648
void applySaveFile(SaveMgr* mgr, u8 file)
{
    SaveData* savedata = const_cast<SaveData*>(mgr->mpSavedata);
    u8* main_record = reinterpret_cast<u8*>(savedata) + kSavedataMainRecordBase + file * kSavedataRecordSize;
    u8* backup_record = reinterpret_cast<u8*>(savedata) + kSavedataBackupBase + file * kSavedataRecordSize;
    u8* record = backup_record[0] != 0 ? backup_record : main_record;

    resetCourseInfo_(CourseInfo::instance());

    FieldGameData& game_data = FieldGame::instance()->getGameData();

    if (record[0] == 0)
    {
        new (&game_data) FieldGameData();

        if (saveApplyFlag() != 0)
            writeDefaultPlayerTable_(mgr, reinterpret_cast<u8*>(&game_data));

        enterMapFromGameData_(&game_data, 1);
        resetMainGameRunState_();
        return;
    }

    sanitizeRecord_(mgr, record);

    new (&game_data) FieldGameData();
    enterMapFromGameData_(&game_data, record[1]);

    if (saveApplyFlag() == 0)
        writeIndexPlayerTable_(reinterpret_cast<u8*>(&game_data));
    else
        writeDefaultPlayerTable_(mgr, reinterpret_cast<u8*>(&game_data));

    applyPlayerEntryFlags_(&game_data, record[0x10], record[0x11], record[0x12], record[0x13]);

    for (u32 i = 0; i < 4; ++i)
    {
        FieldPlayerData& entry = game_data.getPlayerData(i);

        u32 lives = record[0x0C + i];
        if (lives > 99)
            lives = 99;
        entry.life_cnt = lives;

        entry.player_mode = static_cast<PlayerMode>(record[0x14 + i]);

        entry.has_star = false;

        entry._3c[0] = record[0x18 + i];
    }

    game_data.setCoinNum(record[0x02]);

    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&game_data) + 0x164) = record[0x4E];

    u32 play_seconds = *reinterpret_cast<const u32*>(record + 0x30);
    if (static_cast<s32>(play_seconds) < 0)
        play_seconds = 0;
    else if (play_seconds > kMaxPlaySeconds)
        play_seconds = kMaxPlaySeconds;

    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&game_data) + 0x138) = play_seconds;
    copyPlayerSetupToMainGame_(record, play_seconds);
}

// Address: 0x02987200
// Opening a file runs this before continueFirstSave. It calls
// SaveMgr::clearQuickAndWrite for header.last_file, then sets byte +0x9B3D.
// continueFirstSave only looks at the story record, so the quick save has to
// already be in that record or the next launch loads the older story.

// Address: 0x029872BC
void continueFirstSave(sead::TaskBase* src_task)
{
    SaveMgr* mgr = SaveMgr::instance();
    SaveData* savedata = const_cast<SaveData*>(mgr->mpSavedata);
    u8 file = savedata->header.last_file;
    u8* main_record = reinterpret_cast<u8*>(savedata) + kSavedataMainRecordBase + file * kSavedataRecordSize;
    bool has_record = main_record[0] != 0;

    sead::TaskClassID next_scene(reinterpret_cast<sead::TaskFactory>(
        has_record ? kCourseSelectTaskFactory : kMenuSceneWrapperFactory));
    Scene::instance()->setNextScene(src_task, next_scene);

    MainGame::instance()->setNewGame(!has_record);
    *reinterpret_cast<u16*>(kNewGameFlag) = 0;

    FieldGameData& game_data = FieldGame::instance()->getGameData();
    for (s32 i = 0; i < 4; ++i)
    {
        FieldPlayerData& entry = game_data.getPlayerData(i);
        if (entry.is_entry && entry.character == cPlayerCharacter_Mario)
            entry.player_mode = cPlayerMode_Normal;
    }

    *reinterpret_cast<u32*>(kSceneInstance + 0x6C) = 0x00020002;
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&game_data) + 0x14C) = 0;

    *reinterpret_cast<u8*>(reinterpret_cast<u8*>(MainGame::instance()) + 0x14) =
        *reinterpret_cast<const u8*>(*reinterpret_cast<const u32*>(kPlayLogSource) + kPlayLogDeviceByte);

    updateStoryPlayText_(mgr);
}
