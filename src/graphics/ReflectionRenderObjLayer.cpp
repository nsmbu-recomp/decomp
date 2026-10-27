#include <graphics/ReflectionRenderObjLayer.h>
#include <graphics/CullViewFrustum.h>
#include <graphics/RenderObjLayer.h>

#include <gfx/seadCamera.h>
#include <gfx/seadColor.h>
#include <gfx/seadProjection.h>

#include <cstring>

namespace {

constexpr uintptr_t cDefaultCamera     = 0x105FB7F0u;
constexpr uintptr_t cDefaultProjection = 0x105FB854u;
constexpr uintptr_t cViewTemplate      = 0x105FA254u;
constexpr uintptr_t cViewVtbl          = 0x1019084Cu;

// Address: 0x024F99C4
void sub_024F99C4(ReflectionRenderObjLayer* p_self, void* p_view, const void* p_camera);

// Address: 0x024F9AB4
void sub_024F9AB4(ReflectionRenderObjLayer* p_self);

// Address: 0x024F9B38
void sub_024F9B38(ReflectionRenderObjLayer* p_self);

// Address: 0x024F9B80
void sub_024F9B80(ReflectionRenderObjLayer* p_self);

// Address: 0x024D6070
void sub_024D6070(void* p_state, void* p_target);

// Address: 0x024D6120
void sub_024D6120(void* p_state, void* p_target, u32 arg_2, u32 arg_3);

// Address: 0x02A04B94
void sub_02A04B94(void* p_view, s32 arg);

// Address: 0x02A058F0
void sub_02A058F0(void* p_object);

// Address: 0x02A074C8
void sub_02A074C8(const void* p_view, const void* p_size);

// Address: 0x02A0783C
void sub_02A0783C(const void* p_view, const void* p_size);

// Address: 0x02A07E70
void sub_02A07E70(void* p_viewport);

// Address: 0x02A07F90
void sub_02A07F90(void* p_viewport);

// Address: 0x02A1F088
void sub_02A1F088(void* p_target);

// Address: 0x02A24A64
void sub_02A24A64(void* p_target);

// Address: 0x02A25990
void sub_02A25990(void* p_state);

} // namespace

