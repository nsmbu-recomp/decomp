#include <layout/ChangePlayerNum.h>
#include <layout/PaCursor.h>
#include <layout_sys/AnimLayout.h>
#include <layout_sys/UtilCursorButtonMgr.h>
#include <state/StateID.h>
#include <state/StateMethod.h>

#include <layer/aglRenderInfo.h>

#include <math/seadMatrix.h>
#include <nw/lyt/lyt_Pane.h>
#include <prim/seadSafeString.h>

namespace {

constexpr u32 kOffsetLayout            = 0x24u;
constexpr u32 kOffsetEc4Block          = 0xEC4u;
constexpr u32 kOffsetPaCursor          = 0x1D8Cu;
constexpr u32 kOffsetCursorWork        = 0x1DB0u;
constexpr u32 kOffsetPlayerBtnBase     = 0x20A0u;
constexpr u32 kOffsetPlayerBtnStride   = 0x0B84u;
constexpr u32 kOffsetPlayerBtnCount    = 5u;
constexpr u32 kOffsetCharSelectBase    = 0x5A34u;
constexpr u32 kOffsetCharFlagBase      = 0x5A44u;
constexpr u32 kOffsetCharUsedBase      = 0x5A58u;
constexpr u32 kOffsetPlayerSlotBase    = 0x5A60u;
constexpr u32 kOffsetPlayerSlotStride  = 0x15B4u;
constexpr u32 kOffsetPlayerSlotCount   = 4u;
constexpr u32 kOffsetTailTextId        = 0x660u;
constexpr u32 kOffsetTailScale         = 0x664u;
constexpr u32 kOffsetTailPadConnected  = 0x668u;
constexpr u32 kOffsetTailField669      = 0x669u;
constexpr u32 kOffsetTailField66A      = 0x66Au;
constexpr u32 kOffsetTailField66B      = 0x66Bu;
constexpr u32 kOffsetTailField66C      = 0x66Cu;
constexpr u32 kOffsetTailField66D      = 0x66Du;
constexpr u32 kOffsetTailMsgRef        = 0x140u;

constexpr uintptr_t kGuestVtableChangePlayerNum     = 0x100C8CE0u;
constexpr uintptr_t kGuestStateIdInitial            = 0x1020D3D4u;
constexpr uintptr_t kGuestStateIdExecuteCloseReq    = 0x1020D454u;
constexpr uintptr_t kGuestLayoutResPath             = 0x100C73FCu;
constexpr uintptr_t kGuestLayoutBuildResA         = 0x100C8B70u;
constexpr uintptr_t kGuestLayoutBuildResB         = 0x100C8BE0u;
constexpr uintptr_t kGuestAnimResRegister           = 0x1020D398u;
constexpr uintptr_t kGuestAnimResTable              = 0x1020D368u;
constexpr uintptr_t kGuestAnimResFlag             = 0x1020D3C8u;
constexpr uintptr_t kGuestAnimResFlag2            = 0x1020D3CCu;
constexpr uintptr_t kGuestPaCursorParentName        = 0x100C7324u;
constexpr uintptr_t kGuestPaCursorField1FCC         = 0x100C7478u;
constexpr uintptr_t kGuestSaveApplyMgr              = 0x101E7D9Cu;
constexpr uintptr_t kGuestDefaultScale              = 0x100C8B6Cu;
constexpr uintptr_t kGuestCompareScale              = 0x100C8C2Cu;
constexpr uintptr_t kGuestPlayerBtnNameTable        = 0x100C8B58u;
constexpr uintptr_t kGuestCharSelectInitTable       = 0x1021D4B4u;
constexpr uintptr_t kGuestCharSelectInitTable2      = 0x1021D4BCu;

template<typename T>
T& at(u8* base, u32 offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

template<typename T>
const T& rodata(uintptr_t address)
{
    return *reinterpret_cast<const T*>(address);
}

u8* tailFields(u8* self)
{
    return self + 0xB130u;
}

StateMethod& stateMethod(u8* self)
{
    return at<StateMethod>(self, 0x10u);
}

Multi2D& layout(u8* self)
{
    return at<Multi2D>(self, kOffsetLayout);
}

PaCursor& paCursor(u8* self)
{
    return at<PaCursor>(self, kOffsetPaCursor);
}

// Address: 0x02AFA350
extern void seadHeapConstruct_(void* buf, u32 size, void* ctor, u32 ctorArg1, u32 ctorArg2, void* ctorArg3);

// Address: 0x02AFA490
extern void seadHeapDestruct_(void* buf, u32 size, void* dtor, u32 dtorArg, u32 zero, u32 zero2);

// Address: 0x02AFA52C
extern void sub_02AFA52C_(void* guestFunc, void* outDesc);

// Address: 0x025F9E28
extern void sub_025F9E28_(void* p_at_ec4);

// Address: 0x025F9F44
extern void sub_025F9F44_(void* p_at_ec4, s32 flag);

// Address: 0x025F9FCC
extern void sub_025F9FCC_(void* p_at_ec4, Multi2D* p_layout);

// Address: 0x025F8284
extern void sub_025F8284_(PaCursor* p_cursor, const sead::SafeString& name, s32 size);

// Address: 0x025F8E9C
extern void sub_025F8E9C_(PaCursor* p_cursor, s32 arg);

// Address: 0x025F8F2C
extern void sub_025F8F2C_(PaCursor* p_cursor, const sead::SafeString& name, const sead::SafeString& arg);

// Address: 0x025F86FC
extern void sub_025F86FC_(PaCursor* p_cursor, const sead::SafeString& name, s32 arg);

// Address: 0x025F85B8
extern void sub_025F85B8_(PaCursor* p_cursor);

// Address: 0x025FA094
extern void sub_025FA094_(void* p_at_ec4);

// Address: 0x0257298
extern void sub_0257298_(void* p_player_slot);

// Address: 0x02541B0
extern void sub_02541B0_(void* p_at_650, s32 index);

// Address: 0x024C6DC0
extern void sub_024C6DC0_(void* p_player_btn, const sead::SafeString& name, s32 player_idx, s32 arg, s32 arg2);

// Address: 0x025542E0
extern void sub_025542E0_(ChangePlayerNum* self);

// Address: 0x02554480
extern void sub_02554480_(ChangePlayerNum* self);

// Address: 0x02557E48
extern void sub_02557E48_(void* p_player_slot);

// Address: 0x025580B8
extern f32 sub_025580B8_(void* p_player_slot);

// Address: 0x02558070
extern void sub_02558070_(void* p_player_slot, const agl::lyr::RenderInfo& render_info);

// Address: 0x024C7764
extern void sub_024C7764_(void* p_player_btn);

// Address: 0x024C78D4
extern void sub_024C78D4_(void* p_player_btn);

// Address: 0x02682C2C
extern void sub_02682C2C_(void* p_at_1db0, s32 zero);

// Address: 0x02513AFC
extern s32 sub_02513AFC_(void* mgr, s32 arg);

// Address: 0x02513A54
extern void sub_02513A54_(void* mgr, s32 arg);

// Address: 0x02513D8C
extern void sub_02513D8C_(void* mgr);

// Address: 0x029C57A8
extern void sub_029C57A8_();

// Address: 0x02A05A9C
extern void* sub_02A05A9C_(const agl::lyr::RenderInfo& render_info);

// Address: 0x02672C90 (ChangePlayerNum passes a message id in r4)
extern void multi2dSetTextBoxMessageFromTextID_(Multi2D* p_layout, u32 text_id);

void initPaCursorForLayout(u8* self)
{
    PaCursor& cursor = paCursor(self);
    const sead::SafeString& parent_name = rodata<sead::SafeString>(kGuestPaCursorParentName);
    sub_025F8284_(&cursor, parent_name, 0x18);
    at<u32>(self, kOffsetPaCursor + 0x240u) = static_cast<u32>(kGuestPaCursorField1FCC);
    sub_025F8E9C_(&cursor, 9);
    sub_025F8F2C_(&cursor, parent_name, rodata<sead::SafeString>(0x100C7468u));
    sub_025F86FC_(&cursor, parent_name, 1);
}

void registerLayoutAnimResources(u8* self, const sead::SafeString* p_anim_table, s32 count)
{
    s32 stack_count = count;
    layout(self).animResRegister(p_anim_table, stack_count);
    initPaCursorForLayout(self);
}

} // namespace

CREATE_STATE_ID(ChangePlayerNum, CloseRequest)

// Address: 0x02553AB8
ChangePlayerNum::ChangePlayerNum()
    : mStateMgr(*this, rodata<StateID>(kGuestStateIdInitial))
    , mLayout()
    , mIsOpen(false)
    , _b799(0)
    , mIsVisible(false)
{
    u8* self = reinterpret_cast<u8*>(this);

    *reinterpret_cast<uintptr_t*>(self + 0xB7A0u) = kGuestVtableChangePlayerNum;

    sub_025F9E28_(self + kOffsetEc4Block);
    new (self + kOffsetPaCursor) PaCursor();

    seadHeapConstruct_(self + kOffsetPlayerBtnBase, 0x3994u, reinterpret_cast<void*>(0x024C69F8u), 5u, 0x0B84u, nullptr);
    seadHeapConstruct_(self + kOffsetPlayerSlotBase, 0x56D0u, reinterpret_cast<void*>(0x02556FF8u), 4u, 0x15B4u, nullptr);
    seadHeapConstruct_(self + 0x650u, 0x650u, reinterpret_cast<void*>(0x02514150u), 4u, 0x194u, nullptr);

    u8* tail = tailFields(self);
    at<u8>(tail, kOffsetTailPadConnected) = 0;
    at<u8>(tail, kOffsetTailField66C) = 0;
    at<u8>(tail, kOffsetTailField66D) = 0;
    at<u8>(tail, kOffsetTailField66B) = 0;
    at<u8>(tail, kOffsetTailField669) = 0;
    at<u8>(tail, kOffsetTailField66A) = 0;
    at<f32>(tail, kOffsetTailScale) = rodata<f32>(kGuestDefaultScale);

    u8* p_byte = self + 0x650u;
    for (s32 i = 0; i < 4; ++i)
        p_byte[i] = 0;
}

// Address: 0x02553D24
ChangePlayerNum::~ChangePlayerNum()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self == nullptr)
        return;

    seadHeapDestruct_(self + 0x650u, 0x194u, reinterpret_cast<void*>(0x025140E8u), 4u, 0u, 0u);
    seadHeapDestruct_(self + kOffsetPlayerSlotBase, 0x15B4u, reinterpret_cast<void*>(0x025571F0u), 4u, 0u, 0u);
    seadHeapDestruct_(self + kOffsetPlayerBtnBase, 0x0B84u, reinterpret_cast<void*>(0x02556A04u), 5u, 0u, 0u);

    paCursor(self).~PaCursor();
    sub_025F9F44_(self + kOffsetEc4Block, 2);

    mLayout.~AnimLayoutBase();

    if ((reinterpret_cast<uintptr_t>(this) & 1u) != 0u)
    {
        seadHeapDestruct_(self, 0u, nullptr, 0u, 0u, 0u);
    }
}

