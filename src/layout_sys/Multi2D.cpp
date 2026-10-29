#include <layout_sys/AnimLayout.h>

#include <cstring>

namespace {

constexpr uintptr_t kRodata100E = 0x100E0000u;

f32 rodataF32_(u32 offset)
{
    return *reinterpret_cast<const f32*>(kRodata100E + offset);
}

// MainGame::sInstance                          Address: 0x101E7D7C
constexpr uintptr_t kMainGameInstancePtr = 0x101E7D7Cu;

Multi2D::Animation* animSlot_(Multi2D* self, s32 anim_index)
{
    u32 anim_count = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(self) + 0xe78);
    Multi2D::Animation* p_anim_slot =
        *reinterpret_cast<Multi2D::Animation**>(reinterpret_cast<u8*>(self) + 0xe7c);
    if (static_cast<u32>(anim_index) < anim_count)
        p_anim_slot = p_anim_slot + anim_index;
    return p_anim_slot;
}

void callAnimatorVtable24_(nw::lyt::Animator* p_animator, s32 arg)
{
    uintptr_t vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_animator) + 0x14);
    using Fn = void (*)(nw::lyt::Animator*, s32);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(vtable + 0x24));
    fn(p_animator, arg);
}

void callAnimatorVtable9c_(nw::lyt::Animator* p_animator, nw::lyt::Animator::PlayType type, f32 speed)
{
    uintptr_t vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_animator) + 0x14);
    using Fn = void (*)(nw::lyt::Animator*, nw::lyt::Animator::PlayType, f32);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(vtable + 0x9c));
    fn(p_animator, type, speed);
}

void callAnimatorVtableBc_(nw::lyt::Animator* p_animator)
{
    uintptr_t vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_animator) + 0x14);
    using Fn = void (*)(nw::lyt::Animator*);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(vtable + 0xbc));
    fn(p_animator);
}

void* getPaneRuntimeTypeNode_(nw::lyt::Pane* pane)
{
    uintptr_t holder = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(pane) + 0x8);
    uintptr_t holder_vtable = *reinterpret_cast<uintptr_t*>(holder);
    using Fn = void* (*)(nw::lyt::Pane*);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(holder_vtable + 0x14));
    return fn(pane);
}

bool typeNodeIsOrDerivesFrom_(void* type_node, uintptr_t target_typeinfo)
{
    while (type_node != nullptr)
    {
        if (reinterpret_cast<uintptr_t>(type_node) == target_typeinfo)
            return true;
        type_node = *reinterpret_cast<void**>(type_node);
    }
    return false;
}

void initPictureTypeinfoOnce_()
{
    constexpr uintptr_t kGuard = 0x101E9D74u;
    constexpr uintptr_t kAuxGuard = 0x101E9D64u;
    constexpr uintptr_t kAuxZero = 0x101EAF1Cu;
    constexpr uintptr_t kTypeinfoSlot = 0x101FAF20u;
    if (*reinterpret_cast<u32*>(kGuard) != 0)
        return;
    *reinterpret_cast<u32*>(kGuard) = 1;
    if (*reinterpret_cast<u32*>(kAuxGuard) == 0)
    {
        *reinterpret_cast<u32*>(kAuxGuard) = 1;
        *reinterpret_cast<u32*>(kAuxZero) = 0;
    }
    *reinterpret_cast<uintptr_t*>(kTypeinfoSlot) = kAuxZero;
}

void initTextBoxTypeinfoOnce_()
{
    constexpr uintptr_t kGuard = 0x101F9DC0u;
    constexpr uintptr_t kTypeinfoSlot = 0x101FAF28u;
    constexpr uintptr_t kTypeinfoRoot = 0x101FAF2Cu;
    constexpr uintptr_t kAuxGuard = 0x101F9D78u;
    constexpr uintptr_t kAuxGuard2 = 0x101E9D64u;
    constexpr uintptr_t kAuxZero = 0x101EAF1Cu;
    if (*reinterpret_cast<u32*>(kGuard) != 0)
        return;
    *reinterpret_cast<u32*>(kGuard) = 1;
    if (*reinterpret_cast<u32*>(kAuxGuard) == 0)
    {
        *reinterpret_cast<u32*>(kAuxGuard) = 1;
        if (*reinterpret_cast<u32*>(kAuxGuard2) == 0)
        {
            *reinterpret_cast<u32*>(kAuxGuard2) = 1;
            *reinterpret_cast<u32*>(kAuxZero) = 0;
        }
        *reinterpret_cast<uintptr_t*>(kTypeinfoRoot) = kAuxZero;
    }
    *reinterpret_cast<uintptr_t*>(kTypeinfoSlot) = kTypeinfoRoot;
}

