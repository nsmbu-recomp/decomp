#include <course_select/CourseSelectObjActor.h>

#include <actor/ActorCreateParam.h>

// Address: 0x02201C04
CourseSelectObjActor::CourseSelectObjActor(const ActorCreateParam& param)
    : CourseSelectDemoActor(param)
    , _108(-1)
{
}

// Address: 0x02201C70
s32 CourseSelectObjActor::queryMapStateA()
{
    return decomp::call<s32>(0x021F0008u, mMapWorld, mMapCourseSlot, mMapParamF8);
}

// Address: 0x02201C84
s32 CourseSelectObjActor::queryMapStateB()
{
    return decomp::call<s32>(0x021F07A8u, mMapWorld, mMapCourseSlot);
}

// Address: 0x02201C90
s32 CourseSelectObjActor::queryMapStateC()
{
    return decomp::call<s32>(0x021F05BCu, mMapWorld, mMapCourseSlot);
}