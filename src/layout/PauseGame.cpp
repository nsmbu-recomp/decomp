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

constexpr u32 kPadDecideBit          = 0x2000u;

constexpr uintptr_t kGuestAnimNameOut = 0x10216E34u;
constexpr uintptr_t kGuestStateIdControllerWindow = 0x10216D74u;
constexpr uintptr_t kGuestDefaultPointerPos = 0x105F9E54u;
constexpr uintptr_t kGuestButtonMgrDefaultPos = 0x105FA740u;
constexpr uintptr_t kGuestLayoutPointerOriginX = 0x100D108Cu;
constexpr uintptr_t kGuestLayoutPointerOriginY = 0x100D1090u;
constexpr uintptr_t kGuestSelectCursorInstance = 0x101D17A8u;

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