void initBoundingTypeinfoOnce_()
{
    constexpr uintptr_t kGuard = 0x101E9D68u;
    constexpr uintptr_t kAuxGuard = 0x101E9D64u;
    constexpr uintptr_t kAuxZero = 0x101EAF1Cu;
    constexpr uintptr_t kTypeinfoSlot = 0x101FAF30u;
    if (*reinterpret_cast<u32*>(kGuard) != 0)
        return;
    *reinterpret_cast<u32*>(kGuard) = 1;
    if (*reinterpret_cast<u32*>(kAuxGuard) == 0)
    {
        *reinterpret_cast<u32*>(kAuxGuard) = 1;
        *reinterpret_cast<u32*>(kAuxZero) = 0;
    }
    *reinterpret_cast<uintptr_t*>(kTypeinfoSlot) = kAuxZero;
}

void initWindowTypeinfoOnce_()
{
    constexpr uintptr_t kGuard = 0x101E9D9Cu;
    constexpr uintptr_t kAuxGuard = 0x101E9D64u;
    constexpr uintptr_t kAuxZero = 0x101EAF1Cu;
    constexpr uintptr_t kTypeinfoSlot = 0x101FAF24u;
    if (*reinterpret_cast<u32*>(kGuard) != 0)
        return;
    *reinterpret_cast<u32*>(kGuard) = 1;
    if (*reinterpret_cast<u32*>(kAuxGuard) == 0)
    {
        *reinterpret_cast<u32*>(kAuxGuard) = 1;
        *reinterpret_cast<u32*>(kAuxZero) = 0;
    }
    *reinterpret_cast<uintptr_t*>(kTypeinfoSlot) = kAuxZero;
}

void initPartsTypeinfoOnce_()
{
    constexpr uintptr_t kGuard = 0x101E9D6Cu;
    constexpr uintptr_t kAuxGuard = 0x101E9D64u;
    constexpr uintptr_t kAuxZero = 0x101EAF1Cu;
    constexpr uintptr_t kTypeinfoSlot = 0x101FAF38u;
    if (*reinterpret_cast<u32*>(kGuard) != 0)
        return;
    *reinterpret_cast<u32*>(kGuard) = 1;
    if (*reinterpret_cast<u32*>(kAuxGuard) == 0)
    {
        *reinterpret_cast<u32*>(kAuxGuard) = 1;
        *reinterpret_cast<u32*>(kAuxZero) = 0;
    }
    *reinterpret_cast<uintptr_t*>(kTypeinfoSlot) = kAuxZero;
}

void initPartsLayoutTypeinfoOnce_()
{
    constexpr uintptr_t kGuard = 0x101E9D70u;
    constexpr uintptr_t kTypeinfoSlot = 0x101FAF34u;
    if (*reinterpret_cast<u32*>(kGuard) != 0)
        return;
    *reinterpret_cast<u32*>(kGuard) = 1;
    *reinterpret_cast<uintptr_t*>(kTypeinfoSlot) = 0;
}

template<typename T>
T* dynamicCastPane_(nw::lyt::Pane* pane, uintptr_t target_typeinfo, void (*init_fn)())
{
    init_fn();
    if (pane == nullptr)
        return nullptr;
    if (typeNodeIsOrDerivesFrom_(getPaneRuntimeTypeNode_(pane), target_typeinfo))
        return reinterpret_cast<T*>(pane);
    return nullptr;
}

// Address: 0x02AC4F40
void sub_02AC4F40(nw::lyt::DrawInfo* p_draw_info);

// Address: 0x029F1858
void* sub_029F1858(sead::Heap* heap, size_t size, u32 alignment);

// Address: 0x02679F58
LayoutObj* sub_02679F58(LayoutObj* p_obj);

// Address: 0x02AC51B0
u32 sub_02AC51B0(LayoutObj* p_obj, const char* str);

// Address: 0x02AB839C
void sub_02AB839C(void* out_key, u32 hash);

// Address: 0x02AB84A0
void* sub_02AB84A0(void* map, void* key);

// Address: 0x02AB84B8
void sub_02AB84B8(void* key);

// Address: 0x029F18DC
void* sub_029F18DC(u32 size, u32 alignment);

// Address: 0x02A07A54
void sub_02A07A54(void* lock);

// Address: 0x02A07AF8
void sub_02A07AF8(void* lock);

// Address: 0x02AC5554
nw::lyt::GroupArrayAnimator* sub_02AC5554(LayoutObj* p_obj, const char* anim_name, s32 zero, s32 zero2);

// Address: 0x02AC5338
nw::lyt::Animator* sub_02AC5338(LayoutObj* p_obj, const char* anim_name, s32 group_index, s32 zero);

// Address: 0x024F7944
void* sub_024F7944(void* map, const char* name);

// Address: 0x024F7A40
void* sub_024F7A40(void* map, const char* name, void* value);

// Address: 0x02AB6E5C
f64 sub_02AB6E5C(nw::lyt::Animator* p_animator);

// Address: 0x02AC5A8C
LayoutObj* sub_02AC5A8C(LayoutObj* p_obj, const char* name);

// Address: 0x0267A188
TextBox* sub_0267A188(PartsLayoutObj* p_parts, const sead::SafeString& name);

// Address: 0x0267A2E4
nw::lyt::Bounding* sub_0267A2E4(PartsLayoutObj* p_parts, const sead::SafeString& name);