// Address: 0x02553E28
void ChangePlayerNum::create()
{
    u8* self = reinterpret_cast<u8*>(this);

    layout(self).createLayoutObj(nullptr);
    mLayout.readResource(rodata<sead::SafeString>(kGuestLayoutResPath));

    u8* save_apply = *reinterpret_cast<u8**>(kGuestSaveApplyMgr);
    bool use_drc_layout = (save_apply[0x11] != 0);
    auto* p_cursor_mgr = reinterpret_cast<UtilCursorButtonMgr*>(self + kOffsetCursorWork);

    if (use_drc_layout)
    {
        mLayout.build(
            rodata<sead::SafeString>(kGuestLayoutBuildResA),
            p_cursor_mgr,
            nullptr,
            nullptr);
    }
    else
    {
        mLayout.build(
            rodata<sead::SafeString>(kGuestLayoutBuildResB),
            p_cursor_mgr,
            nullptr,
            nullptr);
    }

    volatile u32* p_anim_res_ready = reinterpret_cast<volatile u32*>(kGuestAnimResFlag);
    if (*p_anim_res_ready == 0u)
    {
        void* anim_desc = reinterpret_cast<void*>(kGuestAnimResTable);
        sub_02AFA52C_(reinterpret_cast<void*>(0x101D3EC8u), anim_desc);

        const sead::SafeString* p_names = reinterpret_cast<const sead::SafeString*>(anim_desc);
        s32 count = 2;
        registerLayoutAnimResources(self, p_names, count);
    }
    else
    {
        const sead::SafeString& anim_name = rodata<sead::SafeString>(kGuestAnimResRegister);
        s32 count = 2;
        layout(self).animResRegister(&anim_name, count);
        initPaCursorForLayout(self);
    }

    volatile u32* p_second_flag = reinterpret_cast<volatile u32*>(kGuestAnimResFlag2);
    if (*p_second_flag != 0u)
    {
        nw::lyt::Pane** pp_pane = reinterpret_cast<nw::lyt::Pane**>(self + 0x650u);
        s32 pane_count = 4;
        layout(self).NPaneRegister(pp_pane, reinterpret_cast<const sead::SafeString*>(0x1020D390u), pane_count);
    }
    else
    {
        *reinterpret_cast<u32*>(kGuestAnimResFlag2) = 1u;
        void* anim_desc = reinterpret_cast<void*>(kGuestAnimResTable - 0x30u);
        sub_02AFA52C_(reinterpret_cast<void*>(0x101D3EE0u), anim_desc);
        nw::lyt::TextBox** pp_text = reinterpret_cast<nw::lyt::TextBox**>(tailFields(self) + kOffsetTailTextId);
        const sead::SafeString& parts_name = rodata<sead::SafeString>(0x100C8BD4u);
        s32 text_count = 1;
        layout(self).PaTPaneRegister(pp_text, parts_name, reinterpret_cast<const sead::SafeString*>(0x1020D390u), text_count);
    }

    sub_025F9FCC_(self + kOffsetEc4Block, &layout(self));

    u8* p_at_650 = self + 0x650u;
    for (s32 i = 0; i < 4; ++i)
    {
        void* p_entry = p_at_650 + i * 0x194u;
        if (static_cast<u32>(i) >= 4u)
            p_entry = p_at_650;
        sub_02541B0_(p_entry, i);
    }

    u8* p_slots = self + kOffsetPlayerSlotBase;
    for (s32 i = 0; i < static_cast<s32>(kOffsetPlayerSlotCount); ++i)
    {
        void* p_slot = p_slots + i * kOffsetPlayerSlotStride;
        if (static_cast<u32>(i) >= kOffsetPlayerSlotCount)
            p_slot = p_slots;
        sub_0257298_(p_slot);
    }

    u8* p_btns = self + kOffsetPlayerBtnBase;
    u32 name_idx = 0u;
    for (s32 i = 0; i < static_cast<s32>(kOffsetPlayerBtnCount); ++i)
    {
        void* p_btn = p_btns;
        u32 stride_idx = name_idx;
        if (static_cast<u32>(i) < kOffsetPlayerBtnCount)
            p_btn = p_btns + stride_idx * 4u;
        s32 player_idx = i - (stride_idx / 5) * 5;
        const sead::SafeString& btn_name =
            rodata<sead::SafeString>(kGuestPlayerBtnNameTable + stride_idx * 4u);
        sub_024C6DC0_(p_btn, btn_name, player_idx, 0, 0);
        ++name_idx;
        p_btns += kOffsetPlayerBtnStride;
    }
}

