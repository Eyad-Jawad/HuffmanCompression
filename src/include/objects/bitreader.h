#pragma once
#include "inc.h"

class BitReader {
    private: 
        encodedChars buffer;
    public:
        BitReader () {
            buffer.n = buffer.len = 0; // for the sentinel bit
        }

        bool readBits(std::ofstream &f, uint8_t chunk, std::unordered_map <uint64_t, int> &table) {
            int bufferSizeInBits = sizeof(chunk) * 8;
            for (int i = 0; i < bufferSizeInBits; i++) {
                buffer.n <<= 1;
                buffer.n += ((chunk >> (bufferSizeInBits - 1 - i)) & 1); 
                buffer.len++;
                // shift buffer and add the current bit

                // if the current combination of bits is valid, write it
                if (table.find(buffer.getId()) != table.end()) {
                    if (table[buffer.getId()] == -1) return false; // our EOF, it returns false to signal the reader to stop
        
                    f.write((char*) &table[buffer.getId()], 1);
                    buffer.n = buffer.len = 0;
                }
            }
            return true;
        }
};