// Address: 0x0267A414
nw::lyt::Picture* sub_0267A414(PartsLayoutObj* p_parts, const sead::SafeString& name);

// Address: 0x02681BFC
void sub_02681BFC(TextBox* p_text_box, s32 arg);

// Address: 0x029F1594
void* sub_029F1594(size_t size);

// Address: 0x02A0A2FC
void sub_02A0A2FC(sead::IDisposer* p_disposer);

// Address: 0x02AB9928
void sub_02AB9928(LayoutAllocator* p_allocator);

// Address: 0x02AA2960
void sub_02AA2960(TagProcessor* p_tag_processor);

// Address: 0x02AA29A0
void sub_02AA29A0(TagProcessor* p_tag_processor, s32 arg);

// Address: 0x02AB9B88
void sub_02AB9B88(nw::lyt::DrawInfo* p_draw_info, s32 arg);

// Address: 0x02A0A384
void sub_02A0A384(sead::IDisposer* p_disposer, s32 arg);

// Address: 0x029F1748
void sub_029F1748(void* p_obj);

// Address: 0x02AB9B9C
void sub_02AB9B9C(nw::lyt::DrawInfo* p_draw_info);

nw::lyt::Pane* findPaneOnLayout_(LayoutObj* p_obj, nw::lyt::Pane* p_root, const sead::SafeString& name)
{
    const char* cstr = name.cstr();
    uintptr_t layout_vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_root) + 0x8);
    layout_vtable = *reinterpret_cast<uintptr_t*>(layout_vtable);
    using Fn = nw::lyt::Pane* (*)(nw::lyt::Pane*, const char*, s32);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(layout_vtable + 0x5c));
    return fn(p_root, cstr, 1);
}

} // namespace

// Address: 0x026711C8
void Multi2D::createLayoutObj(sead::Heap* heap)
{
    mAllocator.setHeap(heap);
    sub_02AC4F40(&mDrawInfo);
    LayoutObj* p_obj = reinterpret_cast<LayoutObj*>(sub_029F1858(heap, 0x34, 4));
    if (p_obj == nullptr)
        p_obj = sub_02679F58(p_obj);
    mpObj = p_obj;
}

// Address: 0x0267122C
void Multi2D::animResRegister(const sead::SafeString& anim_res_name)
{
    const char* name_cstr = anim_res_name.cstr();
    u32 hash = sub_02AC51B0(mpObj, name_cstr);
    u8 key_buf[0x20];
    sub_02AB839C(key_buf, hash);
    void* p_map = &mAnimResMap;
    AnimRes* p_existing = reinterpret_cast<AnimRes*>(sub_02AB84A0(p_map, key_buf));
    if (p_existing == nullptr)
    {
        sub_02AB84B8(key_buf);
        return;
    }

    void* p_slot = sub_029F18DC(p_existing != nullptr ? 4u : 4u, 4u);
    sub_02A07A54(reinterpret_cast<void*>(0x101F8000u));
    anim_res_name.cstr();
    sub_02A07AF8(reinterpret_cast<void*>(0x101F8000u));

    nw::lyt::GroupArrayAnimator* p_group =
        sub_02AC5554(mpObj, name_cstr, 0, 0);
    sub_02A07A54(reinterpret_cast<void*>(0x101F8000u));
    sub_02A07AF8(reinterpret_cast<void*>(0x101F8000u));
    if (p_group == nullptr)
    {
        sub_02AB84B8(key_buf);
        return;
    }

    u32 group_num = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_group) + 0x1c);
    if (group_num == 1)
    {
        *reinterpret_cast<nw::lyt::GroupArrayAnimator**>(p_slot) = p_group;
        AnimRes* p_new = reinterpret_cast<AnimRes*>(sub_024F7944(p_map, name_cstr));
        if (p_new != nullptr)
        {
            AnimRes* p_anim_res = p_new + 1;
            if (p_anim_res != nullptr && reinterpret_cast<uintptr_t>(p_anim_res) != 0)
            {
                p_anim_res->p_group_array_animator = p_group;
                p_anim_res->p_animators = reinterpret_cast<nw::lyt::Animator**>(p_slot);
                p_anim_res->group_num = group_num;
            }
        }
        sub_02AB84B8(key_buf);
        return;
    }

    nw::lyt::Animator** p_animators = reinterpret_cast<nw::lyt::Animator**>(p_slot);
    if (group_num != 0)
    {
        s32 group_index = 0;
        nw::lyt::Animator** p_write = p_animators;
        do
        {
            nw::lyt::Animator* p_anim = sub_02AC5338(mpObj, name_cstr, group_index, 0);
            if (p_anim == nullptr)
            {
                sub_02AB84B8(key_buf);
                return;
            }
            *p_write = p_anim;
            p_write++;
            group_index++;
            group_num--;
        } while (group_num != 0);
    }

    AnimRes* p_bucket = reinterpret_cast<AnimRes*>(sub_024F7A40(p_map, name_cstr, p_slot));
    *reinterpret_cast<AnimRes**>(reinterpret_cast<u8*>(p_bucket)) = p_bucket;
    AnimRes* p_res = sub_024F7944(p_map, name_cstr) != nullptr
        ? reinterpret_cast<AnimRes*>(sub_024F7944(p_map, name_cstr)) + 1
        : nullptr;
    if (p_res != nullptr && reinterpret_cast<uintptr_t>(p_res) != 0)
    {
        p_res->p_group_array_animator = p_group;
        p_res->p_animators = p_animators;
        p_res->group_num = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_group) + 0x1c);
    }
    sub_02AB84B8(key_buf);
}


