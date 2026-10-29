#include <map/CourseData.h>

namespace {

// Address: 0x02AFA350
extern void sub_02AFA350(void* self, s32 count, s32 size, u32 ctor);

} // namespace

const DistantViewData* CourseDataFile::getBg2Data(u16 id) const
{
    const DistantViewData* dv_data = static_cast<const DistantViewData*>(mBlock[cBlock_DistantViewData]);
    s32 dv_data_num = mBlockEntryNum[cBlock_DistantViewData];

    for (s32 i = 0; i < dv_data_num; i++)
        if (dv_data[i].id == id)
            return &(dv_data[i]);

    return nullptr;
}

const AreaData* CourseDataFile::getAreaData(u8 id, sead::BoundBox2f* p_box) const
{
    const AreaData* area_data = static_cast<const AreaData*>(mBlock[cBlock_AreaData]);

    for (u32 i = 0; i < mBlockEntryNum[cBlock_AreaData]; i++)
    {
        if (area_data[i].id == id)
        {
            if (p_box != nullptr)
                getAreaBox_(p_box, area_data[i]);

            return &(area_data[i]);
        }
    }

    return nullptr;
}

void CourseDataFile::getAreaBox_(sead::BoundBox2f* p_box, const AreaData& area_data, f32 delta) const
{
    s32 min_y = area_data.offset.y + area_data.size.y;

    sead::Vector2f min(
         f32(area_data.offset.x) - delta,
        -f32(             min_y) - delta
    );

    sead::Vector2f max(
        min.x + area_data.size.x + delta + delta,
        min.y + area_data.size.y + delta + delta
    );

    p_box->set(min, max);
}

// Address: 0x02699BD8
const NextGoto* CourseDataFile::getNextGoto(u8 id) const
{
    const NextGoto* next_goto = static_cast<const NextGoto*>(mBlock[cBlock_NextGoto]);
    s32 next_goto_num = mBlockEntryNum[cBlock_NextGoto];

    for (s32 i = 0; i < next_goto_num; i++)
        if (next_goto[i].id == id)
            return &(next_goto[i]);

    return nullptr;
}

// Address: 0x02699C08
const MapActorData* CourseDataFile::getMapActor(u16 type, const MapActorData* p_start) const
{
    u32 map_actor_num = mBlockEntryNum[cBlock_MapActor];
    if (map_actor_num == 0)
        return nullptr;

    const MapActorData* map_actor = static_cast<const MapActorData*>(mBlock[cBlock_MapActor]);
    const MapActorData* map_actor_end = map_actor + map_actor_num;

    if (p_start != nullptr)
        map_actor = p_start + 1;

    while (map_actor < map_actor_end)
    {
        if (map_actor->type == type)
            return map_actor;

        map_actor++;
    }

    return nullptr;
}

// Address: 0x0269A020
const Location* CourseDataFile::getLocation(sead::BoundBox2f* p_box, u8 id) const
{
    const Location* location = static_cast<const Location*>(mBlock[cBlock_Location]);

    for (u32 i = 0; i < mBlockEntryNum[cBlock_Location]; i++)
    {
        if (location[i].id == id)
        {
            if (p_box != nullptr)
                getAreaBox_(p_box, *reinterpret_cast<const AreaData*>(&(location[i])), 0.0f);

            return &(location[i]);
        }
    }

    return nullptr;
}

// Address: 0x0269A0A8
const RailInfo* CourseDataFile::getRailInfo(u8 id) const
{
    const RailInfo* rail_info = static_cast<const RailInfo*>(mBlock[cBlock_RailInfo]);
    if (rail_info == nullptr)
        return nullptr;

    for (u32 i = 0; i < mBlockEntryNum[cBlock_RailInfo]; i++)
        if (rail_info[i].id == id)
            return &(rail_info[i]);

    return nullptr;
}

// Address: 0x0269A0F4
const RailPoint* CourseDataFile::getRailPoint(u8 rail_id) const
{
    const RailInfo* rail_info = getRailInfo(rail_id);
    if (rail_info == nullptr)
        return nullptr;

    const RailPoint* rail_point = static_cast<const RailPoint*>(mBlock[cBlock_RailPoint]);
    return &(rail_point[rail_info->point.start]);
}

CourseData* CourseData::sInstance = nullptr;

// Address: 0x0269A144
CourseData::CourseData()
{
    sub_02AFA350(&mFile, 4, 0x3B8, 0x026A94FC);
}

// Address: 0x0269A2DC
CourseDataFile* CourseData::getFile(s32 index)
{
    if ((u32)index >= 4)
        index = 0;

    CourseDataFile* p_cd_file = &(mFile[index]);
    if (!p_cd_file->getBlock(CourseDataFile::cBlock_AreaData))
        return nullptr;

    return p_cd_file;
}
