#include <course_select/CourseSelectObjActor.h>

#include <new>

namespace {

u32 word(void* self, u32 offset)
{
    return static_cast<u32>(decomp::be32(reinterpret_cast<u8*>(self) + offset));
}

}

// Address: 0x02201C04
CourseSelectObjActor::CourseSelectObjActor(const ActorCreateParam& param)
{
    new (this) CourseSelectDemoActor(param);

    u8* self = reinterpret_cast<u8*>(this);
    decomp::be32(self + 0x108) = 0xFFFFFFFFu;
    decomp::be32(self + 0x4C) = 0x10048168u;
}

// Address: 0x02201C70
s32 CourseSelectObjActor::queryMapStateA()
{
    return decomp::call<s32>(0x021F0008u, word(this, 0xF0), word(this, 0xFC), word(this, 0xF8));
}

// Address: 0x02201C84
s32 CourseSelectObjActor::queryMapStateB()
{
    return decomp::call<s32>(0x021F07A8u, word(this, 0xF0), word(this, 0xFC));
}

// Address: 0x02201C90
s32 CourseSelectObjActor::queryMapStateC()
{
    return decomp::call<s32>(0x021F05BCu, word(this, 0xF0), word(this, 0xFC));
}

// Address: 0x02201C9C
s32 CourseSelectObjActor::queryMapStateD()
{
    return decomp::call<s32>(0x021F0690u, word(this, 0xF0), word(this, 0xFC));
}

// Address: 0x02201CA8
s32 CourseSelectObjActor::queryMapStateE()
{
    return decomp::call<s32>(0x021F071Cu, word(this, 0xF0), word(this, 0xFC));
}