// Address: 0x0267153C
void Multi2D::animStartBaseSetup_(s32 anim_index, const sead::SafeString& anim_res_name,
    nw::lyt::Animator::PlayType type, f32 speed, nw::lyt::Animator* p_animator)
{
    Animation* p_slot = animSlot_(this, anim_index);
    p_slot->p_animator = p_animator;
    p_slot->anim_res_name = anim_res_name;

    nw::lyt::Animator* p_cur = p_slot->p_animator;
    if (p_cur != nullptr)
        callAnimatorVtable24_(p_cur, 1);

    p_slot->_c = rodataF32_(0x29F8);

    nw::lyt::Animator* p_bind = p_slot->p_animator;
    if (p_bind != nullptr)
        callAnimatorVtable9c_(p_bind, type, speed);

    p_slot->oneshot_playback_done = false;
}

// Address: 0x02671730
nw::lyt::Animator* Multi2D::reverseAnimeStartSetup(s32 anim_index, const sead::SafeString& anim_res_name,
    nw::lyt::Animator::PlayType type)
{
    Animation* p_slot = animSlot_(this, anim_index);
    nw::lyt::Animator* p_prev = p_slot->p_animator;
    if (p_prev != nullptr)
        callAnimatorVtable24_(p_prev, 0);

    AnimRes* p_anim_res = mAnimResMap.find(anim_res_name);
    nw::lyt::Animator* p_src;
    if (p_anim_res != nullptr)
        p_src = reinterpret_cast<nw::lyt::Animator*>(p_anim_res->p_group_array_animator);
    else
        p_src = *reinterpret_cast<nw::lyt::Animator**>(nullptr);

    animStartBaseSetup_(anim_index, anim_res_name, type, rodataF32_(0x2A00), p_src);
    return p_slot->p_animator;
}

// Address: 0x026717E0
nw::lyt::Animator* Multi2D::groupAnimeStartSetup(s32 anim_index, const sead::SafeString& anim_res_name,
    s32 group, nw::lyt::Animator::PlayType type)
{
    Animation* p_slot = animSlot_(this, anim_index);
    AnimRes* p_anim_res = mAnimResMap.find(anim_res_name);
    nw::lyt::Animator* p_group_anim = p_slot->p_animator;
    if (p_anim_res != nullptr)
    {
        if (static_cast<u32>(group) >= p_anim_res->group_num)
            return p_group_anim;
        p_group_anim = p_anim_res->p_animators[group];
    }
    if (p_group_anim != nullptr)
        callAnimatorVtable24_(p_group_anim, 0);

    nw::lyt::Animator* p_src = nullptr;
    if (p_anim_res != nullptr && static_cast<u32>(group) < p_anim_res->group_num)
        p_src = p_anim_res->p_animators[group];

    animStartBaseSetup_(anim_index, anim_res_name, type, rodataF32_(0x29FC), p_src);
    return p_group_anim;
}

// Address: 0x026718A8
nw::lyt::Animator* Multi2D::groupReverseAnimeStartSetup(s32 anim_index, const sead::SafeString& anim_res_name,
    s32 group, nw::lyt::Animator::PlayType type)
{
    Animation* p_slot = animSlot_(this, anim_index);
    AnimRes* p_anim_res = mAnimResMap.find(anim_res_name);
    nw::lyt::Animator* p_group_anim = p_slot->p_animator;
    if (p_anim_res != nullptr)
    {
        if (static_cast<u32>(group) >= p_anim_res->group_num)
            return p_group_anim;
        p_group_anim = p_anim_res->p_animators[group];
    }
    if (p_group_anim != nullptr)
        callAnimatorVtable24_(p_group_anim, 0);

    nw::lyt::Animator* p_src = nullptr;
    if (p_anim_res != nullptr && static_cast<u32>(group) < p_anim_res->group_num)
        p_src = p_anim_res->p_animators[group];

    animStartBaseSetup_(anim_index, anim_res_name, type, rodataF32_(0x2A00), p_src);
    return p_group_anim;
}

