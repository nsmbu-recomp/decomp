#include <layout/PauseGame.h>
#include <layout_sys/AnimLayout.h>
#include <state/StateID.h>
#include <state/StateMethod.h>

#include <prim/seadSafeString.h>

namespace {

constexpr u32 kOffsetStateMethod = 0x220u;

constexpr uintptr_t kGuestAnimNameOut = 0x10216E34u;
constexpr uintptr_t kGuestStateIdControllerWindow = 0x10216D74u;

template<typename T>
const T& rodata(uintptr_t address)
{
    return *reinterpret_cast<const T*>(address);
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

} // namespace

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
