#include <actor/AttentionMgr.h>

#include <actor/ActorMgr.h>

namespace {

constexpr u32 kLookatListNodeOffset = 0x24u;

// Address: 0x029F1594
void* sub_029F1594(u32 size);

// Address: 0x029F18DC
void* sub_029F18DC(u32 size, u32 align);

// Address: 0x02A0A2FC
void sub_02A0A2FC(void* p);

// Address: 0x029F1A50
void sub_029F1A50(sead::OffsetList<AttentionLookat>* list, sead::ListNode* node);

// Address: 0x029F1A70
void sub_029F1A70(void* node);

sead::ListNode* lookatListNode_(AttentionLookat& lookat)
{
    return reinterpret_cast<sead::ListNode*>(reinterpret_cast<u8*>(&lookat) + kLookatListNodeOffset);
}

const sead::ListNode* lookatListNode_(const AttentionLookat& lookat)
{
    return reinterpret_cast<const sead::ListNode*>(reinterpret_cast<const u8*>(&lookat) + kLookatListNodeOffset);
}

} // namespace

const f32 AttentionMgr::cMaxDistanceSqMax = 262144.0f;

f32 AttentionMgr::sMaxDistanceSq[AttentionLookat::cDistance_Num] = {
    25600.0f,
    57600.0f,
    262144.0f,
    65536.0f,
};

SEAD_SINGLETON_DISPOSER_IMPL(AttentionMgr)

// Address: 0x0200CB88
AttentionMgr::AttentionMgr()
{
    u8* self = reinterpret_cast<u8*>(this);
    if (self == nullptr)
    {
        self = static_cast<u8*>(sub_029F1594(0x24));
        if (self == nullptr)
            return;
    }

    decomp::be32(self + 0x20) = 0x10001364u;
    u8* node = self + 0x10;
    decomp::be32(node + 4) = static_cast<u32>(reinterpret_cast<uintptr_t>(node));
    decomp::be32(node + 0) = static_cast<u32>(reinterpret_cast<uintptr_t>(node));
    decomp::be32(node + 8) = 0;
    decomp::be32(node + 0xC) = 0xFFFFFFFFu;
    decomp::be32(self + 0x1C) = kLookatListNodeOffset;
}

// Address: 0x0200CC8C
void AttentionMgr::entry(const AttentionLookat& lookat)
{
    u8* self = reinterpret_cast<u8*>(this);
    u32 off = static_cast<u32>(decomp::be32(self + 0x1C));
    sub_029F1A50(reinterpret_cast<sead::OffsetList<AttentionLookat>*>(self + 0x10),
                 reinterpret_cast<sead::ListNode*>(reinterpret_cast<u8*>(const_cast<AttentionLookat*>(&lookat)) + off));
    decomp::be32(self + 0x18) = static_cast<u32>(decomp::be32(self + 0x18)) + 1u;
}

// Address: 0x0200CCD0
void AttentionMgr::release(const AttentionLookat& lookat)
{
    u8* self = reinterpret_cast<u8*>(this);
    u32 off = static_cast<u32>(decomp::be32(self + 0x1C));
    u8* node = reinterpret_cast<u8*>(const_cast<AttentionLookat*>(&lookat)) + off;
    u32 next = static_cast<u32>(decomp::be32(node + 4));
    u32 prev = static_cast<u32>(decomp::be32(node));
    if (next == 0 && prev == 0)
        return;
    sub_029F1A70(node);
    decomp::be32(self + 0x18) = static_cast<u32>(decomp::be32(self + 0x18)) - 1u;
}

// Address: 0x0200CD2C
const AttentionLookat* AttentionMgr::search(const ActorBase& actor_self, const sead::Vector2f& pos_self)
{
    u8* self = reinterpret_cast<u8*>(this);
    const u32 node_offset = static_cast<u32>(decomp::be32(self + 0x1C));
    const AttentionLookat* end = reinterpret_cast<const AttentionLookat*>(self + 0x10 - node_offset);
    u32 first = static_cast<u32>(decomp::be32(self + 0x14));
    const AttentionLookat* it = reinterpret_cast<const AttentionLookat*>(static_cast<uintptr_t>(first - node_offset));

    f32 best_dist_sq = cMaxDistanceSqMax;
    const AttentionLookat* best = nullptr;

    while (it != end)
    {
        ActorBase* owner = ActorMgr::instance()->getActorPtr(it->getOwnerID());
        if (owner != nullptr && owner != &actor_self)
        {
            const sead::Vector2f& pos = it->getPos();
            f32 dx = pos.x - pos_self.x;
            f32 dy = pos.y - pos_self.y;
            f32 dist_sq = dx * dx + dy * dy;

            u32 type = static_cast<u32>(it->getMaxDistanceType());
            f32 max_sq = sMaxDistanceSq[type];
            if (dist_sq <= max_sq && dist_sq <= best_dist_sq)
            {
                best_dist_sq = dist_sq;
                best = it;
            }
        }

        u32 next = static_cast<u32>(decomp::be32(reinterpret_cast<const u8*>(lookatListNode_(*it)) + 4));
        it = reinterpret_cast<const AttentionLookat*>(static_cast<uintptr_t>(next - node_offset));
    }

    return best;
}
