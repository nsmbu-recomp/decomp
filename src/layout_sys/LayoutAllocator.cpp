#include <layout_sys/LayoutAllocator.h>

namespace {

// Address: 0x02A0D554
sead::Heap* sub_02A0D554(void* p_heap_mgr);

} // namespace

// Address: 0x02679708
void* LayoutAllocator::Alloc(size_t size, u32 alignment)
{
    u32 heap = decomp::be32(reinterpret_cast<u8*>(this) + 4);
    u32 vtable = decomp::be32(heap + 0x0Cu);
    auto fn = decomp::make_fn<void*, sead::Heap*, size_t, u32>(decomp::fn_addr(vtable + 0x34u));
    return fn(reinterpret_cast<sead::Heap*>(static_cast<uintptr_t>(heap)), size, alignment);
}

// Address: 0x0267971C
void LayoutAllocator::Free(void* memory)
{
    u32 heap = decomp::be32(reinterpret_cast<u8*>(this) + 4);
    u32 vtable = decomp::be32(heap + 0x0Cu);
    auto fn = decomp::make_fn<void, sead::Heap*, void*>(decomp::fn_addr(vtable + 0x3Cu));
    fn(reinterpret_cast<sead::Heap*>(static_cast<uintptr_t>(heap)), memory);
}

// Address: 0x02679730
void LayoutAllocator::setHeap(sead::Heap* heap)
{
    u32 value = static_cast<u32>(reinterpret_cast<uintptr_t>(heap));
    if (value == 0)
        value = static_cast<u32>(reinterpret_cast<uintptr_t>(sub_02A0D554(nullptr)));
    decomp::be32(reinterpret_cast<u8*>(this) + 4) = value;
}
