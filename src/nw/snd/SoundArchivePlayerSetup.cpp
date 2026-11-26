#include <cstdio>

struct SoundArchivePlayerSetupParams {
    void* archive_slice;
    void* field_04;
    void* buffer_primary;
    void* archive_holder;
    void* mem_c8;
    void* buffer_primary_copy;
    void* mem_d0;
    u32 combined_offset;
    void* mem_dc;
    void* mem_e0;
    void* heap_arg;
};

constexpr u32 kSoundSetupEnableByte = 0x101E8FD0u;

// Address: 0x02ADF878
s32 sub_02ADF878(
    void* sound_startable,
    void* archive_slice,
    void* field_08,
    void* field_0c,
    void* archive_holder,
    void* field_14,
    void* field_18,
    void* field_1c,
    void* field_20)
{
    return *reinterpret_cast<u8*>(kSoundSetupEnableByte);
}

// Address: 0x02ADAE50
s32 sub_02ADAE50(void* sound_startable, void* archive_slice, u32* stack_size_a, u32 total_size);

// Address: 0x02AD6CF0
s32 sub_02AD6CF0(void* archive_slice, void* out_pair);

// Address: 0x02ADAF9C
s32 sub_02ADAF9C(void* sound_startable, void* archive_slice, u32* stack_size_a, u32 total_size);

// Address: 0x02ADB06C
s32 sub_02ADB06C(void* sound_startable, void* mem_at_20, u32* stack_size_a, u32 total_size);

// Address: 0x02ADB13C
s32 sub_02ADB13C(void* sound_startable, void* mem_at_14, u32* stack_size_a, u32 total_size);

// Address: 0x02ADB20C
s32 sub_02ADB20C(void* sound_startable, void* mem_at_10, u32* stack_size_a, u32 total_size);

// Address: 0x02ADB290
s32 sub_02ADB290(
    void* sound_startable,
    void* out_pair,
    u32 total_size,
    void* holder);

// Address: 0x02ADAD00
u32 sub_02ADAD00(void* sound_startable, void* archive_slice);

// Address: 0x02AE03FC
s32 sub_02AE03FC(void* player_ext, u32 size_a, u32 size_b, u32 aux, void* tail);

// Address: 0x02ADB470
s32 sub_02ADB470(
    void* sound_startable,
    void* archive_slice,
    u32 size_a,
    u32 size_b,
    void* tail_holder)
{
    if ((size_a & 7u) != 0)
        return 0;

    u32 required = sub_02ADAD00(sound_startable, archive_slice);
    if (size_b < required)
        return 0;

    u8 block[0x28] = {};
    if (sub_02AD6CF0(archive_slice, block + 8) == 0)
        return 0;

    u32 aux = reinterpret_cast<u32*>(block + 8)[4];
    u8* player = reinterpret_cast<u8*>(sound_startable);
    if (sub_02AE03FC(player + 0x90, size_a, size_b, aux, tail_holder) == 0)
        return 0;

    return 1;
}

// Address: 0x02ADB338
s32 sub_02ADB338(
    void* sound_startable,
    void* archive_slice,
    u32 size_a,
    u32 size_b,
    void* holder)
{
    u8 frame[0x40];
    u32* sp8 = reinterpret_cast<u32*>(frame + 8);
    void** sp_c = reinterpret_cast<void**>(frame + 0xC);
    void** sp_10 = reinterpret_cast<void**>(frame + 0x10);
    void** sp_14 = reinterpret_cast<void**>(frame + 0x14);
    void** sp_20 = reinterpret_cast<void**>(frame + 0x20);

    std::fprintf(stderr, "carve cursor=%08x size=%08x holder=%p\n", size_a, size_b, holder);
    *sp8 = size_a;
    if ((size_a & 0x1Fu) != 0)
        return 0;

    u32 total = size_a + size_b;
    if (sub_02ADAE50(sound_startable, archive_slice, sp8, total) == 0)
    {
        std::fprintf(stderr, "carve failed\n");
        return 0;
    }
    std::fprintf(stderr, "carve table=%08x\n", static_cast<u32>(decomp::be32(reinterpret_cast<u8*>(sound_startable) + 0x1C)));

    if (sub_02AD6CF0(archive_slice, sp_c) == 0)
        return 0;

    if (sub_02ADAF9C(sound_startable, archive_slice, sp8, total) == 0)
        return 0;

    if (sub_02ADB06C(sound_startable, *sp_20, sp8, total) == 0)
        return 0;

    if (sub_02ADB13C(sound_startable, *sp_14, sp8, total) == 0)
        return 0;

    if (sub_02ADB20C(sound_startable, *sp_10, sp8, total) == 0)
        return 0;

    if (holder != nullptr)
    {
        u32 out_pair[2] = {};
        if (sub_02ADB290(sound_startable, out_pair, total, holder) == 0)
            return 0;
    }

    u8* player = reinterpret_cast<u8*>(sound_startable);
    decomp::be32(player + 0xAC) = size_a;
    decomp::beptr<void*>(player + 4) = archive_slice;
    decomp::be32(player + 0xB0) = size_b;
    return 1;
}

// Address: 0x02ADB530
s32 sub_02ADB530(
    void* sound_startable,
    void* archive_slice,
    void* archive_holder,
    void* buffer,
    u32 buffer_size,
    void* heap_arg,
    void* mem_d0,
    u32 combined)
{
    std::fprintf(stderr, "setup flag=%u\n", *reinterpret_cast<u8*>(kSoundSetupEnableByte));
    if (sub_02ADF878(
            sound_startable,
            archive_slice,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr) == 0)
        return 0;

    if (sub_02ADB338(
            sound_startable,
            archive_slice,
            static_cast<u32>(reinterpret_cast<uintptr_t>(buffer)),
            buffer_size,
            heap_arg) == 0)
        return 0;

    if (sub_02ADB470(
            sound_startable,
            archive_slice,
            static_cast<u32>(reinterpret_cast<uintptr_t>(mem_d0)),
            combined,
            nullptr) == 0)
        return 0;

    u8* player = reinterpret_cast<u8*>(sound_startable);
    decomp::beptr<void*>(player + 0xB8) = archive_holder;
    decomp::beptr<void*>(player + 0x80) = player + 0x84;
    return 1;
}

// Address: 0x02ADB634
s32 sub_02ADB634(void* sound_startable, SoundArchivePlayerSetupParams* params)
{
    s32 ok = sub_02ADB530(
        sound_startable,
        params->archive_slice,
        params->archive_holder,
        params->mem_c8,
        static_cast<u32>(reinterpret_cast<uintptr_t>(params->buffer_primary)),
        params->heap_arg,
        params->mem_d0,
        params->combined_offset);
    if (params->mem_d0 == nullptr)
        return ok;

    u32 pair[4] = {};
    if (sub_02AD6CF0(params->archive_slice, pair + 2) == 0)
        return 0;

    u32 divisor = pair[2];
    if (divisor == 0)
        return ok;

    u8* startable = reinterpret_cast<u8*>(sound_startable);
    void* cursor = *reinterpret_cast<void**>(startable + 0x78);
    void* limit = startable + 0x78;
    if (cursor >= limit)
        return ok;

    u32 quotient = params->combined_offset / divisor;
    *reinterpret_cast<void**>(reinterpret_cast<u8*>(cursor) + 0x6A44) = params->mem_d0;
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(cursor) + 0x6A48) = quotient;
    return ok;
}
