#ifndef INC_H
#define INC_H

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>

constexpr size_t CHUNK_SIZE = 1024 * 256;

#include "types/encodedchars.h"
#include "objects/tree.h"
#include "objects/bitwriter.h"
#include "objects/bitreader.h"
#include "types/pqcomp.h"

template <typename T>
inline void writeBytes(std::ofstream &f, T buffer) {
    T n = static_cast<T>(buffer);
    f.write(reinterpret_cast<const char*>(&n), sizeof(n));
}

void writeHeader (
    std::ofstream &o, 
    std::priority_queue <
        std::shared_ptr <
            TreeNode>, 
        std::vector <
            std::shared_ptr <
                TreeNode>
            >, 
        PQComp> 
    &vals, 
    int &fileSize,
    std::shared_ptr <TreeNode> head
);

std::shared_ptr <
    TreeNode
> readHeader (
    std::ifstream &c, 
    int &fileSizeBeforeCompression
);

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