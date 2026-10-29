#include <layout_sys/TagProcessor.h>

namespace {

using ContextType = TagProcessor::ContextType;
using Operation = TagProcessor::Operation;

// Address: 0x02679334
void* sub_02679334(void* p_singleton, s32 arg);

// Address: 0x02A9F838
void sub_02A9F838(void* p_ctx, f32 arg);

// Address: 0x02A9FD8C
void sub_02A9FD8C(void* p_ctx, u8 arg);

// Address: 0x02AA2E40
void sub_02AA2E40(void* p_stack, s32 arg);

// Address: 0x02AA2B4C
Operation sub_02AA2B4C(TagProcessor* p_self, u16 code, ContextType* p_context);

// Address: 0x02AA2BB4
Operation sub_02AA2BB4(TagProcessor* p_self, nw::ut::Rect* p_rect, u16 code, ContextType* p_context);

// Address: 0x101DAB4C
constexpr uintptr_t kFontSingletonSlot = 0x101DAB4Cu;

bool advanceTagCursor_(ContextType* p_context, u16 code, u8** pp_tag_out)
{
    u8* p_cursor = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4);
    u16 tag_kind = *reinterpret_cast<u16*>(p_cursor - 2);
    u8* p_tag = p_cursor - 2;
    u8* p_next = nullptr;

    if (tag_kind == 0xE) {
        u16 rel = *reinterpret_cast<u16*>(p_tag + 6);
        p_next = p_tag + rel + 8;
        if (p_tag == nullptr)
            return false;
    } else if (tag_kind == 0xF) {
        if (p_tag == nullptr)
            return false;
        p_next = p_tag + 6;
    } else {
        return false;
    }

    *reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4) = p_next;
    *pp_tag_out = p_tag;
    return true;
}

// Address: 0x0267BF7C
Operation sub_0267BF7C(TagProcessor* p_self, ContextType* p_context)
{
    void* p_font_singleton = *reinterpret_cast<void**>(kFontSingletonSlot);
    void* p_font = sub_02679334(p_font_singleton, 2);

    ContextType* p_ctx = p_context;
    void* p_ctx_obj = *reinterpret_cast<void**>(p_ctx);
    void* p_glyph = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28);
    void* p_glyph_vtable = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_glyph) + 4);
    using GlyphFn = s32 (*)(void*, u8);
    GlyphFn glyph_fn = reinterpret_cast<GlyphFn>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_glyph_vtable) + 0x24));
    u8 glyph_index = *reinterpret_cast<u8*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4) + 1);
    s32 glyph_width = glyph_fn(p_glyph, glyph_index);

    f64 scale_raw = static_cast<f64>(static_cast<u32>(glyph_width) | 0x80000000u);
    scale_raw -= *reinterpret_cast<const f64*>(0x100E3D88u);
    f32 scale = static_cast<f32>(scale_raw);
    f32 font_size = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x10);
    f32 scaled = scale * font_size;

    u8* p_saved = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28);
    *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28) = p_font;
    sub_02A9F838(p_ctx_obj, scaled);

    void* p_font_vtable = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_font) + 4);
    using FontMetricFn = s32 (*)(void*);
    FontMetricFn metric_fn =
        reinterpret_cast<FontMetricFn>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_font_vtable) + 0xAC));
    s32 metric = metric_fn(p_font);

    f64 metric_raw = static_cast<f64>(static_cast<u32>(-metric) | 0x80000000u);
    metric_raw -= *reinterpret_cast<const f64*>(0x100E3D88u);
    f32 metric_f = static_cast<f32>(metric_raw);
    f32 font_size2 = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x10);
    f32 addend = metric_f * font_size2;
    f32 line_height = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x18);
    line_height = line_height + addend;
    *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x18) = line_height;
    sub_02A9FD8C(p_ctx_obj, glyph_index);

    u8* p_cursor = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4);
    *reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4) = p_cursor + 2;

    sub_02A9F838(p_ctx_obj, scaled);
    line_height = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x18);
    *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x18) = line_height - addend;
    *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28) = p_saved;

    return static_cast<Operation>(1);
}

