#include <game/CourseTimer.h>

namespace {

// Address: 0x024C13BC
extern void courseTimerUpdate_(CourseTimer* self);

}

SEAD_SINGLETON_DISPOSER_IMPL(CourseTimer)

// Address: 0x024C1344
u32 CourseTimer::toUnits(u16 sec)
{
    return u32(sec) << 12;
}

// Address: 0x024C134C
void CourseTimer::setTimer(u16 sec)
{
    mTime = toUnits(sec);
    mBeginTime = mTime;
}

// Address: 0x024C14C0
void CourseTimer::addTimeLimitSeconds(u16 sec)
{
    u32& max_time = *reinterpret_cast<u32*>(reinterpret_cast<uintptr_t>(0x10219568));

    mTime += toUnits(sec);

    if (mTime > max_time)
        mTime = max_time;

    courseTimerUpdate_(this);
}