// Address: 0x02671970
s32 Multi2D::getAnimGroupIndex(const sead::SafeString& anim_res_name, const sead::SafeString& group_name) const
{
    const char* anim_cstr = anim_res_name.cstr();
    u32 hash = sub_02AC51B0(const_cast<LayoutObj*>(mpObj), anim_cstr);
    u8 key_buf[0x20];
    sub_02AB839C(key_buf, hash);
    void* p_map = const_cast<void*>(reinterpret_cast<const void*>(&mAnimResMap));
    const AnimRes* p_found = reinterpret_cast<const AnimRes*>(sub_02AB84A0(p_map, key_buf));
    sub_02AB84B8(key_buf);
    if (p_found == nullptr)
        return -1;

    const char* group_cstr = group_name.cstr();
    u32 group_num = p_found->group_num;
    if (group_num == 0)
        return -1;

    s32 index = 0;
    const u8* p_name_table = reinterpret_cast<const u8*>(p_found->p_group_array_animator) + 0x28;
    do
    {
        const char* entry_cstr = reinterpret_cast<const char*>(p_name_table);
        const char* a = group_cstr;
        const char* b = entry_cstr;
        while (*a != 0 && *a == *b)
        {
            ++a;
            ++b;
        }
        if (*a == 0 && *b == 0)
            return index;
        ++index;
        p_name_table += 0x1c;
        group_num--;
    } while (group_num != 0);

    return -1;
}

// Address: 0x02671AD4
const sead::SafeString& Multi2D::getAnimResName(s32 anim_index) const
{
    const Animation* p_slot = animSlot_(const_cast<Multi2D*>(this), anim_index);
    return p_slot->anim_res_name;
}

// Address: 0x02671AF4
nw::lyt::Animator* Multi2D::getAnimatorObj(s32 anim_index)
{
    Animation* p_slot = animSlot_(this, anim_index);
    return p_slot->p_animator;
}

// Address: 0x02671B14
void Multi2D::setAnimFrame(s32 anim_index, f32 frame)
{
    Animation* p_slot = animSlot_(this, anim_index);
    p_slot->_c = frame;
}

// Address: 0x02671B38
f32 Multi2D::getAnimFrame(s32 anim_index) const
{
    const Animation* p_slot = animSlot_(const_cast<Multi2D*>(this), anim_index);
    return p_slot->_c;
}

// Address: 0x02671B5C
void Multi2D::animEndSetup(s32 anim_index)
{
    Animation* p_slot = animSlot_(this, anim_index);
    nw::lyt::Animator* p_animator = p_slot->p_animator;
    if (p_animator != nullptr)
        callAnimatorVtableBc_(p_animator);

    p_slot = animSlot_(this, anim_index);
    p_animator = p_slot->p_animator;
    if (p_animator != nullptr)
        callAnimatorVtable24_(p_animator, 0);
}

// Address: 0x02671BEC
f32 Multi2D::getAnimFrameSize(s32 anim_index) const
{
    const Animation* p_slot = animSlot_(const_cast<Multi2D*>(this), anim_index);
    nw::lyt::Animator* p_animator = p_slot->p_animator;
    f64 size = sub_02AB6E5C(p_animator);
    return static_cast<f32>(size - static_cast<f64>(rodataF32_(0x2A08)));
}

// Address: 0x02671C48
nw::lyt::Pane* Multi2D::getRootPane()
{
    LayoutObj* p_obj = mpObj;
    nw::lyt::Pane* p_layout_root = *reinterpret_cast<nw::lyt::Pane**>(reinterpret_cast<u8*>(p_obj) + 0xc);
    return *reinterpret_cast<nw::lyt::Pane**>(reinterpret_cast<u8*>(p_layout_root) + 0xc);
}

// Address: 0x02671C54
void Multi2D::animResRegister(const sead::SafeString* p_anim_res_names, const s32& anim_res_num)
{
    s32 count = anim_res_num;
    for (s32 i = 0; i < count; ++i)
        animResRegister(p_anim_res_names[i]);
}


// Address: 0x02671CCC
nw::lyt::Pane* Multi2D::findPaneByName_(const sead::SafeString& name)
{
    LayoutObj* p_obj = mpObj;
    nw::lyt::Pane* p_root = *reinterpret_cast<nw::lyt::Pane**>(reinterpret_cast<u8*>(p_obj) + 0xc);
    return findPaneOnLayout_(p_obj, p_root, name);
}

// Address: 0x02671D3C
void Multi2D::NPaneRegister(nw::lyt::Pane** pp_panes, const sead::SafeString* p_names, const s32& num)
{
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = findPaneByName_(p_names[i]);
}

// Address: 0x02671DA4
nw::lyt::Picture* Multi2D::findPictureByName_(const sead::SafeString& name)
{
    nw::lyt::Pane* pane = findPaneByName_(name);
    return dynamicCastPane_<nw::lyt::Picture>(pane, 0x101FAF20u, initPictureTypeinfoOnce_);
}

// Address: 0x02671E90
void Multi2D::PPaneRegister(nw::lyt::Picture** pp_panes, const sead::SafeString* p_names, const s32& num)
{
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = findPictureByName_(p_names[i]);
}

// Address: 0x02671EF8
TextBox* Multi2D::findTextBoxByName_(const sead::SafeString& name)
{
    nw::lyt::Pane* pane = findPaneByName_(name);
    return dynamicCastPane_<TextBox>(pane, 0x101FAF28u, initTextBoxTypeinfoOnce_);
}

