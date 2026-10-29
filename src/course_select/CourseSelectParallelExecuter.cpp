#include <heap/seadDisposer.h>

class CourseSelectParallelExecuter
{
public:
    // Address: 0x02206634
    CourseSelectParallelExecuter();
    // Address: 0x022066DC
    ~CourseSelectParallelExecuter();

    // Address: 0x02206788
    void initialize(sead::Heap* heap);

    // Address: 0x02206A80
    void clear();

    // Address: 0x02206B7C
    void executeAll();

protected:
    void*   mItem[0x1000];      // +0x0000
    s32     mItemNum;           // +0x4000

    struct WorkBuffer           // +0x4004
    {
        s32     mCount;
        s32     mCapacity;
        void**  mpBuffer;
        void*   mInline[3];
    };
    static_assert(sizeof(WorkBuffer) == 0x18);

    WorkBuffer  mWorkBuffer;
    u32         _401c;          // +0x401C
    const void* _4020;          // +0x4020
};
static_assert(sizeof(CourseSelectParallelExecuter) == 0x4024);

// Address: 0x02206634
CourseSelectParallelExecuter::CourseSelectParallelExecuter()
{
    _4020 = reinterpret_cast<const void*>(0x10058B98);
    mItemNum = 0;

    mWorkBuffer.mCount = 0;
    mWorkBuffer.mCapacity = 3;
    mWorkBuffer.mpBuffer = mWorkBuffer.mInline;

    _401c = 0;
}

// Address: 0x022066DC
CourseSelectParallelExecuter::~CourseSelectParallelExecuter()
{
    void** p_item = mWorkBuffer.mpBuffer;
    void** p_item_end = p_item + mWorkBuffer.mCount;

    _4020 = reinterpret_cast<const void*>(0x10058B98);

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
