#include <input/InputMgr.h>

namespace {

// Address: 0x025141B0
extern void inputMgrInitControllerSlot_(void* controller, s32 slot_index);

}

// Address: 0x0250D1EC
void InputMgr::setupCourseControllers_()
{
    u8* self = reinterpret_cast<u8*>(this);
    void** table = reinterpret_cast<void**>(self + 0x4E0u);
    for (s32 index = 0; index < 4; index++)
        inputMgrInitControllerSlot_(table[index], index);
    void* extra = *reinterpret_cast<void**>(self + 0x4F0u);
    inputMgrInitControllerSlot_(extra, 4);
    mControllersRegistered = true;
}