// Address: 0x02672010
void Multi2D::TPaneRegister(TextBox** pp_panes, const sead::SafeString* p_names, const s32& num)
{
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = findTextBoxByName_(p_names[i]);
}

// Address: 0x02672078
nw::lyt::Bounding* Multi2D::findBoundingByName_(const sead::SafeString& name)
{
    nw::lyt::Pane* pane = findPaneByName_(name);
    return dynamicCastPane_<nw::lyt::Bounding>(pane, 0x101FAF30u, initBoundingTypeinfoOnce_);
}

// Address: 0x02672164
void Multi2D::BPaneRegister(nw::lyt::Bounding** pp_panes, const sead::SafeString* p_names, const s32& num)
{
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = findBoundingByName_(p_names[i]);
}

// Address: 0x026721CC
nw::lyt::Window* Multi2D::findWindowByName_(const sead::SafeString& name)
{
    nw::lyt::Pane* pane = findPaneByName_(name);
    return dynamicCastPane_<nw::lyt::Window>(pane, 0x101FAF24u, initWindowTypeinfoOnce_);
}

// Address: 0x026722B8
void Multi2D::WPaneRegister(nw::lyt::Window** pp_panes, const sead::SafeString* p_names, const s32 num)
{
    for (s32 i = num; i > 0; --i)
        pp_panes[i - 1] = findWindowByName_(p_names[i - 1]);
}

// Address: 0x02672324
LayoutObj* Multi2D::getPartsLayoutObj_(const sead::SafeString& parts_pane_name)
{
    const char* name_cstr = parts_pane_name.cstr();
    LayoutObj* p_found = sub_02AC5A8C(mpObj, name_cstr);
    initPartsLayoutTypeinfoOnce_();
    if (p_found == nullptr)
        return nullptr;
    PartsLayoutObj* p_parts = reinterpret_cast<PartsLayoutObj*>(
        *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_found) + 0xac));
    if (p_parts == nullptr)
        return nullptr;
    if (typeNodeIsOrDerivesFrom_(getPaneRuntimeTypeNode_(reinterpret_cast<nw::lyt::Pane*>(p_parts)), 0x101FAF34u))
        return p_found;
    return nullptr;
}

// Address: 0x02672410
PartsLayoutObj* Multi2D::findPartsLayoutByPaneName_(const sead::SafeString& name)
{
    LayoutObj* p_obj = getPartsLayoutObj_(name);
    initPartsLayoutTypeinfoOnce_();
    if (p_obj == nullptr)
        return nullptr;
    PartsLayoutObj* p_parts = reinterpret_cast<PartsLayoutObj*>(
        *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_obj) + 0xac));
    if (p_parts == nullptr)
        return nullptr;
    if (typeNodeIsOrDerivesFrom_(getPaneRuntimeTypeNode_(reinterpret_cast<nw::lyt::Pane*>(p_parts)), 0x101FAF34u))
        return p_parts;
    return nullptr;
}

// Address: 0x026724D8
void Multi2D::PaRootPaneRegister(nw::lyt::Pane** pp_panes, const sead::SafeString* p_parts_pane_names, const s32& num)
{
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
    {
        PartsLayoutObj* p_parts = findPartsLayoutByPaneName_(p_parts_pane_names[i]);
        pp_panes[i] = *reinterpret_cast<nw::lyt::Pane**>(reinterpret_cast<u8*>(p_parts) + 0xc);
    }
}

// Address: 0x02672544
void Multi2D::PaTPaneRegister(TextBox** pp_panes, const sead::SafeString& parts_pane_name,
    const sead::SafeString* p_names, const s32& num)
{
    PartsLayoutObj* p_parts = findPartsLayoutByPaneName_(parts_pane_name);
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = sub_0267A188(p_parts, p_names[i]);
}

// Address: 0x026725B8
void Multi2D::PaBPaneRegister(nw::lyt::Bounding** pp_panes, const sead::SafeString& parts_pane_name,
    const sead::SafeString* p_names, const s32& num)
{
    PartsLayoutObj* p_parts = findPartsLayoutByPaneName_(parts_pane_name);
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = sub_0267A2E4(p_parts, p_names[i]);
}

// Address: 0x0267262C
void Multi2D::PaPPaneRegister(nw::lyt::Picture** pp_panes, const sead::SafeString& parts_pane_name,
    const sead::SafeString* p_names, const s32& num)
{
    PartsLayoutObj* p_parts = findPartsLayoutByPaneName_(parts_pane_name);
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = sub_0267A414(p_parts, p_names[i]);
}

