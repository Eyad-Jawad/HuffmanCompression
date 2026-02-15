#pragma once
#include "tree.h"
#include <cstdint>
#include <vector>

class BitReader {
    private: 
        std::shared_ptr <TreeNode> dummy;
    public:
        BitReader (std::shared_ptr <TreeNode> head) {
            dummy = head;
        }

        void readBits(std::ofstream &f, const std::array<uint8_t, CHUNK_SIZE> &chunks, std::shared_ptr <TreeNode> head, int &remainingBytes) {
            std::vector <uint8_t> writingBuffer;

            // reading every chunk (byte) in our 256kb
            for (int i = 0; i < CHUNK_SIZE;  i++) {

                // the end of the encoded file
                if (remainingBytes <= 0) break;
                uint8_t chunk = chunks[i];
                int bufferSizeInBits = sizeof(chunk) * 8;

                // loop over bits in the chunk
                for (int j = 0; j < bufferSizeInBits; j++) {
                    if (remainingBytes <= 0) break;

                    // if it 1 go right, else (0) go left
                    if (((chunk >> (bufferSizeInBits - 1 - j)) & 1)) 
                        dummy = dummy->right;
                    else 
                        dummy = dummy->left; 

                    if (dummy->isItALeaf()) {
                        writingBuffer.push_back(dummy->charValueInInt);
                        dummy = head;
                        remainingBytes--;
                    }
                }
            }
            f.write(reinterpret_cast<char*> (writingBuffer.data()), writingBuffer.size());
        }
};
