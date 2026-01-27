#pragma once
#include "inc.h"

struct PQComp {
    bool operator() (
        const std::shared_ptr <TreeNode> &a, 
        const std::shared_ptr <TreeNode> &b
      ) const {
        return a->weightOfChar > b->weightOfChar;
    }
};