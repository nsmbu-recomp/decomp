#include <game/AreaTask.h>

#include <game_info/CourseInfo.h>
#include <system/MainGame.h>

AreaTask* AreaTask::sInstance = nullptr;

// Address: 0x024B814C
void AreaTask::setCourseOut()
{
    mIsCourseOut = true;
}

// Address: 0x024B77DC
void AreaTask::requestCourseOut_Normal()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_Normal);
    mCourseOutKind = 3;
}

// Address: 0x024B7808
void AreaTask::requestCourseOut_Down()
{
    MainGame::instance()->setCourseExitType(cCourseExitType_Dead);
    mCourseOutKind = 2;
}

// Address: 0x024B8158
void AreaTask::requestCourseOut_Secret()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_Secret);
    mCourseOutKind = 3;
}

// Address: 0x024B8184
void AreaTask::requestCourseOut_TreasureChest()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_TreasureChest);
    mCourseOutKind = 3;
}

// Address: 0x024B81AC
void AreaTask::requestCourseOut_TreasureChestFail()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_TreasureChestFail);
    mCourseOutKind = 3;
}

// Address: 0x024B81D4
void AreaTask::requestCourseOut_Subject()
{
    MainGame* main_game = MainGame::instance();
    main_game->setCourseExitType(cCourseExitType_Beaten);
    main_game->setCourseBeatenType(cCourseBeatenType_Normal);
    mCourseOutKind = 2;
}

// Address: 0x024B82A4
void AreaTask::prepareCourseOutEnter(u32 next_goto_key)
{
    CourseInfo* course_info = CourseInfo::instance();
    u8 search_id = static_cast<u8>(((next_goto_key << 1) & 0xFFFFFFFEu) + 1);

    // f_024B81FC(search_id, &file_no) walks CourseData files for NextGoto.id == search_id.
    (void)search_id;

    course_info->setNextWorldNo(course_info->getWorldNo());
    course_info->setNextCourseNo(course_info->getCourseNo());
    // setNextFileNo / setNextNextGotoNo from the looked-up NextGoto
    course_info->setNextSceneEnterType(3);

    // site 0x024B8324
    setCourseOut();
}

// Address: 0x025CF020
u32 AreaTask::getCourseOutHudFlag()
{
    return instance()->mCourseOutKind != 1u;
}
