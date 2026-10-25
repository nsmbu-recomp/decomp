#include <system/ErrorViewer.h>

namespace
{

constexpr uintptr_t kGuestPtr101D2E00 = 0x101D2E00u;
constexpr uintptr_t kGuestPtr101E7D04 = 0x101E7D04u;
constexpr uintptr_t kGuestPtr101E7D44 = 0x101E7D44u;
constexpr uintptr_t kGuestPtr101E7D68 = 0x101E7D68u;

struct ErrorViewerShowStack
{
    void*   mpLayout;
    u32     field_0C;
    u32     mDisplayKind;
    void*   mpLayoutScratch;
};

template<typename T>
T& fieldAt(u8* base, u32 offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

// Address: 0x029CB9F8
u32 errorViewerGetTick_();

// Address: 0x029C5F38
void* errorViewerBeginSession_(void* mgr, u32 tick, s32 negStoredValue);

// Address: 0x025113B8
void guest_025113B8_(void* obj, u32 bitIndex, u32 set);

// Address: 0x029C2D60
void errorViewerPrepareResources_(void* resRoot);

// Address: 0x029F18DC
void* errorViewerCreateLayout_(void* heap, void* session, u32 layoutKind);

// Address: 0x02B07418
void errorViewerInitLayoutTiming_(void* layout, u32 zero, u32 tick);

// Address: 0x02A9EE60
void errorViewerInitShowStack_(ErrorViewerShowStack* stack);

// Address: 0x029CBB34
void errorViewerSetupMessage_(ErrorViewer* self, void* message);

// Address: 0x029FF888
void* errorViewerGetDrawHost_(ErrorViewer* self);

// Address: 0x029FF410
void* errorViewerGetLayer_(void* drawHost);

// Address: 0x029FF420
void errorViewerReleaseLayer_(void* drawHost);

// Address: 0x029C4DA8
u8 errorViewerReadResultFlag_();

// Address: 0x029C4D34
s32 errorViewerSetActiveState_(s32 state);

void* errorViewerTimerObject_(ErrorViewer* self)
{
    u8* base = reinterpret_cast<u8*>(self);
    void* link = fieldAt<void*>(base, 0x5Cu);
    void* table = fieldAt<void*>(reinterpret_cast<u8*>(link), 0xB0u);
    u32 index = fieldAt<u32>(reinterpret_cast<u8*>(table), 0x58u);
    void* entry = *reinterpret_cast<void**>(reinterpret_cast<u8*>(table) + 0x44u + index * 4u);
    return fieldAt<void*>(reinterpret_cast<u8*>(entry), 0x20u);
}

u32 errorViewerTimerDeadline_(void* timerObject)
{
    u8* obj = reinterpret_cast<u8*>(timerObject);
    void* vtable = fieldAt<void*>(obj, 0u);
    using Fn = u32 (*)(void*, u32);
    auto fn = reinterpret_cast<Fn>(*reinterpret_cast<void**>(reinterpret_cast<u8*>(vtable) + 0x7Cu));
    return fn(timerObject, 4u);
}

void* errorViewerCreateDrawObject_(void* drawHost, u32 param)
{
    void* vtable = fieldAt<void*>(reinterpret_cast<u8*>(drawHost), 0u);
    using Fn = void* (*)(void*, u32);
    auto fn = reinterpret_cast<Fn>(*reinterpret_cast<void**>(reinterpret_cast<u8*>(vtable) + 0x2Cu));
    return fn(drawHost, param);
}

void errorViewerWaitOnLayer_(void* layer, void* drawHost)
{
    void* waitObject = fieldAt<void*>(reinterpret_cast<u8*>(layer), 0x48u);
    if (waitObject == nullptr)
        return;

    void* vtable = fieldAt<void*>(reinterpret_cast<u8*>(waitObject), 0u);
    using Fn = void (*)(void*, void*, u32);
    auto fn = reinterpret_cast<Fn>(*reinterpret_cast<void**>(reinterpret_cast<u8*>(vtable) + 0xCu));
    fn(waitObject, drawHost, 3u);
}

s32 negateS32(s32 value)
{
    return -value;
}

}

// Address: 0x029CBBB4
s32 ErrorViewer::show(void* message, u8 displayParam)
{
    u8* selfBytes = reinterpret_cast<u8*>(this);

    void* timerObject = errorViewerTimerObject_(this);
    if (timerObject != nullptr)
    {
        u32 deadline = errorViewerTimerDeadline_(timerObject);
        u32 tick = errorViewerGetTick_();
        if (deadline > tick)
        {
            mIsShowing = 0;
            void* notifyTarget = *reinterpret_cast<void**>(kGuestPtr101D2E00);
            if (notifyTarget != nullptr)
                guest_025113B8_(notifyTarget, 2u, 1u);
        }
        else
        {
            timerObject = nullptr;
        }
    }

    if (timerObject == nullptr)
    {
        s32 negStored = negateS32(mStoredValue);
        u32 tick = errorViewerGetTick_();
        void* mgr = *reinterpret_cast<void**>(kGuestPtr101E7D44);
        void* session = errorViewerBeginSession_(mgr, tick, negStored);
        timerObject = session;
        mIsShowing = 1;
        void* notifyTarget = *reinterpret_cast<void**>(kGuestPtr101D2E00);
        if (notifyTarget != nullptr)
            guest_025113B8_(notifyTarget, 2u, 1u);
    }

    void* resRoot = *reinterpret_cast<void**>(kGuestPtr101E7D04);
    errorViewerPrepareResources_(resRoot);

    ErrorViewerShowStack stack{};
    stack.mpLayout = nullptr;
    stack.field_0C = 0;
    stack.mDisplayKind = 0;
    stack.mpLayoutScratch = nullptr;

    u32 tick = errorViewerGetTick_();
    void* layout = errorViewerCreateLayout_(nullptr, timerObject, 4u);
    mpLayout = layout;
    stack.mpLayout = layout;
    stack.mpLayoutScratch = reinterpret_cast<void*>(selfBytes + 0x550u);

    tick = errorViewerGetTick_();
    errorViewerInitLayoutTiming_(layout, 0u, tick);

    stack.field_0C = 1;
    void* displayInfo = *reinterpret_cast<void**>(kGuestPtr101E7D68);
    u32 displayMode = fieldAt<u32>(reinterpret_cast<u8*>(displayInfo), 0x10u);
    u32 displayKind;
    if (displayMode == 1u)
        displayKind = 2u;
    else if (displayMode == 2u)
        displayKind = 5u;
    else
        displayKind = 1u;
    stack.mDisplayKind = displayKind;

    errorViewerInitShowStack_(&stack);

    mDisplayParam = displayParam;
    errorViewerSetupMessage_(this, message);

    void* drawHost = errorViewerGetDrawHost_(this);
    void* drawObject = errorViewerCreateDrawObject_(drawHost, 1u);
    void* layer = errorViewerGetLayer_(drawObject);
    errorViewerWaitOnLayer_(layer, drawObject);
    fieldAt<u32>(reinterpret_cast<u8*>(layer), 0x44u) = 3u;
    errorViewerReleaseLayer_(drawObject);

    mResultFlag = errorViewerReadResultFlag_();
    return errorViewerSetActiveState_(1);
}
