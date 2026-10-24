#include <course_select/CourseSelectObjActor.h>

// Address: 0x02201C04
CourseSelectObjActor::CourseSelectObjActor(const ActorCreateParam& param)
    : CourseSelectDemoActor(param)
    , _108(-1)
{
}

// Address: 0x02201C70
s32 CourseSelectObjActor::queryMapStateA()
{
    // Tail-calls f_021F0008(mMapWorld, mMapCourseSlot, mMapParamF8)
    return 0;
}

// Address: 0x02201C84
s32 CourseSelectObjActor::queryMapStateB()
{
    // Tail-calls f_021F07A8(mMapWorld, mMapCourseSlot)
    return 0;
}

// Address: 0x02201C90
s32 CourseSelectObjActor::queryMapStateC()
{
    // Tail-calls f_021F05BC(mMapWorld, mMapCourseSlot)
    return 0;
}

// Address: 0x02201C9C
s32 CourseSelectObjActor::queryMapStateD()
{
    // Tail-calls f_021F0690(mMapWorld, mMapCourseSlot)
    return 0;
}

// Address: 0x02201CA8
s32 CourseSelectObjActor::queryMapStateE()
{
    // Tail-calls f_021F071C(mMapWorld, mMapCourseSlot)
    return 0;
}
