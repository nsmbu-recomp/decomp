#include <game/AreaLayerMgr.h>
#include <graphics/AnimModel.h>
#include <graphics/ModelG3d.h>
#include <graphics/Renderer.h>
#include <graphics/RenderObjLayer.h>
#include <graphics/RenderObjRenderMgr.h>
#include <map/BgRenderer.h>
#include <map/LayerID.h>
#include <map/UnitID.h>

#include <layer/aglRenderer.h>

SEAD_SINGLETON_DISPOSER_IMPL(Renderer)

Renderer::Renderer()
    : mpLayer(nullptr)
    , mDefaultOpaRenderPass(3)
    , mDefaultXluRenderPass(6)
{
}

Renderer::~Renderer()
{
}

void Renderer::setViewBoundBox(const sead::BoundBox2f& box)
{
    mViewBoundBox.setMin(box.getMin());
    mViewBoundBox.setMax(box.getMax());
}

// Address: 0x024FA76C
void Renderer::calcForAreaTask()
{
    const f32 min_x = mViewBoundBox.getMin().x;
    const f32 min_y = mViewBoundBox.getMin().y;
    const f32 max_x = mViewBoundBox.getMax().x;
    const f32 max_y = mViewBoundBox.getMax().y;

    mProjection3D.setNear(1.0f);
    mProjection3D.setFar(20000.0f);
    mProjection3D.setTBLR(-min_y, -max_y, min_x, max_x);

    mCamera3D.getPos().set(0.0f, 0.0f, 10000.0f);
    mCamera3D.getAt().set(0.0f, 0.0f, -1.0f);
    mCamera3D.getUp().set(sead::Vector3f::ey);
    mCamera3D.getUp().normalize();
    mCamera3D.updateViewMatrix();

    agl::lyr::Renderer* p_agl_renderer = agl::lyr::Renderer::instance();
    const uintptr_t agl_base = reinterpret_cast<uintptr_t>(p_agl_renderer);

    u32 layer_count = *reinterpret_cast<const u32*>(agl_base + 0xda8);
    uintptr_t layer_table = *reinterpret_cast<uintptr_t*>(agl_base + 0xdac);
    uintptr_t layer_entry = layer_table;
    if (layer_count > 9)
        layer_entry += 0x24;
    agl::lyr::Layer* p_layer_3d = *reinterpret_cast<agl::lyr::Layer**>(layer_entry);
    p_layer_3d->setProjection(&mProjection3D);

    layer_count = *reinterpret_cast<const u32*>(agl_base + 0xda8);
    layer_table = *reinterpret_cast<uintptr_t*>(agl_base + 0xdac);
    layer_entry = layer_table;
    if (layer_count > 9)
        layer_entry += 0x24;
    p_layer_3d = *reinterpret_cast<agl::lyr::Layer**>(layer_entry);
    p_layer_3d->setCamera(&mCamera3D);

    layer_count = *reinterpret_cast<const u32*>(agl_base + 0xda8);
    layer_table = *reinterpret_cast<uintptr_t*>(agl_base + 0xdac);
    layer_entry = layer_table;
    if (layer_count > 0x17)
        layer_entry += 0x5c;
    agl::lyr::Layer* p_layer_3d_drc = *reinterpret_cast<agl::lyr::Layer**>(layer_entry);
    p_layer_3d_drc->setProjection(&mProjection3D);

    layer_count = *reinterpret_cast<const u32*>(agl_base + 0xda8);
    layer_table = *reinterpret_cast<uintptr_t*>(agl_base + 0xdac);
    layer_entry = layer_table;
    if (layer_count > 0x17)
        layer_entry += 0x5c;
    p_layer_3d_drc = *reinterpret_cast<agl::lyr::Layer**>(layer_entry);
    p_layer_3d_drc->setCamera(&mCamera3D);

    layer_count = *reinterpret_cast<const u32*>(agl_base + 0xda8);
    layer_table = *reinterpret_cast<uintptr_t*>(agl_base + 0xdac);
    layer_entry = layer_table;
    if (layer_count > 8)
        layer_entry += 0x20;
    agl::lyr::Layer* p_final_koopa_layer = *reinterpret_cast<agl::lyr::Layer**>(layer_entry);
    if (p_final_koopa_layer != nullptr)
    {
        const f32 center_x = (min_x + max_x) * 0.5f;
        const f32 center_y = (-min_y - max_y) * 0.5f;
        const f32 half_size_x = (max_x - min_x) * 0.5f;
        const f32 half_size_y = (max_y - min_y) * 0.5f;

        mProjectionFinalKoopa.setNear(1.0f);
        mProjectionFinalKoopa.setFar(20000.0f);
        mProjectionFinalKoopa.setTBLR(half_size_y, -half_size_y, -half_size_x, half_size_x);

        mCameraFinalKoopa.getPos().set(center_x, center_y, 10000.0f);
        mCameraFinalKoopa.getAt().set(center_x, center_y, -1.0f);
        mCameraFinalKoopa.getUp().set(sead::Vector3f::ey);
        mCameraFinalKoopa.getUp().normalize();
        mCameraFinalKoopa.updateViewMatrix();

        p_final_koopa_layer->setProjection(&mProjectionFinalKoopa);
        p_final_koopa_layer->setCamera(&mCameraFinalKoopa);
    }
}

