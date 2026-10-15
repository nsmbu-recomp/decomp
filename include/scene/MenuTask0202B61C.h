#pragma once

#include <framework/seadCalculateTask.h>
#include <framework/seadTaskMgr.h>
#include <state/StateMethod.h>

class MenuTask0202B61C : public sead::CalculateTask   // vtbl Address: 0x10175F88
{
public:
    // Address: 0x0297C5D8
    MenuTask0202B61C(const sead::TaskConstructArg& arg);
    // Address: 0x0297C4A8
    virtual ~MenuTask0202B61C();

    // Address: 0x0297D268
    void enterFileState();

    // Address: 0x0297D58C
    void enterChallengeSelectState();

    // Address: 0x0297D5A8
    void openChallengeSelect();

    // Address: 0x0297EC3C
    void confirmFile();

private:
    // Address: 0x0297EAAC
    void copySaveToCourseInfo();

private:
    u8          _c8[0x1DC - 0xC8];
    StateMethod mStateMethod;
    u8          _1ec[0xDB1 - 0x1EC];
    u8          mGateSelect;
    u8          mGateStory;
    u8          _db3[0x1CAD - 0xDB3];
    u8          _1cad;
    u8          _1cae[0x3E94 - 0x1CAE];
};
static_assert(sizeof(MenuTask0202B61C) == 0x3E94);

// Address: 0x0202B61C
extern template
sead::TaskBase* sead::TTaskFactory<MenuTask0202B61C>(const sead::TaskConstructArg& arg);
