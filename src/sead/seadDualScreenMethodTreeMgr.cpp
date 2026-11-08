#include <system/DualScreenMethodTreeMgr.h>

namespace sead {

namespace {

// Address: 0x029FF618
void MethodTreeNode_call_(MethodTreeNode* node);

} // namespace

// Address: 0x029FD944
void DualScreenMethodTreeMgr::drawTop()
{
    MethodTreeNode* node = reinterpret_cast<MethodTreeNode*>(reinterpret_cast<u8*>(this) + 0x13C);
    MethodTreeNode_call_(node);
}

// Address: 0x029FD94C
void DualScreenMethodTreeMgr::drawBtm()
{
    MethodTreeNode* node = reinterpret_cast<MethodTreeNode*>(reinterpret_cast<u8*>(this) + 0x28C);
    MethodTreeNode_call_(node);
}

// Address: 0x029FD93C
void DualScreenMethodTreeMgr::calc()
{
    MethodTreeNode* node = reinterpret_cast<MethodTreeNode*>(reinterpret_cast<u8*>(this) + 0x40);
    MethodTreeNode_call_(node);
}

} // namespace sead
