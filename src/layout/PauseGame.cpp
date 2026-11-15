#include <layout/PauseGame.h>
#include <layout_sys/AnimLayout.h>
#include <layout_sys/UtilCursorButtonMgr.h>
#include <game/CourseTask.h>
#include <game/FieldGameMode.h>
#include <game/SelectCursor.h>
#include <state/StateID.h>
#include <state/StateMethod.h>
#include <system/MainGame.h>

#include <math/seadVector.h>
#include <prim/seadSafeString.h>

namespace {

constexpr u32 kOffsetStateMethod = 0x220u;

constexpr u32 kOffsetButtonArray    = 0x0Cu;
constexpr u32 kOffsetButtonIndex    = 0x10u;
constexpr u32 kOffsetCursorButtonMgr = 0x14u;
constexpr u32 kOffsetModeWord        = 0x1Cu;
constexpr u32 kOffsetCursorType      = 0x48u;
constexpr u32 kOffsetCursorIndex     = 0x4Cu;
constexpr u32 kOffsetPadWord         = 0x50u;
constexpr u32 kOffsetPointerFlag     = 0x5Cu;
constexpr u32 kOffsetPadPrev         = 0x15Cu;
constexpr u32 kOffsetPointerPos      = 0x160u;
constexpr u32 kOffsetLayoutStep      = 0x1E8u;
constexpr u32 kOffsetShown           = 0xA90u;
constexpr u32 kOffsetActive          = 0x250u;
constexpr u32 kOffsetPadLatch        = 0xA92u;
constexpr u32 kOffsetTouchDecide     = 0xA94u;
constexpr u32 kOffsetPadLatchBytes   = 0xA84u;
constexpr u32 kOffsetPadSlotBase     = 0x2A0u;
constexpr u32 kOffsetMenuNode258     = 0x258u;
constexpr u32 kOffsetPlayerNode278   = 0x278u;
constexpr u32 kOffsetPlayerNode27C   = 0x27Cu;
constexpr u32 kViewLayerPauseMenu    = 0xEu;

constexpr u32 kPadDecideBit          = 0x2000u;
constexpr u32 kPadPlusBackMask       = 0x4800u;

constexpr u32 kOffsetMinIndex        = 0x204u;
constexpr u32 kOffsetMaxIndex        = 0x208u;
constexpr u32 kOffsetLastTouchIndex  = 0x1AA0u;
constexpr u32 kOffsetButtonCount     = 0x1AA4u;
constexpr u32 kOffsetControllerLeave = 0x201u;

constexpr s32 kIndexTouchLeft        = 0x19;
constexpr s32 kIndexTouchRight         = 0x1A;

constexpr uintptr_t kGuestAnimNameOut = 0x10216E34u;
constexpr uintptr_t kGuestStateIdOut = 0x10216DD4u;
constexpr uintptr_t kGuestStateIdBackOut = 0x10216DF4u;
constexpr uintptr_t kGuestStateIdControllerWindow = 0x10216D74u;
constexpr uintptr_t kGuestDefaultPointerPos = 0x105F9E54u;
constexpr uintptr_t kGuestButtonMgrDefaultPos = 0x105FA740u;
constexpr uintptr_t kGuestLayoutPointerOriginX = 0x100D108Cu;
constexpr uintptr_t kGuestLayoutPointerOriginY = 0x100D1090u;
constexpr uintptr_t kGuestSelectCursorInstance = 0x101D17A8u;
constexpr uintptr_t kGuestPadManager           = 0x101D2E40u;
constexpr uintptr_t kGuestControllerTable      = 0x101D1614u;
constexpr uintptr_t kGuestCoursePauseGame      = 0x101D830Cu;
constexpr uintptr_t kGuestPauseOpenSeMgr       = 0x101E7C84u;

constexpr u32 kCoursePauseSlotLayout           = 0x2Cu;
constexpr u32 kCoursePauseSlotNode40           = 0x40u;
constexpr u32 kCoursePauseSlotNode4C           = 0x4Cu;
constexpr u32 kCoursePauseSlotNode50           = 0x50u;
constexpr u32 kCoursePauseSlotCourseCalcLatch  = 0x2D69u;
constexpr u32 kCoursePauseSlotTouchLatch       = 0x2D58u;

constexpr uintptr_t kGuestAnimNameIn           = 0x1020DC80u;
constexpr uintptr_t kGuestAnimNamePause4       = 0x1020DC88u;
constexpr uintptr_t kGuestAnimNamePause2       = 0x1020DC90u;
constexpr uintptr_t kGuestPauseOpenSeName        = 0x100D97D0u;
constexpr uintptr_t kGuestPauseOpenAnimFrame   = 0x100D97CCu;

// Address: 0x0255E2C0
extern void sub_0255E2C0(void* course_pause_slot);

// Address: 0x029B5534
extern void sub_029B5534_(void* mgr, const void* name);

void callLayoutButtonOpenVirtual_(Multi2D* layout)
{
    void* host = *reinterpret_cast<void**>(reinterpret_cast<u8*>(layout) + 0x14u);
    if (host == nullptr)
        return;

    using OpenFn = void (*)(Multi2D*);
    OpenFn fn = reinterpret_cast<OpenFn>(
        *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(host) + 0xBCu));
    if (fn == nullptr)
        return;

