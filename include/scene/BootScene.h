#pragma once

#include <framework/seadCalculateTask.h>
#include <framework/seadTaskMgr.h>

class BootScene : public sead::CalculateTask   // vtbl Address: 0x101759D8
{
public:
    // Address: 0x0297AC48
    BootScene(const sead::TaskConstructArg& arg);

private:
    u32 _c8[(0x103C - 0xC8) / sizeof(u32)];
};
static_assert(sizeof(BootScene) == 0x103C);

// Address: 0x0202930C
extern template
sead::TaskBase* sead::TTaskFactory<BootScene>(const sead::TaskConstructArg& arg);
