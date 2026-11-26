constexpr u32 kArchiveLoadGateA = 0x101E9B18u;
constexpr u32 kArchiveLoadGateB = 0x101E9B1Cu;
constexpr u32 kArchiveLoadMessageA = 0x101EBBC8u;
constexpr u32 kArchiveLoadMessageB = 0x101EBBCCu;
constexpr u32 kArchiveLoadLabelRodata = 0x1018D80Cu;

static s32 invokeArchiveHolderLoad_(void* node, u32 message)
{
    u32 child = decomp::be32(reinterpret_cast<u8*>(node) + 4);
    return decomp::call<s32>(decomp::fn_addr(child + 0x0Cu), node, message);
}

// Address: 0x029E779C
void* sub_029E779C(void* archive_holder)
{
    if (archive_holder == nullptr)
        return nullptr;

    void* node = *reinterpret_cast<void**>(reinterpret_cast<u8*>(archive_holder) + 0x220);
    if (node == nullptr)
        return nullptr;

    u32 kind = *reinterpret_cast<u32*>(node);
    if (kind < 1)
    {
        if (decomp::be32(kArchiveLoadGateA) == 0)
        {
            decomp::be32(kArchiveLoadMessageA) = kArchiveLoadLabelRodata;
            decomp::be32(kArchiveLoadGateA) = 1;
        }
    }
    else if (kind == 1)
    {
        if (decomp::be32(kArchiveLoadGateB) == 0)
        {
            decomp::be32(kArchiveLoadMessageB) = kArchiveLoadLabelRodata;
            decomp::be32(kArchiveLoadGateB) = 1;
        }
    }
    else
    {
        return nullptr;
    }

    u32 message = kind < 1 ? kArchiveLoadMessageA : kArchiveLoadMessageB;
    if (invokeArchiveHolderLoad_(node, message) == 0)
        return nullptr;

    return reinterpret_cast<u8*>(node) + 8;
}
