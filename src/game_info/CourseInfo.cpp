#include <game_info/CourseInfo.h>

SEAD_SINGLETON_DISPOSER_IMPL(CourseInfo)

// Address: 0x024D281C
CourseInfo::CourseInfo()
{
    mCourseNow.world_no = 0;
    mCourseNow.course_no = 0;
    mCourseNow.file_no = 0;
    mCourseNow.next_goto_no = 0xFF;
    mCourseNow.area_no = 0;
    mCourseNow.scene_enter_type = 0;

    mCourseNext.world_no = 0;
    mCourseNext.course_no = 0;
    mCourseNext.file_no = 0;
    mCourseNext.next_goto_no = 0xFF;
    mCourseNext.scene_enter_type = 0;
}

// Address: 0x024D293C
void CourseInfo::setNextWorldNo(u8 world_no)
{
    if (mCourseNext.world_no != mCourseNow.world_no)
        return;

    mCourseNext.world_no = world_no;
}

// Address: 0x024D2954
void CourseInfo::setNextCourseNo(u8 course_no)
{
    if (mCourseNext.course_no != mCourseNow.course_no)
        return;

    mCourseNext.course_no = course_no;
}

// Address: 0x024D296C
void CourseInfo::setNextFileNo(u8 file_no)
{
    if (mCourseNext.file_no != mCourseNow.file_no)
        return;

    mCourseNext.file_no = file_no;
}

// Address: 0x024D2984
void CourseInfo::setNextNextGotoNo(u8 next_goto_no)
{
    if (mCourseNext.next_goto_no != mCourseNow.next_goto_no)
        return;

    mCourseNext.next_goto_no = next_goto_no;
}

// Address: 0x024D299C
void CourseInfo::setNextSceneEnterType(u32 type)
{
    mCourseNext.scene_enter_type = type;
}

// Address: 0x024D3910
bool CourseInfo::isShortPlay() const
{
    return mCourseNow.course_no == 0x2D;
}

// Address: 0x024D3928
bool CourseInfo::isStaffCredit() const
{
    return mCourseNow.course_no == 0x28;
}

// Address: 0x024D3940
bool CourseInfo::isTitle() const
{
    return mCourseNow.course_no == 0x26;
}
