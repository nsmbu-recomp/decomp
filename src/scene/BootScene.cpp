#include <scene/BootScene.h>

#include <scene/Scene.h>
#include <state/IStateFactory.h>

namespace {

constexpr uintptr_t kStateIdGuard = 0x101E9E9Cu;
constexpr uintptr_t kStateRegCounter = 0x101E9F04u;
constexpr uintptr_t kStateRegInitFlag = 0x101E9F00u;
constexpr uintptr_t kBootStateTable = 0x1023BFCCu;
constexpr uintptr_t kStateExitWait = 0x1023BFECu;
constexpr uintptr_t kStateExitReady = 0x1023C00Cu;
constexpr uintptr_t kBootSceneStateVtable = 0x10175918u;
constexpr uintptr_t kFlowTaskFactory = 0x0297B9F8u;
constexpr uintptr_t kStateLinkTagPrimary = 0x101E771Cu;
constexpr uintptr_t kStateLinkTagSecondary = 0x101E7728u;
constexpr uintptr_t kStateLinkTagTertiary = 0x101E7734u;

constexpr u32 kFnResetExitFrameCount = 0x0297B130u;
constexpr u32 kFnExecuteExitWaitState = 0x0297B13Cu;
constexpr u32 kFnSetExitReady = 0x0297B17Cu;
constexpr u32 kFnExecuteExitReadyState = 0x0297B188u;
constexpr u32 kFnTryConsumeExitLatch = 0x0297B1A4u;
constexpr u32 kFnFinalizePrimary = 0x0297B5ECu;
constexpr u32 kFnFinalizeHandoff = 0x0297B5F0u;
constexpr u32 kFnEnterConsume = 0x0297B5F4u;
constexpr u32 kFnFinalizeConsume = 0x0297B5F8u;

const StateID& stateIdAt(uintptr_t address)
{
    return *reinterpret_cast<const StateID*>(address);
}

IStateFactory& bootSceneStateFactory()
{
    return *reinterpret_cast<IStateFactory*>(kBootStateTable);
}

struct BootStateRegRow {
    u8 bytes[0x20];
};

void writeBootStateRow(
    BootStateRegRow& row,
    u32 counter,
    u32 enter,
    u32 execute,
    u32 finalize)
{
    u8* p = row.bytes;
    *reinterpret_cast<u32*>(p + 0x00) = counter;
    *reinterpret_cast<u32*>(p + 0x04) = kBootSceneStateVtable;
    *reinterpret_cast<u16*>(p + 0x08) = 0;
    *reinterpret_cast<u16*>(p + 0x0A) = 0xFFFF;
    *reinterpret_cast<u32*>(p + 0x0C) = enter;
    *reinterpret_cast<u16*>(p + 0x10) = 0;
    *reinterpret_cast<u32*>(p + 0x14) = execute;
    *reinterpret_cast<u16*>(p + 0x12) = 0xFFFF;
    *reinterpret_cast<u16*>(p + 0x18) = 0;
    *reinterpret_cast<u16*>(p + 0x1A) = 0xFFFF;
    *reinterpret_cast<u32*>(p + 0x1C) = finalize;
}

void writePrimaryRow(BootStateRegRow& row, u32 counter)
{
    writeBootStateRow(row, counter, kFnResetExitFrameCount, kFnExecuteExitWaitState, kFnFinalizePrimary);
}

void writeHandoffRow(BootStateRegRow& row, u32 counter)
{
    writeBootStateRow(row, counter, kFnSetExitReady, kFnExecuteExitReadyState, kFnFinalizeHandoff);
}

void writeConsumeRow(BootStateRegRow& row, u32 counter)
{
    writeBootStateRow(row, counter, kFnEnterConsume, kFnTryConsumeExitLatch, kFnFinalizeConsume);
}

u32& stateRegCounter()
{
    return *reinterpret_cast<u32*>(kStateRegCounter);
}

u32& stateRegInitFlag()
{
    return *reinterpret_cast<u32*>(kStateRegInitFlag);
}

volatile u32& stateIdGuardWord()
{
    return *reinterpret_cast<volatile u32*>(kStateIdGuard);
}

void registerPrimary(BootStateRegRow& row, u32 counter)
{
    writePrimaryRow(row, counter);
    linkStateRegistration(reinterpret_cast<void*>(kStateLinkTagPrimary));
}

void registerHandoff(BootStateRegRow& row, u32 counter)
{
    writeHandoffRow(row, counter);
    linkStateRegistration(reinterpret_cast<void*>(kStateLinkTagSecondary));
}

void registerConsume(BootStateRegRow& row, u32 counter)
{
    writeConsumeRow(row, counter);
    linkStateRegistration(reinterpret_cast<void*>(kStateLinkTagTertiary));
}

} // namespace

