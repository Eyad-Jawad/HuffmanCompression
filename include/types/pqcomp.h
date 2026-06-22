#pragma once
#include "objects/tree.h"

struct PQComp {
    bool operator() (
        const std::shared_ptr <TreeNode> &a, 
        const std::shared_ptr <TreeNode> &b
      ) const {
        if (a->weightOfChar == b->weightOfChar) return a->charValueInInt > b->charValueInInt;
        return a->weightOfChar > b->weightOfChar;
    }
};