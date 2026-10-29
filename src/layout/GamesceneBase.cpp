#include <layout/GamesceneBase.h>

#include <actor/ActorUniqueID.h>

namespace {

// Address: 0x10214E2C
s32& gamesceneCoinTextInitFlag()
{
    return *reinterpret_cast<s32*>(0x10214E2Cu);
}

// Address: 0x10214E00
void* gamesceneCoinTextTable()
{
    return reinterpret_cast<void*>(0x10214E00u);
}

// Address: 0x02AFA52C
void sub_02AFA52C(void* ctor_site, void* vtable);

// Address: 0x026819F8
void sub_026819F8(void* text_box, void* format_table, s32 coin_num, s32 use_format);

} // namespace

// Address: 0x025D8EDC
void GamesceneBase::setCoinNumText(s32 coin_num)
{
    if (gamesceneCoinTextInitFlag() == 0)
    {
        gamesceneCoinTextInitFlag() = 1;

        void* table = gamesceneCoinTextTable();
        *reinterpret_cast<u32*>(table) = 0x100D3B74;
        *reinterpret_cast<u32*>(reinterpret_cast<u8*>(table) + 4) = 0x100D3A70;

        sub_02AFA52C(reinterpret_cast<void*>(0x101D7534u), table);
    }

    u8* self = reinterpret_cast<u8*>(this);
    u32 index = *reinterpret_cast<u32*>(self + 0x08);
    void** table = *reinterpret_cast<void***>(self + 0x340);
    void* text_box = table[index];

    sub_026819F8(text_box, gamesceneCoinTextTable(), coin_num, 1);
}

// Address: 0x025DA288
void GamesceneBase::registerActor(ActorUniqueID id)
{
    u8* table = reinterpret_cast<u8*>(this) + 0x480;
    u32 id_value = id.getValue();

    for (s32 slot = 0; slot < 0x40; ++slot)
    {
        u32* entry = reinterpret_cast<u32*>(table + slot * 4);
        if (*entry != 0)
            continue;

        *entry = id_value;
        return;
    }
}
