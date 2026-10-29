#include <course_select/CourseSelectMgr.h>

// Address: 0x021F8A40
SEAD_SINGLETON_DISPOSER_IMPL(CourseSelectMgr)

// Address: 0x021F9FC8
CourseSelectNodePath* CourseSelectMgr::getPath(u32 world, u32 section)
{
    u32 idx = (world << 1) + section;
    CourseSelectNodePath** table = mPathTable0;

    if (idx < 0x12)
        table = &mPathTable0[idx];

    return *table;
}

// Address: 0x021F9FEC
CourseSelectNodePath* CourseSelectMgr::getMapPath(u32 world, u32 section)
{
    u32 idx = (world << 1) + section;
    CourseSelectNodePath** table = mPathTable1;

    if (idx < 0x12)
        table = &mPathTable1[idx];

    return *table;
}

// Address: 0x021C9464
CourseSelectPathNode* CourseSelectMgr::getNode(CourseSelectNodePath* path, u32 node_index)
{
    if (node_index >= 0x100)
        return nullptr;

    return &path->node[node_index];
}

// Address: 0x02242A84
CourseSelectPathNode* getPathNodeFromPose(const CourseSelectMapPose* pose)
{
    CourseSelectNodePath* path = CourseSelectMgr::instance()->getMapPath(
        static_cast<u32>(pose->world),
        static_cast<u32>(pose->section));

    return CourseSelectMgr::getNode(path, static_cast<u32>(pose->node_index));
}

// Address: 0x02242AC8
CourseSelectPathNode* getPathNodeFromPose_Thunk(const CourseSelectMapPose* pose)
{
    return getPathNodeFromPose(pose);
}

// Address: 0x02242C70
s32 getCourseSlotFromPose(const CourseSelectMapPose* pose)
{
    CourseSelectPathNode* node = getPathNodeFromPose(pose);

    if (node == nullptr)
        return -1;

    return node->course_slot;
}

// Address: 0x02242AD0
sead::Vector3f* getPathNodePosFromPose(const CourseSelectMapPose* pose)
{
    CourseSelectPathNode* node = getPathNodeFromPose_Thunk(pose);
    return &node->pos;
}

// Address: 0x021F8590
CourseSelectMgr::CourseSelectMgr()
    : mChangePlayerNum()
{
    _10[0] = static_cast<u32>(-1);
    _10[1] = 0;

    for (s32 i = 0; i < 0x12; i++)
        mPathTable0[i] = nullptr;

    for (s32 i = 0; i < 0x12; i++)
        mPathTable1[i] = nullptr;

    u8* p_this = reinterpret_cast<u8*>(this);

    for (s32 i = 0; i < 9; i++)
        *reinterpret_cast<u32*>(p_this + 0xA8 + i * 4) = 0;

    for (s32 i = 0; i < 9; i++)
    {
        *reinterpret_cast<u32*>(p_this + 0x91C + i * 8) = 5;
        *reinterpret_cast<u32*>(p_this + 0x920 + i * 8) = 5;
    }

    for (s32 i = 0; i < 0xD8 / 4; i++)
    {
        *reinterpret_cast<u32*>(p_this + 0x964 + i * 4) = 0;
        *reinterpret_cast<u32*>(p_this + 0xA3C + i * 4) = 0;
    }

    for (s32 i = 0; i < 9; i++)
    {
        *reinterpret_cast<u32*>(p_this + 0x47788 + i * 0xC) = 0;
        *reinterpret_cast<u32*>(p_this + 0x4778C + i * 0xC) = 0;
        *reinterpret_cast<u32*>(p_this + 0x47790 + i * 0xC) = 0;
    }

    for (s32 i = 0; i < 9; i++)
        p_this[0x477F4 + i] = static_cast<u8>(i);

    for (s32 i = 0; i < 0x1E; i++)
        *reinterpret_cast<u32*>(p_this + 0x40438 + i * 4) = 0;
}

// Address: 0x021F8AC4
CourseSelectMgr::~CourseSelectMgr()
{
}