    fn(layout);
}

// Address: 0x02513AFC
extern s32 sub_02513AFC_(void* mgr, s32 player_no);

// Address: 0x02513A54
extern s32 sub_02513A54_(void* mgr, s32 slot);

void sub_025BE1F0(PauseGame* self);

template<typename T>
const T& rodata(uintptr_t address)
{
    return *reinterpret_cast<const T*>(address);
}

template<typename T>
T& fieldAt(u8* base, u32 offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

Multi2D& layout(PauseGame* self)
{
    return **reinterpret_cast<Multi2D**>(reinterpret_cast<u8*>(self));
}

StateMethod& stateMethod(PauseGame* self)
{
    return *reinterpret_cast<StateMethod*>(reinterpret_cast<u8*>(self) + kOffsetStateMethod);
}

bool animatorLowBitFinished(nw::lyt::Animator* p_animator)
{
    u32 flags = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_animator) + 0x2Cu);
    return (flags & 1u) != 0u;
}

void* buttonAtIndex(u8* self, s32 index)
{
    void** button_array = fieldAt<void**>(self, kOffsetButtonArray);
    return button_array[index];
}

SelectCursor* selectCursor()
{
    return *reinterpret_cast<SelectCursor**>(kGuestSelectCursorInstance);
}

bool pauseMenuLayoutReady_(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    void* p_step = *reinterpret_cast<void**>(bytes + kOffsetLayoutStep);
    if (p_step == nullptr)
        return false;

    using ReadyFn = s32 (*)(PauseGame*);
    uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(p_step);
    ReadyFn ready = reinterpret_cast<ReadyFn>(vtable[0x14u / sizeof(uintptr_t)]);
    return ready(self) != 0;
}

u8 decideSubfe(u32 bit)
{
    const s32 a = static_cast<s32>(bit) - 1;
    const s32 b = static_cast<s32>(bit);
    return static_cast<u8>(static_cast<u32>(a) + static_cast<u32>(~b) + 1u);
}

void setNodePauseBit(u8* node, bool paused)
{
    if (node == nullptr)
        return;

    u8 flags = node[0x44u];
    if (paused)
        node[0x44u] = static_cast<u8>((flags & static_cast<u8>(~1u)) | 1u);
    else
        node[0x44u] = static_cast<u8>(flags & static_cast<u8>(~1u));
}

void callButtonVirtual(void* button_list, u32 button_offset, u32 arg)
{
    if (button_list == nullptr)
        return;

    void* button = *reinterpret_cast<void**>(reinterpret_cast<u8*>(button_list) + button_offset);
    if (button == nullptr)
        return;

    void* layout_obj = *reinterpret_cast<void**>(button);
    if (layout_obj == nullptr)
        return;

    uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(layout_obj);
    using ButtonFn = void (*)(void*, u32);
    ButtonFn fn = reinterpret_cast<ButtonFn>(vtable[0x54u / sizeof(uintptr_t)]);
    fn(layout_obj, arg);
}

UtilCursorButtonMgr& buttonMgr(u8* self)
{
    return fieldAt<UtilCursorButtonMgr>(self, kOffsetCursorButtonMgr);
}

