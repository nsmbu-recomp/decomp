#include <boss/FinalKoopaJr.h>

#include <actor/Actor.h>
#include <actor/ActorBase.h>
#include <actor/ActorCollision.h>
#include <actor/ActorMultiState.h>
#include <enemy/Enemy.h>

// Address: 0x020A4C50
bool FinalKoopaJr::isKindOf(const sead::RuntimeTypeInfo& type) const
{
    if (&type == &getRuntimeTypeInfoStatic())
        return true;

    if (&type == &FinalKoopaJrBase::getRuntimeTypeInfoStatic())
        return true;

    if (&type == &Boss::getRuntimeTypeInfoStatic())
        return true;

    if (&type == &Enemy::getRuntimeTypeInfoStatic())
        return true;

    if (&type == &ActorMultiState::getRuntimeTypeInfoStatic())
        return true;

    if (&type == &ActorCollision::getRuntimeTypeInfoStatic())
        return true;

    if (&type == &Actor::getRuntimeTypeInfoStatic())
        return true;

    return &type == &ActorBase::getRuntimeTypeInfoStatic();
}