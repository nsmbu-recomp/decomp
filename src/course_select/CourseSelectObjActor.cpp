#include <course_select/CourseSelectObjActor.h>

#include <actor/ActorCreateParam.h>

// Address: 0x02201C04
CourseSelectObjActor::CourseSelectObjActor(const ActorCreateParam& param)
    : CourseSelectDemoActor(param)
    , _108(-1)
{
}