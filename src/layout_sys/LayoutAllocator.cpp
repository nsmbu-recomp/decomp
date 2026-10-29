#include <layout_sys/LayoutAllocator.h>

namespace {

// Address: 0x101F8040
constexpr uintptr_t kCurrentHeapSlot = 0x101F8040u;

// Address: 0x02A0D554
sead::Heap* sub_02A0D554(void* p_heap_mgr);

} // namespace

// Address: 0x02679708
void* LayoutAllocator::Alloc(size_t size, u32 alignment)
{
    sead::Heap* heap = mpHeap;
    uintptr_t vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(heap) + 0xC);
    using Fn = void* (*)(sead::Heap*, size_t, u32);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(vtable + 0x34));
    return fn(heap, size, alignment);
}

// Address: 0x0267971C
void LayoutAllocator::Free(void* memory)
{
    sead::Heap* heap = mpHeap;
    uintptr_t vtable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<u8*>(heap) + 0xC);
    using Fn = void (*)(sead::Heap*, void*);
    Fn fn = reinterpret_cast<Fn>(*reinterpret_cast<uintptr_t*>(vtable + 0x3C));
    fn(heap, memory);
}

// Address: 0x02679730
void LayoutAllocator::setHeap(sead::Heap* heap)
{
    if (heap == nullptr) {
        void* p_mgr = *reinterpret_cast<void**>(kCurrentHeapSlot);
        heap = sub_02A0D554(p_mgr);
    }
    mpHeap = heap;
}
