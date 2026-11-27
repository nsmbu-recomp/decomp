#include <layout/GamesceneBase.h>

namespace {

u32 rotl2_align(u32 value)
{
    return ((value << 2) | (value >> 30)) & 0xFFFFFFFCu;
}

} // namespace

// Address: 0x025D8EDC
void GamesceneBase::setCoinNumText(s32 coin_num)
{
    if (decomp::be32(reinterpret_cast<void*>(0x10214E2Cu)) == 0)
    {
        decomp::be32(reinterpret_cast<void*>(0x10214E2Cu)) = 1;

        void* table = reinterpret_cast<void*>(0x10214E00u);
        decomp::be32(table) = 0x100D3B74u;
        decomp::be32(reinterpret_cast<u8*>(table) + 4) = 0x100D3A70u;

        decomp::call<void>(0x02AFA52Cu, reinterpret_cast<void*>(0x101D7534u), table);
    }

    u8* self = reinterpret_cast<u8*>(this);
    u32 index = decomp::be32(self + 0x08);
    u32 compressed = decomp::be32(self + 0x340);
    u32 array_base = rotl2_align(compressed);
    void* text_box = reinterpret_cast<void*>(static_cast<u32>(
        decomp::be32(reinterpret_cast<u8*>(static_cast<uintptr_t>(array_base)) + index)));

    decomp::call<void>(0x026819F8u, text_box, reinterpret_cast<void*>(0x10214E00u), coin_num, 1);
}