#include <course_select/CourseSelectActor.h>

namespace {

u8* bytes(void* p)
{
    return reinterpret_cast<u8*>(p);
}

const u8* bytes(const void* p)
{
    return reinterpret_cast<const u8*>(p);
}

void copy_f32(u8* dst, const u8* src)
{
    decomp::bef32(dst) = static_cast<f32>(decomp::bef32(src));
}

void copy_u32(u8* dst, const u8* src)
{
    decomp::be32(dst) = static_cast<u32>(decomp::be32(src));
}

void store_f32(u8* dst, f32 value)
{
    decomp::bef32(dst) = value;
}

void store_vec3(u8* dst, f32 value)
{
    store_f32(dst, value);
    store_f32(dst + 4, value);
    store_f32(dst + 8, value);
}

}

// Address: 0x021B2310
CourseSelectActor::CourseSelectActor(const ActorCreateParam& param)
{
    const u8* src = bytes(&param);
    u8* self = bytes(this);

    decomp::call<void>(0x02002CE0u, this, &param);
    decomp::be32(self + 0x4C) = 0x10042DBCu;

    copy_f32(self + 0x50, src + 0x10);
    copy_f32(self + 0x54, src + 0x14);
    copy_f32(self + 0x58, src + 0x18);
    copy_f32(self + 0x5C, src + 0x10);
    copy_f32(self + 0x60, src + 0x14);
    copy_f32(self + 0x64, src + 0x18);

    // Pool at 0x10042D80, 0x10042D84, 0x10042D88, 0x10042D8C.
    store_vec3(self + 0x68, 0.0f);
    store_vec3(self + 0x74, 0.0f);
    store_vec3(self + 0x80, 1.0f);
    store_vec3(self + 0x8C, 0.0f);
    store_vec3(self + 0x98, 0.0f);
    store_vec3(self + 0xA4, 0.0f);

    decomp::be32(self + 0xB0) = 0;
    copy_u32(self + 0xB4, src + 0x1C);
    decomp::be32(self + 0xB8) = 0;
    decomp::be32(self + 0xBC) = 0;
    copy_u32(self + 0xC0, src + 0x1C);
    decomp::be32(self + 0xC4) = 0;
    store_f32(self + 0xC8, 0.0f);
    store_f32(self + 0xCC, -7.0f);
    store_f32(self + 0xD0, -40.0f);
    store_f32(self + 0xD4, 0.0f);
    self[0xD8] = 0;
}

// Address: 0x021B2528
bool CourseSelectActor::preExecute_()
{
    return true;
}

// Address: 0x021B2658
bool CourseSelectActor::preDraw_()
{
    if (!decomp::call<bool>(0x02002F98u, this))
        return false;

    u8 flag = bytes(this)[0xD8];
    if ((flag & 1) != 0)
        return false;

    return (flag & 2) == 0;
}
