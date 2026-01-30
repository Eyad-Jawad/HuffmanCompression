#ifndef INC_H
#define INC_H


#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>

#include "types/encodedchars.h"
#include "objects/bitwriter.h"
#include "objects/bitreader.h"
#include "objects/tree.h"
#include "types/pqcomp.h"

inline bool isItALeaf(std::shared_ptr <TreeNode> head) {
    return !head->left && !head->right;
}

template <typename T>
inline void writeBytes(std::ofstream &f, T buffer) {
    T n = static_cast<T>(buffer);
    f.write(reinterpret_cast<const char*>(&n), sizeof(n));
}

std::priority_queue <
    std::shared_ptr <TreeNode>, 
    std::vector <std::shared_ptr <TreeNode>>, 
    PQComp
> getCount (
    std::ifstream &f,
    int &fileSize
);

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
    int depth,
    std::unordered_map <int, encodedChars> &table
);

void compress (
    std::ifstream &f, 
    std::string fileName
);

void decompress (
    std::string fileName
);

#endif