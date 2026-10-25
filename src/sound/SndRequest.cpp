#include <sound/SndRequest.h>

#include <system/ResMgr.h>

namespace {

constexpr uintptr_t kSaveApplyMgrPtr = 0x101E7D9Cu;
constexpr uintptr_t kResMgrInstancePtr = 0x101E7D94u;

// Address: 0x024C0EA4
char* sead_snprintf(char* dst, const char* fmt, ...);

using RequestVFunc14 = void (*)(SndRequest* self);

void invokeRequestVFunc14(SndRequest* request)
{
    void* sub = request->mpSub;
    auto vtable = *reinterpret_cast<void***>(sub);
    auto fn = reinterpret_cast<RequestVFunc14>(vtable[0x14 / sizeof(void*)]);
    fn(request);
}

} // namespace

// Address: 0x024BD6EC
void SndRequest::queueCourseUnitLoad(SndRequest* request, s32 match_count, u32 use_rdash_label)
{
L_024BD6EC:
    {
        u8* save_mgr = *reinterpret_cast<u8**>(kSaveApplyMgrPtr);
        if (save_mgr[0x11] == 0)
            goto L_024BD724;
    }
    goto L_024BD720;

L_024BD704:
    if (match_count == 0)
        goto L_024BD724;
    {
        u8* save_mgr = *reinterpret_cast<u8**>(kSaveApplyMgrPtr);
        if (save_mgr[0x11] == 0)
            goto L_024BD724;
    }
L_024BD720:
    use_rdash_label = 1;

L_024BD724:
    invokeRequestVFunc14(request);

    void* actor = request->mpActor;

    alignas(4) char label_rdash[0x40];
    alignas(4) char label_unit[0x40];
    const char* label;

    if (use_rdash_label == 0)
        goto L_024BD788;

    label = sead_snprintf(label_rdash, "rdash://unit/%s.szs", static_cast<const char*>(actor));
    {
        ResMgr* res_mgr = *reinterpret_cast<ResMgr**>(kResMgrInstancePtr);
        SndQueue_pushRequest(res_mgr, request, label, 0, 1);
    }
    return;

L_024BD788:
    label = sead_snprintf(label_unit, "unit/%s.szs", static_cast<const char*>(actor));
    {
        ResMgr* res_mgr = *reinterpret_cast<ResMgr**>(kResMgrInstancePtr);
        SndQueue_pushRequest(res_mgr, request, label, 0, 1);
    }
}
