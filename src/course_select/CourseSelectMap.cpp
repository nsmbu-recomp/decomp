#include <course_select/CourseSelectMgr.h>
#include <course_select/CourseSelectTask.h>
#include <graphics/Model.h>
#include <graphics/Renderer.h>
#include <system/MainGame.h>
#include <system/ResMgr.h>

#include <heap/seadDisposer.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>

class MapModel
{
public:
    // Address: 0x022961AC
    MapModel(sead::Heap* heap, const sead::SafeString& name);

    // Address: 0x02296B38
    void initialize(u32 world, u32 index);

    // Address: 0x02297A24
    void draw() const;

    Model* getModel() const
    {
        void* const* pp_holder = reinterpret_cast<void* const*>(
            reinterpret_cast<const u8*>(this) + 0x04);

        return static_cast<Model*>(*reinterpret_cast<void**>(*pp_holder));
    }
};

class CourseSelectParallelExecuter
{
public:
    // Address: 0x02206634
    CourseSelectParallelExecuter();
    // Address: 0x022066DC
    virtual ~CourseSelectParallelExecuter();

    // Address: 0x02206788
    void initialize(sead::Heap* heap);

    // Address: 0x02206A80
    void clear();

    // Address: 0x02206B7C
    void executeAll();

protected:
    void*   mItem[0x1000];      // +0x0000
    s32     mItemNum;           // +0x4000

    struct WorkBuffer           // +0x4004
    {
        s32     mCount;
        s32     mCapacity;
        void**  mpBuffer;
        void*   mInline[3];
    };
    static_assert(sizeof(WorkBuffer) == 0x18);

    WorkBuffer  mWorkBuffer;
    u32         _401c;          // +0x401C
    const void* _4020;          // +0x4020
};
static_assert(sizeof(CourseSelectParallelExecuter) == 0x4024);

class CourseSelectMap
{
    // createInstance()                              Address: 0x021F55F8
    // sInstance                                     Address: 0x101CBD34
    // SingletonDisposer_::sStaticDisposer           Address: 0x101CBD38
    // SingletonDisposer_::vtbl                      Address: 0x10047870
    SEAD_SINGLETON_DISPOSER(CourseSelectMap)

public:
    // Address: 0x021F5548
    CourseSelectMap();
    // Address: 0x021F5678
    virtual ~CourseSelectMap();

    // Address: 0x021F56CC
    MapModel* loadModel(u32 world, u32 index);

    // Address: 0x021F5800
    void getBoneWorldMatrix(u32 world, u32 index, const char* p_bone_name, sead::Matrix34f* p_mtx) const;

    // Address: 0x021F59A8
    void getBoneWorldPos(sead::Vector3f* p_pos, u32 world, u32 index, const char* p_bone_name) const;

    // Address: 0x021F6C14
    void initialize();

    // Address: 0x021F6FE8
    void calc();

    // Address: 0x021F7114
    void draw(agl::lyr::Layer* p_layer);

protected:
    struct ModelList            // +0x10, ctor Address: 0x021F76FC
    {
        void*   mHead;          // +0x00
        void*   mTail;          // +0x04
        s32     mCount;         // +0x08
        s32     mOffset;        // +0x0C
    };
    static_assert(sizeof(ModelList) == 0x10);

    ModelList                   mModelList[9];  // +0x10
    bool                        _a0;            // +0xA0
    CourseSelectParallelExecuter mExecuter;     // +0xA4
};
static_assert(sizeof(CourseSelectMap) == 0x40C8);

static const u32 sDrawOrder[9] = { 8, 5, 6, 0, 1, 4, 3, 7, 2 };

// Address: 0x101CD0C0
extern void* g_101CD0C0;

namespace {

// Address: 0x021F5D20
void sub_021F5D20(CourseSelectMap* p_map);

// Address: 0x021F62E0
void sub_021F62E0(CourseSelectMap* p_map);

// Address: 0x021F6AEC
void sub_021F6AEC(CourseSelectMap* p_map, u32 world, u32 value, f32 f, s32 i);

// Address: 0x021F6B3C
void sub_021F6B3C(CourseSelectMap* p_map);

// Address: 0x021FAC24
void sub_021FAC24(CourseSelectMgr* p_mgr, Model* p_model, u32 world, s32 index);

// Address: 0x0228A620
void sub_0228A620(u32 arg);

// Address: 0x0228A948
void sub_0228A948(void* p_arg);

// Address: 0x0229A4A8
void sub_0229A4A8(void* p_model);

// Address: 0x0229B44C
void sub_0229B44C(void* p_model);

// Address: 0x029F1A50
void sub_029F1A50(void* p_list, void* p_node);

// Address: 0x02AFA350
extern void sub_02AFA350(void* self, s32 count, s32 size, u32 ctor);

} // namespace

