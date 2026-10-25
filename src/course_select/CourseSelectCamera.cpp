#include <course_select/CourseSelectCamera.h>

#include <system/MainGame.h>

namespace {

constexpr s32 kFocusPathWorld = 4;
constexpr s32 kFocusPathSection = 1;

constexpr f32 kFocusSetupFloatD4 = 350.0f;
constexpr f32 kFocusCameraHeightOffset = 180.0f;

constexpr u32 kFocusSearchFiltersPhase0[] = {
    0x00080000, 0x00200000, 0x00200000, 0x02000000,
    0x02000000, 0x00800000, 0x00800000, 0x00080000,
};

constexpr u32 kFocusSearchFiltersPhase1[] = {
    0x00100000, 0x01000000, 0x01000000, 0x00400000,
    0x00400000, 0x00400000, 0x00400000, 0x00100000,
};

constexpr u32 kFocusSearchFiltersPhase2[] = {
    0x00800000, 0x02000000, 0x02000000, 0x00200000,
    0x00200000, 0x00080000, 0x00080000, 0x00800000,
};

static u32 getPathNodeCount_(CourseSelectNodePath* path)
{
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(path) + 0x1C);
}

// Address: 0x021D2B34
static s32 testPathNodeFlag_(CourseSelectNodePath* path, u32 node_index, u32 mask)
{
    if (node_index >= 0x100)
        return 0;

    u8* entry = reinterpret_cast<u8*>(path) + 0x28 + node_index * 0x448;
    s32 flags = *reinterpret_cast<s32*>(entry + 0x430);
    return flags & mask;
}

// Address: 0x021D6490
static void setMapCourseTitle_(void* title_state, s32 map_world, s32 focus_phase, void* focus_fields)
{
    (void)title_state;
    (void)map_world;
    (void)focus_phase;
    (void)focus_fields;
}

} // namespace

// Address: 0x0227C858
CourseSelectPathNode* CourseSelectCamera::findFocusPathNode(u32 search_mode)
{
    CourseSelectMapPose pose;
    pose.world = kFocusPathWorld;
    pose.section = kFocusPathSection;
    pose.node_index = -1;

    u32 filter = 0;
    const u8 player_count = *reinterpret_cast<const u8*>(
        reinterpret_cast<const u8*>(MainGame::instance()) + 0x822);

    if (mFocusSearchPhase < 1) {
        u32 table_index = (static_cast<u32>(player_count) << 1) + search_mode;
        filter = kFocusSearchFiltersPhase0[table_index];
    } else if (mFocusSearchPhase == 1) {
        u32 table_index = (static_cast<u32>(player_count) << 1) + search_mode;
        filter = kFocusSearchFiltersPhase1[table_index];
    } else if (mFocusSearchPhase == 2) {
        u32 table_index = (static_cast<u32>(player_count) << 1) + search_mode;
        filter = kFocusSearchFiltersPhase2[table_index];
    } else {
        filter = 0;
    }

    CourseSelectNodePath* path = CourseSelectMgr::instance()->getPath(
        static_cast<u32>(pose.world), static_cast<u32>(pose.section));

    if (path != nullptr) {
        u32 node_count = getPathNodeCount_(path);
        for (u32 node_index = 0; node_index < node_count; node_index++) {
            if (testPathNodeFlag_(path, node_index, filter) != 0) {
                pose.node_index = static_cast<s32>(node_index);
                return getPathNodeFromPose_Thunk(&pose);
            }
        }
    }

    return getPathNodeFromPose_Thunk(&pose);
}

// Address: 0x0227CA20
CourseSelectPathNode* CourseSelectCamera::findFocusPathNodeCheckpoint()
{
    return findFocusPathNode(0);
}

// Address: 0x0227D310
CourseSelectPathNode* CourseSelectCamera::findFocusPathNodeCourse()
{
    return findFocusPathNode(1);
}

void CourseSelectCamera::copyFocusFromNode_(const CourseSelectPathNode* node, bool copy_view)
{
    mFocusWorldTag = node->_50;
    mFocusCourseTag = node->_54;
    mFocusNodeIndex = node->node_index;
    mMapParamF8 = node->node_index;

    if (copy_view) {
        mPos = node->pos;
        mFocusViewY = node->pos.y + kFocusCameraHeightOffset;
    }

    updateMapTitle_();
}

void CourseSelectCamera::updateMapTitle_()
{
    MainGame* game = MainGame::instance();
    void* title_state = reinterpret_cast<u8*>(game) + 0x3D8;
    setMapCourseTitle_(title_state, mMapWorld, mFocusSearchPhase, &mFocusWorldTag);
}

// Address: 0x0227CCDC
void CourseSelectCamera::applyFocusFromCheckpointSearch()
{
    _d4 = kFocusSetupFloatD4;
    CourseSelectPathNode* node = findFocusPathNodeCheckpoint();
    copyFocusFromNode_(node, true);
}

// Address: 0x0227E284
void CourseSelectCamera::applyFocusFromCourseSearch()
{
    CourseSelectPathNode* node = findFocusPathNodeCourse();
    copyFocusFromNode_(node, false);
}
