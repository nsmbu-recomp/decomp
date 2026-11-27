#include <course_select/CourseSelectDemoActor.h>

#include <course_select/CourseSelectActor.h>

#include <new>

// Address: 0x021D4CB4
CourseSelectDemoActor::CourseSelectDemoActor(const ActorCreateParam& param)
{
    new (this) CourseSelectActor(param);

    u8* self = reinterpret_cast<u8*>(this);
    const u8* src = reinterpret_cast<const u8*>(&param);

    decomp::be32(self + 0x4C) = 0x10044AE0u;
    decomp::bef32(self + 0xDC) = 1.0f;
    decomp::bef32(self + 0xE0) = 1.0f;
    decomp::bef32(self + 0xE4) = 1.0f;
    decomp::be32(self + 0xE8) = 0;
    decomp::be32(self + 0xEC) = 0;
    decomp::be32(self + 0xF0) = src[0x22];
    decomp::be32(self + 0xF4) = src[0x20];
    decomp::be32(self + 0xF8) = src[0x24];
    decomp::be32(self + 0xFC) = src[0x21];
    decomp::be32(self + 0x100) = 0;
    self[0x104] = 0;
}
