#include <course_select/CourseSelectNodeType.h>

// Address: 0x021EF6C8
CourseSelectNodeType getCourseSelectNodeType(s32 course_slot)
{
    if (course_slot < 0)
        return cCourseSelectNodeType_Other;

    u8 id = static_cast<u8>(course_slot);

    if (decomp::call<bool>(0x024D37A0u, id))
        return cCourseSelectNodeType_Course;

    if (decomp::call<bool>(0x024D37D4u, id))
        return cCourseSelectNodeType_1;

    if (decomp::call<bool>(0x024D37ECu, id))
        return cCourseSelectNodeType_2;

    if (decomp::call<bool>(0x024D380Cu, id))
        return cCourseSelectNodeType_3;

    if (decomp::call<bool>(0x024D382Cu, id))
        return cCourseSelectNodeType_7;

    if (decomp::call<bool>(0x024D384Cu, id))
        return cCourseSelectNodeType_4;

    if (decomp::call<bool>(0x024D37B8u, id))
        return cCourseSelectNodeType_8;

    if (decomp::call<bool>(0x024D38BCu, id))
        return cCourseSelectNodeType_9;

    if (course_slot == 37)
        return cCourseSelectNodeType_6;

    if (course_slot == 36)
        return cCourseSelectNodeType_5;

    return cCourseSelectNodeType_Other;
}