// Address: 0x021F55F8
SEAD_SINGLETON_DISPOSER_IMPL(CourseSelectMap)

// Address: 0x021F5548
CourseSelectMap::CourseSelectMap()
    : _a0(true)
    , mExecuter()
{
    sub_02AFA350(&mModelList, 9, 0x10, 0x021F76FC);

    for (u32 i = 0; i < 9; i++)
        mModelList[i].mOffset = 0x72C;

    sub_0228A620(0);
}

// Address: 0x021F5678
CourseSelectMap::~CourseSelectMap()
{
}

// Address: 0x021F56CC
MapModel* CourseSelectMap::loadModel(u32 world, u32 index)
{
    sead::FormatFixedSafeString<128> name("CS_W%d", world + 1);
    sead::FormatFixedSafeString<128> path("course_select/%s.szs", name.cstr());

    ResMgr* p_res_mgr = ResMgr::instance();
    bool exists = false;

    if (index == 0)
        exists = p_res_mgr->isArchiveResLoaded(path);

    if (!exists)
    {
        sead::FormatFixedSafeString<128> suffixed_name(
            "CS_W%d%c", world + 1, static_cast<char>('a' + index));
        sead::FormatFixedSafeString<128> suffixed_path(
            "course_select/%s.szs", suffixed_name.cstr());

        if (!p_res_mgr->isArchiveResLoaded(suffixed_path))
            return nullptr;
    }

    MapModel* p_model = new MapModel(nullptr, name);

    if (p_model == nullptr)
        return nullptr;

    p_model->initialize(world, index);
    return p_model;
}

// Address: 0x021F5800
void CourseSelectMap::getBoneWorldMatrix(u32 world, u32 index, const char* p_bone_name, sead::Matrix34f* p_mtx) const
{
    const ModelList* p_list = &mModelList[0];

    if (world < 9)
        p_list = &mModelList[world];

    void* p_item = nullptr;

    if (p_list->mCount > 0 && p_list->mTail != nullptr)
        p_item = reinterpret_cast<u8*>(p_list->mTail) - p_list->mOffset;

    for (s32 i = 0; i < s32(index) && p_item != nullptr; i++)
    {
        u8* p_node = reinterpret_cast<u8*>(p_item) + p_list->mOffset;
        void* p_prev = *reinterpret_cast<void**>(p_node + 0x04);

        if (p_prev == p_list)
            p_item = nullptr;
        else
            p_item = reinterpret_cast<u8*>(p_prev) - p_list->mOffset;
    }

    Model* p_model = nullptr;

    if (p_item != nullptr)
        p_model = static_cast<MapModel*>(p_item)->getModel();

    if (p_model == nullptr)
    {
        p_mtx->makeIdentity();
        return;
    }

    s32 bone_index = p_model->searchBoneIndex(sead::SafeString(p_bone_name));

    if (bone_index < 0)
        p_mtx->makeIdentity();
    else
        p_model->getBoneWorldMatrix(bone_index, p_mtx);
}

// Address: 0x021F59A8
void CourseSelectMap::getBoneWorldPos(sead::Vector3f* p_pos, u32 world, u32 index, const char* p_bone_name) const
{
    sead::Matrix34f mtx;

    getBoneWorldMatrix(world, index, p_bone_name, &mtx);

    *p_pos = sead::Vector3f(mtx(0, 3), mtx(1, 3), mtx(2, 3));
}

