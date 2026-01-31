#pragma once
#include "inc.h"

class BitReader {
    private: 
        encodedChars buffer;
    public:
        BitReader () {
            buffer.n = buffer.len = 0;
        }

        void readBits(std::ofstream &f, std::array<uint8_t, CHUNK_SIZE> &chunks, std::unordered_map <uint64_t, int> &table, int &remainingBytes) {
            std::vector <uint8_t> writingBuffer;
            for (int i = 0; i < CHUNK_SIZE;  i++) {
                if (remainingBytes <= 0) break;
                uint8_t chunk = chunks[i];
                int bufferSizeInBits = sizeof(chunk) * 8;
                for (int j = 0; j < bufferSizeInBits; j++) {
                    buffer.n <<= 1;
                    buffer.n += ((chunk >> (bufferSizeInBits - 1 - j)) & 1); 
                    buffer.len++;
                    // shift buffer and add the current bit
    
                    // if the current combination of bits is valid, write it
                    auto it = table.find(buffer.getId());
                    if (it != table.end()) {
                        writingBuffer.push_back(it->second);
                        buffer.n = buffer.len = 0;
                        remainingBytes--;
                    }
                }
            }
            f.write(reinterpret_cast<char*> (writingBuffer.data()), writingBuffer.size());
        }
};
