#include <system/MainGame.h>

#include <graphics/Mii_IconFacelineObj.h>
#include <graphics/Mii_IconRenderBuffer.h>

#include <cstring>
#include <new>

namespace {

// Address: 0x02B07418
extern void OSBlockSet(void* p_dst, u8 value, u32 size);

// Address: 0x02A0A2FC
extern void sub_02A0A2FC(void* self);

// Address: 0x02A13780
extern u32 sub_02A13780(void* self);

// Address: 0x02A1E9DC
extern void sub_02A1E9DC(void* self);

// Address: 0x02A1EFDC
extern void sub_02A1EFDC(void* self);

// Address: 0x02A1F16C
extern void sub_02A1F16C(void* self);

// Address: 0x02A249A0
extern void sub_02A249A0(void* self);

// Address: 0x02AFA350
extern void sub_02AFA350(void* self, s32 count, s32 size, u32 ctor);

// Address: 0x02AFA52C
extern void sub_02AFA52C(void* self);

// Address: 0x024BAE94
extern void sub_024BAE94(void* self);

// Address: 0x024BBBD4
extern void sub_024BBBD4(void* self);

// Address: 0x021D63F8
extern void sub_021D63F8(void* self);

// Address: 0x021D6540
extern void sub_021D6540(void* self);

// Address: 0x024CD4A4
extern void sub_024CD4A4(void* self);

// Address: 0x029CD1E8
extern void sub_029CD1E8(MainGame* self);

// Address: 0x029CD22C
extern void sub_029CD22C(MainGame* self);

// Address: 0x029CD290
extern void sub_029CD290(MainGame* self);

// Address: 0x029F16CC
extern void* sub_029F16CC(u32 size, u32 align);

// Address: 0x02510378
extern u32 sub_02510378();

template<typename T>
T& fieldAt(u8* base, u32 offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

void* singletonAt(u32 address)
{
    return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(address));
}

void constructString(u8* p_object)
{
    fieldAt<char*>(p_object, 0x00) = reinterpret_cast<char*>(p_object + 0x0C);
    fieldAt<u32>(p_object, 0x04) = 0x10189F78;
    fieldAt<u32>(p_object, 0x08) = 0x104;
    fieldAt<u8>(p_object, 0x10F) = 0;

    fieldAt<u32>(p_object, 0x04) = 0x1018A070;
    fieldAt<char*>(p_object, 0x00)[0] = 0;

    fieldAt<u32>(p_object, 0x04) = 0x1018A088;
}

const char* getNameString()
{
    return *reinterpret_cast<const char**>(reinterpret_cast<uintptr_t>(0x1022D3D4));
}

void initNameString()
{
    static bool s_isInitialized = false;

    if (s_isInitialized)
        return;

    s_isInitialized = true;

    *reinterpret_cast<u32*>(reinterpret_cast<uintptr_t>(0x1022D3D4)) = 0x1018A0B4;
    *reinterpret_cast<u32*>(reinterpret_cast<uintptr_t>(0x1022D3D8)) = 0x10189F60;
    sub_02AFA52C(reinterpret_cast<void*>(0x101E7D70));
}

void copyNameString(char* p_dst)
{
    const char* p_src = getNameString();
    u32 length = static_cast<u32>(std::strlen(p_src));

    std::memcpy(p_dst, p_src, length);
    p_dst[length] = 0;
}

void clearPlayerEntries(u8* p_array)
{
    for (s32 i = 0; i < 9; ++i)
    {
        for (s32 j = 0; j < 2; ++j)
        {
            u8* p_block = p_array + i * 0x5D0 + j * 0x2E8;

            for (s32 k = 0; k < 0x1F; ++k)
            {
                u8* p_entry = p_block + k * 0x18;

                fieldAt<f32>(p_entry, 0x00) = 0.0f;
                fieldAt<f32>(p_entry, 0x04) = 0.0f;
                fieldAt<f32>(p_entry, 0x08) = 0.0f;
                fieldAt<f32>(p_entry, 0x0C) = 0.0f;
                fieldAt<f32>(p_entry, 0x10) = 0.0f;
                fieldAt<f32>(p_entry, 0x14) = 0.0f;
            }
        }
    }
}

void clearMiiEntries(u8* p_array)
{
    for (s32 i = 0; i < 9; ++i)
    {
        for (s32 j = 0; j < 2; ++j)
        {
            u8* p_block = p_array + i * 0x2400 + j * 0x1200;

            for (s32 k = 0; k < 0x80; ++k)
            {
                u8* p_entry = p_block + k * 0x24;

                copyNameString(*reinterpret_cast<char**>(p_entry));
                fieldAt<f32>(p_entry, 0x14) = 0.0f;
                fieldAt<f32>(p_entry, 0x18) = 0.0f;
                fieldAt<f32>(p_entry, 0x1C) = 0.0f;
                fieldAt<u32>(p_entry, 0x20) = 0;
            }
        }
    }
}

} // namespace

