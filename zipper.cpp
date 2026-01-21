#include <algorithm>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <queue>
#include <string>

class TreeNode {
    public:
        int val, weight;
        std::shared_ptr <TreeNode>  right, left; // std::unique_ptr<TreeNode> ??
        TreeNode (int v, int w, std::shared_ptr <TreeNode>  l = nullptr, std::shared_ptr <TreeNode>  r = nullptr) {
            val = v;
            weight = w;
            left  = std::move(l);
            right = std::move(r);
        }
};

struct PQComp {
    bool operator() (const std::shared_ptr <TreeNode> &a, const std::shared_ptr <TreeNode> &b) const {
        return a->weight > b->weight;
    }
};

class BitWriter {
    private:
        uint64_t bits;
        int lengthOfBits;
        uint8_t buffer;
        int bufferSizeInBytes;
        int bufferSizeInBits;
    public:
        BitWriter () {
            bits = 0;
            lengthOfBits = 0;
            buffer = 0;
            bufferSizeInBytes = sizeof(buffer);
            bufferSizeInBits = bufferSizeInBytes * 8;
        }

        void bitStorer (std::ofstream &f, uint16_t charInput) {
            int lenInBits = -1;
            uint16_t tempCharHolder = charInput;
            while (tempCharHolder > 0) {
                lenInBits++;
                tempCharHolder >>= 1;
            }

            lengthOfBits += lenInBits;
            charInput ^= (1 << lenInBits);
            bits = ((bits << lenInBits) | charInput);

            while (lengthOfBits >= bufferSizeInBits) {
                bitWriter(f);
            }
        }

        void bitWriter (std::ofstream &f) {
            int shift = lengthOfBits - bufferSizeInBits;
            buffer = (bits >> (shift));
            f.write((reinterpret_cast<char*> (&buffer)), bufferSizeInBytes);

            bits ^= ((bits >> shift) << shift);
            lengthOfBits -= bufferSizeInBits;
            buffer = 0;
        }

        void flushBitWriter (std::ofstream &f) {
            if (lengthOfBits > 0) {
                buffer = (bits << (bufferSizeInBits - lengthOfBits));
                f.write((reinterpret_cast<char*> (&buffer)), bufferSizeInBytes);
                bits = lengthOfBits = buffer = 0;
            }
        }
};

class BitReader {
    private: 
        uint16_t buffer;
    public:
        BitReader () {
            buffer = 1;
        }

        bool readBits(std::ofstream &f, uint8_t chunk, std::unordered_map <int, int> &table) {
            int bufferSizeInBits = sizeof(chunk) * 8;
            for (int i = 0; i < bufferSizeInBits; i++) {
                buffer <<= 1;
                buffer += ((chunk >> (bufferSizeInBits - 1 - i)) & 1);

                if (table.find(buffer) != table.end()) {
                    if (table[buffer] == -1) return false; // our EOF, it returns false to signal the reader to stop
        
                    f.write((char*) &table[buffer], 1);
                    buffer = 1;
                }
            }
            return true;
        }
};


std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> getCount(std::ifstream &f);
std::shared_ptr <TreeNode> makeHuffTree(std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq);
void makeTable (std::shared_ptr <TreeNode> head, int i, std::unordered_map <int, int> &table);


