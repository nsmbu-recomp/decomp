#include <course_select/CourseSelectPlayer.h>

#include <actor/ActorCreateParam.h>
#include <course_select/CourseSelectMgr.h>
#include <course_select/CourseSelectNodeType.h>
#include <system/MainGame.h>

// Address: 0x0226FA30
CourseSelectPlayer::CourseSelectPlayer(const ActorCreateParam& param)
    : CourseSelectObjActor(param)
    , mpAnimModel(nullptr)
    , mModelPos(0.0f, 0.0f, 0.0f)
    , _11c(0)
{
}

// Address: 0x0226FACC
void CourseSelectPlayer::updateModelMtx_()
{
    // Builds a matrix from +0xB0/+0xB4/+0xB8 and +0x50, then pushes it to AnimModel.
}

// Address: 0x0226FB80
void CourseSelectPlayer::playModelAnm_()
{
    // AnimModel::playAnmFrameCtrl; optional AnimModel::calc when +0xD8 bit set.
}

// Address: 0x0226FBC4
ActorBase::Result CourseSelectPlayer::create_()
{
    // Model create / AnimModel::init / updateModelMtx_ / playModelAnm_ / light map
    // omitted — see gen through 0x0226FD10.

    MainGame* main_game = MainGame::instance();

    // site 0x0226FD14 — immediately before pose read (runtime reseeds here)
    CourseSelectMapPose pose;
    pose.world = main_game->getMapWorld();
    pose.section = main_game->getMapSection();
    pose.node_index = main_game->getMapNodeIndex();

    s32 course_slot = getCourseSlotFromPose(&pose);
    CourseSelectNodeType node_type = getCourseSelectNodeType(course_slot);

    _11c = 0;

    // Happy path requires:
    //   1) MainGame+0x2C bit 3 (value 8) — map-pose gate
    //   2) node_type == 2 (course_slot 20 or 21)
    //   3) mMapWorld == pose.world
    //   4) mMapCourseSlot == course_slot (node+0x58)
    // Otherwise L_02270268: queryMapStateB, then often ActorMgr::createLater.
    bool pose_gate = main_game->isMapPoseValid();

    if (!pose_gate
        || node_type != cCourseSelectNodeType_2
        || mMapWorld != pose.world
        || mMapCourseSlot != course_slot)
    {
        // L_02270268
        if (queryMapStateB() == 0)
        {
            // Further branches on MainGame+0x20 / queryMapState*; may createLater
            // with profile from 0x10004B10 using player +0xF0..+0xFC and mModelPos.
        }

        return cResult_Success;
    }

    // Type-2 happy path continues with MainGame+0x20 / queryMapStateA..E and
    // ActorMgr::createImmediately for companion actors (gen 0x0226FDBC+).
    s32 map_state = queryMapStateA();
    (void)map_state;

    return cResult_Success;
}
