#ifndef INC_H
#define INC_H


#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>


#include "objects/bitwriter.h"
#include "objects/bitreader.h"
#include "objects/tree.h"

inline bool isItALeaf(std::shared_ptr <TreeNode> head) {
    return !head->left && !head->right;
}

struct PQComp {
    bool operator() (
        const std::shared_ptr <TreeNode> &a, 
        const std::shared_ptr <TreeNode> &b
      ) const {
        return a->weightOfChar > b->weightOfChar;
    }
};

std::priority_queue <
    std::shared_ptr <TreeNode>, 
    std::vector <std::shared_ptr <TreeNode>>, 
    PQComp
> getCount(std::ifstream &f);

std::shared_ptr <TreeNode> makeHuffTree(
        std::priority_queue <
        std::shared_ptr <TreeNode>, 
        std::vector <std::shared_ptr <TreeNode>>, 
        PQComp
    > pq
);

void makeTable (
    std::shared_ptr <TreeNode> head, 
    int i, 
    std::unordered_map <int, int> &table
);

std::unordered_map <int, int> compress (
    std::ifstream &f, 
    std::string fileName
);

void decompress (
    std::unordered_map <int, int> table,
    std::string fileName
);

#endif