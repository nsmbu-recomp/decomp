#include <layout/AttentionScreen.h>
#include <layout_sys/AnimLayout.h>
#include <state/StateID.h>
#include <state/StateMethod.h>

#include <layer/aglRenderInfo.h>

#include <prim/seadSafeString.h>

namespace {

constexpr u32 kOffsetLayout       = 0x24u;
constexpr u32 kOffsetNPaneArray     = 0xED8u;
constexpr u32 kOffsetPPaneArray     = 0xEE0u;
constexpr u32 kOffsetIsOpen         = 0xEECu;
constexpr u32 kOffsetRequestClose = 0xEEDu;
constexpr u32 kOffsetIsVisible    = 0xEEEu;
constexpr u32 kOffsetType         = 0xEF0u;
constexpr u32 kOffsetVtableWord   = 0xEF4u;

constexpr u32 kLayerIdAttention   = 0xEu;

constexpr uintptr_t kGuestVtableAttentionScreen = 0x100C52A0u;
constexpr uintptr_t kGuestStateIdInitial        = 0x1021B568u;
constexpr uintptr_t kGuestStateIdOut          = 0x1021B5A8u;
constexpr uintptr_t kGuestLayoutResCommon     = 0x100C5230u;
constexpr uintptr_t kGuestLayoutBuildFile     = 0x100C5250u;
constexpr uintptr_t kGuestAnimResFlag         = 0x1021B55Cu;
constexpr uintptr_t kGuestAnimResTable        = 0x1021B51Cu;
constexpr uintptr_t kGuestAnimNameIn          = 0x100C5290u;
constexpr uintptr_t kGuestAnimNameLoopArrow   = 0x100C5294u;
constexpr uintptr_t kGuestAnimNameOut         = 0x100C528Cu;
constexpr uintptr_t kGuestNPaneNameFlag       = 0x1021B560u;
constexpr uintptr_t kGuestNPaneNameTable      = 0x1021B534u;
constexpr uintptr_t kGuestPPaneNameFlag       = 0x1021B564u;
constexpr uintptr_t kGuestPPaneNameTable      = 0x1021B544u;
constexpr uintptr_t kGuestNPaneName0          = 0x100C5268u;
constexpr uintptr_t kGuestNPaneName1          = 0x100C5280u;
constexpr uintptr_t kGuestPPaneName0          = 0x100C5238u;
constexpr uintptr_t kGuestPPaneName1          = 0x100C5244u;
constexpr uintptr_t kGuestPPaneName2          = 0x100C5274u;
constexpr uintptr_t kGuestSaveApplyMgr        = 0x101E7D9Cu;

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

Multi2D& layout(u8* self)
{
    return at<Multi2D>(self, kOffsetLayout);
}

StateMethod& stateMethod(u8* self)
{
    return at<StateMethod>(self, 0x10u);
}

nw::lyt::Pane** nPanes(u8* self)
{
    return reinterpret_cast<nw::lyt::Pane**>(self + kOffsetNPaneArray);
}

nw::lyt::Picture** pPanes(u8* self)
{
    return reinterpret_cast<nw::lyt::Picture**>(self + kOffsetPPaneArray);
}

u8 paneByteAt44(nw::lyt::Pane* p_pane)
{
    return *reinterpret_cast<u8*>(reinterpret_cast<u8*>(p_pane) + 0x44u);
}

void setPaneByteAt44(nw::lyt::Pane* p_pane, u8 value)
{
    *reinterpret_cast<u8*>(reinterpret_cast<u8*>(p_pane) + 0x44u) = value;
}

u8 paneVisWithoutLowBit(u8 value)
{
    return static_cast<u8>(value & 0xFEu);
}

bool animatorLowBitClear(nw::lyt::Animator* p_animator)
{
    u32 flags = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_animator) + 0x2Cu);
    return (flags & 1u) == 0u;
}

void stopAnimatorViaVtable_(nw::lyt::Animator* p_animator)
{
    if (p_animator == nullptr)
        return;

    void* p_obj = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_animator) + 0x14u);
    void* vtable = *reinterpret_cast<void**>(p_obj);
    using Fn = void (*)();
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(vtable) + 0xCCu));
    fn();
}

// Address: 0x02AFA490
extern void seadHeapDestruct_(void* buf, u32 size, void* dtor, u32 dtorArg, u32 zero, u32 zero2);

// Address: 0x02AFA52C
extern void sub_02AFA52C_(void* guestFunc, void* outDesc);

// Address: 0x02A05A9C
extern agl::lyr::DrawContext* sub_02A05A9C_(const agl::lyr::RenderInfo& render_info);