SEAD_SINGLETON_DISPOSER_IMPL(MainGame)

// Address: 0x029CD2E8
MainGame::MainGame()
{
    u8* self = reinterpret_cast<u8*>(this);

    _10 = 0;
    mIsSinglePlayerDRC = false;
    _18 = 0;
    mIsNewGame = false;
    mCourseExitType = cCourseExitType_Exit;
    mCourseBeatenType = cCourseBeatenType_Normal;
    _28 = 0;
    mReplayFlag = 0;
    fieldAt<u32>(self, 0x30) = 0;

    sub_024CD4A4(self + 0x34);

    OSBlockSet(self + 0x64, 0, 0x10);

    fieldAt<u32>(self, 0x74) = 0;
    fieldAt<u8>(self, 0x78) = 0;
    fieldAt<u32>(self, 0x7C) = 0;
    fieldAt<u32>(self, 0x80) = 0;
    fieldAt<u32>(self, 0x84) = 0;

    constructString(self + 0x88);

    fieldAt<u32>(self, 0x198) = 0;
    for (u32 offset = 0x19C; offset <= 0x1BC; offset += 4)
        fieldAt<u32>(self, offset) = 0xFFFFFFFF;

    fieldAt<u32>(self, 0x1C0) = 0;
    fieldAt<u32>(self, 0x1C4) = 0;

    for (u32 offset = 0x1C8; offset <= 0x1E8; offset += 4)
        fieldAt<u32>(self, offset) = 0xFFFFFFFF;

    fieldAt<u32>(self, 0x1EC) = 6;
    fieldAt<u32>(self, 0x1F0) = 0;
    fieldAt<u32>(self, 0x1F4) = 0;

    fieldAt<u8>(self, 0x1F8) = static_cast<u8>(
        static_cast<u64>(sub_02A13780(singletonAt(0x101E806C))) * 12 >> 32);

    for (u32 offset = 0x1F9; offset < 0x201; ++offset)
        fieldAt<u8>(self, offset) = 0;

    fieldAt<u8>(self, 0x201) = 0;
    fieldAt<u32>(self, 0x204) = 0;
    fieldAt<u16>(self, 0x208) = 0;
    fieldAt<u8>(self, 0x20B) = 0;
    fieldAt<u8>(self, 0x20C) = 0;
    fieldAt<u8>(self, 0x20D) = 0;
    fieldAt<u8>(self, 0x214) = 0;
    fieldAt<u32>(self, 0x218) = 0;

    sub_02AFA350(self + 0x21C, 0x12, 0x0C, 0x029DEDB4);

    fieldAt<u32>(self, 0x2F4) = 0;

    sub_024BAE94(self + 0x31C);
    sub_021D63F8(self + 0x3D8);

    fieldAt<u32>(self, 0x824) = 0;
    fieldAt<u32>(self, 0x828) = 0;
    fieldAt<u8>(self, 0x82C) = 0;
    fieldAt<u8>(self, 0x82D) = 0;
    mIsNabbitEntry = false;
    mIsNabbitEntryMii = false;
    fieldAt<u8>(self, 0x830) = 0;
    fieldAt<u8>(self, 0x831) = 0;

    for (u32 offset = 0x834; offset <= 0x858; offset += 0x0C)
    {
        fieldAt<f32>(self, offset + 0x00) = 0.0f;
        fieldAt<f32>(self, offset + 0x04) = 0.0f;
        fieldAt<f32>(self, offset + 0x08) = 0.0f;
    }

    sub_02AFA350(self + 0x864, 9, 0x5D0, 0x029DEE24);
    sub_02AFA350(self + 0x3CB4, 9, 0x2400, 0x029DEF20);

    fieldAt<u8>(self, 0x180E0) = 0;
    fieldAt<u8>(self, 0x180E1) = 0;
    fieldAt<u8>(self, 0x180E2) = 0;

    sub_02A0A2FC(self + 0x180E4);
    fieldAt<u32>(self, 0x180F0) = 0x1018A0A0;
    sub_02A1EFDC(self + 0x180F4);
    sub_02A249A0(self + 0x18248);
    sub_02A1F16C(self + 0x182E8);
    sub_02A249A0(self + 0x18448);
    sub_02A1E9DC(self + 0x184E8);

    fieldAt<u32>(self, 0x18544) = 0;

    new (static_cast<void*>(self + 0x18548)) Mii::IconFacelineObj();

    sub_02AFA350(self + 0x1891C, 4, 0x460, 0x029DEF78);
    sub_02AFA350(self + 0x19A9C, 0x1E, 0x460, 0x029DEF78);

    mIsShadowkunQuest = false;
    fieldAt<u8>(self, 0x21DFC) = 0;
    fieldAt<u8>(self, 0x21DFD) = 0;
    fieldAt<u8>(self, 0x21DFE) = 0xFF;
    fieldAt<u8>(self, 0x21DFF) = 0xFF;
    fieldAt<u8>(self, 0x21E00) = 0;
    fieldAt<u8>(self, 0x21E01) = 0;
    fieldAt<u8>(self, 0x21E02) = 0;
    fieldAt<u16>(self, 0x21E0A) = 0;
    fieldAt<u8>(self, 0x21E0C) = 0;
    fieldAt<u8>(self, 0x21E0D) = 0;
    fieldAt<u8>(self, 0x21E0E) = 0;
    fieldAt<u8>(self, 0x21E0F) = 0;
    fieldAt<u16>(self, 0x21E10) = 0;
    fieldAt<u8>(self, 0x21E12) = 0;
    fieldAt<u8>(self, 0x21E13) = 0;
    fieldAt<u8>(self, 0x21E14) = 0;

    for (s32 i = 0; i < 0x1E; ++i)
        _21ddc[i] = 0;

    fieldAt<char*>(self, 0x88)[0] = 0;

    sub_024BBBD4(self + 0x31C);
    sub_021D6540(self + 0x3D8);
    sub_029CD1E8(this);

    for (u32 offset = 0x180B8; offset <= 0x180DC; offset += 4)
        fieldAt<u32>(self, offset) = 0x0E;

    sub_029CD22C(this);

    OSBlockSet(self + 0x64, 0, 0x10);

    fieldAt<u8>(self, 0x82C) = 0;
    fieldAt<u8>(self, 0x82D) = 0;

    for (u32 offset = 0x21E03; offset <= 0x21E09; ++offset)
        fieldAt<u8>(self, offset) = 0;

    sub_029CD290(this);

    initNameString();

    clearPlayerEntries(self + 0x864);
    clearMiiEntries(self + 0x3CB4);

    fieldAt<u32>(self, 0x180B4) = reinterpret_cast<u32>(sub_029F16CC(0x1100, 0x40));
    fieldAt<u32>(self, 0x80) = reinterpret_cast<u32>(sub_029F16CC(sub_02510378(), 0x40));
}

// Address: 0x029CEAD4
void MainGame::initMiiIconRenderBuffer(sead::Heap* heap)
{
    const CaptureRenderBuffer::TextureInfo info =
    {
        0x80, 0x80, static_cast<agl::TextureFormat>(0x1D)
    };

    Mii::IconRenderBuffer* p_buffers =
        reinterpret_cast<Mii::IconRenderBuffer*>(reinterpret_cast<u8*>(this) + 0x1891C);

    for (s32 i = 0; i < 4; ++i)
        p_buffers[i].initialize(heap, info);

    Mii::IconRenderBuffer* p_icon_buffer =
        reinterpret_cast<Mii::IconRenderBuffer*>(reinterpret_cast<u8*>(this) + 0x180E4);
    p_icon_buffer->initialize(heap, info);

    p_buffers = reinterpret_cast<Mii::IconRenderBuffer*>(reinterpret_cast<u8*>(this) + 0x19A9C);

    for (s32 i = 0; i < 0x1E; ++i)
        p_buffers[i].initialize(heap, info);
}