// Address: 0x026726A0
void Multi2D::PaNPaneRegister(nw::lyt::Pane** pp_panes, const sead::SafeString& parts_pane_name,
    const sead::SafeString* p_names, const s32& num)
{
    PartsLayoutObj* p_parts = findPartsLayoutByPaneName_(parts_pane_name);
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
    {
        const sead::SafeString& child_name = p_names[i];
        const char* cstr = child_name.cstr();
        nw::lyt::Pane* p_root = *reinterpret_cast<nw::lyt::Pane**>(reinterpret_cast<u8*>(p_parts) + 0xc);
        pp_panes[i] = findPaneOnLayout_(reinterpret_cast<LayoutObj*>(p_parts), p_root, child_name);
        (void)cstr;
    }
}

// Address: 0x0267273C
nw::lyt::Parts* Multi2D::findPartsByName_(const sead::SafeString& name)
{
    nw::lyt::Pane* pane = findPaneByName_(name);
    return dynamicCastPane_<nw::lyt::Parts>(pane, 0x101FAF38u, initPartsTypeinfoOnce_);
}

// Address: 0x02672828
void Multi2D::PaPaneRegister(nw::lyt::Parts** pp_panes, const sead::SafeString* p_names, const s32& num)
{
    s32 count = num;
    for (s32 i = 0; i < count; ++i)
        pp_panes[i] = findPartsByName_(p_names[i]);
}


// Address: 0x02672890
void Multi2D::animPlay(const sead::Matrix34f& view_mtx, const sead::Matrix34f* p_model_mtx)
{
    u32 anim_count = mAnimBuffer.size();
    for (u32 anim_index = 0; anim_index < anim_count; ++anim_index)
    {
        Animation* p_slot = animSlot_(this, static_cast<s32>(anim_index));
        nw::lyt::Animator* p_animator = p_slot->p_animator;
        if (p_animator == nullptr)
            continue;
        if (p_slot->oneshot_playback_done)
            continue;
        if ((reinterpret_cast<u8*>(p_animator)[0x2c] & 1) == 0)
            continue;
        p_slot->oneshot_playback_done = true;
    }

    std::memcpy(reinterpret_cast<u8*>(this) + 0x58, &view_mtx, sizeof(view_mtx));

    bool has_model = p_model_mtx != nullptr;
    LayoutObj* p_obj = mpObj;
    if (has_model)
    {
        nw::lyt::Pane* p_root = getRootPane();
        u8 flags = reinterpret_cast<u8*>(p_root)[0x44];
        reinterpret_cast<u8*>(p_root)[0x44] = flags | 0x20;
        *reinterpret_cast<const sead::Matrix34f**>(reinterpret_cast<u8*>(p_root) + 0x78) = p_model_mtx;
    }

    uintptr_t layout_vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_obj) + 0x30);
    using CalcFn = void (*)(LayoutObj*);
    CalcFn calc = reinterpret_cast<CalcFn>(*reinterpret_cast<uintptr_t*>(layout_vtable + 0x4c));
    calc(p_obj);

    using DrawSetupFn = void (*)(LayoutObj*, nw::lyt::DrawInfo*, bool);
    DrawSetupFn draw_setup = reinterpret_cast<DrawSetupFn>(*reinterpret_cast<uintptr_t*>(layout_vtable + 0x5c));
    draw_setup(p_obj, &mDrawInfo, has_model);

    using DrawFn = void (*)(LayoutObj*);
    DrawFn draw = reinterpret_cast<DrawFn>(*reinterpret_cast<uintptr_t*>(layout_vtable + 0x54));
    draw(p_obj);

    if (has_model)
    {
        nw::lyt::Pane* p_root = getRootPane();
        u8 flags = reinterpret_cast<u8*>(p_root)[0x44];
        reinterpret_cast<u8*>(p_root)[0x44] = flags & static_cast<u8>(~0x20);
    }
}

// Address: 0x02672A4C
void Multi2D::animPlay(s32 view_layer_id, const sead::Matrix34f* p_model_mtx)
{
    void* p_layer_mgr = *reinterpret_cast<void**>(0x101E8A2Cu);
    u32 layer_count = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_layer_mgr) + 0xda8);
    void** pp_layers = *reinterpret_cast<void***>(reinterpret_cast<u8*>(p_layer_mgr) + 0xdac);
    if (static_cast<u32>(view_layer_id) >= layer_count)
        view_layer_id = 0;
    const sead::Matrix34f& view_mtx =
        *reinterpret_cast<const sead::Matrix34f*>(reinterpret_cast<u8*>(pp_layers[view_layer_id]) + 0x44);
    animPlay(view_mtx, p_model_mtx);
}

