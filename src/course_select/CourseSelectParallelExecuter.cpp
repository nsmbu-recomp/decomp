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