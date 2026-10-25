#include <graphics/ModelMtxUpload.h>

struct OrthoProjectionDelegate
{
    u8 _00[0x2C];
    void (*syncParams)(OrthoProjectionSync* projection, void* params_at_4);
    void (*buildViewProj)(OrthoProjectionSync* projection, sead::Matrix44f* dst, void* params_at_4, void* field_84);
};

struct OrthoProjectionSync
{
    u8              mDirty0;
    u8              mDirty1;
    u8              _02[2];
    u8              mParams[0x40];
    sead::Matrix44f mViewProj;
    void*           mField84;
    OrthoProjectionDelegate* mpDelegate;
};

namespace {

// Address: 0x02A059B0
void OrthoProjection_sync_(OrthoProjectionSync* projection)
{
    u8 saved = 0;

    if (projection->mDirty0 != 0)
    {
        OrthoProjectionDelegate* delegate = projection->mpDelegate;
        delegate->syncParams(projection, projection->mParams);
        projection->mDirty0 = 0;
        projection->mDirty1 = 1;
        delegate->buildViewProj(projection, &projection->mViewProj, projection->mParams, projection->mField84);
        projection->mDirty1 = saved;
    }
    else if (projection->mDirty1 != 0)
    {
        OrthoProjectionDelegate* delegate = projection->mpDelegate;
        delegate->buildViewProj(projection, &projection->mViewProj, projection->mParams, projection->mField84);
        projection->mDirty1 = saved;
    }
}

}

// Address: 0x02A05A9C
sead::Matrix44f* OrthoProjection_syncViewProj(OrthoProjectionSync* projection)
{
    OrthoProjection_sync_(projection);
    return &projection->mViewProj;
}

// Address: 0x02AA56EC
void ModelMtxUpload_calcViewFromModelEx(void* model_ex, s32 view_index)
{
    void* model_obj = *reinterpret_cast<void**>(static_cast<u8*>(model_ex) + 0x18);
    ModelMtxUpload_calcView(model_obj, view_index);
}

// Address: 0x02AA56F4
void ModelMtxUpload_uploadShapeBlockMatrices(void* model_ex, s32 view_index)
{
    u8* self = static_cast<u8*>(model_ex);
    u8* model_obj = static_cast<u8*>(*reinterpret_cast<void**>(self + 0x18));
    u16& model_flags = *reinterpret_cast<u16*>(model_obj + 4);
    model_flags = static_cast<u16>(model_flags | 0x10);

    u16 shape_count = *reinterpret_cast<u16*>(self + 0x14);
    u8* mtx_block_base = static_cast<u8*>(*reinterpret_cast<void**>(model_obj + 0x10));
    u8* shape_array = static_cast<u8*>(*reinterpret_cast<void**>(self + 0x1C));

    for (u16 i = 0; i < shape_count; i++)
    {
        u8* entry = shape_array + i * 0x28;
        u8* shape = static_cast<u8*>(*reinterpret_cast<void**>(entry));

        if (shape[0x16] != 0)
            continue;

        if (entry[9] != 0)
            continue;

        u16 shape_index = *reinterpret_cast<u16*>(shape + 0x10);
        u8* dst_mtx = mtx_block_base + shape_index * 0x30;

        ModelMtxUpload_writeShapeEntry(entry, 0, dst_mtx, view_index);
    }
}
