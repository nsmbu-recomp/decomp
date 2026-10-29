#include <scene/Scene.h>

#include <game/FieldGame.h>
#include <game_info/CourseInfo.h>

namespace {

// Address: 0x0299477C
extern bool sub_0299477C(Scene* self, sead::TaskBase* p_src_task, const sead::TaskClassID* next_scene, u32 fader_info);

// Address: 0x02029B9C
extern void sub_02029B9C(const sead::TaskClassID* next_scene, u32 snd_param);

// Address: 0x02983ABC
extern bool sub_02983ABC(Fader* fader);

// Address: 0x02983B28
extern u32 sub_02983B28(Fader* fader);

constexpr uintptr_t kStorySceneTaskFactory = 0x0202B4C8u;

}

SEAD_SINGLETON_DISPOSER_IMPL(Scene)

// Address: 0x02993D68
Scene::Scene()
{
    mCreateCallback.bind(this, &Scene::create);

    mOldScene = sead::TaskClassID();
    mNowScene = sead::TaskClassID();
    mNextScene = sead::TaskClassID();

    mpTaskMgr = nullptr;
    mpFader = nullptr;
    mFaderInfo = 0;
    _70 = 0;
    mFadeCenter.set(0.0f, 0.0f, 0.0f);
    _80 = 1;
}

// Address: 0x02994164
bool Scene::setNextScene(sead::TaskBase* p_src_task, const sead::TaskClassID& next_scene, u32 fader_info, u32 snd_param_unused)
{
    if (!sub_0299477C(this, p_src_task, &next_scene, fader_info))
        return false;

    sub_02029B9C(&next_scene, snd_param_unused);
    return true;
}

// Address: 0x029941D0
bool Scene::setCourse_Story(sead::TaskBase* p_src_task, s32 world_no, s32 course_no)
{
    CourseInfo* course_info = CourseInfo::instance();
    course_info->setNextWorldNo(world_no);
    course_info->setNextCourseNo(course_no);

    sead::TaskClassID next_scene(reinterpret_cast<sead::TaskFactory>(kStorySceneTaskFactory));
    if (!setNextScene(p_src_task, next_scene, 0, 0))
        return false;

    FieldGameData& game_data = FieldGame::instance()->getGameData();
    const CheckPointData& checkpoint = *reinterpret_cast<const CheckPointData*>(&game_data);
    if (game_data.checkEnterCourse(world_no, course_no) == -1)
    {
        course_info->setNextFileNo(0);
        course_info->setNextNextGotoNo(0xFF);
        course_info->setNextSceneEnterType(1);
    }
    else
    {
        course_info->setNextFileNo(checkpoint.file_no);
        course_info->setNextNextGotoNo(checkpoint.next_goto_no);
        course_info->setNextSceneEnterType(2);
    }

    return true;
}

// Address: 0x029945B8
bool Scene::isFadein() const
{
    return sub_02983ABC(mpFader);
}

// Address: 0x02994494
u32 sceneHasPendingNext(Scene* scene)
{
    Fader* fader = *reinterpret_cast<Fader**>(reinterpret_cast<u8*>(scene) + 0x68);
    return sub_02983B28(fader);
}
