#include <course_select/CourseSelectLayerMgr.h>
#include <graphics/RenderObjLayer.h>
#include <graphics/RenderObjRenderMgr.h>

#include <gfx/seadGraphics.h>

namespace {

struct LayerCreateInfo
{
    const char* name;
    const char* debug_name;
};

// Address: 0x021EEFF8
RenderObjLayerBase* createShadowRenderObjLayer(s32 layer_id, const LayerCreateInfo* p_info, s32 arg, sead::Heap* heap);

// Address: 0x021EF074
RenderObjLayerBase* createReflectionRenderObjLayer(s32 layer_id, const LayerCreateInfo* p_info, s32 arg, sead::Heap* heap);

// Address: 0x021EF0F0
RenderObjLayerBase* createProcLayer(s32 layer_id, const LayerCreateInfo* p_info, s32 arg, sead::Heap* heap);

// Address: 0x021EF180
RenderObjLayerBase* createRenderObjLayer(s32 layer_id, const LayerCreateInfo* p_info, s32 arg, sead::Heap* heap);

} // namespace

// Address: 0x021EE9E4
void CourseSelectLayerMgr::createLayers(sead::Heap* heap)
{
    sead::Graphics::instance()->lockDrawContext();

    CourseSelectLayerMgr* self = CourseSelectLayerMgr::instance();

    RenderObjRenderMgr* p_mgr_course_select =
        new (heap, 4) RenderObjRenderMgr(sead::SafeString("CourseSelect"));
    p_mgr_course_select->initialize(6, 0x400, 5, 5, heap);

    RenderObjRenderMgr* p_mgr_course_select_world =
        new (heap, 4) RenderObjRenderMgr(sead::SafeString("CourseSelectWorld"));
    p_mgr_course_select_world->initialize(1, 0x200, 5, 5, heap);

    const char* const debug_name = "";  // Address: 0x10046F68

    {
        LayerCreateInfo info = { "DepthShadow", debug_name };
        RenderObjLayerBase* p_layer = createShadowRenderObjLayer(5, &info, 0, heap);
        p_layer->setRenderMgr(p_mgr_course_select);
        *reinterpret_cast<u8*>(reinterpret_cast<uint8_t*>(p_layer) + 0x1151) = 1;
    }

    {
        LayerCreateInfo info = { "Reflection", debug_name };
        RenderObjLayerBase* p_layer = createReflectionRenderObjLayer(3, &info, 0, heap);
        p_layer->setRenderMgr(p_mgr_course_select);
    }

    {
        LayerCreateInfo info = { "2D_Pre3D", debug_name };
        createProcLayer(6, &info, 0, heap);
    }

    {
        LayerCreateInfo info = { "3D", debug_name };
        RenderObjLayerBase* p_layer = createRenderObjLayer(7, &info, 0, heap);
        p_layer->setRenderMgr(p_mgr_course_select);
    }

    {
        LayerCreateInfo info = { "3D_World", debug_name };
        RenderObjLayerBase* p_layer = createRenderObjLayer(9, &info, 0, heap);
        p_layer->setRenderMgr(p_mgr_course_select_world);
    }

    {
        LayerCreateInfo info = { "2D", debug_name };
        RenderObjLayerBase* p_layer = createProcLayer(8, &info, 0, heap);
        *reinterpret_cast<s32*>(reinterpret_cast<uint8_t*>(p_layer) + 0x60) = 4;
    }

    {
        LayerCreateInfo info = { "2D_Menu", debug_name };
        RenderObjLayerBase* p_layer = createProcLayer(0xA, &info, 0, heap);
        *reinterpret_cast<s32*>(reinterpret_cast<uint8_t*>(p_layer) + 0x60) = 4;
    }

    {
        LayerCreateInfo info = { "DepthShadow_DRC", debug_name };
        RenderObjLayerBase* p_layer = createShadowRenderObjLayer(0x17, &info, 1, heap);
        p_layer->setRenderMgr(p_mgr_course_select);
        *reinterpret_cast<u8*>(reinterpret_cast<uint8_t*>(p_layer) + 0x1151) = 1;
    }

    {
        LayerCreateInfo info = { "ReflectionDRC", debug_name };
        RenderObjLayerBase* p_layer = createReflectionRenderObjLayer(0x15, &info, 1, heap);
        p_layer->setRenderMgr(p_mgr_course_select);
    }

    {
        LayerCreateInfo info = { "2D_Pre3D_DRC", debug_name };
        createProcLayer(0x18, &info, 1, heap);
    }

    {
        LayerCreateInfo info = { "3D_DRC", debug_name };
        RenderObjLayerBase* p_layer = createRenderObjLayer(0x19, &info, 1, heap);
        p_layer->setRenderMgr(p_mgr_course_select);
    }

    {
        LayerCreateInfo info = { "2D_DRC", debug_name };
        RenderObjLayerBase* p_layer = createProcLayer(0x1A, &info, 1, heap);
        *reinterpret_cast<s32*>(reinterpret_cast<uint8_t*>(p_layer) + 0x60) = 4;
    }

    sead::Graphics::instance()->unlockDrawContext();
}
