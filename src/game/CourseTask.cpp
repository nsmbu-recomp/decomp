#include <game/CourseTask.h>

#include <game/BalloonMgr.h>
#include <game/CoinBattleScoreMgr.h>
#include <game/CommonWindowMgr.h>
#include <game/CourseTimer.h>
#include <game/FieldGame.h>
#include <game/FlagCtrl.h>
#include <game/SelectCursor.h>
#include <game/SmallScoreMgr.h>
#include <game_info/CourseInfo.h>
#include <input/InputMgr.h>
#include <map/CoinOrigin.h>
#include <map_obj/ActorCoinMgr.h>
#include <system/CommentMgr.h>
#include <system/MainGame.h>
#include <system/ResMgr.h>

#include <actor/ActorMgr.h>
#include <actor/ActorStatus.h>
#include <enemy/TottenMgr.h>
#include <event/EventMgr.h>
#include <fukidashi/FukidashiMgr.h>
#include <layout/GamesceneBase.h>

#include <actor/ScoreMgr.h>

#include <prim/seadSafeString.h>

#include <cstring>

namespace {

u8& courseInfoByte(CourseInfo* course_info, u32 offset)
{
    return reinterpret_cast<u8*>(course_info)[offset];
}

u8& taskByte(CourseTask* task, u32 offset)
{
    return reinterpret_cast<u8*>(task)[offset];
}

u32& taskWord(CourseTask* task, u32 offset)
{
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(task) + offset);
}

FieldGameMode& taskGameMode(CourseTask* task)
{
    return *reinterpret_cast<FieldGameMode*>(reinterpret_cast<u8*>(task) + 0x280);
}

CommentMgr* loadCommentMgrFromData()
{
    return *reinterpret_cast<CommentMgr**>(reinterpret_cast<uintptr_t>(0x101E7D5C));
}

u8* mainGameStoryBytes(MainGame* main_game)
{
    return reinterpret_cast<u8*>(main_game) + 0x2001DFA;
}

void* loadSingletonAt(u32 address)
{
    return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(address));
}

} // namespace

// Address: 0x024D29A4
void CourseInfo_applyNextWorldCourseToNow(CourseInfo* course_info)
{
    courseInfoByte(course_info, 0x10) = course_info->getNextWorldNo();
    courseInfoByte(course_info, 0x11) = course_info->getNextCourseNo();
}

// Address: 0x024D3888
bool CourseInfo_isCourseNo0x39(CourseInfo* course_info)
{
    return course_info->getCourseNo() == 0x39;
}

// Address: 0x024D38B4
bool CourseInfo_isSpecialCourseNo(CourseInfo* course_info)
{
    u8 course_no = course_info->getCourseNo();
    if (course_no - 0x1f <= 3)
        return true;
    if (course_no - 0xca <= 3)
        return true;
    return false;
}

// Address: 0x024D29B8
void CourseInfo_commitNextFromCourseData(CourseInfo* course_info)
{
    void* course_data = CourseData_getFile(courseInfoByte(course_info, 0x1e));
    if (courseInfoByte(course_info, 0x1f) == 0xff)
    {
        CourseTask* course_task = CourseTask::instance();
        if (course_task != nullptr)
        {
            FieldGameMode::Mode mode = taskGameMode(course_task).getMode();
            if (mode == FieldGameMode::cBoostRush || mode == FieldGameMode::cCoinBattle)
            {
                u8* file = reinterpret_cast<u8*>(course_data);
                void** chain = reinterpret_cast<void**>(file + 8);
                u8 file_no = reinterpret_cast<u8*>(chain[0])[0x13];
                if (file_no != 0)
                {
                    if (chain[0] != chain[1])
                        file_no = reinterpret_cast<u8*>(chain[1])[0x13];
                }
                courseInfoByte(course_info, 0x1f) = file_no;
            }
        }
    }
    u8* raw = reinterpret_cast<u8*>(course_info);
    u32 next_scene = *reinterpret_cast<u32*>(raw + 0x20);
    *reinterpret_cast<u32*>(raw + 0x20) = 0;
    *reinterpret_cast<u32*>(raw + 0x18) = next_scene;
    raw[0x13] = raw[0x1f];
    raw[0x12] = raw[0x1e];
    raw[0x14] = reinterpret_cast<u8*>(course_data)[0xd];
}

