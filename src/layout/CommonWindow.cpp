#include <layout/CommonWindow.h>

#include <layout_sys/AnimLayout.h>

namespace {

// Address: 0x029F1594
void* sub_029F1594(u32 size);

// Address: 0x029F1748
void sub_029F1748(void* p_obj);

// Address: 0x02AFA350
void sub_02AFA350(void* self, s32 count, s32 size, u32 ctor);

// Address: 0x02AFA490
void sub_02AFA490(void* self, s32 count, s32 size, u32 dtor, s32 a, s32 b);

// Address: 0x100CCD58
constexpr u32 kCommonWindowVtbl = 0x100CCD58u;

// Address: 0x100DC3CC
extern const u32 kCommonWindowLeftButtonByColor[];

// Address: 0x100DC3E0
extern const u32 kCommonWindowRightButtonByColor[];

u32 loadLeftButtonId_(const CommonWindow* self)
{
    u32 color = *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(self) + 0x1AA0);
    return kCommonWindowLeftButtonByColor[color];
}

u32 loadRightButtonId_(const CommonWindow* self)
{
    u32 color = *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(self) + 0x1AA0);
    return kCommonWindowRightButtonByColor[color];
}

bool compareCursorButton_(const CommonWindow* self, u32 (*loader)(const CommonWindow*))
{
    u32 expected = loader(self);
    u32 stored = *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(self) + 0x1128);
    return stored == expected;
}

} // namespace

// Address: 0x02585958
CommonWindow::CommonWindow()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self == nullptr)
    {
        self = static_cast<u8*>(sub_029F1594(0x1ABC));
        if (self == nullptr)
            return;
    }

    *reinterpret_cast<u32*>(self + 0x1AB8) = kCommonWindowVtbl;

    new (self + 0x10) FStateMgr<CommonWindow>(*this);
    new (self + 0x24) AnimLayout<2>();
    new (self + 0xED8) PaCursor();

    if (*reinterpret_cast<u32*>(self + 0x11EC) == 0)
        sub_029F1594(0x20);
    if (*reinterpret_cast<u32*>(self + 0x120C) == 0)
        sub_029F1594(0x5C);
    if (*reinterpret_cast<u32*>(self + 0x1268) == 0)
        sub_029F1594(0x3C);
    if (*reinterpret_cast<u32*>(self + 0x12A4) == 0)
        sub_029F1594(0x4);
    if (*reinterpret_cast<u32*>(self + 0x12A8) == 0)
        sub_029F1594(0x4);

    u8* controllers = self + 0x12AC;
    if (*reinterpret_cast<u32*>(controllers) == 0)
    {
        if (sub_029F1594(0x7E4) == nullptr)
            return;

        sub_02AFA350(controllers, 5, 0x194, 0x02514150);
    }

    for (u32 offset = 0x1A98; offset <= 0x1AB7; ++offset)
        self[offset] = 0;

    self[0x1AA4] = static_cast<u8>(-1);

    u8* pane_ptrs = self + 0x11E8;
    for (s32 i = 0; i < 8; ++i)
        *reinterpret_cast<u32*>(pane_ptrs + i * 4) = 0;

    u8* text_ptrs = self + 0x1208;
    for (s32 i = 0; i < 0x17; ++i)
        *reinterpret_cast<u32*>(text_ptrs + i * 4) = 0;

    u8* parts_ptrs = self + 0x1264;
    for (s32 i = 0; i < 0xF; ++i)
        *reinterpret_cast<u32*>(parts_ptrs + i * 4) = 0;
}

// Address: 0x02585B90
CommonWindow::~CommonWindow()
{
    if (this == nullptr)
        return;

    u8* self = reinterpret_cast<u8*>(this);

    sub_02AFA490(self + 0x12AC, 5, 0x194, 0x025140E8, 0, 0);

    reinterpret_cast<PaCursor*>(self + 0xED8)->~PaCursor();

    reinterpret_cast<AnimLayout<2>*>(self + 0x24)->~AnimLayoutBase();
}

// Address: 0x02586CF4
bool CommonWindow::cursorIsOnLeftButton() const
{
    return compareCursorButton_(this, loadLeftButtonId_);
}

// Address: 0x02586D28
bool CommonWindow::cursorIsOnRightButton() const
{
    return compareCursorButton_(this, loadRightButtonId_);
}
