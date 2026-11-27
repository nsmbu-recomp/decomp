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

    mMapWorld = param.param_ex_0.course_select.world;
    mMapSection = param.param_ex_0.course_select.world_sub_letter;
    mMapParamF8 = param.param_ex_1.course_select.node_id;
    mMapCourseSlot = param.param_ex_0.course_select._1;
    _100 = 0;
    _104 = 0;

    decomp::call<void>(0x02002CE0u, this, &param);
    decomp::be32(reinterpret_cast<u8*>(this) + 0x4Cu) = 0x10044AE0u;
}