// Address: 0x02672A78
void Multi2D::setPlayerColor(const sead::SafeString& color_anim_res_name, s32 player_no, s32 view_layer_id,
    bool force_5th_color)
{
    if (player_no < 0)
        return;

    void* p_table = *reinterpret_cast<void**>(0x101D15F0u);
    if (p_table == nullptr)
        p_table = *reinterpret_cast<void**>(0x101D1604u);

    u8* p_entry = reinterpret_cast<u8*>(p_table);
    if (static_cast<u32>(player_no) >= 4u)
        p_entry = p_entry + 0x138;
    else
        p_entry = p_entry + 0x24 + static_cast<u32>(player_no) * 0x48;

    f32 speed_a = rodataF32_(0x29F8);
    f32 speed_b = rodataF32_(0x29FC);
    animStartSetup(0, color_anim_res_name, static_cast<nw::lyt::Animator::PlayType>(0));

    s32 entry_kind = *reinterpret_cast<s32*>(p_entry + 0x10);
    if (force_5th_color && *reinterpret_cast<s32*>(p_entry + 0xc) == 5)
        entry_kind = 5;

    switch (entry_kind)
    {
    case 1:
        setAnimFrame(0, speed_a);
        animPlay(view_layer_id, nullptr);
        animEndSetup(0);
        break;
    case 2:
        setAnimFrame(0, speed_b);
        animPlay(view_layer_id, nullptr);
        animEndSetup(0);
        break;
    case 3:
        setAnimFrame(0, rodataF32_(0x2A10));
        animPlay(view_layer_id, nullptr);
        animEndSetup(0);
        break;
    case 4:
        setAnimFrame(0, rodataF32_(0x2A14));
        animPlay(view_layer_id, nullptr);
        animEndSetup(0);
        break;
    case 5:
        setAnimFrame(0, rodataF32_(0x2A18));
        animPlay(view_layer_id, nullptr);
        animEndSetup(0);
        break;
    default:
        setAnimFrame(0, speed_b);
        animPlay(view_layer_id, nullptr);
        animEndSetup(0);
        break;
    }
}

// Address: 0x02672C90
void Multi2D::setTextBoxMessageFromTextID(TextBox& text_box, bool is_urcc)
{
    u8* main_game = *reinterpret_cast<u8**>(kMainGameInstancePtr);
    u8 saved = main_game[0x14];
    if (saved == 0)
        main_game[0x14] = is_urcc ? 1 : 0;
    sub_02681BFC(&text_box, 0);
    main_game[0x14] = saved;
}

// Address: 0x02672CE8
void Multi2D::initializeLayoutObj_()
{
    void* p_vtbl_thunk = *reinterpret_cast<void**>(0x101EAB54u);
    LayoutObj* p_obj = mpObj;
    *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(this) + 0xc4) =
        reinterpret_cast<uintptr_t>(p_vtbl_thunk) + 0x151c;
    uintptr_t layout_vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_obj) + 0x30);
    using InitFn = void (*)(LayoutObj*, TagProcessor*);
    InitFn init = reinterpret_cast<InitFn>(*reinterpret_cast<uintptr_t*>(layout_vtable + 0x6c));
    init(p_obj, &mTagProcessor);
}

// Address: 0x02672D14
void Multi2D::draw(const sead::Matrix44f& proj_mtx)
{
    sub_02AB9B9C(&mDrawInfo);
    LayoutObj* p_obj = mpObj;
    uintptr_t layout_vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_obj) + 0x30);
    using DrawFn = void (*)(LayoutObj*, nw::lyt::DrawInfo*);
    DrawFn draw = reinterpret_cast<DrawFn>(*reinterpret_cast<uintptr_t*>(layout_vtable + 0x64));
    draw(p_obj, &mDrawInfo);
}

// Address: 0x02672D5C
Multi2D::Multi2D(Animation* p_anim_buf, s32 anim_buf_size)
{
    sub_02A0A2FC(this);
    mpObj = nullptr;
    mAllocator = LayoutAllocator();
    *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(this) + 0xc) = 0x100E2A1Cu;
    sub_02AB9928(&mAllocator);

    if (reinterpret_cast<u8*>(this) + 0xe4 != nullptr)
    {
        void* p_map_mem = sub_029F1594(0xd94);
        if (p_map_mem == nullptr)
            return;
        *reinterpret_cast<u32*>(p_map_mem) = 0;
        *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_map_mem) + 0xc) = 0;
        *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_map_mem) + 0x10) = 0x20;
        *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_map_mem) + 4) =
            reinterpret_cast<void*>(reinterpret_cast<u8*>(p_map_mem) + 0x14);
    }

    if (reinterpret_cast<u8*>(this) + 0xe78 != nullptr)
    {
        void* p_buf = sub_029F1594(8);
        if (p_buf != nullptr && anim_buf_size > 0 && p_anim_buf != nullptr)
        {
            *reinterpret_cast<Animation**>(p_buf) = p_anim_buf;
            *reinterpret_cast<s32*>(reinterpret_cast<u8*>(p_buf) + 4) = anim_buf_size;
        }
    }

    sub_02AA2960(&mTagProcessor);
    *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(this) + 0xe80) = 0x100E4580u;
    *reinterpret_cast<f32*>(reinterpret_cast<u8*>(this) + 0xe84) = rodataF32_(0x29FC);
}

// Address: 0x02672E98
Multi2D::~Multi2D()
{
    if (this == nullptr)
        return;
    sub_02AA29A0(&mTagProcessor, 0);
    sub_02AB9B88(&mDrawInfo, 2);
    sub_02A0A384(this, 0);
    if ((reinterpret_cast<uintptr_t>(this) & 1) != 0)
        sub_029F1748(this);
}

