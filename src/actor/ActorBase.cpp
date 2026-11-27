#include <actor/ActorBase.h>
#include <actor/Profile.h>
#include <event/EventMgr.h>

s32 ActorBase::getProfileID() const
{
    return mpActorProfile->getID();
}

void ActorBase::removeChild(ActorBase* p_child)
{
    mChildList.erase(p_child);
    p_child->mpParent = nullptr;
}

ActorBase::ActorBase(const ActorCreateParam& param)
{
    // Guest vtable lives at +0x4C and the heap pointer at +0. The host body called
    // sead::HeapMgr::getCurrentHeap, which is not linked in this build.
    decomp::call<void>(0x02002CE0u, this, &param);
}

ActorBase::~ActorBase()
{
    while (true)
    {
        ActorBase* p_child = mChildList.front();
        if (p_child == nullptr)
            break;

        removeChild(p_child);
    }

    if (mpParent != nullptr)
        mpParent->removeChild(this);
}

bool ActorBase::preCreate_()
{
    return true;
}

ActorBase::Result ActorBase::create_()
{
    return cResult_Success;
}

void ActorBase::postCreate_(MainState state)
{
    switch (state)
    {
    case cState_None:
        break;
    case cState_Failed:
        break;
    case cState_Success:
        break;
    case cState_Wait:
        break;
    }
}

bool ActorBase::preExecute_()
{
    EventMgr* event_mgr = EventMgr::instance();
    if (event_mgr == nullptr || event_mgr->getCurrentEvent() == nullptr)
        return true;

    return event_mgr->isJoin(this);
}

bool ActorBase::execute_()
{
    return true;
}

void ActorBase::postExecute_(MainState state)
{
    switch (state)
    {
    case cState_None:
        break;
    case cState_Failed:
        break;
    case cState_Success:
        break;
    }
}

void ActorBase::finalUpdate_()
{
}

bool ActorBase::preDraw_()
{
    return true;
}

bool ActorBase::draw_()
{
    return true;
}

void ActorBase::postDraw_(MainState state)
{
    switch (state)
    {
    case cState_None:
        break;
    case cState_Failed:
        break;
    case cState_Success:
        break;
    }
}

bool ActorBase::preDelete_()
{
    return true;
}

ActorBase::Result ActorBase::doDelete_()
{
    return cResult_Success;
}

void ActorBase::postDelete_(MainState state)
{
    switch (state)
    {
    case cState_None:
        break;
    case cState_Failed:
        break;
    case cState_Success:
        break;
    case cState_Wait:
        break;
    }
}
