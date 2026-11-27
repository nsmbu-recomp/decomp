#include <game_info/CourseInfo.h>

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