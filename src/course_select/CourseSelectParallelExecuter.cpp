#include <course_select/CourseSelectParallelExecuter.h>

// Address: 0x02206634
CourseSelectParallelExecuter::CourseSelectParallelExecuter()
{
    _4020 = 0x10058B98u;
    mItemNum = 0;

    mWorkBuffer.mCount = 0;
    mWorkBuffer.mCapacity = 3;
    mWorkBuffer.mpBuffer = reinterpret_cast<void**>(reinterpret_cast<u8*>(this) + 0x4010u);

    _401c = 0;
}

// Address: 0x022066DC
CourseSelectParallelExecuter::~CourseSelectParallelExecuter()
{
    void** p_item = mWorkBuffer.mpBuffer;
    void** p_item_end = p_item + mWorkBuffer.mCount;

    _4020 = 0x10058B98u;

    for (; p_item != p_item_end; p_item++)
    {
        void** pp_object = static_cast<void**>(*p_item);

        if (pp_object == nullptr)
            continue;

        void** p_vtbl = static_cast<void**>(*pp_object);
        typedef void (*DestroyFunc)(void* p_object, s32 arg);

        DestroyFunc p_destroy = reinterpret_cast<DestroyFunc>(p_vtbl[0x44 / sizeof(void*)]);
        p_destroy(pp_object, 0);
    }
}