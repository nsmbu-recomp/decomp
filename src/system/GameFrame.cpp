#include <graphics/LayerMgr.h>
#include <system/GameFrame.h>

#include <framework/seadMethodTree.h>
#include <framework/seadTaskMgr.h>
#include <gfx/cafe/seadGraphicsCafe.h>
#include <gfx/seadFrameBuffer.h>
#include <gfx/seadGraphics.h>

namespace {

u32& frameworkProcInitFlag()
{
    static u32 flag = 0;
    return flag;
}

void*& frameworkProcDrawCallbackSlot()
{
    static void* slot = nullptr;
    return slot;
}

}

void GameFrame::bindFrameBuffer_(sead::FrameBuffer* buffer)
{
    if (buffer != nullptr)
        buffer->bind();
}

void GameFrame::invokeDrawAt0xAC_()
{
    uintptr_t mgr = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8_t*>(this) + 0x24);
    using DrawFn = void (*)(GameFrame*);
    DrawFn draw = reinterpret_cast<DrawFn>(*reinterpret_cast<uintptr_t*>(mgr + 0xac));
    draw(this);
}

sead::TaskBase* GameFrame::runRootTaskProcDraw_(sead::TaskBase* root)
{
    if (root == nullptr)
        return nullptr;
    uintptr_t node = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8_t*>(root) + 0x3c);
    using ProcDrawFn = sead::TaskBase* (*)(sead::TaskBase*);
    ProcDrawFn proc_draw = reinterpret_cast<ProcDrawFn>(*reinterpret_cast<uintptr_t*>(node + 0xc));
    if (proc_draw(root) == nullptr)
        return nullptr;
    return root;
}

void GameFrame::callRootMethodTree_(sead::TaskBase* root, u32 byte_offset)
{
    if (root == nullptr)
        return;
    sead::MethodTreeNode* node =
        reinterpret_cast<sead::MethodTreeNode*>(reinterpret_cast<uint8_t*>(root) + byte_offset);
    node->call();
}

// Address: 0x029C4FE4
void GameFrame::procDraw_()
{
    if (frameworkProcInitFlag() == 0)
    {
        frameworkProcInitFlag() = 1;
        frameworkProcDrawCallbackSlot() = reinterpret_cast<void*>(0x1018526C);
    }

    sead::TaskBase* root = getTaskMgr()->getRootTask();
    root = runRootTaskProcDraw_(root);

    GX2SampleTopGPUCycle(&(mGpuCounters[cGpuCounter_TVDrawBegin]));
    if (mDefaultFrameBuffer != nullptr)
        bindFrameBuffer_(mDefaultFrameBuffer);
    callRootMethodTree_(root, 0x13c);
    GX2SampleBottomGPUCycle(&(mGpuCounters[cGpuCounter_TVDrawEnd]));
    invokeDrawAt0xAC_();

    GX2SampleTopGPUCycle(&(mGpuCounters[cGpuCounter_DRCDrawBegin]));
    if (mDefaultFrameBufferDRC != nullptr)
        bindFrameBuffer_(mDefaultFrameBufferDRC);
    callRootMethodTree_(root, 0x28c);
    GX2SampleBottomGPUCycle(&(mGpuCounters[cGpuCounter_DRCDrawEnd]));
    invokeDrawAt0xAC_();

    GX2SetContextState(sead::GraphicsCafe::instance()->getGX2ContextState());
    GX2Flush();
}

// Address: 0x029C52CC
void GameFrame::procCalc_()
{
    LayerMgr* layer_mgr = LayerMgr::instance();

    if (mCalcPassCount > 0)
    {
        if (layer_mgr != nullptr)
            layer_mgr->clearRenderMgr();

        sead::GameFrameworkCafe::procCalc_();

        sead::MethodTreeNode* app_calc_node = getMethodTreeMgr()->getRootMethodTreeNode(1);
        for (s32 i = 1; i < mCalcPassCount; i++)
            app_calc_node->call();

        if (layer_mgr == nullptr)
            return;

        layer_mgr->calcDL();
        calc_();
        return;
    }

    s32 v = mSkippedCalcCount + 1;
    if (v < -mCalcPassCount)
    {
        mSkippedCalcCount = v;
        return;
    }

    if (layer_mgr != nullptr)
        layer_mgr->clearRenderMgr();

    sead::GameFrameworkCafe::procCalc_();
    mSkippedCalcCount = 0;

    if (layer_mgr == nullptr)
        return;

    layer_mgr->calcDL();
    calc_();
}

// Address: 0x029C5104
void GameFrame::calc_()
{
    GX2DrawDone();

    if (mFrameBuffer != nullptr)
    {
        mFrameBuffer->copyToDisplayBuffer(mDisplayBuffer);
        mFrameBuffer = nullptr;
        GX2SetContextState(sead::GraphicsCafe::instance()->getGX2ContextState());
        if (mFrameBufferDRC != nullptr)
            goto drc_via_default;
        goto copy_pending_drc;
    }

    if (mDefaultFrameBuffer != nullptr)
        mDefaultFrameBuffer->copyToDisplayBuffer(mDisplayBuffer);

    GX2SetContextState(sead::GraphicsCafe::instance()->getGX2ContextState());
    if (mFrameBufferDRC != nullptr)
        goto drc_via_default;

copy_pending_drc:
    if (mFrameBufferDRC != nullptr)
    {
        mFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
        mFrameBufferDRC = nullptr;
        if (mDefaultFrameBufferDRC != nullptr)
            goto present_common;
        goto copy_default_drc;
    }

    if (mDefaultFrameBufferDRC != nullptr)
    {
        mDefaultFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
        if (mFrameBufferDRC != nullptr)
        {
            mFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
            mFrameBufferDRC = nullptr;
            goto present_common;
        }
        goto copy_default_drc;
    }

copy_default_drc:
    if (mDefaultFrameBufferDRC != nullptr)
        mDefaultFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
    goto present_common;

drc_via_default:
    if (mDefaultFrameBufferDRC != nullptr)
    {
        mDefaultFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
        if (mFrameBufferDRC != nullptr)
        {
            mFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
            mFrameBufferDRC = nullptr;
            goto present_early;
        }
        goto copy_default_drc;
    }

    if (mFrameBufferDRC != nullptr)
    {
        mFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
        mFrameBufferDRC = nullptr;
        goto present_early;
    }

    if (mDefaultFrameBufferDRC != nullptr)
        mDefaultFrameBufferDRC->copyToDisplayBuffer(mDisplayBufferDRC);
    goto present_common;

present_early:
    GX2SetContextState(sead::GraphicsCafe::instance()->getGX2ContextState());
    GX2Flush();
    sead::Graphics::instance()->lockDrawContext();
    LayerMgr::instance()->calcParallelExec();
    sead::Graphics::instance()->unlockDrawContext();
    return;

present_common:
    GX2SetContextState(sead::GraphicsCafe::instance()->getGX2ContextState());
    GX2Flush();
    sead::Graphics::instance()->lockDrawContext();
    LayerMgr::instance()->calcParallelExec();
    sead::Graphics::instance()->unlockDrawContext();
}
