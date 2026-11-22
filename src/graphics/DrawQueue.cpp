#include <graphics/DrawQueue.h>

#include <graphics/ModelFFL.h>

#include <cstring>

namespace {

constexpr u32 kCriticalSectionOffset = 0x35D4;
constexpr u32 kOpaqueListOffset = 0x268;
constexpr u32 kMiiListOffset = 0xE7C;
constexpr u32 kMultiListOffset = 0x1A90;
constexpr u32 kReflectionListOffset = 0x26A4;

// Address: 0x024D6EAC
bool sub_024D6EAC(ModelDrawMgr* mgr, u32 layer_node, void** p_out, s32 pass_count);

// Address: 0x02AF9AD4
void sub_02AF9AD4(f32* sort_key, f32 a, f32 b, f32 c);

// Address: 0x02AF986C
void sub_02AF986C(Mtxf* mtx, const void* src);

// Address: 0x0205ABC8
void sub_0205ABC8(Mtxf* draw_mtx, const void* sort_pos, Mtxf* out);

// Address: 0x02975894
void sub_02975894(Model* model, Mtxf* draw_info);

// Address: 0x024E8E50
void sub_024E8E50(void* node_sub, f32* out_sort);

// Address: 0x024E8EB0
void sub_024E8EB0(Model* model, void* draw_packet);

// Address: 0x02AFA350
void sub_02AFA350(void* dst, s32 count, s32 stride, const void* src);

using ModelVFunc7C = void (*)(Model* model, Mtxf* draw_info);
using ModelVFunc74 = void (*)(Model* model);
using ModelVFunc8C = void (*)(Model* model, const void* field_10);

PendingDrawList* listAt_(ModelDrawMgr* mgr, u32 offset)
{
    return reinterpret_cast<PendingDrawList*>(reinterpret_cast<u8*>(mgr) + offset);
}

void recycleFront_(PendingDrawList& list)
{
    if (list.mCount <= 0)
        return;

    DrawItem* item = list.mBuffer[0];
    item->mLayerNode = reinterpret_cast<u32>(list.mpFree);
    list.mpFree = item;
}

void buildMatrix_(Mtxf& draw_mtx, const DrawItem& item)
{
    sub_02AF9AD4(&draw_mtx.v.x, item.mSortKey0, item.mSortKey1, item.mSortKey2);
    draw_mtx.YrotM(item.mAngleY);
    draw_mtx.XrotM(item.mAngleX);
    draw_mtx.ZrotM(item.mAngleZ);
}

void drainList_(ModelDrawMgr* mgr, PendingDrawList& list,
                void (*process)(ModelDrawMgr*, DrawItem*, void**))
{
    while (list.mCount > 0)
    {
        DrawItem* item = list.mBuffer[0];
        void* out = nullptr;
        process(mgr, item, &out);
        recycleFront_(list);
        sub_029F1F4C(&list, 0, 1);
    }
}

void processOpaque_(ModelDrawMgr* mgr, DrawItem* item, void** p_out)
{
    Model* model = item->mpModel;
    Mtxf draw_mtx;
    buildMatrix_(draw_mtx, *item);

    auto** vtable = reinterpret_cast<uintptr_t**>(model);
    reinterpret_cast<ModelVFunc7C>((*vtable)[0x7C / 4])(model, &draw_mtx);
    reinterpret_cast<ModelVFunc8C>((*vtable)[0x8C / 4])(model, item + 1);
    reinterpret_cast<ModelVFunc74>((*vtable)[0x74 / 4])(model);

    *p_out = model;
    sub_024D6EAC(mgr, item->mLayerNode, p_out, 1);
}

void processMii_(ModelDrawMgr* mgr, DrawItem* item, void** p_out)
{
    ModelFFL* model = static_cast<ModelFFL*>(item->mpModel);
    Mtxf draw_mtx;
    buildMatrix_(draw_mtx, *item);

    sub_02AF986C(&draw_mtx, reinterpret_cast<u8*>(model) + 0x838);

    reinterpret_cast<f32*>(reinterpret_cast<u8*>(model) + 0x868))[0] = item->mSortKey0;
    reinterpret_cast<f32*>(reinterpret_cast<u8*>(model) + 0x86C))[0] = item->mSortKey1;
    reinterpret_cast<f32*>(reinterpret_cast<u8*>(model) + 0x870))[0] = item->mSortKey2;

    model->calcModelMtx();
    model->setSpecialDrawType();

    *p_out = model;
    sub_024D6EAC(mgr, item->mLayerNode, p_out, 1);
}