int main(int argc, char *argv[]) {
    clock_t programTime = clock();
    if (argc != 2) {
        std::cout << "Proper use: ./zipper fileName\nPlease try again\n";
        return 1; 
    }
    std::ifstream f (argv[1], std::ios::binary);
    // std::ifstream f ("test.txt", std::ios::binary);
    if (!f) {
        std::cout << "Error: Could not read the file\n";
        return 1;
    }
    
    std::cout << "Reading the file...\n";
    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> vals = getCount(f);
    f.close();
    clock_t readingFile = clock() - programTime;
    std::cout << "Done reading the file!: " << readingFile/1000.0 << "\n\n";

    std::cout << "Making the encoding tree...\n";
    std::shared_ptr <TreeNode> head = makeHuffTree(vals);

    std::cout << "Making the encoding table...\n";
    std::unordered_map <int, int> table = {};
    makeTable(head, 1, table); // 1 = 00000001 in binary

    clock_t treeAndTableTime = clock() - programTime - readingFile;
    std::cout << "Done making the huffman tree and table!: " << treeAndTableTime/1000.0 << "\n\n";

    std::ofstream o ("output.bin", std::ios::binary);

    std::ifstream d(argv[1], std::ios::binary);
    // std::ifstream d("test.txt", std::ios::binary);
    if (!d || !o) {
        std::cout << "Error: Could not read the file\n";
        return 1;
    }

    std::cout << "Writing the encoded file...\n";
    int sizeBeforeComp = 0;
    int buf;
    BitWriter bits;
    while (true) {
        buf = d.get();
        sizeBeforeComp++;
        if (buf == EOF) {
            bits.bitStorer(o, table[-1]);
            bits.flushBitWriter(o);
            break;
        }
        bits.bitStorer(o, table[buf]);
    }
    o.close(); d.close();
    clock_t encodignTime = clock() - programTime - readingFile - treeAndTableTime;
    std::cout << "Done writing the encoded file!: " << encodignTime/1000.0 << "\n\n";
    
    std::unordered_map <int, int> invTable;
    for (const auto &v : table) {
        invTable[v.second] = v.first;
    }

    std::ofstream ot ("output.txt", std::ios::binary);
    std::ifstream c ("output.bin", std::ios::binary);
    if (!ot || !c) {
        std::cout << "Error: Could not open the file\n";
        return 1;
    }
    
    std::cout << "Writing the decoded file...\n";
    BitReader bitsR;
    int sizeAfterComp = 0;
    uint8_t readingBuf;
    while (c.read(reinterpret_cast<char*>(&readingBuf), 1)) {
        sizeAfterComp++;
        bool state = bitsR.readBits(ot, readingBuf, invTable);
        if (!state) break;
    }
    clock_t decodingTime = clock() - programTime - readingFile - treeAndTableTime - encodignTime;
    std::cout << "Done writing the decoded file!: " << decodingTime/1000.0 << "\n\n";

    std::cout << std::string(40, '*') << '\n' << std::string(18, ' ') << "DONE!\n" << std::string(40, '*') << "\n\n";
    std::cout << "It took the program " << (clock() - programTime)/1000.0 << "s to zip and unzip the file " << argv[1];
    std::cout << " which is of size: " << (sizeBeforeComp/1024.0) << "KB before compression, and of size: ";
    std::cout << (sizeAfterComp/1024.0) << "KB after compression";
    std::cout << "saving " << ((sizeBeforeComp - sizeAfterComp)/ (float) sizeBeforeComp) * 100 << "%\n";
    return 0;
}

std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> getCount(std::ifstream &f) {
    std::unordered_map <int, int> counter = {};
    int buffer;
    while (buffer = f.get()) {
        if (buffer == EOF) break;
        counter[buffer]++;
    }

    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq;
    for (auto element : counter) 
        pq.push(std::make_shared <TreeNode> (element.first, element.second));
    return pq;
}

std::shared_ptr <TreeNode> makeHuffTree(std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq) {
    pq.push(std::make_shared <TreeNode> (-1, 1));
    while (pq.size() > 1) {
        std::shared_ptr <TreeNode> a = pq.top(); pq.pop();        
        std::shared_ptr <TreeNode> b = pq.top(); pq.pop();
        pq.push(std::make_shared <TreeNode> (-1, a->weight + b->weight, a, b));
    }
    return pq.top();
}

void makeTable (std::shared_ptr <TreeNode> head, int i, std::unordered_map <int, int> &table)
{
    if (!head) return;
    if (!head->left && !head->right) {
            table[head->val] = i;
            return;
        }
    makeTable(head->left,  (i << 1),     table);
    makeTable(head->right, (i << 1) + 1, table);
}