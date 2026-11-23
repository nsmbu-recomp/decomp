#include <boss/Boss.h>
#include <boss/BossDemo.h>

// Address: 0x0202E5B8
void BossDemo::sub_0202E5B8()
{
    Actor* p_boss = bossSearch();
    if (p_boss != nullptr)
    {
        mBossID = p_boss->getActorUniqueID();
        if (!mBossID.isValid())
            return;
    }
    else if (!mBossID.isValid())
    {
        return;
    }

    if (!checkBattleStDemo())
        return;

    if (p_boss == nullptr)
        return;

    if (p_boss->getBgCheck() == nullptr)
        return;

    if (p_boss == nullptr)
        return;

    static_cast<Boss*>(p_boss)->enterDemo();

    if (!checkBattle())
        return;

    mStateMgr.changeState(StateID_BattleStDemo);
}