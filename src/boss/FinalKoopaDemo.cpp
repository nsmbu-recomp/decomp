#include <boss/FinalKoopaDemo.h>

#include <actor/Actor.h>
#include <actor/ActorMgr.h>
#include <actor/ProfileInfo.h>

// Address: 0x02093F20
Actor* FinalKoopaDemo::bossSearch()
{
    const ActorMgr::iterator itr =
        ActorMgr::instance()->find(ProfileInfo::cProfileID_FinalKoopaJr, ActorMgr::instance()->getActorBegin());
    Actor* p_actor = static_cast<Actor*>(const_cast<ActorBase*>(*itr));

    if (p_actor == nullptr)
        return nullptr;

    if (!p_actor->isKindOf(Actor::getRuntimeTypeInfoStatic()))
        return nullptr;

    return p_actor;
}