// Address: 0x024BD86C
void CourseTask::setupCourse()
{
    CommentMgr* comment_mgr = loadCommentMgrFromData();
    if (comment_mgr != nullptr)
    {
        u32 comment_state = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(comment_mgr) + 0x10);
        if (comment_state == 2 || comment_state == 1)
            CommentMgr_onCourseSetup(comment_mgr);
    }

    CourseTask_allocCourseHeap(0);
    CourseSetupStack_pushSentinels();

    CourseInfo* course_info = CourseInfo::instance();
    CourseInfo_applyNextWorldCourseToNow(course_info);

    std::memcpy(&mGameData, &FieldGame::instance()->getGameData(), sizeof(FieldGameData));
    mGameData.resetEntriesUnk30();
    setupNabbitEntry();

    MainGame* main_game = MainGame::instance();
    u8* story_bytes = mainGameStoryBytes(main_game);
    story_bytes[0x12] = 0;

    if (!CourseInfo_isCourseNo0x39(course_info))
    {
        if (!CourseInfo_isSpecialCourseNo(course_info))
        {
            if (mGameData.checkEnterCourse(course_info->getWorldNo(), course_info->getCourseNo()) < 0)
            {
                bool should_clear = (courseInfoByte(course_info, 0x1e) == 2);
                if (!should_clear && course_info->getNextSceneEnterType() == 2)
                    should_clear = true;
                if (should_clear)
                    mGameData.clearCheckPoint();
            }
        }
    }

    for (s32 glyph_index = 0; glyph_index < 3; ++glyph_index)
    {
        s8 glyph = static_cast<s8>(taskByte(this, 0x134 + glyph_index));
        setGlyph(glyph_index, glyph);
    }

    u8 picture_glyph = PicturePaneMgr_lookupGlyph(reinterpret_cast<void*>(reinterpret_cast<u8*>(main_game) + 0x34),
        course_info->getWorldNo(), course_info->getCourseNo());
    taskByte(this, 0x264) = picture_glyph;

    FieldGameMode::Mode game_mode = taskGameMode(this).getMode();
    bool flag_28d = false;
    if (game_mode == FieldGameMode::cStory)
    {
        u32 main_flags = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(main_game) + 0x2c);
        if ((main_flags & 1) != 0 && (main_flags & 2) != 0)
            flag_28d = true;
        if (!flag_28d && CourseInfo_isCourseNo0x39(course_info) && !course_info->isTitle())
            flag_28d = true;
    }
    else if (game_mode <= FieldGameMode::cCoinBattle)
    {
        if (CourseInfo_isCourseNo0x39(course_info) && course_info->isTitle())
            flag_28d = true;
    }
    else if (game_mode == FieldGameMode::cChallenge)
    {
        if (SaveData_getPlayMode() == 4)
            flag_28d = true;
    }
    else
    {
        taskByte(this, 0x28d) = 0;
        if (taskGameMode(this).getMode() == FieldGameMode::cChallenge)
        {
            taskByte(this, 0x28d) = flag_28d ? 1 : 0;
            if (course_info->isStaffCredit() && !CourseInfo_isSpecialCourseNo(course_info))
                taskByte(this, 0x260) = 0;
        }
    }
    if (game_mode == FieldGameMode::cStory || game_mode <= FieldGameMode::cCoinBattle || game_mode == FieldGameMode::cChallenge)
    {
        taskByte(this, 0x28d) = flag_28d ? 1 : 0;
        if (taskGameMode(this).getMode() != FieldGameMode::cChallenge)
        {
            if (course_info->isStaffCredit())
            {
                if (!CourseInfo_isSpecialCourseNo(course_info))
                    taskByte(this, 0x260) = 0;
            }
        }
    }

    if (story_bytes[0] != 0)
    {
        u8 world_no = course_info->getWorldNo();
        if (world_no < 7)
        {
            u8 bit = static_cast<u8>(1 << world_no);
            u8 story_byte = story_bytes[2];
            taskByte(this, 0x304) = static_cast<u8>(((story_byte & bit) - 1) >> 31);
        }
        else
            taskByte(this, 0x304) = 0;
    }
    else
        taskByte(this, 0x304) = 0;

    struct GamesceneAllocPair
    {
        u32 vtable;
        u32 object;
    };
    GamesceneAllocPair gamescene_pair = {};
    gamescene_pair.vtable = 0x100B7240;
    GamesceneBase* gamescene = GamesceneFactory_create(&gamescene_pair, reinterpret_cast<void*>(0x100B703C), 0, 1, 0);
    s32 gamescene_heap_id = 1;
    if (gamescene != nullptr)
        gamescene_heap_id = GamesceneFactory_release(gamescene);

    if (InputMgr::instance() == nullptr)
        InputMgr::createInstance();

    InputMgr* input_mgr = InputMgr::instance();
    u8* save_entry = reinterpret_cast<u8*>(main_game) + 0x88;
    if (save_entry[0] != 0)
    {
        if (input_mgr != nullptr)
            InputMgr_applySaveEntry_(input_mgr, save_entry, 2, 0);
    }
    else if (taskGameMode(this).getMode() == FieldGameMode::cChallenge)
    {
        if (input_mgr != nullptr)
            InputMgr_applySaveEntry_(input_mgr, save_entry, 1, 0);
    }
    else
    {
        if (input_mgr != nullptr)
            InputMgr_applySaveEntry_(input_mgr, save_entry, 0, 0);
    }

    if (gamescene != nullptr)
        gamescene->vf2C();
    if (gamescene_heap_id != 1)
        GamesceneFactory_release(gamescene_heap_id);

    EventMgr::createInstance();
    ActorMgr::createInstance();
    ActorStatus::createInstance();
    f_0269B0D8();
    f_0268E0EC();

    GamesceneAllocPair gamescene_b_pair = {};
    gamescene_b_pair.vtable = 0x100B724C;
    GamesceneBase* gamescene_b = GamesceneFactory_create(&gamescene_b_pair, reinterpret_cast<void*>(0x100B703C), 0, 1, 0);
    s32 gamescene_b_heap_id = 1;
    if (gamescene_b != nullptr)
        gamescene_b_heap_id = GamesceneFactory_release(gamescene_b);
    f_0268F428();
    if (gamescene_b != nullptr)
        gamescene_b->vf2C();
    if (gamescene_b_heap_id != 1)
        GamesceneFactory_release(gamescene_b_heap_id);

    f_0268BE54();
    f_02916AF0();
    f_02742D2C();
    CoinBattleScoreMgr::createInstance();
    ActorCollisionCheckMgr_createInstance();
    f_0269BB7C();
    TouchDrcMgr_createInstance();
    f_020043F0();
    ActorCoinMgr::createInstance();
    BlockMgr_createInstance();
    ScoreMgr::createInstance();
    CourseTimer::createInstance();
    ActorBgCollisionMgr_createInstance();
    UnitBgCollisionHolder_createInstance();
    f_024C1838();
    f_022C9288();
    FlagCtrl::createInstance();
    CoinOrigin::createInstance();
    SmallScoreMgr::createInstance();
    CommonWindowMgr::createInstance();
    SelectCursor::createInstance();
    createGamesceneByMode();
    FukidashiMgr::createInstance();
    f_0250A698();
    f_02816A7C();
    BalloonMgr::createInstance();
    f_02891458();
    f_028914EC();
    f_024D967C();
    f_024D9744();
    f_028D3A60();
    f_028D3B40();
    f_024ED434();

    if (!taskGameMode(this).isWmMode())
    {
        GamesceneAllocPair wm_pair = {};
        wm_pair.vtable = 0x100B7258;
        GamesceneBase* wm_hud = GamesceneFactory_create(&wm_pair, reinterpret_cast<void*>(0x100B703C), 0, 1, 0);
        s32 wm_heap_id = 1;
        if (wm_hud != nullptr)
            wm_heap_id = GamesceneFactory_release(wm_hud);
        f_024EDAA0();
        if (wm_hud != nullptr)
            wm_hud->vf2C();
        if (wm_heap_id != 1)
            GamesceneFactory_release(wm_heap_id);
    }

    CoinOrigin::initialize();
    f_024C18B8();

    GamesceneAllocPair pack_pair = {};
    pack_pair.vtable = 0x100B7264;
    GamesceneBase* pack_hud = GamesceneFactory_create(&pack_pair, reinterpret_cast<void*>(0x100B703C), 0, 1, 0);
    s32 pack_heap_id = 1;
    if (pack_hud != nullptr)
        pack_heap_id = GamesceneFactory_release(pack_hud);

    comment_mgr = loadCommentMgrFromData();
    if (comment_mgr != nullptr)
    {
        u32 comment_state = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(comment_mgr) + 0x10);
        if (comment_state != 2 && comment_state != 1)
        {
            alignas(4) char course_pack_name[32];
            CourseResPackNameBuilder_build(course_info->getWorldNo(), course_info->getCourseNo());
            ResMgr::instance()->loadCourseResPack(sead::SafeString(course_pack_name), nullptr);
            CourseResPackNameBuilder_teardown();
            CourseData_releaseFileRef();
        }
    }

    for (s32 file_index = 0; file_index < 4; ++file_index)
    {
        void* file = CourseData_getFile(file_index);
        if (file != nullptr)
        {
            CourseTask_bindCourseResNode(file);
            CourseTask_bindCourseResNode(reinterpret_cast<u8*>(file) + 0x20);
            CourseTask_bindCourseResNode(reinterpret_cast<u8*>(file) + 0x40);
            CourseTask_bindCourseResNode(reinterpret_cast<u8*>(file) + 0x60);
        }
    }

    if (pack_hud != nullptr)
        pack_hud->vf2C();
    if (pack_heap_id != 1)
        GamesceneFactory_release(pack_heap_id);

    CourseInfo_commitNextFromCourseData(course_info);
    course_info->setNextSceneEnterType(course_info->getNextSceneEnterType());
    taskByte(this, 0xca) = static_cast<u8>((course_info->getNextSceneEnterType() ^ 2) == 0);
    MainGame_setCourseBanner(course_info->getWorldNo(), course_info->getCourseNo());
    f_026CA03C();

    if (taskGameMode(this).getMode() == FieldGameMode::cChallenge)
        taskWord(this, 0x308) = CourseTask_selectEntryCharacter(this);

    f_0202ADC4();
    ActorResLoader_load(1, 0);
    if (!taskGameMode(this).isDvMiiMode())
        f_022A6048(this, taskWord(this, 0x58));

    FlagCtrl::initialize();

    if (taskGameMode(this).getMode() == FieldGameMode::cChallenge)
        ChallengeMgr_setCourseFlag();

    if (ChallengeMgr_isActive())
    {
        u8 file_no = courseInfoByte(course_info, 0x12);
        u8* course_file = reinterpret_cast<u8*>(CourseData_getFile(file_no));
        u16 timer_a = 0x1f4;
        u16 timer_b = 0x12c;
        u16 timer_c = 0xc8;
        if (course_file != nullptr)
        {
            u16* header = reinterpret_cast<u16*>(*reinterpret_cast<void**>(course_file + 8));
            if (header[5] != 0)
                timer_a = header[5];
            if (header[10] != 0)
                timer_b = header[10];
            if (header[11] != 0)
                timer_c = header[11];
        }
        if (story_bytes[0] == 0)
        {
            timer_a = 0x64;
            timer_c = 0x64;
        }
        u8 world_digit = courseInfoByte(course_info, 0x13);
        u8* entry = reinterpret_cast<u8*>(*reinterpret_cast<void**>(course_file + 8));
        if (world_digit != entry[0x10])
        {
            u8* alt = reinterpret_cast<u8*>(*reinterpret_cast<void**>(course_file + 0));
            if (world_digit != alt[0x13])
            {
                u32 list_count = *reinterpret_cast<u32*>(course_file + 0x98);
                u8* list_base = reinterpret_cast<u8*>(*reinterpret_cast<void**>(course_file + 0x20));
                bool set = false;
                for (u32 i = 0; i < list_count; ++i)
                {
                    u8* node = list_base + i * 0x18;
                    if (*reinterpret_cast<u16*>(node) != 0x19)
                        continue;
                    u32 packed = *reinterpret_cast<u32*>(node + 8);
                    if (static_cast<u8>(packed >> 16) != courseInfoByte(course_info, 0x13))
                        continue;
                    u8 sub = static_cast<u8>(packed & 0xf);
                    if (sub < 1)
                        CourseTimer::instance()->setTimer(timer_a);
                    else if (sub == 1)
                        CourseTimer::instance()->setTimer(timer_c);
                    else
                        CourseTimer::instance()->setTimer(timer_a);
                    set = true;
                    break;
                }
                if (!set)
                    CourseTimer::instance()->setTimer(timer_a);
            }
        }
    }

    ActorMgr::initialize();
    f_0291713C();
    f_02742F9C();
    f_0269BC94();
    f_0268E888();
    f_0268F7F0();
    f_0250A840();

    if (taskWord(this, 0xcc) != 0)
    {
        using VFunc = void(*)();
        uintptr_t* object = reinterpret_cast<uintptr_t*>(taskWord(this, 0xcc));
        VFunc fn = reinterpret_cast<VFunc>(object[3]);
        fn();
    }
    if (taskWord(this, 0xd0) != 0)
        f_02657CA4();
    if (loadSingletonAt(0x101D7284) != nullptr)
        f_025CE934();
    if (loadSingletonAt(0x101D31F8) != nullptr)
        f_0252E6C4();
    if (loadSingletonAt(0x101D5224) != nullptr)
        f_0258B084();
    if (loadSingletonAt(0x101D3728) != nullptr)
        f_0253B5CC();
    if (taskWord(this, 0x120) != 0)
        f_02532D14();
    if (taskWord(this, 0xd4) != 0)
        f_02666DA8();
    if (taskWord(this, 0xd8) != 0)
        f_0258C1F0();
    if (taskWord(this, 0xe4) != 0)
        f_02650F7C();
    if (taskWord(this, 0xe8) != 0)
        f_02650344();
    if (taskWord(this, 0xec) != 0)
        f_0264E25C();
    if (taskWord(this, 0xdc) != 0)
        f_02537290();
    if (taskWord(this, 0xe0) != 0)
        f_025396C4();
    if (taskWord(this, 0xf4) != 0)
    {
        LightMapMgr_initialize();
        f_02551B04();
    }
    if (taskWord(this, 0xf8) != 0)
        f_025F4B24();
    if (taskWord(this, 0xfc) != 0)
        f_025BE710();
    if (taskWord(this, 0x100) != 0)
        f_02646F58();
    if (taskWord(this, 0x10c) != 0)
    {
        f_02627FF4();
        f_026287BC();
    }
    if (taskWord(this, 0x104) != 0)
        f_02614CD0();
    if (taskWord(this, 0x108) != 0)
        f_02534138();
    if (taskWord(this, 0x110) != 0)
        f_02667C9C();
    if (taskWord(this, 0x114) != 0)
        f_026533B0();
    if (taskWord(this, 0x118) != 0)
        f_02657278();
    if (taskWord(this, 0x11c) != 0)
        f_025846D4();
    if (taskWord(this, 0xf0) != 0)
        f_025F281C();

    f_024CC8E8();
    f_024BC4C4();
    SelectCursor::create();
    if (loadSingletonAt(0x101E830C) != nullptr)
        f_025FBCF0();
    f_025F2F50();
    f_024B3544();
    if (loadSingletonAt(0x101D4264) != nullptr)
        f_0255B524();
    f_026444A8();
    if (loadSingletonAt(0x101EA700) != nullptr)
        f_026661D8();

    TottenMgr::createInstance();
    f_024230A8();
    AreaTask_createFromCourseTask(this);
    CourseTask_onSetupComplete();
}
