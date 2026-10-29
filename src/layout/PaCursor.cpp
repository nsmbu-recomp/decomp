#include <layout/PaCursor.h>

#include <layout_sys/UtilCursorButtonMgr.h>

namespace {

// Address: 0x029F1594
void* sub_029F1594(u32 size);

// Address: 0x029F1748
void sub_029F1748(void* p_obj);

// Address: 0x02514150
void sub_02514150(SysControllerWrapper* wrapper);

// Address: 0x025140E8
void sub_025140E8(SysControllerWrapper* wrapper, s32 flags);

// Address: 0x02681FC4
void sub_02681FC4(UtilCursorButtonMgr* mgr);

// Address: 0x02682D5C
void sub_02682D5C(UtilCursorButtonMgr* mgr, const char* name, s32 count);

// Address: 0x100D6B34
constexpr u32 kPaCursorVtbl = 0x100D6B34u;

// Address: 0x025F872C
void sub_025F872C(PaCursor* cursor, u32 value);

} // namespace

// Address: 0x025F8024
PaCursor::PaCursor()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self == nullptr)
    {
        self = static_cast<u8*>(sub_029F1594(0x314));
        if (self == nullptr)
            return;
    }

    *reinterpret_cast<u32*>(self + 0x310) = kPaCursorVtbl;

    new (self + 0x10) FStateMgr<PaCursor>(*this);
    sub_02681FC4(reinterpret_cast<UtilCursorButtonMgr*>(self + 0x24));
    sub_02514150(reinterpret_cast<SysControllerWrapper*>(self + 0x58));

    if (*reinterpret_cast<u32*>(self + 0x1EC) == 0)
        sub_029F1594(0x29);
    if (*reinterpret_cast<u32*>(self + 0x215) == 0)
        sub_029F1594(0x29);

    u8* string_obj = self + 0x240;
    if (*reinterpret_cast<u32*>(string_obj) == 0)
    {
        if (sub_029F1594(8) == nullptr)
            return;

        *reinterpret_cast<u32*>(string_obj + 0) = 0x10000044;
        *reinterpret_cast<u32*>(string_obj + 4) = 0x100D69E0;
    }

    u8* vector_obj = self + 0x248;
    if (*reinterpret_cast<u32*>(vector_obj) == 0)
        sub_029F1594(8);

    self[0x25D] = 0;
    self[0x265] = 0;
    *reinterpret_cast<u32*>(self + 0x250) = 0x29;
    self[0x26A] = 0;
    *reinterpret_cast<u32*>(self + 0x254) = 0x29;
    self[0x269] = 0;
    self[0x26B] = 0;
    self[0x260] = 0;
    self[0x25F] = 0;
    self[0x264] = 0;
    self[0x263] = 0;
    self[0x25E] = 0;
    self[0x268] = 0;
    self[0x267] = 0;
    self[0x266] = 0;
    self[0x262] = 0;
    self[0x261] = 0;
    self[0x25C] = 0;

    u8* button_state = self + 0x26C;
    if (*reinterpret_cast<u32*>(button_state) == 0)
        sub_029F1594(0xA4);

    for (s32 i = 0; i < 0x29; ++i)
        self[0x1EC + i] = 1;

    for (s32 i = 0; i < 0x29; ++i)
        self[0x215 + i] = 1;

    sub_02682D5C(reinterpret_cast<UtilCursorButtonMgr*>(self + 0x24),
                 reinterpret_cast<const char*>(0x100D6B1C),
                 9);

    u8* extra_state = self + 0x268;
    for (s32 i = 0; i < 0x29; ++i)
        *reinterpret_cast<u32*>(extra_state + i * 4) = 0;
}

// Address: 0x025F8230
PaCursor::~PaCursor()
{
    if (this == nullptr)
        return;

    sub_025140E8(reinterpret_cast<SysControllerWrapper*>(reinterpret_cast<u8*>(this) + 0x58), 2);
}

// Address: 0x025F8788
void PaCursor::triggerDecide()
{
    u8* self = reinterpret_cast<u8*>(this);
    u32 value = self[0x260] != 0 ? 1u : 0u;
    self[0x25F] = 1;
    sub_025F872C(this, value);
}