// Address: 0x025546B8
void ChangePlayerNum::execute()
{
    u8* self = reinterpret_cast<u8*>(this);

    if (!mIsOpen)
        return;

    u8* tail = tailFields(self);
    void* pad_mgr = *reinterpret_cast<void**>(0x101D2E40u);

    u8 prev_pad = at<u8>(tail, kOffsetTailField66D);
    s32 pad_result = sub_02513AFC_(pad_mgr, 0);
    if (pad_result == 0)
    {
        if (at<u8>(tail, kOffsetTailField66D) != 0u)
        {
            void* p_obj = at<void*>(tail, kOffsetTailMsgRef);
            void* p_vtbl_obj = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_obj) + 0x158u);
            using Fn = s32 (*)();
            Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_vtbl_obj) + 0x2Cu));
            if (fn() != 0)
            {
                sub_02513A54_(pad_mgr, 0);
            }
        }
    }
    else
    {
        at<u8>(tail, kOffsetTailField66D) = static_cast<u8>(pad_result);
    }

    stateMethod(self).executeStateMethod();

    u8 cur_pad = at<u8>(tail, kOffsetTailField66D);
    if (prev_pad != cur_pad)
    {
        u32 text_id = at<u32>(tail, kOffsetTailTextId);
        multi2dSetTextBoxMessageFromTextID_(&layout(self), text_id);
    }

    sub_025F85B8_(&paCursor(self));
    sub_025FA094_(self + kOffsetEc4Block);
    layout(self).animPlay(0xEu, nullptr);
    sub_025542E0_(this);

    u8* p_slots = self + kOffsetPlayerSlotBase;
    for (s32 i = 0; i < static_cast<s32>(kOffsetPlayerSlotCount); ++i)
    {
        void* p_slot = p_slots + i * kOffsetPlayerSlotStride;
        if (static_cast<u32>(i) >= kOffsetPlayerSlotCount)
            p_slot = p_slots;
        sub_02557E48_(p_slot);
    }

    f32 default_scale = rodata<f32>(kGuestCompareScale);
    at<f32>(tail, kOffsetTailScale) = default_scale;

    u8* p_slot_iter = p_slots;
    for (s32 i = 0; i < static_cast<s32>(kOffsetPlayerSlotCount); ++i)
    {
        void* p_slot = p_slot_iter;
        if (static_cast<u32>(i) >= kOffsetPlayerSlotCount)
            p_slot = p_slots;
        f32 scale = sub_025580B8_(p_slot);
        if (scale != default_scale)
        {
            at<f32>(tail, kOffsetTailScale) = scale;
            sub_02554480_(this);
            u8* p_btns = self + kOffsetPlayerBtnBase;
            for (s32 j = 0; j < static_cast<s32>(kOffsetPlayerBtnCount); ++j)
            {
                void* p_btn = p_btns + j * kOffsetPlayerBtnStride;
                if (static_cast<u32>(j) >= kOffsetPlayerBtnCount)
                    p_btn = p_btns;
                sub_024C7764_(p_btn);
            }
            break;
        }
        p_slot_iter += kOffsetPlayerSlotStride;
    }

    if (at<u8>(tail, kOffsetTailField66C) != 0u)
    {
        u32 vtable_word = *reinterpret_cast<u32*>(tail);
        if ((vtable_word & (1u << 11)) != 0u)
        {
            sub_029C57A8_();
            sub_02513D8C_(*reinterpret_cast<void**>(0x101D2E40u));
        }
    }
}

