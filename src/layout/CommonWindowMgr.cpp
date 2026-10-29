#include <game/CommonWindowMgr.h>

namespace {

// Address: 0x029F1594
void* sub_029F1594(u32 size);

// Address: 0x02AFA350
void sub_02AFA350(void* self, s32 count, s32 size, u32 ctor);

// Address: 0x025141B0
extern void inputMgrInitControllerSlot_(void* controller, s32 slot_index);

// Address: 0x02585CC4
void sub_02585CC4(CommonWindow* window);

// Address: 0x025869C8
void sub_025869C8(CommonWindow* window);

// Address: 0x02586AD4
void sub_02586AD4(const CommonWindow* window, const agl::lyr::RenderInfo& render_info);

// Address: 0x02586B4C
void sub_02586B4C(const CommonWindow* window, const agl::lyr::RenderInfo& render_info);

// Address: 0x02586C1C
void sub_02586C1C(CommonWindow* window);

// Address: 0x02586BDC
void sub_02586BDC(CommonWindow* window);

// Address: 0x0267745C
void sub_0267745C(s32 se_id);

template<typename T>
T& fieldAt(u8* base, u32 offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

bool controllerTriggerBit_(SysControllerWrapper& wrapper, u32 bit_index)
{
    u32 value = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&wrapper));
    return ((value >> bit_index) & 1) != 0;
}

SysControllerWrapper* getActiveController_(CommonWindowMgr* mgr)
{
    u8* self = reinterpret_cast<u8*>(mgr);
    SysController::Id id = fieldAt<SysController::Id>(self, 0x22B0);
    SysControllerWrapper* controllers =
        reinterpret_cast<SysControllerWrapper*>(self + 0x1ACC);

    if (static_cast<u32>(id) >= static_cast<u32>(SysController::cId_Max))
        return &controllers[0];

    return &controllers[static_cast<u32>(id)];
}

} // namespace

// Address: 0x024BC444
// SingletonDisposer_::~SingletonDisposer_()    Address: 0x024BC874
SEAD_SINGLETON_DISPOSER_IMPL(CommonWindowMgr)

// Address: 0x024BC3C4
CommonWindowMgr::CommonWindowMgr()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self == nullptr)
    {
        self = static_cast<u8*>(sub_029F1594(0x22B8));
        if (self == nullptr)
            return;
    }

    new (self + 0x10) CommonWindow();

    u8* controllers = self + 0x1ACC;
    if (*reinterpret_cast<u32*>(controllers) == 0)
    {
        sub_02AFA350(controllers, 5, 0x194, 0x02514150);
    }

    mIsActive = false;
    mCurrentControllerId = static_cast<SysController::Id>(0);
}

// Address: 0x024BC4C4
void CommonWindowMgr::create()
{
    sub_02585CC4(&mCommonWindow);

    for (s32 slot = 0; slot < SysController::cId_Max; ++slot)
        inputMgrInitControllerSlot_(&mControllers[slot], slot);
}

// Address: 0x024BC540
void CommonWindowMgr::execute()
{
    if (!mIsActive)
        return;

    sub_025869C8(&mCommonWindow);

    if (!mCommonWindow.mIsFadeOut)
        mIsActive = false;
}

// Address: 0x024BC590
void CommonWindowMgr::draw(const agl::lyr::RenderInfo& render_info) const
{
    if (!mIsActive)
        return;

    sub_02586AD4(&mCommonWindow, render_info);
}

// Address: 0x024BC5A4
void CommonWindowMgr::drawDRC(const agl::lyr::RenderInfo& render_info) const
{
    if (!mIsActive)
        return;

    if (mCommonWindow.mIsSystemDRC)
        return;

    sub_02586B4C(&mCommonWindow, render_info);
}

// Address: 0x024BC5C4
void CommonWindowMgr::drawDRC_System(const agl::lyr::RenderInfo& render_info) const
{
    if (!mIsActive)
        return;

    if (!mCommonWindow.mIsSystemDRC)
        return;

    sub_02586B4C(&mCommonWindow, render_info);
}

// Address: 0x024BC5E4
CommonWindowMgr::PadTrigStatus CommonWindowMgr::processPadTrig_TwoButtons(bool enable_back_button)
{
    u8* self = reinterpret_cast<u8*>(this);

    if (fieldAt<u8>(self, 0x1147) != 0)
    {
        mCommonWindow.mIsDecidedTwoButtons = true;
        return cStatus_Decide;
    }

    if (fieldAt<u8>(self, 0x1ABA) != 0)
        return cStatus_Back;

    if (fieldAt<u8>(self, 0x1146) != 0)
        return cStatus_Back;

    if (fieldAt<u8>(self, 0x114A) != 0)
        return cStatus_Dpad;

    SysControllerWrapper* controller = getActiveController_(this);
    if (controllerTriggerBit_(*controller, 0x12))
        return cStatus_Decide;

    if (enable_back_button)
    {
        if (controllerTriggerBit_(*controller, 0x11))
        {
            s32 se_id = fieldAt<u8>(self, 0x1ABE) != 0 ? 9 : 5;
            sub_0267745C(se_id);
            return cStatus_Decide;
        }
    }

    if (mCommonWindow.cursorIsOnLeftButton())
    {
        if (controllerTriggerBit_(*getActiveController_(this), 0x18))
        {
            sub_02586C1C(&mCommonWindow);
            return cStatus_Idle;
        }
    }
    else if (mCommonWindow.cursorIsOnRightButton())
    {
        if (controllerTriggerBit_(*getActiveController_(this), 0x19))
        {
            sub_02586BDC(&mCommonWindow);
            return cStatus_Idle;
        }
    }

    return cStatus_Idle;
}

// Address: 0x024BC7D4
CommonWindowMgr::PadTrigStatus CommonWindowMgr::processPadTrig_OneButton()
{
    u8* self = reinterpret_cast<u8*>(this);

    if (fieldAt<u8>(self, 0x1147) != 0)
        return cStatus_Decide;

    if (fieldAt<u8>(self, 0x1ABA) != 0)
        return cStatus_Back;

    if (fieldAt<u8>(self, 0x1146) != 0)
        return cStatus_Back;

    if (fieldAt<u8>(self, 0x114A) != 0)
        return cStatus_Dpad;

    SysControllerWrapper* controller = getActiveController_(this);
    if (controllerTriggerBit_(*controller, 0x12))
        return cStatus_Idle;

    if (controllerTriggerBit_(*controller, 0x11))
        return static_cast<PadTrigStatus>(1);

    return cStatus_Idle;
}
