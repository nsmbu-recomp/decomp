#include <course_select/CourseSelectDemoActor.h>

#include <actor/ActorCreateParam.h>

// Address: 0x021D4CB4
CourseSelectDemoActor::CourseSelectDemoActor(const ActorCreateParam& param)
    : CourseSelectActor(param)
{
    _dc = 0.0f;
    _e0 = 0.0f;
    _e4 = 0.0f;
    _e8 = 0;
    _ec = 0;

    // Bytes from ActorCreateParam (gen 0x021D4D1C):
    //   +0x22 → +0xF0, +0x20 → +0xF4, +0x24 → +0xF8, +0x21 → +0xFC
    mMapWorld = param.param_ex_0.course_select.world;
    mMapSection = param.param_ex_0.course_select.world_sub_letter;
    mMapParamF8 = param.param_ex_1.course_select.node_id;
    mMapCourseSlot = param.param_ex_0.course_select._1;
    _100 = 0;
    _104 = 0;
}
