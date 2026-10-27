#include <sound/SndAudioMgr.h>
#include <sound/SndSpeakerMgr.h>

#include <nw/snd/snd_SoundArchive.h>

namespace {

// MainGame::sInstance                          Address: 0x101E7D7C
constexpr uintptr_t kMainGameInstancePtr = 0x101E7D7Cu;

using StartSoundImplFn = bool (*)(SndAudioMgr*, nw::snd::SoundHandle*, const char*);

bool invokeStartSoundImpl(SndAudioMgr* self, nw::snd::SoundHandle* p_handle, const char* label)
{
    uintptr_t sound_startable = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8_t*>(self) + 0x3d0);
    StartSoundImplFn start_sound_impl = reinterpret_cast<StartSoundImplFn>(*reinterpret_cast<uintptr_t*>(sound_startable + 0x24));
    return start_sound_impl(self, p_handle, label);
}

// Address: 0x02AD6988
s32 convertLabelStringToSoundId(nw::snd::SoundArchive* archive, const char* label);

// Address: 0x02AD6998
u32 getSoundParam(nw::snd::SoundArchive* archive, s32 sound_id);

// Address: 0x029B5324
bool startSystemSeOnSoundHandle_(SndAudioMgr* self, const char* label, nw::snd::OutputLine line_flag);

} // namespace

// Address: 0x029B5434
bool SndAudioMgr::startSound(nw::snd::SoundHandle* p_handle, const char* label, nw::snd::OutputLine line_flag)
{
    bool started = invokeStartSoundImpl(this, p_handle, label);
    if (!started)
        return started;

    nw::snd::SoundArchive* archive = getSoundArchive();
    s32 sound_id = convertLabelStringToSoundId(archive, label);

    archive = getSoundArchive();
    u32 param = getSoundParam(archive, sound_id);

    SndSpeakerMgr::instance()->setRemoteSend(p_handle, line_flag, param);
    return started;
}

// Address: 0x029B54C0
bool SndAudioMgr::startSystemSe(const char* label, nw::snd::OutputLine line_flag)
{
    if (line_flag == 0x20 || line_flag == 0x21)
        return startSystemSeOnSoundHandle_(this, label, line_flag);

    u8* main_game = *reinterpret_cast<u8**>(kMainGameInstancePtr);
    if (main_game[0x14] != 0)
        return startSystemSeOnSoundHandle_(this, label, static_cast<nw::snd::OutputLine>(0x21));

    nw::snd::SoundHandle* p_handle = reinterpret_cast<nw::snd::SoundHandle*>(reinterpret_cast<uint8_t*>(this) + 0x2c);
    return startSound(p_handle, label, line_flag);
}
