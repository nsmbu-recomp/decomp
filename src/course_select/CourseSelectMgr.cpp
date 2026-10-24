#include <course_select/CourseSelectMgr.h>

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
