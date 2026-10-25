#include <layout_sys/AnimLayout.h>

namespace {

void callAnimatorVtable24_(nw::lyt::Animator* p_animator, s32 arg)
{
    uintptr_t vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_animator) + 0x14);
    using Fn = void (*)(nw::lyt::Animator*, s32);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(vtable + 0x24));
    fn(p_animator, arg);
}

}

// Address: 0x02671680
nw::lyt::Animator* Multi2D::animStartSetup(s32 anim_index, const sead::SafeString& anim_res_name, nw::lyt::Animator::PlayType type)
{
    u32 anim_count = mAnimBuffer.size();
    Animation* p_anim_slot = mAnimBuffer.getBufferPtr();
    if (static_cast<u32>(anim_index) < anim_count)
        p_anim_slot = p_anim_slot + anim_index;

    nw::lyt::Animator* p_animator = p_anim_slot->p_animator;
    if (p_animator != nullptr)
        callAnimatorVtable24_(p_animator, 0);

    AnimRes* p_anim_res = mAnimResMap.find(anim_res_name);
    nw::lyt::Animator* p_src_animator;
    if (p_anim_res != nullptr)
        p_src_animator = reinterpret_cast<nw::lyt::Animator*>(p_anim_res->p_group_array_animator);
    else
        p_src_animator = *reinterpret_cast<nw::lyt::Animator**>(nullptr);

    animStartBaseSetup_(anim_index, anim_res_name, type, 1.0f, p_src_animator);

    return p_animator;
}