bool padMsbBit(u32 pad, u32 msb_bit)
{
    return (pad & (1u << (31u - msb_bit))) != 0u;
}

bool buttonSelectable(void* button)
{
    if (button == nullptr)
        return false;
    const u32 flags = *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(button) + 4u);
    return (flags & 0x80000000u) != 0u;
}

bool tryMoveToIndex(u8* self, s32 index)
{
    if (!buttonSelectable(buttonAtIndex(self, index)))
        return false;

    fieldAt<s32>(self, kOffsetButtonIndex) = index;
    sub_025BE1F0(reinterpret_cast<PauseGame*>(self));
    buttonMgr(self).setSelectedButton(buttonAtIndex(self, index));
    return true;
}

bool searchSelectable(u8* self, s32 start, s32 end, s32 step)
{
    for (s32 index = start; index != end; index += step) {
        if (tryMoveToIndex(self, index))
            return true;
    }
    return false;
}

bool animatorMsbSet(nw::lyt::Animator* p_animator)
{
    if (p_animator == nullptr)
        return false;
    const u32 flags = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_animator) + 0x2Cu);
    return (flags & 0x80000000u) != 0u;
}

void runButtonVirtualFromButton(void* button, u32 arg)
{
    if (button == nullptr)
        return;

    void* layout_obj = *reinterpret_cast<void**>(reinterpret_cast<u8*>(button) + 0x1Cu);
    if (layout_obj == nullptr)
        return;

    uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(layout_obj);
    using ButtonFn = void (*)(void*, u32);
    ButtonFn fn = reinterpret_cast<ButtonFn>(vtable[0x54u / sizeof(uintptr_t)]);
    fn(layout_obj, arg);
}

void runListButtonVirtual(u8* self, s32 count, u32 arg)
{
    void** button_array = fieldAt<void**>(self, kOffsetButtonArray);
    for (s32 i = 0; i < count; ++i)
        runButtonVirtualFromButton(button_array[i], arg);
}

// Address: 0x025FDEB8
void sub_025FDEB8(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    const s32 index = fieldAt<s32>(bytes, kOffsetLastTouchIndex);
    fieldAt<s32>(bytes, kOffsetButtonIndex) = index;
    sub_025BE1F0(self);
    buttonMgr(bytes).setSelectedButton(buttonAtIndex(bytes, index));
}

// Address: 0x025FDF08
void sub_025FDF08(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    void* root = buttonAtIndex(bytes, 0);

    if (buttonSelectable(*reinterpret_cast<void**>(reinterpret_cast<u8*>(root) + 0x64u))) {
        fieldAt<s32>(bytes, kOffsetButtonIndex) = kIndexTouchLeft;
        sub_025BE1F0(self);
        buttonMgr(bytes).setSelectedButton(
            *reinterpret_cast<void**>(reinterpret_cast<u8*>(root) + 0x64u));
    }

    if (buttonSelectable(*reinterpret_cast<void**>(reinterpret_cast<u8*>(root) + 0x68u))) {
        fieldAt<s32>(bytes, kOffsetButtonIndex) = kIndexTouchRight;
        sub_025BE1F0(self);
        buttonMgr(bytes).setSelectedButton(
            *reinterpret_cast<void**>(reinterpret_cast<u8*>(root) + 0x68u));
    }
}

void enterBackOut(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    bytes[kOffsetControllerLeave] = 0;
    bytes[0xA93] = 1;
    bytes[0x1FF] = 1;

    const s32 count = fieldAt<s32>(bytes, kOffsetButtonCount);
    runListButtonVirtual(bytes, count, 0u);

    selectCursor()->hide(2);
    stateMethod(self).changeStateMethod(rodata<StateID>(kGuestStateIdBackOut));
}

void handleModeOne(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    void** button_array = fieldAt<void**>(bytes, kOffsetButtonArray);
    const s32 count = fieldAt<s32>(bytes, kOffsetButtonCount);

    for (s32 i = 0; i < count; ++i) {
        void* button = button_array[i];
        if (reinterpret_cast<const u8*>(button)[0x7Cu] == 0)
            continue;

        fieldAt<s32>(bytes, kOffsetButtonIndex) = i;
        sub_025BE1F0(self);
        buttonMgr(bytes).setSelectedButton(button);
        buttonMgr(bytes).triggerDecide();
        return;
    }
}