// Address: 0x025548D0
void ChangePlayerNum::draw(const agl::lyr::RenderInfo& render_info) const
{
    if (!mIsVisible)
        return;

    void* p_proj = sub_02A05A9C_(render_info);
    const sead::Matrix44f& proj = *reinterpret_cast<const sead::Matrix44f*>(p_proj);
    const_cast<Multi2D&>(layout(const_cast<u8*>(reinterpret_cast<const u8*>(this)))).draw(proj);
}

// Address: 0x0255491C
void ChangePlayerNum::drawBtn(const agl::lyr::RenderInfo& render_info) const
{
    if (!mIsVisible)
        return;

    u8* self = const_cast<u8*>(reinterpret_cast<const u8*>(this));
    u8* p_slots = self + kOffsetPlayerSlotBase;
    for (s32 i = 0; i < static_cast<s32>(kOffsetPlayerSlotCount); ++i)
    {
        void* p_slot = p_slots + i * kOffsetPlayerSlotStride;
        if (static_cast<u32>(i) >= kOffsetPlayerSlotCount)
            p_slot = p_slots;
        sub_02558070_(p_slot, render_info);
    }
}

// Address: 0x025549A8
void ChangePlayerNum::gather()
{
    if (!mIsVisible)
        return;

    u8* self = reinterpret_cast<u8*>(this);
    u8* p_btns = self + kOffsetPlayerBtnBase;
    u8* p_base = p_btns;
    for (s32 i = 0; i < static_cast<s32>(kOffsetPlayerBtnCount); ++i)
    {
        void* p_btn = p_base + i * kOffsetPlayerBtnStride;
        if (static_cast<u32>(i) >= kOffsetPlayerBtnCount)
            p_btn = p_btns;
        sub_024C78D4_(p_btn);
    }
}

