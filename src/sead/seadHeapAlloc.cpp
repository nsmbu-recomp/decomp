namespace sead {

class HeapMgr;

// Address: 0x02A0D554
HeapMgr* HeapMgr_getCurrentHeap();

}

// Address: 0x029F18DC
void* sub_029F18DC(void* size_or_null, sead::HeapMgr* heap, u32 alignment)
{
    sead::HeapMgr* active = heap;
    if (active == nullptr)
        active = sead::HeapMgr_getCurrentHeap();
    if (active == nullptr)
        return nullptr;

    u32 size = static_cast<u32>(reinterpret_cast<uintptr_t>(size_or_null));
    u32 vtable = decomp::be32(reinterpret_cast<u8*>(active) + 0x0C);
    auto alloc = decomp::make_fn<void*, sead::HeapMgr*, u32, u32>(decomp::fn_addr(vtable + 0x34));
    return alloc(active, size, alignment);
}