// Address: 0x024F9C04
void ReflectionRenderObjLayer::render_(const agl::lyr::RenderInfo& render_info)
{
    const f32 zero = 0.0f;
    const f32 one  = 1.0f;

    u8* const self = reinterpret_cast<u8*>(this);

    alignas(16) u8 stack[0x60];
    u8* const view     = stack;
    u8* const viewport = stack + 0x18;

    switch (render_info.getRenderStep())
    {
    case RenderObjLayer::cRenderStep_CalcView:
    {
        if (mpRenderMgr == nullptr)
            return;
        if (mpCameraProjection == nullptr)
            return;
        if (*reinterpret_cast<s32*>(reinterpret_cast<u8*>(mpRenderMgr) + 0x1c) == 0)
            return;

        const void* p_camera = *reinterpret_cast<void**>(reinterpret_cast<u8*>(mpCameraProjection) + 0x44);
        if (p_camera == nullptr)
            p_camera = reinterpret_cast<const void*>(cDefaultCamera);

        std::memcpy(view, reinterpret_cast<const void*>(cViewTemplate + 4), 0x30);
        *reinterpret_cast<const void**>(view + 0x30) = reinterpret_cast<const void*>(cViewVtbl);
        *reinterpret_cast<f32*>(view + 0x34) = zero;
        *reinterpret_cast<f32*>(view + 0x38) = zero;
        *reinterpret_cast<f32*>(view + 0x3c) = 10.0f;
        *reinterpret_cast<f32*>(view + 0x40) = zero;
        *reinterpret_cast<f32*>(view + 0x44) = zero;
        *reinterpret_cast<f32*>(view + 0x48) = zero;
        *reinterpret_cast<f32*>(view + 0x4c) = zero;
        *reinterpret_cast<f32*>(view + 0x50) = one;
        *reinterpret_cast<f32*>(view + 0x54) = zero;

        sub_024F99C4(this, view, p_camera);

        const void* p_projection = *reinterpret_cast<void**>(reinterpret_cast<u8*>(mpCameraProjection) + 0x48);
        if (p_projection == nullptr)
            p_projection = reinterpret_cast<const void*>(cDefaultProjection);

        reinterpret_cast<CullViewFrustum*>(self + 0x924)->update(
            *reinterpret_cast<const sead::Camera*>(view),
            *reinterpret_cast<const sead::PerspectiveProjection*>(p_projection)
        );

        mpRenderMgr->calcView(
            mViewIndex,
            *reinterpret_cast<const sead::Camera*>(view),
            *reinterpret_cast<const sead::PerspectiveProjection*>(p_projection),
            nullptr,
            reinterpret_cast<CullViewFrustum*>(self + 0x924),
            self + 0x9e4
        );

        sub_02A04B94(view, 2);
        return;
    }

    case RenderObjLayer::cRenderStep_UpdateGPUBuffer:
    {
        if (mpRenderMgr == nullptr)
            return;
        if (mpCameraProjection == nullptr)
            return;
        if (*reinterpret_cast<s32*>(reinterpret_cast<u8*>(mpRenderMgr) + 0x1c) == 0)
            return;

        mpRenderMgr->calcGPU(mViewIndex);
        return;
    }

    case RenderObjLayer::cRenderStep_Opa:
    {
        if (mpRenderMgr == nullptr)
            return;
        if (mpCameraProjection == nullptr)
            return;
        if (*reinterpret_cast<s32*>(reinterpret_cast<u8*>(mpRenderMgr) + 0x1c) == 0)
            return;

        sub_024F9AB4(this);
        sub_024F9B80(this);

        sub_024D6070(self + 0x480, mpColorTarget);
        sub_024D6120(self + 0x5d4, mpDepthTarget, 0, 0);

        *reinterpret_cast<u32*>(self + 0x708) = 0;
        *reinterpret_cast<u32*>(self + 0x684) = 0;
        GX2InitDepthBufferHiZEnable(self + 0x688);

        const s32 color_w0 = *reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(mpColorTarget) + 0x08);
        const s32 color_w1 = *reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(mpColorTarget) + 0x78);
        const s32 color_h0 = *reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(mpColorTarget) + 0x0c);
        const s32 color_h1 = *reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(mpColorTarget) + 0x7c);

        const s32 width  = (color_w1 < color_w0) ? color_w0 : color_w1;
        const s32 height = (color_h1 < color_h0) ? color_h0 : color_h1;

        const f32 f_width  = static_cast<f32>(width);
        const f32 f_height = static_cast<f32>(height);

        f32 v0_x;
        f32 v0_y;
        f32 v1_x;
        f32 v1_y;

        if (zero < f_width)
        {
            if (zero < f_height)
            {
                v0_x = zero;
                v0_y = zero;
                v1_x = f_width;
                v1_y = f_height;
            }
            else
            {
                v0_x = zero;
                v0_y = f_height;
                v1_x = f_width;
                v1_y = zero;
            }
        }
        else
        {
            if (zero < f_height)
            {
                v0_x = f_width;
                v0_y = zero;
                v1_x = zero;
                v1_y = f_height;
            }
            else
            {
                v0_x = f_width;
                v0_y = f_height;
                v1_x = zero;
                v1_y = zero;
            }
        }

        *reinterpret_cast<f32*>(self + 0x744) = f_width;
        *reinterpret_cast<f32*>(self + 0x748) = f_height;
        *reinterpret_cast<f32*>(self + 0x74c) = v0_x;
        *reinterpret_cast<f32*>(self + 0x750) = v0_y;
        *reinterpret_cast<f32*>(self + 0x754) = v1_x;
        *reinterpret_cast<f32*>(self + 0x758) = v1_y;

        mRenderBuffer.setRenderTargetColorNullAll();
        mRenderBuffer.setRenderTargetColor(reinterpret_cast<agl::RenderTargetColor*>(self + 0x480), 0);
        mRenderBuffer.setRenderTargetDepth(reinterpret_cast<agl::RenderTargetDepth*>(self + 0x5d4));

        sub_02A058F0(self + 0x744);

        sub_02A07E70(viewport);
        viewport[0x00] = 1;
        viewport[0x01] = 1;
        *reinterpret_cast<u32*>(viewport + 0x0c) &= ~1u;
        sub_02A07F90(viewport);

        GX2SetPolygonOffset(zero, zero, zero, zero, zero);

        sub_02A074C8(view, self + 0x744);
        sub_02A0783C(view, self + 0x744);

        const sead::Color4f clear_color(zero, zero, one, zero);
        mRenderBuffer.clear(0, 3, clear_color, one, 0);

        if (_9FC)
            mpRenderMgr->drawReflectionOpa(mViewIndex, render_info);

        sub_02A058F0(const_cast<void*>(static_cast<const void*>(render_info.getFrameBuffer())));
        sub_02A0783C(render_info.getViewport(), render_info.getFrameBuffer());
        return;
    }

    case RenderObjLayer::cRenderStep_Xlu:
    {
        if (mpRenderMgr == nullptr)
            return;
        if (mpCameraProjection == nullptr)
            return;
        if (*reinterpret_cast<s32*>(reinterpret_cast<u8*>(mpRenderMgr) + 0x1c) == 0)
            return;
        if (mpColorTarget == nullptr)
            return;
        if (mpDepthTarget == nullptr)
            return;

        sub_02A074C8(view, self + 0x744);
        sub_02A0783C(view, self + 0x744);
        sub_02A058F0(self + 0x744);

        if (_9FC)
            mpRenderMgr->drawReflectionXlu(mViewIndex, render_info);

        sub_024F9B38(this);

        sub_02A058F0(const_cast<void*>(static_cast<const void*>(render_info.getFrameBuffer())));
        sub_02A0783C(render_info.getViewport(), render_info.getFrameBuffer());

        sub_02A1F088(self + 0x480);
        sub_02A24A64(mpColorTarget);

        u8* const p_state = self + 0x784;
        const u8* const p_color = reinterpret_cast<const u8*>(mpColorTarget);

        const bool is_same =
            *reinterpret_cast<const u32*>(self + 0x86c) != 0 &&
            *reinterpret_cast<const u32*>(p_state + 0x04) == *reinterpret_cast<const u32*>(p_color + 0x04) &&
            *reinterpret_cast<const u32*>(p_state + 0x08) == *reinterpret_cast<const u32*>(p_color + 0x08) &&
            *reinterpret_cast<const u32*>(p_state + 0x0c) == *reinterpret_cast<const u32*>(p_color + 0x0c) &&
            *reinterpret_cast<const u32*>(p_state + 0x10) == *reinterpret_cast<const u32*>(p_color + 0x10) &&
            *reinterpret_cast<const u32*>(p_state + 0x14) == *reinterpret_cast<const u32*>(p_color + 0x14) &&
            *reinterpret_cast<const u32*>(p_state + 0x18) == *reinterpret_cast<const u32*>(p_color + 0x18) &&
            *reinterpret_cast<const u32*>(p_state + 0x38) == *reinterpret_cast<const u32*>(p_color + 0x38) &&
            *reinterpret_cast<const u32*>(p_state + 0x34) == *reinterpret_cast<const u32*>(p_color + 0x34) &&
            *reinterpret_cast<const u32*>(p_state + 0x1c) == *reinterpret_cast<const u32*>(p_color + 0x1c);

        if (is_same)
        {
            const u32 v0 = *reinterpret_cast<const u32*>(p_color + 0x28);
            const u32 v1 = *reinterpret_cast<const u32*>(p_color + 0x30);

            *reinterpret_cast<u32*>(p_state + 0x120) = v0;
            *reinterpret_cast<u32*>(p_state + 0x128) = v1;
            *reinterpret_cast<u32*>(p_state + 0x028) = v0;
            *reinterpret_cast<u32*>(p_state + 0x030) = v1;
        }
        else
        {
            sub_02A25990(p_state);
        }

        void* p_renderer = *reinterpret_cast<void**>(reinterpret_cast<u8*>(mpCameraProjection) + 0x420);
        *reinterpret_cast<void**>(reinterpret_cast<u8*>(p_renderer) + 0x4c0) = p_state;
        return;
    }

    default:
        return;
    }
}
