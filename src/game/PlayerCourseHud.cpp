#include <game/PlayerCourseHud.h>

#include <game/AreaTask.h>
#include <layout_sys/AnimLayout.h>
#include <state/StateID.h>
#include <state/StateMethod.h>
#include <system/MainGame.h>

#include <math/seadVector.h>

namespace {

constexpr uintptr_t kGuestCourseHudScratch = 0x101D7284u;
constexpr uintptr_t kGuestCourseHudStateId = 0x1021B3FCu;
constexpr uintptr_t kGuestDefaultStockItemPos = 0x105F9E54u;
constexpr uintptr_t kGuestStockItemPosAdjustY = 0x100C4F74u;
constexpr uintptr_t kGuestStockItemPosAdjustX = 0x100C4F70u;

const StateID& stateIdAt(uintptr_t address)
{
    return *reinterpret_cast<const StateID*>(address);
}

const f32& rodataF32(uintptr_t address)
{
    return *reinterpret_cast<const f32*>(address);
}

const sead::Vector2f& rodataVec2f(uintptr_t address)
{
    return *reinterpret_cast<const sead::Vector2f*>(address);
}

template<typename T>
T& fieldAt(u8* base, u32 offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

// Address: 0x0252F598
extern void courseHudPrepare_(PlayerCourseHud* self);

// Address: 0x0252F884
extern void courseHudSyncItemListAnims_(PlayerCourseHud* self);

// Address: 0x02682608
extern void courseHudDrawSpinIcon_(void* spin_at_8c4, u32 flip_x, u32 flip_y, f32* pos_out);

// Address: 0x02645854
extern void courseHudDrawItemListBase_(void* item_list_at_910);

}

// Address: 0x0252F900
void PlayerCourseHud::drawStockItemList()
{
    u8* self = reinterpret_cast<u8*>(this);

    courseHudPrepare_(this);

    (void)*reinterpret_cast<volatile u32*>(kGuestCourseHudScratch);

    u32 course_out = AreaTask::getCourseOutHudFlag();
    bool single_drc = MainGame::instance()->isSinglePlayerDRC();

    if (course_out != 0u)
    {
        if (!single_drc)
        {
            StateMethod& state_method = fieldAt<StateMethod>(self, 0x17F0u);
            const StateID& target_state = stateIdAt(kGuestCourseHudStateId);
            if (state_method.getStateID()->isEqual(target_state))
            {
                state_method.changeStateMethod(target_state);
                return;
            }
            return;
        }
    }
    else
    {
        if (single_drc)
        {
            StateMethod& state_method = fieldAt<StateMethod>(self, 0x17F0u);
            state_method.executeStateMethod();
            courseHudSyncItemListAnims_(this);

            const sead::Vector2f& default_pos = rodataVec2f(kGuestDefaultStockItemPos);
            sead::Vector2f pos;
            if ((fieldAt<u32>(self, 0x67B8u) & 1u) != 0u)
                pos = fieldAt<sead::Vector2f>(self, 0x68BCu);
            else
                pos = default_pos;

            f32 stack_pos[2];
            stack_pos[0] = pos.x;
            stack_pos[1] = pos.y;
            stack_pos[1] = rodataF32(kGuestStockItemPosAdjustY) - stack_pos[1];
            stack_pos[1] = -stack_pos[1];
            stack_pos[0] = stack_pos[0] - rodataF32(kGuestStockItemPosAdjustX);

            if (fieldAt<u8>(self, 0x6151u) != 0u)
            {
                u32 flip_y;
                if ((fieldAt<u32>(self, 0x68B8u) & 0x2000u) == 0u)
                    flip_y = 1u;
                else
                    flip_y = 0u;
                u32 flip_x;
                if ((fieldAt<u32>(self, 0x67ACu) & 0x2000u) == 0u)
                    flip_x = 1u;
                else
                    flip_x = 0u;
                courseHudDrawSpinIcon_(
                    reinterpret_cast<void*>(self + 0x8C4u),
                    flip_x,
                    flip_y,
                    stack_pos);
            }

            courseHudDrawItemListBase_(reinterpret_cast<void*>(self + 0x910u));

            Multi2D& item_list = fieldAt<Multi2D>(self, 0x1804u);
            item_list.animPlay(0x1Cu, nullptr);
        }
    }
}
