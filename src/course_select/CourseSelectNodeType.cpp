#include <course_select/CourseSelectNodeType.h>

// Address: 0x024D37A0
bool isCourseSelectCourseNo(u8 course_no)
{
    return course_no <= 11;
}

// Address: 0x024D37D4
bool isCourseSelectCourseNo_19(u8 course_no)
{
    return course_no == 19;
}

// Address: 0x024D37EC
bool isCourseSelectCourseNo_20_21(u8 course_no)
{
    return course_no == 20 || course_no == 21;
}

// Address: 0x024D380C
bool isCourseSelectCourseNo_22_23(u8 course_no)
{
    return course_no == 22 || course_no == 23;
}

// Address: 0x024D382C
bool isCourseSelectCourseNo_41_42(u8 course_no)
{
    return course_no == 41 || course_no == 42;
}

// Address: 0x024D384C
bool isCourseSelectCourseNo_Type4(u8 course_no)
{
    if (static_cast<u32>(course_no - 24) < 7)
        return true;

    if (static_cast<u32>(course_no - 46) < 8)
        return true;

    return false;
}

// Address: 0x024D37B8
bool isCourseSelectCourseNo_Type8(u8 course_no)
{
    return static_cast<u32>(course_no - 12) < 5;
}

// Address: 0x024D38BC
bool isCourseSelectCourseNo_Type9(u8 course_no)
{
    if (static_cast<u32>(course_no - 31) < 3)
        return true;

    if (static_cast<u32>(course_no - 54) < 3)
        return true;

    return false;
}

// Address: 0x021EF6C8
CourseSelectNodeType getCourseSelectNodeType(s32 course_slot)
{
    if (course_slot < 0)
        return cCourseSelectNodeType_Other;

    u8 id = static_cast<u8>(course_slot);

    if (isCourseSelectCourseNo(id))
        return cCourseSelectNodeType_Course;

    if (isCourseSelectCourseNo_19(id))
        return cCourseSelectNodeType_1;

    if (isCourseSelectCourseNo_20_21(id))
        return cCourseSelectNodeType_2;

    if (isCourseSelectCourseNo_22_23(id))
        return cCourseSelectNodeType_3;

    if (isCourseSelectCourseNo_41_42(id))
        return cCourseSelectNodeType_7;

    if (isCourseSelectCourseNo_Type4(id))
        return cCourseSelectNodeType_4;

    if (isCourseSelectCourseNo_Type8(id))
        return cCourseSelectNodeType_8;

    if (isCourseSelectCourseNo_Type9(id))
        return cCourseSelectNodeType_9;

    if (course_slot == 37)
        return cCourseSelectNodeType_6;

    if (course_slot == 36)
        return cCourseSelectNodeType_5;

    return cCourseSelectNodeType_Other;
}
