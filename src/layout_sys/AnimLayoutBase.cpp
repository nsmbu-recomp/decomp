#include <layout_sys/AnimLayout.h>
#include <layout_sys/UtilCursorButtonMgr.h>

#include <gfx/seadGraphics.h>
#include <prim/seadSafeString.h>

class AnimText;
class ArcResAccMulti;
class FlexibleTextBox;

namespace {

// Address: 0x101DAB54
constexpr uintptr_t kResMgrInstanceSlot = 0x101DAB54u;

// Address: 0x101F8000
constexpr uintptr_t kGraphicsInstanceSlot = 0x101F8000u;

// Address: 0x100E2A2C
constexpr uintptr_t kAnimLayoutBaseVtbl = 0x100E2A2Cu;

// Address: 0x100E29D0
constexpr uintptr_t kPartsAnimLayoutBaseVtbl = 0x100E29D0u;

// Address: 0x626C7974
constexpr const char kBltyExtension[] = "blty";

// Address: 0x029F1594
void* sub_029F1594(u32 size);

// Address: 0x0267B1DC
ArcResAccMulti* sub_0267B1DC(void* p_mgr, const sead::SafeString& res_name, const sead::SafeString& acc_name);

// Address: 0x02AC4F40
void sub_02AC4F40(void* p_draw_info);

// Address: 0x0267573C
void* sub_0267573C(void* p_param, UtilCursorButtonMgr* p_cursor_btn_mgr, sead::OffsetList<FlexibleTextBox>* p_flexible_text_box_list, sead::OffsetList<AnimText>* p_anim_text_list);

// Address: 0x02AC514C
bool sub_02AC514C(LayoutObj* p_obj, ArcResAccMulti* p_res_acc, void* p_layout_root, void* p_stack, void* p_arg, s32 zero);

} // namespace

// Address: 0x02672F04
AnimLayoutBase::AnimLayoutBase(Animation* p_anim_buf, s32 anim_buf_size)
{
    u8* p_this = reinterpret_cast<u8*>(this);
    if (p_this == nullptr) {
        p_this = static_cast<u8*>(sub_029F1594(0xE8C));
        if (p_this == nullptr)
            return;
    } else {
        Multi2D(p_anim_buf, anim_buf_size);
    }

    *reinterpret_cast<u32*>(p_this + 0xE88) = 0;
    *reinterpret_cast<u32*>(p_this + 0xC) = static_cast<u32>(kAnimLayoutBaseVtbl);
}

// Address: 0x02672F80
AnimLayoutBase::~AnimLayoutBase()
{
    if (this == nullptr)
        return;

    u8* p_this = reinterpret_cast<u8*>(this);
    *reinterpret_cast<u32*>(p_this + 0xC) = static_cast<u32>(kAnimLayoutBaseVtbl);

    sub_02AC4F40(p_this + 0x14);

    ArcResAccMulti* p_res_acc = mpResAcc;
    if (p_res_acc != nullptr) {
        uintptr_t vtable = *reinterpret_cast<uintptr_t*>(p_res_acc);
        using DtorFn = void (*)(ArcResAccMulti*, s32);
        DtorFn dtor = reinterpret_cast<DtorFn>(*reinterpret_cast<uintptr_t*>(vtable + 0xC));
        dtor(p_res_acc, 3);
    }

    Multi2D::~Multi2D();
}

// Address: 0x02673008
void AnimLayoutBase::readResource(const sead::SafeString& res_name)
{
    void* p_mgr = *reinterpret_cast<void**>(kResMgrInstanceSlot);
    const sead::SafeString& acc_name =
        *reinterpret_cast<const sead::SafeString*>(reinterpret_cast<u8*>(this) + 0x18);
    ArcResAccMulti* p_acc = sub_0267B1DC(p_mgr, res_name, acc_name);
    if (mpResAcc == nullptr)
        mpResAcc = p_acc;
}

// Address: 0x02673050
void AnimLayoutBase::build(const sead::SafeString& lyt_filename, UtilCursorButtonMgr* p_cursor_btn_mgr, sead::OffsetList<FlexibleTextBox>* p_flexible_text_box_list, sead::OffsetList<AnimText>* p_anim_text_list)
{
    sead::Graphics** pp_graphics = reinterpret_cast<sead::Graphics**>(kGraphicsInstanceSlot);
    sead::Graphics* p_graphics = *pp_graphics;
    p_graphics->lockDrawContext();

    ArcResAccMulti* p_res_acc = mpResAcc;
    if (p_res_acc != nullptr) {
        const sead::SafeString* p_name = &lyt_filename;
        uintptr_t name_vtable = *reinterpret_cast<uintptr_t*>(const_cast<sead::SafeString*>(p_name));
        using NameFn = void (*)(const sead::SafeString*);
        NameFn name_fn = reinterpret_cast<NameFn>(*reinterpret_cast<uintptr_t*>(name_vtable + 0x14));
        name_fn(p_name);

        void* p_res_root = *reinterpret_cast<void**>(p_res_acc);
        uintptr_t res_vtable = *reinterpret_cast<uintptr_t*>(p_res_root);
        using BuildFn = void* (*)(ArcResAccMulti*, const char*, const sead::SafeString*, s32);
        BuildFn build_fn = reinterpret_cast<BuildFn>(*reinterpret_cast<uintptr_t*>(res_vtable + 0x14));
        void* p_layout = build_fn(p_res_acc, kBltyExtension, p_name, 0);

        if (p_layout != nullptr) {
            alignas(4) u8 stack_param[0x10];
            sub_0267573C(stack_param, p_cursor_btn_mgr, p_flexible_text_box_list, p_anim_text_list);

            LayoutObj* p_obj = *reinterpret_cast<LayoutObj**>(reinterpret_cast<u8*>(this) + 0x10);
            bool ok = sub_02AC514C(p_obj, p_res_acc, p_layout, stack_param, nullptr, 0);
            if (ok) {
                p_graphics = *pp_graphics;
                p_graphics->unlockDrawContext();
                initializeLayoutObj_();
                return;
            }
        }
    }
}

// Address: 0x02673124
PartsAnimLayoutBase::PartsAnimLayoutBase(Animation* p_anim_buf, s32 anim_buf_size)
{
    u8* p_this = reinterpret_cast<u8*>(this);
    if (p_this == nullptr) {
        p_this = static_cast<u8*>(sub_029F1594(0xE88));
        if (p_this == nullptr)
            return;
    } else {
        Multi2D(p_anim_buf, anim_buf_size);
    }

    *reinterpret_cast<u32*>(p_this + 0xC) = static_cast<u32>(kPartsAnimLayoutBaseVtbl);
}

// Address: 0x02673198
void PartsAnimLayoutBase::setLayoutObj_(LayoutObj* p_obj)
{
    *reinterpret_cast<LayoutObj**>(reinterpret_cast<u8*>(this) + 0x10) = p_obj;
    initializeLayoutObj_();
}

// Address: 0x026731A0
void PartsAnimLayoutBase::createLayoutObj(const Multi2D& parent, const sead::SafeString& parts_name)
{
    LayoutObj* p_obj = parent.getPartsLayoutObj_(parts_name);
    setLayoutObj_(p_obj);
}
