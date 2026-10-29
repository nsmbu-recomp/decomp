#include <course_select/CourseSelectObjActor.h>
#include <effect/Effect.h>
#include <graphics/FrameCtrl.h>
#include <graphics/ModelResourceMgr.h>
#include <graphics/ShaderParamAnimation.h>

#include <math/seadVector.h>
#include <prim/seadSafeString.h>

class CourseSelectCoursePoint : public CourseSelectObjActor
{
public:
    // Address: 0x021C9900
    CourseSelectCoursePoint(const ActorCreateParam& param);
    // Address: 0x021CC17C
    virtual ~CourseSelectCoursePoint();

    // Address: 0x021C9E0C
    void playAnim(s32 anim_id, f32 frame);

protected:
    void*           _10c;           // +0x10C
    void*           _110;           // +0x110
    void*           _114;           // +0x114
    void*           _118;           // +0x118
    void*           _11c;           // +0x11C
    void*           _120;           // +0x120
    ShaderParamAnimation* mpAnim;   // +0x124
    sead::Vector3f  mBasePos;       // +0x128
    s32             mAnimId;        // +0x134
    f32             mAnimFrame;     // +0x138
    void*           _13c;           // +0x13C
    f32             _140;           // +0x140
    f32             _144;           // +0x144
    void*           _148;           // +0x148
    u8*             mpState;        // +0x14C
    Effect          mEffect;        // +0x160
    bool            _1a0;           // +0x1A0
    bool            _1a1;           // +0x1A1
    bool            _1a2;           // +0x1A2
    bool            _1a3;           // +0x1A3
};
static_assert(sizeof(CourseSelectCoursePoint) == 0x1A4);

// Address: 0x101FA800
extern sead::SafeString g_101FA800[9];

static const s32 sPlayModes[6] = { 0, 1, 1, 0, 0, 0 };

namespace {

// Address: 0x021C9CA8
void sub_021C9CA8(CourseSelectCoursePoint* p_point);

// Address: 0x029F1594
void* sub_029F1594(u32 size);

} // namespace

// Address: 0x021C9900
CourseSelectCoursePoint::CourseSelectCoursePoint(const ActorCreateParam& param)
    : CourseSelectObjActor(param)
{
    _10c = nullptr;
    _110 = nullptr;
    _114 = nullptr;
    _118 = nullptr;
    _11c = nullptr;
    _120 = nullptr;
    mpAnim = nullptr;

    mBasePos = mPos;

    mAnimId = 0;
    mAnimFrame = 0.0f;
    _13c = nullptr;
    _140 = 1.0f;
    _144 = 60.0f;
    _148 = nullptr;

    u8* p_state = static_cast<u8*>(sub_029F1594(0x14));
    mpState = p_state;

    if (p_state != nullptr)
    {
        *reinterpret_cast<void**>(p_state + 0x00) = p_state + 0x0C;
        *reinterpret_cast<void**>(p_state + 0x04) = reinterpret_cast<void*>(0x100441EC);
        *reinterpret_cast<u32*>(p_state + 0x08) = 5;
        p_state[0x10] = 0;

        *reinterpret_cast<void**>(p_state + 0x04) = reinterpret_cast<void*>(0x10044284);
        p_state[0x0C] = 0;
        *reinterpret_cast<void**>(p_state + 0x04) = reinterpret_cast<void*>(0x1004429C);
    }

    _1a0 = true;
    _1a1 = false;
    _1a2 = false;
    _1a3 = false;

    _108 = static_cast<s32>(param.param_1);
}

// Address: 0x021C9E0C
void CourseSelectCoursePoint::playAnim(s32 anim_id, f32 frame)
{
    mAnimId = anim_id;
    mAnimFrame = frame;

    u8* p_holder = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(this) + 0x10C);
    void** pp_anim = *reinterpret_cast<void***>(p_holder + 0x1C);
    ShaderParamAnimation* p_anim = static_cast<ShaderParamAnimation*>(*pp_anim);

    ModelResource* p_res = ModelResourceMgr::instance()->getResource(
        sead::SafeString("cobCourse"));

    p_anim->playColorAnim(p_res, g_101FA800[anim_id]);

    FrameCtrl* p_frame_ctrl = reinterpret_cast<FrameCtrl*>(p_anim);
    p_frame_ctrl->setPlayMode(static_cast<FrameCtrl::PlayMode>(sPlayModes[anim_id]));

    if (anim_id != 0)
    {
        p_frame_ctrl->setRate(frame);
    }
    else
    {
        p_frame_ctrl->setFrame(frame);
        p_frame_ctrl->setRate(0.0f);
    }

    mpAnim = p_anim;
    sub_021C9CA8(this);
}

// Address: 0x021CC17C
CourseSelectCoursePoint::~CourseSelectCoursePoint()
{
}
