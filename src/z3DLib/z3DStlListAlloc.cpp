#include <stdint.h>
#include <vector>
#include "z3DLib/z3DStlListAlloc.h"

namespace z3D_priv{



struct StlListNodeMemBlock{
    StlListNodeMemBlock(){}
    unsigned char buf_[z3D::kMAX_SIZE_OF_LIST_ELEMENT];
};

// кэш
static std::vector<StlListNodeMemBlock*> s_StlListNodeMemBlockVec;
void* StlListAllocateNode(){

    if (s_StlListNodeMemBlockVec.empty()) {
        s_StlListNodeMemBlockVec.push_back(new StlListNodeMemBlock());
    }
    StlListNodeMemBlock* mem = s_StlListNodeMemBlockVec.back();
    s_StlListNodeMemBlockVec.pop_back();
    return static_cast<void*>(&mem->buf_[0]);
}

void StlListDeallocateNode(void* p){

    StlListNodeMemBlock* mem = static_cast<StlListNodeMemBlock*>(p);
    s_StlListNodeMemBlockVec.push_back(mem);
}

void ClearStlListCache(){
#ifdef Z3D_LISTALLOCATOR_DEBUG
    int cElem = 0;
#endif
    while (!s_StlListNodeMemBlockVec.empty()){
        StlListNodeMemBlock* mem = s_StlListNodeMemBlockVec.back();
        s_StlListNodeMemBlockVec.pop_back();
        delete mem;
#ifdef Z3D_LISTALLOCATOR_DEBUG
        cElem++;
#endif
    }
#ifdef Z3D_LISTALLOCATOR_DEBUG
    printf("z3D Stl cache size was: %d elements\n", cElem);
#endif
}
} // end of z3D_priv