// Address: 0x0267C2A4
Operation sub_0267C2A4(TagProcessor* p_self, nw::ut::Rect* p_rect, ContextType* p_context)
{
    void* p_font_singleton = *reinterpret_cast<void**>(kFontSingletonSlot);
    void* p_font = sub_02679334(p_font_singleton, 2);

    ContextType* p_ctx = p_context;
    void* p_ctx_obj = *reinterpret_cast<void**>(p_ctx);
    u8 glyph_index = *reinterpret_cast<u8*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4) + 1);

    void* p_glyph = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28);
    void* p_glyph_vtable = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_glyph) + 4);
    using GlyphFn = s32 (*)(void*, u8);
    GlyphFn glyph_fn = reinterpret_cast<GlyphFn>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_glyph_vtable) + 0x24));
    s32 glyph_width = glyph_fn(p_glyph, glyph_index);

    f64 scale_raw = static_cast<f64>(static_cast<u32>(glyph_width) | 0x80000000u);
    scale_raw -= *reinterpret_cast<const f64*>(0x100E3D88u);
    f32 scale = static_cast<f32>(scale_raw);

    void* p_saved = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28);
    *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28) = p_font;

    void* p_glyph_vtable2 = *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_glyph) + 4);
    using WidthFn = f32 (*)(void*);
    WidthFn width_fn =
        reinterpret_cast<WidthFn>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(p_glyph_vtable2) + 0x24));
    f32 width = width_fn(p_glyph);

    f32 font_size = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x10);
    f32 scaled_width = scale * font_size;
    f32 line_height = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x10);
    f32 combined = scaled_width * line_height;
    f32 ctx_height = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x14);
    ctx_height = ctx_height + combined;
    *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x14) = ctx_height;

    alignas(4) u8 stack_copy[0x28];
    u8* p_src = reinterpret_cast<u8*>(p_context) - 4;
    for (s32 i = 0; i < 0x13; ++i) {
        *reinterpret_cast<u32*>(stack_copy + i * 4) = *reinterpret_cast<u32*>(p_src + i * 4);
        *reinterpret_cast<u32*>(stack_copy + 4 + i * 4) = *reinterpret_cast<u32*>(p_src + 4 + i * 4);
    }

    sub_02A9F838(p_ctx_obj, scaled_width);

    f32 left = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_rect) + 0x14);
    f32 top = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_context) + 0x14);
    f32 right = width;
    f32 bottom = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_rect) + 0x8);
    f32 min_y = top - left;
    f32 max_y = bottom - left;
    f32 sel_a = (min_y < top) ? top : min_y;
    f32 sel_b = (max_y < bottom) ? bottom : max_y;
    *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_rect) + 0x0) = sel_a;
    *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_rect) + 0x8) = sel_b;

    u8* p_cursor = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4);
    *reinterpret_cast<u8**>(reinterpret_cast<u8*>(p_context) + 4) = p_cursor + 2;

    sub_02AA2E40(stack_copy + 0x10, 2);
    *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_ctx_obj) + 0x28) = p_saved;

    return static_cast<Operation>(1);
}

} // namespace

