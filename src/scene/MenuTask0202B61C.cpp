#include <scene/MenuTask0202B61C.h>

#include <game/SubjectMgr.h>
#include <game_info/CourseInfo.h>
#include <scene/Scene.h>

namespace {

const StateID& stateIdAt(uintptr_t address)
{
    return *reinterpret_cast<const StateID*>(address);
}

constexpr uintptr_t kStateChallengeSelect = 0x1022C184u;
constexpr uintptr_t kStateAfterConfirm = 0x1022C1D0u;

constexpr u32 kCourseTaskFactory = 0x0202B4C8u;
constexpr u32 kChallengeSelectFactory = 0x024C11F4u;

sead::TaskClassID factoryId(u32 factory)
{
    return sead::TaskClassID(reinterpret_cast<sead::TaskFactory>(factory));
}

// Address: 0x024C2E3C
const u8* loadSaveRecord(SubjectMgr* mgr, u8 file)
{
    (void)mgr;
    (void)file;
    return nullptr;
}

} // namespace

void MenuTask0202B61C::enterChallengeSelectState()
{
    mStateMethod.changeStateMethod(stateIdAt(kStateChallengeSelect));
}

void MenuTask0202B61C::openChallengeSelect()
{
    SubjectMgr::instance()->clearSubject();
    Scene::instance()->setNextScene(this, factoryId(kChallengeSelectFactory));
}

void MenuTask0202B61C::copySaveToCourseInfo()
{
    SubjectMgr* subject_mgr = SubjectMgr::instance();
    u8 file = subject_mgr->getSelectedFile();
    const u8* record = loadSaveRecord(subject_mgr, file);
    CourseInfo* course_info = CourseInfo::instance();
    course_info->setNextFileNo(record[0x43]);
    course_info->setNextNextGotoNo(record[0x47]);
}

void MenuTask0202B61C::confirmFile()
{
    if (mGateSelect == 0)
        return;

    SubjectMgr* subject_mgr = SubjectMgr::instance();

    if (mGateStory != 0)
    {
        u8 file = subject_mgr->getSelectedFile();
        subject_mgr->selectSaveFile(file);
        copySaveToCourseInfo();
        CourseInfo::instance()->setNextSceneEnterType(1);
        Scene::instance()->setNextScene(this, factoryId(kCourseTaskFactory));
        _1cad = 1;
        mStateMethod.changeStateMethod(stateIdAt(kStateAfterConfirm));
        return;
    }

    subject_mgr->clearSubject();
    CourseInfo::instance()->setNextSceneEnterType(1);
    Scene::instance()->setNextScene(this, factoryId(kChallengeSelectFactory));
    _1cad = 1;
    mStateMethod.changeStateMethod(stateIdAt(kStateAfterConfirm));
}