void handleModeThree(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    bytes[0x20D] = 1;
    bytes[kOffsetControllerLeave] = 0;

    const s32 count = fieldAt<s32>(bytes, kOffsetButtonCount);
    runListButtonVirtual(bytes, count, 0u);

    stateMethod(self).changeStateMethod(rodata<StateID>(kGuestStateIdOut));
    selectCursor()->pausePlayer(fieldAt<s32>(bytes, kOffsetCursorType));
}

void navigateRows(u8* self, u32 pad)
{
    const s32 index = fieldAt<s32>(self, kOffsetButtonIndex);
    if (padMsbBit(pad, 0x1bu)) {
        const s32 min_index = fieldAt<s32>(self, kOffsetMinIndex);
        if (index > min_index)
            searchSelectable(self, index - 1, min_index - 1, -1);
    } else if (padMsbBit(pad, 0x1au)) {
        const s32 max_index = fieldAt<s32>(self, kOffsetMaxIndex);
        if (index < max_index)
            searchSelectable(self, index + 1, max_index + 1, 1);
    }
}

} // namespace

// Address: 0x025BE1F0
void sub_025BE1F0(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    const s32 index = fieldAt<s32>(bytes, kOffsetButtonIndex);
    void* button = buttonAtIndex(bytes, index);
    const s32 cursor_type = fieldAt<s32>(bytes, kOffsetCursorType);
    SelectCursor* cursor = selectCursor();
    if (cursor == nullptr)
        return;

    cursor->set(button, cursor_type, 0);
}

// Address: 0x025BE214
void sub_025BE214(PauseGame* self)
{
    u8* bytes = reinterpret_cast<u8*>(self);

    if (fieldAt<u32>(bytes, kOffsetModeWord) != 0u)
        return;

    if (!pauseMenuLayoutReady_(self))
        return;

    const s32 index = fieldAt<s32>(bytes, kOffsetButtonIndex);
    void* button = buttonAtIndex(bytes, index);
    if (button == nullptr)
        return;

    const u32 button_kind = *reinterpret_cast<u32*>(button);
    if (button_kind == 4u || button_kind == 5u)
        return;

    MainGame* main_game = MainGame::instance();
    if (main_game == nullptr || reinterpret_cast<const u8*>(main_game)[0x14] == 0)
        return;

    SelectCursor* cursor = selectCursor();
    if (cursor == nullptr)
        return;

    const s32 cursor_index = fieldAt<s32>(bytes, kOffsetCursorIndex);
    fieldAt<s32>(bytes, kOffsetButtonIndex) = cursor_index;
    sub_025BE1F0(self);

    void* selected = buttonAtIndex(bytes, cursor_index);
    UtilCursorButtonMgr& mgr = fieldAt<UtilCursorButtonMgr>(bytes, kOffsetCursorButtonMgr);
    mgr.setSelectedButton(selected);

    const s32 cursor_type = fieldAt<s32>(bytes, kOffsetCursorType);
    cursor->hide(cursor_type);
    cursor->set(selected, cursor_type, 0);
}

