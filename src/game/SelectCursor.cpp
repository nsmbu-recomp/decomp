#include <game/SelectCursor.h>

#include <layout/SelectCursorLayout.h>
#include <layout/SelectCursorMiniLayout.h>

namespace {

constexpr u32 kLayoutBase     = 0x10u;
constexpr u32 kMiniLayoutBase = 0x16330u;

SelectCursorLayout& layoutAt(SelectCursor* cursor, s32 type)
{
    auto* base = reinterpret_cast<SelectCursorLayout*>(reinterpret_cast<u8*>(cursor) + kLayoutBase);
    return base[type];
}

SelectCursorLayout& layoutRoot(SelectCursor* cursor)
{
    return *reinterpret_cast<SelectCursorLayout*>(reinterpret_cast<u8*>(cursor) + kLayoutBase);
}

SelectCursorMiniLayout& miniLayoutAt(SelectCursor* cursor, s32 type)
{
    auto* base = reinterpret_cast<SelectCursorMiniLayout*>(reinterpret_cast<u8*>(cursor) + kMiniLayoutBase);
    return base[type];
}

SelectCursorMiniLayout& miniLayoutRoot(SelectCursor* cursor)
{
    return *reinterpret_cast<SelectCursorMiniLayout*>(reinterpret_cast<u8*>(cursor) + kMiniLayoutBase);
}

// Address: 0x02645934
void selectCursorLayoutSetTarget(SelectCursorLayout* layout, void* button)
{
    u8* self = reinterpret_cast<u8*>(layout);
    *reinterpret_cast<void**>(self + 0xEA0) = button;
    *reinterpret_cast<u32*>(self + 0xEBC) = 0xFFu;
    self[0xEC2] = 1;
    self[0xEC1] = 0;
}

// Address: 0x02646114
void selectCursorMiniLayoutSetTarget(SelectCursorMiniLayout* layout, void* button)
{
    u8* self = reinterpret_cast<u8*>(layout);
    *reinterpret_cast<void**>(self + 0xEA0) = button;
    *reinterpret_cast<u32*>(self + 0xEB8) = 0xFFu;
    self[0xEBE] = 1;
    self[0xEBD] = 0;
}

} // namespace

// Address: 0x024C6744
void SelectCursor::set(void* button, s32 type, s32)
{
    if (button == nullptr)
        return;

    if (type == 0x18)
        return;

    if ((type - 0x12) < 4 || type == 9)
    {
        if (type < 0x18)
            selectCursorMiniLayoutSetTarget(&miniLayoutAt(this, type), button);
        else
            selectCursorMiniLayoutSetTarget(&miniLayoutRoot(this), button);
    }
    else
    {
        if (type < 0x18)
            selectCursorLayoutSetTarget(&layoutAt(this, type), button);
        else
            selectCursorLayoutSetTarget(&layoutRoot(this), button);
    }
}

// Address: 0x02645CB4
void SelectCursorMiniLayout::hideIfEnabled()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self[0xEBE] == 0)
        return;

    self[0xEBC] = 0;
}

// Address: 0x02645FC0
void SelectCursorMiniLayout::disable()
{
    reinterpret_cast<u8*>(this)[0xEBE] = 0;
}

// Address: 0x026454CC
void SelectCursorLayout::hideIfEnabled()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self[0xEC2] == 0)
        return;

    self[0xEC0] = 0;
}

// Address: 0x02645808
void SelectCursorLayout::disable()
{
    reinterpret_cast<u8*>(this)[0xEC2] = 0;
}

// Address: 0x024C68C0
void SelectCursor::pausePlayer(s32 type)
{
    u8* high = reinterpret_cast<u8*>(this) + 0x10000u;

    if ((type - 0x12) < 4)
    {
        u8* slot = high + static_cast<u32>(type) * 0xEC8u;
        slot[0x71ED] = 1;
        return;
    }

    if (type == 9)
    {
        high[0x71ED] = 1;
        return;
    }

    if (type == 0x18)
        return;

    u8* base = reinterpret_cast<u8*>(this) + 0x10u;
    if (type < 0x18)
        base += static_cast<u32>(type) * 0xECCu;
    base[0xEC1] = 1;
}

// Address: 0x024C67B4
void SelectCursor::hide(s32 type)
{
    if ((type - 0x12) < 4 || type == 9)
    {
        if (type < 0x18)
        {
            SelectCursorMiniLayout& layout = miniLayoutAt(this, type);
            layout.hideIfEnabled();
            layout.disable();
        }
        else
        {
            SelectCursorMiniLayout& layout = miniLayoutRoot(this);
            layout.hideIfEnabled();
            layout.disable();
        }
    }
    else
    {
        if (type < 0x18)
        {
            SelectCursorLayout& layout = layoutAt(this, type);
            layout.hideIfEnabled();
            layout.disable();
        }
        else
        {
            SelectCursorLayout& layout = layoutRoot(this);
            layout.hideIfEnabled();
            layout.disable();
        }
    }
}