// Address: 0x021F6C14
void CourseSelectMap::initialize()
{
    mExecuter.initialize(nullptr);

    for (u32 world = 0; world < 9; world++)
    {
        ModelList* p_list = &mModelList[world];
        u32 index = 0;

        MapModel* p_model = loadModel(world, index);

        while (p_model != nullptr)
        {
            sub_029F1A50(p_list, reinterpret_cast<u8*>(p_model) + p_list->mOffset);
            p_list->mCount++;

            index++;
            p_model = loadModel(world, index);
        }
    }

    sub_021F5D20(this);

    for (u32 world = 0; world < 9; world++)
    {
        ModelList* p_list = &mModelList[world];
        s32 index = 0;

        void* p_item = nullptr;

        if (p_list->mCount > 0 && p_list->mTail != nullptr)
            p_item = reinterpret_cast<u8*>(p_list->mTail) - p_list->mOffset;

        while (p_item != nullptr)
        {
            sub_021FAC24(CourseSelectMgr::instance(),
                static_cast<MapModel*>(p_item)->getModel(), world, index);

            u8* p_node = reinterpret_cast<u8*>(p_item) + p_list->mOffset;
            void* p_prev = *reinterpret_cast<void**>(p_node + 0x04);

            p_item = (p_prev == p_list) ? nullptr : reinterpret_cast<u8*>(p_prev) - p_list->mOffset;
            index++;
        }
    }

    sub_021F62E0(this);

    MainGame* p_main_game = MainGame::instance();
    const u32* p_id_table = reinterpret_cast<const u32*>(
        reinterpret_cast<const u8*>(p_main_game) + 0x2F8);

    for (u32 world = 0; world < 9; world++)
        sub_021F6AEC(this, world, p_id_table[world], 1.0f, 1);

    sub_021F6B3C(this);

    ModelList* p_list_1 = &mModelList[1];

    if (p_list_1->mCount > 0 && p_list_1->mTail != nullptr)
    {
        void* p_item = reinterpret_cast<u8*>(p_list_1->mTail) - p_list_1->mOffset;

        if (p_item != nullptr)
            sub_0229B44C(p_item);
    }

    for (u32 world = 0; world < 9; world++)
    {
        ModelList* p_list = &mModelList[world];

        void* p_item = nullptr;

        if (p_list->mCount > 0 && p_list->mTail != nullptr)
            p_item = reinterpret_cast<u8*>(p_list->mTail) - p_list->mOffset;

        while (p_item != nullptr)
        {
            sub_0229A4A8(p_item);

            u8* p_node = reinterpret_cast<u8*>(p_item) + p_list->mOffset;
            void* p_prev = *reinterpret_cast<void**>(p_node + 0x04);

            p_item = (p_prev == p_list) ? nullptr : reinterpret_cast<u8*>(p_prev) - p_list->mOffset;
        }
    }
}

// Address: 0x021F6FE8
void CourseSelectMap::calc()
{
    if (*reinterpret_cast<const u8*>(
            reinterpret_cast<const u8*>(CourseSelectTask::instance()) + 0xDC9) != 0)
        return;

    mExecuter.clear();

    for (u32 world = 0; world < 9; world++)
    {
        ModelList* p_list = &mModelList[world];

        if (p_list->mCount <= 0 || p_list->mTail == nullptr)
            continue;

        void* p_item = reinterpret_cast<u8*>(p_list->mTail) - p_list->mOffset;

        while (p_item != nullptr)
        {
            if (mExecuter.mItemNum < 0x1000)
            {
                mExecuter.mItem[mExecuter.mItemNum] = p_item;
                mExecuter.mItemNum++;
            }

            u8* p_node = reinterpret_cast<u8*>(p_item) + p_list->mOffset;
            void* p_prev = *reinterpret_cast<void**>(p_node + 0x04);

            p_item = (p_prev == p_list) ? nullptr : reinterpret_cast<u8*>(p_prev) - p_list->mOffset;
        }
    }

    mExecuter.executeAll();
    sub_0228A948(g_101CD0C0);
}

// Address: 0x021F7114
void CourseSelectMap::draw(agl::lyr::Layer* p_layer)
{
    Renderer::instance()->setLayer(p_layer, 1);

    for (s32 i = 0; i < 9; i++)
    {
        u32 world = sDrawOrder[i];
        ModelList* p_list = &mModelList[0];

        if (world < 9)
            p_list = &mModelList[world];

        if (p_list->mCount <= 0 || p_list->mTail == nullptr)
            continue;

        void* p_item = reinterpret_cast<u8*>(p_list->mTail) - p_list->mOffset;

        while (p_item != nullptr)
        {
            static_cast<MapModel*>(p_item)->draw();

            u8* p_node = reinterpret_cast<u8*>(p_item) + p_list->mOffset;
            void* p_prev = *reinterpret_cast<void**>(p_node + 0x04);

            p_item = (p_prev == p_list) ? nullptr : reinterpret_cast<u8*>(p_prev) - p_list->mOffset;
        }
    }

    Renderer::instance()->resetLayer();
}
