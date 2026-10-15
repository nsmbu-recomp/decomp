#pragma once

#include <framework/seadCalculateTask.h>
#include <framework/seadTaskMgr.h>

class ChallengeSelectScene : public sead::CalculateTask   // vtbl Address: 0x10176540
{
public:
    // Address: 0x0297F3CC
    ChallengeSelectScene(const sead::TaskConstructArg& arg);

private:
    u32 _c8[(0x11D0 - 0xC8) / sizeof(u32)];
};
static_assert(sizeof(ChallengeSelectScene) == 0x11D0);

// Address: 0x024C11F4
extern template
sead::TaskBase* sead::TTaskFactory<ChallengeSelectScene>(const sead::TaskConstructArg& arg);