void processMulti_(ModelDrawMgr* mgr, DrawItem* item, void** p_out)
{
    Model* model = item->mpModel;
    Mtxf draw_mtx;
    buildMatrix_(draw_mtx, *item);

    sub_0205ABC8(&draw_mtx, item + 1, &draw_mtx);
    sub_02975894(model, &draw_mtx);

    ModelFFL* sub_model = static_cast<ModelFFL*>(*reinterpret_cast<Model**>(reinterpret_cast<u8*>(model) + 0x20));
    sub_model->setSpecialDrawType();

    void** layer_entry = reinterpret_cast<void**>(reinterpret_cast<u8*>(model) + 0x18);
    *p_out = layer_entry != nullptr ? *layer_entry : nullptr;

    sub_024D6EAC(mgr, item->mLayerNode, p_out, 2);
}

void processReflection_(ModelDrawMgr* mgr, DrawItem* item, void** p_out)
{
    ModelFFL* model = static_cast<ModelFFL*>(item->mpModel);
    Mtxf draw_mtx;
    buildMatrix_(draw_mtx, *item);

    sub_02AF986C(&draw_mtx, reinterpret_cast<u8*>(model) + 0x838);

    reinterpret_cast<f32*>(reinterpret_cast<u8*>(model) + 0x868))[0] = item->mSortKey0;
    reinterpret_cast<f32*>(reinterpret_cast<u8*>(model) + 0x86C))[0] = item->mSortKey1;
    reinterpret_cast<f32*>(reinterpret_cast<u8*>(model) + 0x870))[0] = item->mSortKey2;

    model->calcModelMtx();
    model->setSpecialDrawType();

    f32 sort_scratch[4];
    void* node_sub = *reinterpret_cast<void**>(reinterpret_cast<u8*>(item) + 0x2C);
    sub_024E8E50(node_sub, sort_scratch);

    u8 packet[0xA8];
    std::memset(packet, 0, sizeof(packet));
    sub_02AFA350(packet, 4, 0x24, reinterpret_cast<u8*>(mgr) + 0x76B8);
    sub_024E8EB0(model, packet);

    *p_out = model;
    sub_024D6EAC(mgr, item->mLayerNode, p_out, 2);
}

} // namespace

// Address: 0x024D752C
void ModelDrawMgr::buildQueues(const u32* flag)
{
    sead::CriticalSection* lock =
        reinterpret_cast<sead::CriticalSection*>(reinterpret_cast<u8*>(this) + kCriticalSectionOffset);

    lock->lock();
    if (*flag == 0)
    {
        buildOpaqueQueue_();
        buildMiiQueue_();
        buildMultiQueue_();
        buildReflectionQueue_();
    }
    lock->unlock();
}

// Address: 0x024D6F5C
void ModelDrawMgr::buildOpaqueQueue_()
{
    drainList_(this, *listAt_(this, kOpaqueListOffset), processOpaque_);
}

// Address: 0x024D7088
void ModelDrawMgr::buildMiiQueue_()
{
    drainList_(this, *listAt_(this, kMiiListOffset), processMii_);
}

// Address: 0x024D71A4
void ModelDrawMgr::buildMultiQueue_()
{
    drainList_(this, *listAt_(this, kMultiListOffset), processMulti_);
}

// Address: 0x024D72C0
void ModelDrawMgr::buildReflectionQueue_()
{
    drainList_(this, *listAt_(this, kReflectionListOffset), processReflection_);
}