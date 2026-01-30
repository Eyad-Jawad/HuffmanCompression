#pragma once
#include "inc.h"

class BitReader {
    private: 
        encodedChars buffer;
    public:
        BitReader () {
            buffer.n = buffer.len = 0;
        }

        void readBits(std::ofstream &f, uint8_t chunk, std::unordered_map <uint64_t, int> &table, int &remainingBytes) {
            int bufferSizeInBits = sizeof(chunk) * 8;
            for (int i = 0; i < bufferSizeInBits; i++) {
                if (remainingBytes == 0) return; // it means we reached the padding (file end)
                buffer.n <<= 1;
                buffer.n += ((chunk >> (bufferSizeInBits - 1 - i)) & 1); 
                buffer.len++;
                // shift buffer and add the current bit

                // if the current combination of bits is valid, write it
                if (table.find(buffer.getId()) != table.end()) {
                    f.write((char*) &table[buffer.getId()], 1);
                    buffer.n = buffer.len = 0;
                    remainingBytes--;
                }
            }
        }
};