// Address: 0x025BE30C
void sub_025BE30C(PauseGame* self, u8 decide_prev, u8 decide_now)
{
    u8* bytes = reinterpret_cast<u8*>(self);
    const u32 pointer_flag = fieldAt<u32>(bytes, kOffsetPointerFlag);
    const sead::Vector2f& pointer_src = (pointer_flag & 1u) != 0u
        ? fieldAt<sead::Vector2f>(bytes, kOffsetPointerPos)
        : rodata<sead::Vector2f>(kGuestDefaultPointerPos);

    const f32 origin_x = rodata<f32>(kGuestLayoutPointerOriginX);
    const f32 origin_y = rodata<f32>(kGuestLayoutPointerOriginY);
    sead::Vector2f pointer_pos;
    pointer_pos.x = pointer_src.x - origin_x;
    pointer_pos.y = -(pointer_src.y - origin_y);

    MainGame* main_game = MainGame::instance();
    const bool pointer_mode = main_game != nullptr && reinterpret_cast<const u8*>(main_game)[0x14] != 0;
    UtilCursorButtonMgr& mgr = fieldAt<UtilCursorButtonMgr>(bytes, kOffsetCursorButtonMgr);

    const bool use_pad_path = (pointer_mode && decide_prev != 0u) || (!pointer_mode && decide_now != 0u);
    if (!use_pad_path)
    {
        const sead::Vector2f& default_pos = rodata<sead::Vector2f>(kGuestButtonMgrDefaultPos);
        mgr.execute(0, 0, const_cast<sead::Vector2f*>(&default_pos));
        return;
    }

    const u32 pad_prev = fieldAt<u32>(bytes, kOffsetPadPrev);
    const u32 pad_now = fieldAt<u32>(bytes, kOffsetPadWord);
    const u32 bit_prev = (pad_prev & kPadDecideBit) != 0u ? 1u : 0u;
    const u32 bit_now = (pad_now & kPadDecideBit) != 0u ? 1u : 0u;
    mgr.execute(decideSubfe(bit_prev), decideSubfe(bit_now), &pointer_pos);
}

// Address: 0x025FD300
void PauseGame_preExecute(PauseGame* pause_game)
{
    u8* self = reinterpret_cast<u8*>(pause_game);

    MainGame* main_game = MainGame::instance();
    if (main_game != nullptr && reinterpret_cast<const u8*>(main_game)[0x14] != 0)
    {
        self[kOffsetPadLatch] = 0;
        return;
    }

    void* pad_mgr = *reinterpret_cast<void**>(kGuestPadManager);
    for (s32 player_no = 0; player_no < 4; ++player_no)
    {
        if (sub_02513AFC_(pad_mgr, player_no) != 0)
            return;
    }

    self[kOffsetPadLatch] = 0;
    u8* latch = self + kOffsetPadLatchBytes;
    const u8* bind_table = reinterpret_cast<const u8*>(kGuestControllerTable);
    u32 bind_offset = 0;

    for (s32 slot = 0; slot < 3; ++slot)
    {
        const u8* bind_entry = bind_table + 0x14u + bind_offset;
        if (bind_entry[0] == 0)
        {
            latch[slot] = 0;
        }
        else if (latch[slot] == 0)
            latch[slot] = static_cast<u8>(sub_02513A54_(pad_mgr, slot));

        if (latch[slot] != 0)
        {
            self[kOffsetPadLatch] = 1;
            break;
        }

        bind_offset += 0x194u;
    }
}

// Address: 0x025FD500
void PauseGame::executeCourse()
{
    u8* self = reinterpret_cast<u8*>(this);

    if (self[kOffsetShown] != 0)
    {
        PauseGame_preExecute(this);
        stateMethod(this).executeStateMethod();
        layout(this).animPlay(kViewLayerPauseMenu, nullptr);
    }

    u8 decide_prev;
    u8 decide_now;
    if (self[kOffsetTouchDecide] != 0)
    {
        decide_prev = 0;
        decide_now = 1;
    }
    else
    {
        decide_prev = 1;
        decide_now = 0;
    }
    sub_025BE30C(this, decide_prev, decide_now);
}

// Address: 0x025F3834
void PauseGame::execute()
{
    u8* self = reinterpret_cast<u8*>(this);

    if (self[kOffsetActive] != 0)
    {
        stateMethod(this).executeStateMethod();
        layout(this).animPlay(kViewLayerPauseMenu, nullptr);
    }

    sub_025BE30C(this, 1, 1);
}

