#include <game/FieldGame.h>

#include <player/PlayerEnum.h>

// Address: 0x024BAC50
CheckPointData::CheckPointData()
{
    index = -1;
    world_no = 0;
    course_no = 0;
    file_no = 0;
    next_goto_no = 0;
    _08 = 0;

    for (s32 i = 0; i < 2; ++i)
        _0c[i] = 0xFF;

    for (s32 i = 0; i < 2; ++i)
        _0e[i] = 0xFF;

    for (s32 i = 0; i < 3; ++i)
        banner_digit[i] = 0xFF;
}

// Address: 0x024BADCC
s32 FieldGameData::checkEnterCourse(u8 world_no, u8 course_no) const
{
    if (world_no != mCheckPoint.world_no)
        return -1;

    if (course_no != mCheckPoint.course_no)
        return -1;

    return mCheckPoint.index;
}

// Address: 0x024BADF4
void FieldGameData::setCheckPoint(s32 index, u8 set_0c, u8 set_0e, u32 set_08,
                                  u8 world_no, u8 course_no, u8 file_no, u8 next_goto_no,
                                  const u8* banner_digit)
{
    mCheckPoint.index = index;

    if (index < 2)
        mCheckPoint._0c[index] = set_0c;
    else
        mCheckPoint._0c[0] = set_0c;

    if (index < 2)
        mCheckPoint._0e[index] = set_0e;
    else
        mCheckPoint._0e[0] = set_0e;

    mCheckPoint.file_no = file_no;
    mCheckPoint._08 = set_08;
    mCheckPoint.course_no = course_no;
    mCheckPoint.next_goto_no = next_goto_no;
    mCheckPoint.world_no = world_no;

    for (s32 i = 0; i < 3; ++i)
        mCheckPoint.banner_digit[i] = banner_digit[i];
}

// Address: 0x024BAD4C
void FieldGameData::clearCheckPoint()
{
    mCheckPoint.index = -1;
    mCheckPoint.world_no = 0;
    mCheckPoint.course_no = 0;
    mCheckPoint.file_no = 0;
    mCheckPoint.next_goto_no = 0;
    mCheckPoint._08 = 0;

    for (s32 i = 0; i < 2; ++i)
        mCheckPoint._0c[i] = 0xFF;

    for (s32 i = 0; i < 2; ++i)
        mCheckPoint._0e[i] = 0xFF;

    for (s32 i = 0; i < 3; ++i)
        mCheckPoint.banner_digit[i] = 0xFF;
}

// Address: 0x024C1B04
FieldGameData::FieldGameData()
{
    mCheckPoint = CheckPointData();

    for (s32 i = 0; i < cPlayerNum; ++i)
        new (&mPlayerData[i]) FieldPlayerData();

    mCoinNum = 0;
    _138 = 0;
    _13c = 1;
    _140 = 0;
    _160[0] = 0;
    _160[0x8] = 0;
    _160[0x9] = 0;
    _160[0xB] = 0;
    _16c = -1;
    _16e[0] = 0xFF;
    _16e[1] = 0xFF;
    _16e[3] = 3;
    _1a0 = -1;
    _1a2 = -1;
    _1a4 = 0;
    _1a5 = 0;
    mGameMode = FieldGameMode();

    mStatsData = InfoStats();
    _1e0 = false;
}

// Address: 0x024C27B8
FieldPlayerData::FieldPlayerData()
{
    is_entry = false;
    life_cnt = 5;
    player_mode = cPlayerMode_Small;
    _14 = 0;
    mii_slot = Mii::SlotID();
    mii_cap_visible = false;
    is_connected = false;
    has_star = false;
    _1f = 0;

    fukidashi_flag.reset();

    _28[2] = 0;
    coin_cnt = 0;
    enemy_down_cnt = 0;
    _3c[0] = 0;
    _3c[1] = 0;
}

// Address: 0x024C1DF0
s32 FieldGameData::searchEntryByCharacter(u32 character) const
{
    for (s32 i = 0; i < cPlayerNum; ++i)
        if (mPlayerData[i].character == character)
            return i;

    return -1;
}

// Address: 0x024C1E38
FieldPlayerData* FieldGameData::getEntryByCharacter(u32 character)
{
    s32 index = searchEntryByCharacter(character);

    if (index < 0)
        return nullptr;

    if (static_cast<u32>(index) < cPlayerNum)
        return &mPlayerData[index];

    return &mPlayerData[0];
}

// Address: 0x024C1E8C
void FieldGameData::swapEntries(s32 index_a, s32 index_b)
{
    u32 a = static_cast<u32>(index_a) < cPlayerNum ? static_cast<u32>(index_a) : 0;
    u32 b = static_cast<u32>(index_b) < cPlayerNum ? static_cast<u32>(index_b) : 0;

    FieldPlayerData temp = mPlayerData[a];
    mPlayerData[a] = mPlayerData[b];
    mPlayerData[b] = temp;
}

// Address: 0x024C1F50
void FieldGameData::orderEntriesByCharacter(const u32* characters)
{
    for (s32 i = 0; i < cPlayerNum; ++i)
    {
        s32 index = searchEntryByCharacter(characters[i]);
        swapEntries(i, index);
    }
}

// Address: 0x024C203C
void FieldGameData::setCoinNum(s32 num)
{
    mCoinNum = num;
}

// Address: 0x024C2044
void FieldGameData::resetEntriesUnk30()
{
    for (s32 i = 0; i < cPlayerNum; ++i)
        mPlayerData[i]._28[2] = 0;
}

// Address: 0x024C2088
void FieldGameData::resetEntriesUnk2C()
{
    for (s32 i = 0; i < cPlayerNum; ++i)
        mPlayerData[i]._28[1] = 0;
}

// Address: 0x024C20CC
s32 FieldGameData::searchEnteredEntryByCharacter(u32 character) const
{
    for (s32 i = 0; i < cPlayerNum; ++i)
        if (mPlayerData[i].is_entry && mPlayerData[i].character == character)
            return i;

    return -1;
}

// Address: 0x024C2148
s32 FieldGameData::getEnteredEntryNum() const
{
    s32 count = 0;

    for (s32 i = 0; i < cPlayerNum; ++i)
        if (mPlayerData[i].is_entry)
            ++count;

    return count;
}