void Renderer::setLayer(agl::lyr::Layer* p_layer, RenderPassType render_pass_type)
{
    mpLayer = sead::DynamicCast<RenderObjLayerBase>(p_layer);

    switch (render_pass_type)
    {
    case cRenderPassType_Course:
        mDefaultOpaRenderPass = 3;
        mDefaultXluRenderPass = 6;
        break;
    case cRenderPassType_CourseSelect:
        mDefaultOpaRenderPass = 1;
        mDefaultXluRenderPass = 0;
        break;
    case cRenderPassType_DemoScene:
    case cRenderPassType_Misc:
        mDefaultOpaRenderPass = 0;
        mDefaultXluRenderPass = 0;
        break;
    }
}

void Renderer::resetLayer()
{
    mpLayer = nullptr;
}

void Renderer::drawModel(Model* p_model)
{
    s32 opa_render_pass;
    s32 xlu_render_pass;

    if (p_model->hasOpa())
    {
        opa_render_pass = p_model->getOpaRenderPass();
        if (opa_render_pass == cDefaultRenderPass)
            opa_render_pass = mDefaultOpaRenderPass;
    }
    else
    {
        opa_render_pass = -1;
    }

    if (p_model->hasXlu())
    {
        xlu_render_pass = p_model->getXluRenderPass();
        if (xlu_render_pass == cDefaultRenderPass)
            xlu_render_pass = mDefaultXluRenderPass;
    }
    else
    {
        xlu_render_pass = -1;
    }

    p_model->getBaseModelMtx().getTranslation(p_model->getOrderPos());

    mpLayer->getRenderMgr()->pushBackRenderObj(p_model, opa_render_pass, xlu_render_pass);
}

void Renderer::drawModel(const AnimModel* p_model)
{
    drawModel(p_model->getModel());
}

// TODO
/*
void Renderer::drawModel(ModelFFL* p_model)
{
    s32 opa_render_pass;
    s32 xlu_render_pass;

    opa_render_pass = p_model->getOpaRenderPass();
    if (opa_render_pass == cDefaultRenderPass)
        opa_render_pass = mDefaultOpaRenderPass;

    xlu_render_pass = p_model->getXluRenderPass();
    if (xlu_render_pass == cDefaultRenderPass)
        xlu_render_pass = mDefaultXluRenderPass;

    p_model->getBaseModelMtx().getTranslation(p_model->getOrderPos());

    mpLayer->getRenderMgr()->pushBackRenderObj(p_model, opa_render_pass, xlu_render_pass);
}
*/

void Renderer::drawActorBgUnit(UnitID unit, const sead::Vector3f& pos, Angle angle, const sead::Vector3f& scale)
{
    BgRenderer::instance()->drawActorBgUnit(
        (pos.z < 0
            ? cLayerID_Layer2
            : (pos.z < 3600
                  ? cLayerID_Layer1
                  : cLayerID_Layer0_Actor)),
        unit,
        pos,
        Angle3(0, 0, angle),
        scale
    );
}

void Renderer::drawActorBgUnitLayer0(UnitID unit, const sead::Vector3f& pos, Angle angle, const sead::Vector3f& scale)
{
    BgRenderer::instance()->drawActorBgUnit(
        cLayerID_Layer0_Actor,
        unit,
        pos,
        Angle3(0, 0, angle),
        scale
    );
}
