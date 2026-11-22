#include <graphics/DrawQueue.h>

#include <cstring>

extern "C" void OSBlockMove(void* dst, const void* src, u32 size);

// Address: 0x029F1F4C
void sub_029F1F4C(PendingDrawList* list, s32 index, s32 count)
{
    if (list == nullptr || index < 0 || count < 0)
        return;

    const s32 end = index + count;
    if (end > list->mCount)
        return;

    if (count > 0 && end < list->mCount)
    {
        DrawItem** buffer = list->mBuffer;
        OSBlockMove(&buffer[index], &buffer[end], static_cast<u32>((list->mCount - end) * sizeof(DrawItem*)));
    }

    list->mCount -= count;
}