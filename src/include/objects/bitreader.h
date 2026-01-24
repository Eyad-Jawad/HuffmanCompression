#include <unordered_map>
#include <cstdlib>
#include <fstream>

class BitReader {
    private: 
        uint16_t buffer;
    public:
        BitReader () {
            buffer = 1; // for the sentinel bit
        }

        bool readBits(std::ofstream &f, uint8_t chunk, std::unordered_map <int, int> &table) {
            int bufferSizeInBits = sizeof(chunk) * 8;
            for (int i = 0; i < bufferSizeInBits; i++) {
                buffer <<= 1;
                buffer += ((chunk >> (bufferSizeInBits - 1 - i)) & 1); 
                // shift buffer and add the current bit

                // if the current combination of bits is valid, write it
                if (table.find(buffer) != table.end()) {
                    if (table[buffer] == -1) return false; // our EOF, it returns false to signal the reader to stop
        
                    f.write((char*) &table[buffer], 1);
                    buffer = 1;
                }
            }
            return true;
        }
};
