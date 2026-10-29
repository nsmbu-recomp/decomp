#include <game/AreaTask.h>

#include <game_info/CourseInfo.h>
#include <graphics/Renderer.h>
#include <map/CourseData.h>
#include <system/MainGame.h>

namespace {

// Address: 0x02A0A2FC
extern void sub_02A0A2FC(void* self);

// Address: 0x02A2587C
extern void sub_02A2587C(void* self);

// Address: 0x02A25C60
extern void sub_02A25C60(void* self, s32 mode);

// Address: 0x02A3760C
extern void sub_02A3760C(void* self, s32 mode);

// Address: 0x02AFA350
extern void sub_02AFA350(void* self, s32 count, s32 size, u32 ctor);

// Address: 0x02AFA490
extern void sub_02AFA490(void* self, s32 count, s32 size, u32 dtor, s32 a, s32 b);

template<typename T>
T& fieldAt(u8* base, u32 offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

void constructEventSlot(u8* object)
{
    sub_02A0A2FC(object);

    fieldAt<u32>(object, 0x0C) = 0x101B8054;
    fieldAt<u32>(object, 0x10) = 0x100B6884;
    fieldAt<u32>(object, 0x14) = 0x100B6750;
    fieldAt<u32>(object, 0x18) = 0;
    fieldAt<u32>(object, 0x1C) = 0;
    fieldAt<u32>(object, 0x20) = 0x100B6738;
    fieldAt<u32>(object, 0x24) = 0;
    fieldAt<u16>(object, 0x28) = 0;
    fieldAt<u16>(object, 0x2A) = 0;
    fieldAt<u32>(object, 0x2C) = 0;
    fieldAt<u32>(object, 0x20) = 0x100B6800;
}

} // namespace

SEAD_TASK_SINGLETON_DISPOSER_IMPL(AreaTask)

// Address: 0x024B5398
AreaTask::AreaTask(const sead::TaskConstructArg& arg)
    : sead::CalculateTask(arg, "AreaTask")
{
    u8* self = reinterpret_cast<u8*>(this);

    fieldAt<u8>(self, 0xC8) = 0;
    _c9 = 1;
    mIsPrepareDone = false;
    mIsDrawMethodReady = false;

    sub_02AFA350(self + 0xCC, 4, 0x194, 0x02514150);

    fieldAt<u32>(self, 0x734) = 0;
    fieldAt<u32>(self, 0x738) = 0;
    fieldAt<u32>(self, 0x73C) = 0;
    mLoopType = 0;
    mCourseOutKind = 0;
    fieldAt<u8>(self, 0x740) = 0;
    fieldAt<u8>(self, 0x741) = 0;
    fieldAt<u32>(self, 0x744) = 0xFFFFFFFF;
    fieldAt<u8>(self, 0x748) = 0;

    for (u32 offset = 0x749; offset < 0x74D; ++offset)
        fieldAt<u8>(self, offset) = 0;

    mIsCourseOut = false;
    fieldAt<u8>(self, 0x74E) = 0;
    fieldAt<u32>(self, 0x750) = 0;
    fieldAt<u8>(self, 0x754) = 0;

    for (u32 i = 0; i < 11; ++i)
        constructEventSlot(self + 0x758 + i * 0x40);

    fieldAt<u32>(self, 0xA18) = 0x100B64F8;
    sub_02A2587C(self + 0xA1C);

    fieldAt<u32>(self, 0xBBC) = 0x100B64D0;
    fieldAt<f32>(self, 0xBC0) = 640.0f;
    fieldAt<f32>(self, 0xBC4) = 360.0f;
    fieldAt<f32>(self, 0xBC8) = 13.6f;
    fieldAt<f32>(self, 0xBCC) = 6.0f;
    fieldAt<u8>(self, 0xBD0) = 0;
    fieldAt<f32>(self, 0xBD4) = 16.0f;
    fieldAt<u32>(self, 0xBD8) = 0;
    fieldAt<u32>(self, 0xBDC) = 0;
    fieldAt<u32>(self, 0xBE0) = 0;
}

// Address: 0x024B5254
AreaTask::~AreaTask()
{
    u8* self = reinterpret_cast<u8*>(this);

    sub_02A25C60(self + 0xA1C, 2);

    for (u32 i = 0; i < 11; ++i)
        sub_02A3760C(self + 0xA18 - 0x40 - i * 0x40, 2);

    sub_02AFA490(self + 0xCC, 4, 0x194, 0x025140E8, 0, 0);
}

// Address: 0x024B5B20
void AreaTask::setLoopType()
{
    CourseDataFile* p_file = CourseData::instance()->getFile(CourseInfo::instance()->getFileNo());
    const u8* p_options = static_cast<const u8*>(p_file->getBlock(CourseDataFile::cBlock_Options));

    mLoopType = 0;
    if ((*reinterpret_cast<const u16*>(p_options + 8) & 1) != 0)
        mLoopType = 1;
}

// Address: 0x024B8034
f32 AreaTask::getLoopPosX(f32 x)
{
    if (mLoopType == 0)
        return x;

    const f32 min_x = mBound.min.x;
    const f32 max_x = mBound.max.x;
    const f32 range = max_x - min_x;

    for (;;)
    {
        while (x < min_x)
            x += range;

        if (x < max_x)
            break;

        x -= range;
    }

    return x;
}

// Address: 0x024B84A8
void AreaTask::drawLayer3D(const agl::lyr::RenderInfo& render_info)
{
    if (render_info.getRenderStep() != 3)
        return;

    u8* p_lyr_renderer = *reinterpret_cast<u8**>(reinterpret_cast<uintptr_t>(0x101F8A2C));
    s32 layer_num = *reinterpret_cast<s32*>(p_lyr_renderer + 0xDA8);
    agl::lyr::Layer** pp_layers = *reinterpret_cast<agl::lyr::Layer***>(p_lyr_renderer + 0xDAC);
    agl::lyr::Layer* p_layer = pp_layers[layer_num > 9 ? 9 : 0];

    Renderer::instance()->setLayer(p_layer, Renderer::cRenderPassType_Course);
    Renderer::instance()->resetLayer();
}

// Address: 0x024B8B98
void AreaTask::debugDraw(const agl::lyr::RenderInfo& render_info)
{
}

// Address: 0x024B814C
void AreaTask::setCourseOut()
{
    mIsCourseOut = true;
}

// Address: 0x024B77DC
void AreaTask::requestCourseOut_Normal()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_Normal);
    mCourseOutKind = 3;
}