void ensureAnimNameTableInitialized()
{
    volatile u32* p_ready = reinterpret_cast<volatile u32*>(kGuestAnimResFlag);
    if (*p_ready != 0u)
        return;

    sead::SafeString* p_table = reinterpret_cast<sead::SafeString*>(kGuestAnimResTable);
    p_table[0] = rodata<sead::SafeString>(kGuestAnimNameIn);
    p_table[1] = rodata<sead::SafeString>(kGuestAnimNameLoopArrow);
    p_table[2] = rodata<sead::SafeString>(kGuestAnimNameOut);

    *p_ready = 1u;
    sub_02AFA52C_(reinterpret_cast<void*>(0x101D3200u), p_table);
}

void registerAnimResources(u8* self)
{
    ensureAnimNameTableInitialized();

    const sead::SafeString* p_names = reinterpret_cast<const sead::SafeString*>(kGuestAnimResTable);
    s32 count = 3;
    layout(self).animResRegister(p_names, count);
}

void ensureNPaneNameTableInitialized()
{
    volatile u32* p_ready = reinterpret_cast<volatile u32*>(kGuestNPaneNameFlag);
    if (*p_ready != 0u)
        return;

    sead::SafeString* p_table = reinterpret_cast<sead::SafeString*>(kGuestNPaneNameTable);
    p_table[0] = rodata<sead::SafeString>(kGuestNPaneName0);
    p_table[1] = rodata<sead::SafeString>(kGuestNPaneName1);

    *p_ready = 1u;
    sub_02AFA52C_(reinterpret_cast<void*>(0x101D320Cu), p_table);
}

void registerNPanes(u8* self)
{
    ensureNPaneNameTableInitialized();

    s32 count = 2;
    layout(self).NPaneRegister(nPanes(self), reinterpret_cast<const sead::SafeString*>(kGuestNPaneNameTable), count);
}

void ensurePPaneNameTableInitialized()
{
    volatile u32* p_ready = reinterpret_cast<volatile u32*>(kGuestPPaneNameFlag);
    if (*p_ready != 0u)
        return;

    sead::SafeString* p_table = reinterpret_cast<sead::SafeString*>(kGuestPPaneNameTable);
    p_table[0] = rodata<sead::SafeString>(kGuestPPaneName0);
    p_table[1] = rodata<sead::SafeString>(kGuestPPaneName1);
    p_table[2] = rodata<sead::SafeString>(kGuestPPaneName2);

    *p_ready = 1u;
    sub_02AFA52C_(reinterpret_cast<void*>(0x101D3218u), p_table);
}

void registerPPanes(u8* self)
{
    ensurePPaneNameTableInitialized();

    s32 count = 3;
    layout(self).PPaneRegister(pPanes(self), reinterpret_cast<const sead::SafeString*>(kGuestPPaneNameTable), count);
}

void configureOpenPaneVisibility(u8* self, AttentionScreen::Type type)
{
    nw::lyt::Pane* p_pane_a = nPanes(self)[0];
    nw::lyt::Pane* p_pane_b = nPanes(self)[1];

    u8 vis_a = paneByteAt44(p_pane_a);
    u8 vis_b = paneByteAt44(p_pane_b);

    if (type == AttentionScreen::cType_DRC)
    {
        setPaneByteAt44(p_pane_a, static_cast<u8>(paneVisWithoutLowBit(vis_a) + 1u));
        setPaneByteAt44(p_pane_b, paneVisWithoutLowBit(vis_b));
    }
    else
    {
        setPaneByteAt44(p_pane_a, paneVisWithoutLowBit(vis_a));
        setPaneByteAt44(p_pane_b, static_cast<u8>(paneVisWithoutLowBit(vis_b) + 1u));
    }

    at<u8>(self, kOffsetIsOpen) = 1;
}

void configureFadeInPictureVisibility(u8* self, bool save_apply_drc_flag)
{
    nw::lyt::Picture* p_pic_a = pPanes(self)[0];
    nw::lyt::Picture* p_pic_b = pPanes(self)[1];

    u8 vis_a = paneByteAt44(p_pic_a);
    u8 vis_b = paneByteAt44(p_pic_b);

    if (!save_apply_drc_flag)
    {
        setPaneByteAt44(p_pic_a, paneVisWithoutLowBit(vis_a));
        setPaneByteAt44(p_pic_b, static_cast<u8>(paneVisWithoutLowBit(vis_b) + 1u));
    }
    else
    {
        setPaneByteAt44(p_pic_a, static_cast<u8>(paneVisWithoutLowBit(vis_a) + 1u));
        setPaneByteAt44(p_pic_b, paneVisWithoutLowBit(vis_b));
    }
}

} // namespace

CREATE_STATE_ID(AttentionScreen, In)