BootScene::BootScene(const sead::TaskConstructArg& arg)
    : sead::CalculateTask(arg, "BootScene")
    , mStateMethod(bootSceneStateFactory(), stateIdAt(kBootStateTable))
{
    mExitLatch = 0;
}

void BootScene::calc()
{
    calculateBootSceneUi(mUi);
    mStateMethod.executeStateMethod();
}

void BootScene::setExitLatchFromTask()
{
    sead::TaskMgr* mgr = *reinterpret_cast<sead::TaskMgr**>(reinterpret_cast<char*>(this) + 0x5C);
    bootSceneNotifyTaskMgr(mgr);
    mExitLatch = 1;
}

bool BootScene::requestFlowScene()
{
    Scene* scene = Scene::instance();
    if (sceneHasPendingNext(scene) != 0)
        return false;

    sead::TaskClassID next_scene(reinterpret_cast<sead::TaskFactory>(kFlowTaskFactory));
    u32 fader = Scene::FaderInfo::make(10, 10);
    return scene->setNextScene(this, next_scene, fader);
}

void BootScene::resetExitFrameCount()
{
    mExitFrameCount = 0;
}

void BootScene::executeExitWaitState()
{
    if (mHoldExitCount == 0)
        mExitFrameCount++;

    if (mExitLatch == 0)
        return;

    if (mExitFrameCount < 180)
        return;

    mStateMethod.changeStateMethod(stateIdAt(kStateExitWait));
}

void BootScene::setExitReady()
{
    mExitReady = 1;
}

void BootScene::executeExitReadyState()
{
    if (mExitReady == 0)
        return;

    mStateMethod.changeStateMethod(stateIdAt(kStateExitReady));
}

void BootScene::tryConsumeExitLatch()
{
    if (mExitLatch == 0)
        return;

    if (!requestFlowScene())
        return;

    mExitLatch = 0;
}

void registerBootExitStates()
{
    BootStateRegRow& primary = *reinterpret_cast<BootStateRegRow*>(kBootStateTable);
    BootStateRegRow& handoff = *reinterpret_cast<BootStateRegRow*>(kStateExitWait);
    BootStateRegRow& consume = *reinterpret_cast<BootStateRegRow*>(kStateExitReady);

    if (stateIdGuardWord() == 0)
    {
        stateIdGuardWord() = 1;
        stateRegCounter() = 1;
        registerPrimary(primary, 1);

        u32 count = stateRegCounter() + 1;
        stateRegCounter() = count;
        registerHandoff(handoff, count);

        if (stateRegInitFlag() == 0)
        {
            stateRegCounter() = 1;
            registerConsume(consume, 1);
        }
        else
        {
            count = stateRegCounter() + 1;
            stateRegCounter() = count;
            registerConsume(consume, count);
        }
        return;
    }

    u32 count = stateRegCounter() + 1;
    stateRegCounter() = count;
    registerPrimary(primary, count);

    if (stateRegInitFlag() == 0)
    {
        stateRegInitFlag() = 1;
        stateRegCounter() = 1;
        registerHandoff(handoff, 1);

        if (stateRegInitFlag() == 0)
        {
            stateRegCounter() = 1;
            registerConsume(consume, 1);
        }
        else
        {
            count = stateRegCounter() + 1;
            stateRegCounter() = count;
            registerConsume(consume, count);
        }
    }
    else
    {
        count = stateRegCounter() + 1;
        stateRegCounter() = count;
        registerHandoff(handoff, count);

        if (stateRegInitFlag() == 0)
        {
            stateRegCounter() = 1;
            registerConsume(consume, 1);
        }
        else
        {
            count = stateRegCounter() + 1;
            stateRegCounter() = count;
            registerConsume(consume, count);
        }
    }
}
