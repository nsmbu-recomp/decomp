#include <course_select/CourseSelectActor.h>

#include <actor/ActorCreateParam.h>

// Address: 0x021B2310
CourseSelectActor::CourseSelectActor(const ActorCreateParam& param)
    : ActorBase(param)
    , mPos(param.position)
    , mPosPrev(param.position)
    , mScale(1.0f, 1.0f, 1.0f)
    , _74(1.0f, 1.0f, 1.0f)
    , _80(0.0f, 0.0f, 0.0f)
    , _8c(0.0f, 0.0f, 0.0f)
    , _98(0.0f, 0.0f, 0.0f)
    , _a4(0.0f, 0.0f, 0.0f)
    , _b0(0)
    , _b4(static_cast<u32>(s32(param.angle)))
    , _b8(0)
    , _bc(0)
    , _c0(static_cast<u32>(s32(param.angle)))
    , _c4(0)
    , _c8(0.0f)
    , _cc(0.0f)
    , _d0(1.0f)
    , _d4(0.0f)
    , _d8(0)
{
}

// Address: 0x021B2528
bool CourseSelectActor::preExecute_()
{
    return true;
}

// Address: 0x021B2658
bool CourseSelectActor::preDraw_()
{
    return true;
}