// Address: 0x0267C0C4
TagProcessor::Operation TagProcessor::Process(u16 code, ContextType* p_context)
{
    u8* p_tag = nullptr;
    if (!advanceTagCursor_(p_context, code, &p_tag))
        goto L_default;

    {
        u16 cmd = *reinterpret_cast<u16*>(p_tag + 2);
        if (cmd == 0) {
            u16 param = *reinterpret_cast<u16*>(p_tag + 4);
            if (param == 2) {
                u32 color = 0;
                color |= static_cast<u32>(*reinterpret_cast<u8*>(p_tag + 8)) << 24;
                color |= static_cast<u32>(*reinterpret_cast<u8*>(p_tag + 9)) << 16;
                color |= static_cast<u32>(*reinterpret_cast<u8*>(p_tag + 0xA)) << 8;
                color |= static_cast<u32>(*reinterpret_cast<u8*>(p_tag + 0xB));
                void* p_ctx_obj = *reinterpret_cast<void**>(p_context);
                u32 packed = color;
                *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_ctx_obj) + 4) = packed;
                u32 packed2 = color;
                *reinterpret_cast<u32*>(reinterpret_cast<u8*>(p_ctx_obj) + 8) = packed2;
                return static_cast<Operation>(1);
            }
            if (param == 3) {
                u16 size_param = *reinterpret_cast<u16*>(p_tag + 8);
                f64 raw = static_cast<f64>(static_cast<u32>(size_param) | 0x43300000u);
                raw -= *reinterpret_cast<const f64*>(0x100E3D90u);
                f32 scale = static_cast<f32>(raw);
                f32 size = scale * mFontSize / *reinterpret_cast<const f32*>(0x100E3D98u);
                void* p_ctx_obj = *reinterpret_cast<void**>(p_context);
                *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0xC) = size;
                *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x10) = size;
                return static_cast<Operation>(1);
            }
            return static_cast<Operation>(1);
        }
        if (cmd == 0xB) {
            u16 param = *reinterpret_cast<u16*>(p_tag + 4);
            if (param == 2) {
                void* p_ctx_obj = *reinterpret_cast<void**>(p_context);
                f32 value = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0xC);
                mFontSize = value;
                return static_cast<Operation>(1);
            }
            return static_cast<Operation>(1);
        }
        return static_cast<Operation>(1);
    }

L_default:
    if (code == 4)
        return sub_0267BF7C(this, p_context);
    if (code == 5) {
        void* p_ctx_obj = *reinterpret_cast<void**>(p_context);
        f32 value = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0xC);
        mFontSize = value;
        return static_cast<Operation>(1);
    }
    Operation result = sub_02AA2B4C(this, code, p_context);
    return result;
}

// Address: 0x0267C3FC
TagProcessor::Operation TagProcessor::CalcRect(nw::ut::Rect* p_rect, u16 code, ContextType* p_context)
{
    u8* p_tag = nullptr;
    if (!advanceTagCursor_(p_context, code, &p_tag))
        goto L_default;

    {
        u16 cmd = *reinterpret_cast<u16*>(p_tag + 2);
        if (cmd == 0) {
            u16 param = *reinterpret_cast<u16*>(p_tag + 4);
            if (param == 2) {
                u16 size_param = *reinterpret_cast<u16*>(p_tag + 8);
                f64 raw = static_cast<f64>(static_cast<u32>(size_param) | 0x43300000u);
                raw -= *reinterpret_cast<const f64*>(0x100E3D90u);
                f32 scale = static_cast<f32>(raw);
                f32 size = scale * mFontSize / *reinterpret_cast<const f32*>(0x100E3D98u);
                void* p_ctx_obj = *reinterpret_cast<void**>(p_context);
                *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0xC) = size;
                *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0x10) = size;
                return static_cast<Operation>(1);
            }
            return static_cast<Operation>(1);
        }
        if (cmd == 0xB) {
            u16 param = *reinterpret_cast<u16*>(p_tag + 4);
            if (param == 2) {
                void* p_ctx_obj = *reinterpret_cast<void**>(p_context);
                f32 value = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0xC);
                *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_rect) + 4) = value;
                return static_cast<Operation>(1);
            }
            return static_cast<Operation>(1);
        }
        return static_cast<Operation>(1);
    }

L_default:
    if (code == 4)
        return sub_0267C2A4(this, p_rect, p_context);
    if (code == 5) {
        void* p_ctx_obj = *reinterpret_cast<void**>(p_context);
        f32 value = *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_ctx_obj) + 0xC);
        *reinterpret_cast<f32*>(reinterpret_cast<u8*>(p_rect) + 4) = value;
        return static_cast<Operation>(1);
    }
    Operation result = sub_02AA2BB4(this, p_rect, code, p_context);
    return result;
}