// Address: 0x02532B04
AttentionScreen::AttentionScreen()
    : mStateMgr(*this, rodata<StateID>(kGuestStateIdInitial))
    , mLayout()
    , mIsOpen(false)
    , mRequestClose(false)
    , mIsVisible(false)
    , mType(cType_DRC)
{
    u8* self = reinterpret_cast<u8*>(this);

    *reinterpret_cast<uintptr_t*>(self + kOffsetVtableWord) = kGuestVtableAttentionScreen;

    new (&mLayout) AnimLayout<2>();

    for (s32 i = 0; i < mNPane.size(); ++i)
        mNPane[i] = nullptr;
    for (s32 i = 0; i < mPPane.size(); ++i)
        mPPane[i] = nullptr;
}

// Address: 0x02532C74
AttentionScreen::~AttentionScreen()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self == nullptr)
        return;

    at<u8>(self, kOffsetIsVisible) = 0;
    *reinterpret_cast<uintptr_t*>(self + kOffsetVtableWord) = kGuestVtableAttentionScreen;
    at<u8>(self, kOffsetIsOpen) = 0;

    mLayout.~AnimLayoutBase();

    if ((reinterpret_cast<uintptr_t>(this) & 1u) != 0u)
    {
        seadHeapDestruct_(self, 0u, nullptr, 0u, 0u, 0u);
    }
}

// Address: 0x02532D14
void AttentionScreen::create()
{
    u8* self = reinterpret_cast<u8*>(this);

    layout(self).createLayoutObj(nullptr);

    const sead::SafeString& res_common = rodata<sead::SafeString>(kGuestLayoutResCommon);
    layout(self).readResource(res_common);

    const sead::SafeString& lyt_file = rodata<sead::SafeString>(kGuestLayoutBuildFile);
    mLayout.build(lyt_file);

    registerAnimResources(self);
    registerNPanes(self);
    registerPPanes(self);
}

// Address: 0x02532FB0
void AttentionScreen::execute()
{
    u8* self = reinterpret_cast<u8*>(this);

    if (!mIsOpen)
        return;

    stateMethod(self).executeStateMethod();
    layout(self).animPlay(kLayerIdAttention, nullptr);
}

// Address: 0x02532FFC
void AttentionScreen::draw(const agl::lyr::RenderInfo& render_info) const
{
    if (!mIsVisible)
        return;

    if (mType != cType_TV)
        return;

    u8* self = const_cast<u8*>(reinterpret_cast<const u8*>(this));
    agl::lyr::DrawContext* p_ctx = sub_02A05A9C_(render_info);
    layout(self).draw(*p_ctx);
}

// Address: 0x02533050
void AttentionScreen::drawDRC(const agl::lyr::RenderInfo& render_info) const
{
    if (!mIsVisible)
        return;

    if (mType != cType_DRC)
        return;

    u8* self = const_cast<u8*>(reinterpret_cast<const u8*>(this));
    agl::lyr::DrawContext* p_ctx = sub_02A05A9C_(render_info);
    layout(self).draw(*p_ctx);
}

// Address: 0x025330A4
void AttentionScreen::open(Type type)
{
    u8* self = reinterpret_cast<u8*>(this);
    mType = type;
    configureOpenPaneVisibility(self, type);
}

// Address: 0x02533104
void AttentionScreen::requestCloseNow()
{
    u8* self = reinterpret_cast<u8*>(this);

    const StateID& out_state = rodata<StateID>(kGuestStateIdOut);
    if (!stateMethod(self).getStateID()->isEqual(out_state))
        stateMethod(self).changeStateMethod(out_state);

    mRequestClose = true;
}

// Address: 0x025331A0
void AttentionScreen::forceClose()
{
    u8* self = reinterpret_cast<u8*>(this);

    const sead::SafeString& anim_out = rodata<sead::SafeString>(kGuestAnimNameOut);
    mLayout.animStartSetup(0, anim_out, static_cast<nw::lyt::Animator::PlayType>(0));

    nw::lyt::Animator* p_animator = mLayout.getAnimatorObj(0);
    stopAnimatorViaVtable_(p_animator);

    mIsOpen = false;
    mIsVisible = false;
    stateMethod(self).changeStateMethod(rodata<StateID>(kGuestStateIdInitial));
}

// Address: 0x02533228
void AttentionScreen::initializeState_In()
{
    u8* self = reinterpret_cast<u8*>(this);

    const sead::SafeString& anim_in = rodata<sead::SafeString>(kGuestAnimNameIn);
    mLayout.animStartSetup(0, anim_in, static_cast<nw::lyt::Animator::PlayType>(0));

    mIsVisible = true;

    u8* save_apply = *reinterpret_cast<u8**>(kGuestSaveApplyMgr);
    bool save_drc = (save_apply[0x11] != 0);
    configureFadeInPictureVisibility(self, save_drc);
}

// Address: 0x025332D4
void AttentionScreen::executeState_In()
{
    nw::lyt::Animator* p_animator = mLayout.getAnimatorObj(0);
    if (animatorLowBitClear(p_animator))
        mStateMgr.changeState(rodata<StateID>(kGuestStateIdOut));
}

// Address: 0x02533A34
void AttentionScreen::finalizeState_In()
{
}
