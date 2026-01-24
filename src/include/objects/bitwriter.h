#include <fstream>
#include <cstdlib>

class BitWriter {
    private:
        uint64_t bits;
        uint8_t buffer;
        int lengthOfBits;
        int bufferSizeInBytes;
        int bufferSizeInBits;
    public:
        BitWriter () {
            bits = 0;
            buffer = 0;
            lengthOfBits = 0;
            bufferSizeInBytes = sizeof(buffer);
            bufferSizeInBits = bufferSizeInBytes * 8;
        }

        // the input of this method is the file so we can write the bits
        // when 8 bits (1 byte) of encodded characters is accumalated
        // and the charInput which is the encodded character
        // it is 16 btis because in huffman coding chars can get quite lengthy
        void bitStorer (std::ofstream &f, uint16_t charInput) {

            int lenInBits = 0; 
            uint16_t tempCharHolder = charInput;
            // it is 1 to ignore the sentinel bit
            while (tempCharHolder > 1) {
                lenInBits++;
                tempCharHolder >>= 1;
            }

            lengthOfBits += lenInBits;
            charInput ^= (1 << lenInBits); // remove the sentinel bit
            bits = ((bits << lenInBits) | charInput);

            while (lengthOfBits >= bufferSizeInBits) {
                bitWriter(f);
            }
        }

        void bitWriter (std::ofstream &f) {
            int shift = lengthOfBits - bufferSizeInBits;
            buffer = (bits >> (shift));
            // so we get the last 8 bits to the right only
            f.write((reinterpret_cast<char*> (&buffer)), bufferSizeInBytes);

            // removing the last 8 bits on the right using XOR
            bits ^= ((bits >> shift) << shift);
            lengthOfBits -= bufferSizeInBits;
            buffer = 0;
        }

        // this method is here in case the variable bits has some btis that it
        // are less than 8 in length, so we add a padding of 0s and write them
        void flushBitWriter (std::ofstream &f) {
            if (lengthOfBits > 0) {
                buffer = (bits << (bufferSizeInBits - lengthOfBits)); // the 0s padding
                f.write((reinterpret_cast<char*> (&buffer)), bufferSizeInBytes);
                bits = lengthOfBits = buffer = 0;
            }
        }
};
