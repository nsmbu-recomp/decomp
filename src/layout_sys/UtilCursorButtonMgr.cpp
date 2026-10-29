#include <layout_sys/UtilCursorButtonMgr.h>

namespace {

// Address: 0x029F1594
void* sub_029F1594(u32 size);

} // namespace

// Address: 0x02681FC4
UtilCursorButtonMgr::UtilCursorButtonMgr()
{
    u8* p_this = reinterpret_cast<u8*>(this);
    if (p_this == nullptr) {
        p_this = static_cast<u8*>(sub_029F1594(0x34));
        if (p_this == nullptr)
            return;
    }

    *reinterpret_cast<u32*>(p_this + 0x0) = 9;
    *reinterpret_cast<u32*>(p_this + 0x8) = 0;
    *reinterpret_cast<const char**>(p_this + 0x4) =
        reinterpret_cast<const char*>(0x100E46CCu);

    u32 zero = 0;
    if (reinterpret_cast<uintptr_t>(p_this) + 0xC == 0) {
        u8* p_node = static_cast<u8*>(sub_029F1594(0xC));
        if (p_node != nullptr) {
            u8* p_next = p_node + 4;
            *reinterpret_cast<u32*>(p_node + 0x0) = zero;
            *reinterpret_cast<u32*>(p_next + 0x4) = reinterpret_cast<u32>(p_next);
            *reinterpret_cast<u32*>(p_next + 0x0) = reinterpret_cast<u32>(p_next);
        }
    }

    *reinterpret_cast<u32*>(p_this + 0x18) = zero;
    *reinterpret_cast<u32*>(p_this + 0x1C) = zero;
    *reinterpret_cast<u32*>(p_this + 0x20) = zero;
    *reinterpret_cast<u32*>(p_this + 0x24) = zero;

    if (reinterpret_cast<uintptr_t>(p_this) + 0x28 == 0) {
        u8* p_word = static_cast<u8*>(sub_029F1594(4));
        if (p_word != nullptr)
            *reinterpret_cast<u32*>(p_word) = zero;
    } else {
        *reinterpret_cast<u32*>(p_this + 0x28) = zero;
    }

    p_this[0x30] = 0;
    p_this[0x31] = 0;
    *reinterpret_cast<u32*>(p_this + 0x2C) = zero;
}
