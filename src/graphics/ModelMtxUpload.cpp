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
    u8* self = reinterpret_cast<u8*>(projection);
    if (self[0] != 0)
    {
        u32 delegate = static_cast<u32>(decomp::be32(self + 0x90));
        u32 sync = static_cast<u32>(decomp::fn_addr(delegate + 0x2Cu));
        decomp::call<void>(sync, self, self + 4);
        self[0] = 0;
        self[1] = 1;
        delegate = static_cast<u32>(decomp::be32(self + 0x90));
        u32 build = static_cast<u32>(decomp::fn_addr(delegate + 0x34u));
        u32 field = static_cast<u32>(decomp::be32(self + 0x84));
        decomp::call<void>(build, self, self + 0x44, self + 4, reinterpret_cast<void*>(static_cast<uintptr_t>(field)));
        self[1] = 0;
    }
    else if (self[1] != 0)
    {
        u32 delegate = static_cast<u32>(decomp::be32(self + 0x90));
        u32 build = static_cast<u32>(decomp::fn_addr(delegate + 0x34u));
        u32 field = static_cast<u32>(decomp::be32(self + 0x84));
        decomp::call<void>(build, self, self + 0x44, self + 4, reinterpret_cast<void*>(static_cast<uintptr_t>(field)));
        self[1] = 0;
    }
}

}

// Address: 0x02A05A9C
sead::Matrix44f* OrthoProjection_syncViewProj(OrthoProjectionSync* projection)
{
    OrthoProjection_sync_(projection);
    return reinterpret_cast<sead::Matrix44f*>(reinterpret_cast<u8*>(projection) + 0x44);
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
    u16 model_flags = *reinterpret_cast<u16*>(model_obj + 4);
    *reinterpret_cast<u16*>(model_obj + 4) = static_cast<u16>(model_flags | 0x10);

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