// Address: 0x024B7808
void AreaTask::requestCourseOut_Down()
{
    MainGame::instance()->setCourseExitType(cCourseExitType_Dead);
    mCourseOutKind = 2;
}

// Address: 0x024B8158
void AreaTask::requestCourseOut_Secret()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_Secret);
    mCourseOutKind = 3;
}

// Address: 0x024B8184
void AreaTask::requestCourseOut_TreasureChest()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_TreasureChest);
    mCourseOutKind = 3;
}

// Address: 0x024B81AC
void AreaTask::requestCourseOut_TreasureChestFail()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_TreasureChestFail);
    mCourseOutKind = 3;
}

// Address: 0x024B81D4
void AreaTask::requestCourseOut_Subject()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_Normal);
    mCourseOutKind = 2;
}

// Address: 0x024B82A4
void AreaTask::prepareCourseOutEnter(u32 next_goto_key)
{
    CourseInfo* course_info = CourseInfo::instance();
    u8 search_id = static_cast<u8>(((next_goto_key << 1) & 0xFFFFFFFEu) + 1);

    // f_024B81FC(search_id, &file_no) walks CourseData files for NextGoto.id == search_id.
    (void)search_id;

    course_info->setNextWorldNo(course_info->getWorldNo());
    course_info->setNextCourseNo(course_info->getCourseNo());
    // setNextFileNo / setNextNextGotoNo from the looked-up NextGoto
    course_info->setNextSceneEnterType(3);

    // site 0x024B8324
    setCourseOut();
}

// Address: 0x025CF020
u32 AreaTask::getCourseOutHudFlag()
{
    return instance()->mCourseOutKind != 1u;
}