// Address: 0x02554A24
void ChangePlayerNum::setPlayerCharacter(s32 character, s32 player_no)
{
    u8* self = reinterpret_cast<u8*>(this);

    u8* p_used = self + kOffsetCharUsedBase;
    if (static_cast<u32>(character) < 6u)
        p_used += character;
    at<u8>(p_used, 0u) = 1;

    u32* p_char_id = reinterpret_cast<u32*>(self + kOffsetCharSelectBase);
    if (static_cast<u32>(player_no) < 4u)
        p_char_id = reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_char_id) + player_no * 4u);
    *p_char_id = static_cast<u32>(character);

    u8* p_flag = self + kOffsetCharFlagBase;
    if (static_cast<u32>(player_no) < 4u)
        p_flag += player_no;
    at<u8>(p_flag, 0u) = 1;
}

// Address: 0x02554A70
void ChangePlayerNum::resetPlayerCharacter(s32 character, s32 player_no)
{
    u8* self = reinterpret_cast<u8*>(this);

    u8* p_used = self + kOffsetCharUsedBase;
    if (static_cast<u32>(character) < 6u)
        p_used += character;
    at<u8>(p_used, 0u) = 0;

    u32* p_char_id = reinterpret_cast<u32*>(self + kOffsetCharSelectBase);
    if (static_cast<u32>(player_no) < 4u)
        p_char_id = reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_char_id) + player_no * 4u);
    *p_char_id = 6u;

    u8* p_flag = self + kOffsetCharFlagBase;
    if (static_cast<u32>(player_no) < 4u)
        p_flag += player_no;
    at<u8>(p_flag, 0u) = 0;
}

// Address: 0x02555998
void ChangePlayerNum::initializeState_CloseRequest()
{
    u8* self = reinterpret_cast<u8*>(this);
    u8* p_slots = self + kOffsetPlayerSlotBase;
    u8* p_slot = p_slots;
    for (s32 i = 0; i < static_cast<s32>(kOffsetPlayerSlotCount); ++i)
    {
        u8* p_write = p_slot;
        if (static_cast<u32>(i) >= kOffsetPlayerSlotCount)
            p_write = p_slots;
        at<u8>(p_write, 0x15ABu) = 1;
        p_slot += kOffsetPlayerSlotStride;
    }

    at<u32>(self, 0x1FDCu) = 0u;
    sub_02682C2C_(self + kOffsetCursorWork, 0);
    at<u8>(self, 0x1F78u) = 1u;
    paCursor(self).triggerDecide();
}

// Address: 0x02555A28
void ChangePlayerNum::executeState_CloseRequest()
{
    mStateMgr.changeState(rodata<StateID>(kGuestStateIdExecuteCloseReq));
}

// Address: 0x02556C04
void ChangePlayerNum::finalizeState_CloseRequest()
{
}