// Address: 0x025FDF98
void PauseGame::executeState_Open()
{
    u8* self = reinterpret_cast<u8*>(this);
    u32 mode = fieldAt<u32>(self, kOffsetModeWord);
    s32 index = fieldAt<s32>(self, kOffsetButtonIndex);
    const u32 pad = fieldAt<u32>(self, kOffsetPadWord);

    if (mode == 0u) {
        const bool touch_row = (index == kIndexTouchLeft || index == kIndexTouchRight);
        if (touch_row && padMsbBit(pad, 0x19u)) {
            if (!animatorMsbSet(layout(this).getAnimatorObj(0))) {
                if (index == kIndexTouchLeft)
                    sub_025FDEB8(this);
                else
                    sub_025FDF08(this);
            }
        } else if (!touch_row) {
            navigateRows(self, pad);
        }

        if (padMsbBit(pad, 0x18u) && !animatorMsbSet(layout(this).getAnimatorObj(0)))
            sub_025FDF08(this);
        else if (padMsbBit(pad, 0x12u)) {
            buttonMgr(self).triggerDecide();
            self[0xA91] = 1;
        } else if ((pad & kPadPlusBackMask) != 0u)
            enterBackOut(this);

        mode = fieldAt<u32>(self, kOffsetModeWord);
    }

    if (mode == 1u)
        handleModeOne(this);
    else if (mode == 3u)
        handleModeThree(this);

    sub_025BE214(this);

    index = fieldAt<s32>(self, kOffsetButtonIndex);
    if (index != kIndexTouchLeft && index != kIndexTouchRight)
        fieldAt<s32>(self, kOffsetLastTouchIndex) = index;
}

// Address: 0x025FE4A4
void PauseGame::initializeState_BackOut()
{
    u8* self = reinterpret_cast<u8*>(this);
    const sead::SafeString& anim_out = rodata<sead::SafeString>(kGuestAnimNameOut);
    layout(this).animStartSetup(0, anim_out, static_cast<nw::lyt::Animator::PlayType>(0));
    self[0xA91] = 1;
}

// Address: 0x025FE4EC
void PauseGame::executeState_BackOut()
{
    nw::lyt::Animator* p_animator = layout(this).getAnimatorObj(0);
    if (animatorLowBitFinished(p_animator))
        stateMethod(this).changeStateMethod(rodata<StateID>(kGuestStateIdControllerWindow));
}

// Address: 0x025FE554
void PauseGame_setCourseCalcPause(PauseGame* pause_game, u32 paused)
{
    CourseTask* course_task = CourseTask::instance();
    if (course_task == nullptr)
        return;

    FieldGameMode::Mode game_mode = course_task->getGameMode();
    u8* self = reinterpret_cast<u8*>(pause_game);
    u8* node = nullptr;

    if (game_mode == FieldGameMode::cStory)
    {
        MainGame* main_game = MainGame::instance();
        if (main_game == nullptr)
            return;

        u32 replay_flag = *reinterpret_cast<const u32*>(
            reinterpret_cast<const u8*>(main_game) + 0x2Cu);
        if ((replay_flag & 0x40000000u) == 0)
            return;

        node = *reinterpret_cast<u8**>(self + 0x264u);
    }
    else if (game_mode == FieldGameMode::cBoostRush)
    {
        node = *reinterpret_cast<u8**>(self + 0x234u);
    }
    else if (game_mode == FieldGameMode::cChallenge)
    {
        MainGame* main_game = MainGame::instance();
        if (main_game == nullptr)
            return;

        u32 replay_flag = *reinterpret_cast<const u32*>(
            reinterpret_cast<const u8*>(main_game) + 0x2Cu);
        if ((replay_flag & 0x10000000u) != 0)
            node = *reinterpret_cast<u8**>(self + 0x254u);
        else
            node = *reinterpret_cast<u8**>(self + 0x23Cu);
    }
    else
    {
        return;
    }

    if (node == nullptr)
        return;

    u8 flags = node[0x44u];
    node[0x44u] = static_cast<u8>((flags & ~1u) | (paused & 1u));
}

