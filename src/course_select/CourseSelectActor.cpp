#include <course_select/CourseSelectActor.h>

#include <actor/ActorCreateParam.h>

// Address: 0x021B2310
CourseSelectActor::CourseSelectActor(const ActorCreateParam& param)
    : ActorBase(param)
    , mPos(param.position)
    , mPosPrev(param.position)
    , mScale(1.0f, 1.0f, 1.0f)
    , _74(1.0f, 1.0f, 1.0f)
    , _80(0.0f, 0.0f, 0.0f)
    , _8c(0.0f, 0.0f, 0.0f)
    , _98(0.0f, 0.0f, 0.0f)
    , _a4(0.0f, 0.0f, 0.0f)
    , _b0(0)
    , _b4(static_cast<u32>(s32(param.angle)))
    , _b8(0)
    , _bc(0)
    , _c0(static_cast<u32>(s32(param.angle)))
    , _c4(0)
    , _c8(0.0f)
    , _cc(0.0f)
    , _d0(1.0f)
    , _d4(0.0f)
    , _d8(0)
{
    // The C++ constructor stores a host vtable at +0 after ActorBase returns.
    // Build the base again and put the CourseSelectActor guest vtable at +0x4C.
    decomp::call<void>(0x02002CE0u, this, &param);
    decomp::be32(reinterpret_cast<u8*>(this) + 0x4Cu) = 0x10042DBCu;
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

    if ((_d8 & 1) != 0)
        return false;

    return (_d8 & 2) == 0;
}