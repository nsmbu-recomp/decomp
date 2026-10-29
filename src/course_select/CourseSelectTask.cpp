#include <course_select/CourseSelectTask.h>

#include <game/FieldGame.h>

// setInstance_()                              Address: 0x022656C0
// SingletonDisposer_::~SingletonDisposer_()   Address: 0x0226551C
SEAD_TASK_SINGLETON_DISPOSER_IMPL(CourseSelectTask)

// Address: 0x022656E0
CourseSelectTask::CourseSelectTask(const sead::TaskConstructArg& arg)
    : sead::CalculateTask(arg)
{
    u8* p_this = reinterpret_cast<u8*>(this);

    *reinterpret_cast<u32*>(p_this + 0xDC0) = 0;

    p_this[0xDC8] = 0;
    p_this[0xDCA] = 0;
    p_this[0xDCC] = 1;
    p_this[0xDCD] = 0;
    p_this[0xDCE] = 0;
    p_this[0xDCF] = 0;

    _dc9 = false;

    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(FieldGame::instance()) + 0x16C) = 0;
}

// Address: 0x02265548
CourseSelectTask::~CourseSelectTask()
{
}