// Address: 0x025FBA74
void PauseGame_setPlayerPause(PauseGame* pause_game, u32 paused)
{
    u8* self = reinterpret_cast<u8*>(pause_game);
    void* button_list = *reinterpret_cast<void**>(self + kOffsetButtonArray);
    void* pad_mgr = *reinterpret_cast<void**>(kGuestPadManager);
    s32 pad_route = pad_mgr != nullptr
        ? *reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(pad_mgr) + 0x14u)
        : 0;

    if (paused != 0)
    {
        MainGame* main_game = MainGame::instance();
        u32 replay_flag = main_game != nullptr
            ? *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(main_game) + 0x2Cu)
            : 0;
        if ((replay_flag & 0x10000000u) == 0)
            setNodePauseBit(*reinterpret_cast<u8**>(self + kOffsetMenuNode258), true);

        if (pad_route == 0)
        {
            setNodePauseBit(*reinterpret_cast<u8**>(self + kOffsetPlayerNode278), true);
            setNodePauseBit(*reinterpret_cast<u8**>(self + kOffsetPlayerNode27C), false);
            callButtonVirtual(button_list, 0x64u, 1);
        }
        else if (pad_route == 1)
        {
            setNodePauseBit(*reinterpret_cast<u8**>(self + kOffsetPlayerNode278), false);
            setNodePauseBit(*reinterpret_cast<u8**>(self + kOffsetPlayerNode27C), true);
            callButtonVirtual(button_list, 0x68u, 1);
        }
        return;
    }

    setNodePauseBit(*reinterpret_cast<u8**>(self + kOffsetPlayerNode278), false);
    setNodePauseBit(*reinterpret_cast<u8**>(self + kOffsetPlayerNode27C), false);
    callButtonVirtual(button_list, 0x64u, 0);
    callButtonVirtual(button_list, 0x68u, 0);
}

// Address: 0x0255E794
void sub_0255E794(void* course_pause_slot)
{
    u8* self = reinterpret_cast<u8*>(course_pause_slot);

    sub_0255E2C0(course_pause_slot);

    setNodePauseBit(*reinterpret_cast<u8**>(self + kCoursePauseSlotNode40), true);

    Multi2D* layout = *reinterpret_cast<Multi2D**>(self + kCoursePauseSlotLayout);
    const sead::SafeString& anim_in = rodata<sead::SafeString>(kGuestAnimNameIn);
    layout->animStartSetup(0, anim_in, static_cast<nw::lyt::Animator::PlayType>(0));

    void* se_mgr = *reinterpret_cast<void**>(kGuestPauseOpenSeMgr);
    sub_029B5534_(se_mgr, rodata<void>(kGuestPauseOpenSeName));

    setNodePauseBit(*reinterpret_cast<u8**>(self + kCoursePauseSlotNode4C), true);
    setNodePauseBit(*reinterpret_cast<u8**>(self + kCoursePauseSlotNode50), true);

    const f32 open_frame = rodata<f32>(kGuestPauseOpenAnimFrame);
    layout->setAnimFrame(0, open_frame);

    const sead::SafeString& anim_slot4 = rodata<sead::SafeString>(kGuestAnimNamePause4);
    const sead::SafeString& anim_slot2 = rodata<sead::SafeString>(kGuestAnimNamePause2);
    layout->animStartSetup(4, anim_slot4, static_cast<nw::lyt::Animator::PlayType>(0));
    layout->animStartSetup(2, anim_slot2, static_cast<nw::lyt::Animator::PlayType>(0));
    layout->setAnimFrame(2, open_frame);
    layout->setAnimFrame(2, open_frame);

    callLayoutButtonOpenVirtual_(layout);

    self[kCoursePauseSlotTouchLatch] = 0;
    if (self[kCoursePauseSlotCourseCalcLatch] != 0)
    {
        PauseGame* pause_game = *reinterpret_cast<PauseGame**>(kGuestCoursePauseGame);
        PauseGame_setCourseCalcPause(pause_game, 0);
    }

    self[1] = 0;
    self[0] = 1;
}

// Address: 0x0255E988
void sub_0255E988(void* course_pause_slot)
{
    u8* self = reinterpret_cast<u8*>(course_pause_slot);

    if (self[kCoursePauseSlotCourseCalcLatch] != 0)
    {
        PauseGame* pause_game = *reinterpret_cast<PauseGame**>(kGuestCoursePauseGame);
        PauseGame_setCourseCalcPause(pause_game, 1);
    }

    setNodePauseBit(*reinterpret_cast<u8**>(self + kCoursePauseSlotNode4C), false);
    setNodePauseBit(*reinterpret_cast<u8**>(self + kCoursePauseSlotNode50), false);

    Multi2D* layout = *reinterpret_cast<Multi2D**>(self + kCoursePauseSlotLayout);
    const sead::SafeString& anim_in = rodata<sead::SafeString>(kGuestAnimNameIn);
    layout->reverseAnimeStartSetup(0, anim_in, static_cast<nw::lyt::Animator::PlayType>(0));

    self[0] = 1;
}
