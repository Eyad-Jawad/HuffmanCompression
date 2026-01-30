#include "inc.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Proper use: ./zipper fileName outputFileName\nPlease try again\n";
        return 1; 
    }

    std::ifstream f (argv[1], std::ios::binary);
    if (!f) {
        std::cout << "Error: the file is empty!\n";
        return 1;
    }

    compress(f, argv[2]);

    decompress(argv[2]);
    return 0;
}



void compress (std::ifstream &f, std::string fileName) {
    int fileSize = 0;
    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> vals = getCount(f, fileSize);

    std::shared_ptr <TreeNode> head = makeHuffTree(vals);

    std::unordered_map <int, encodedChars> table = {};
    makeTable(head, 0, 0, table);

    std::ofstream o ("compressed " + fileName + ".bin", std::ios::binary);

    if (!o) {
        std::cout << "Error: Could not open a file for compression\n";
        return;
    }

    writeHeader(o, table, fileSize);

    f.clear();
    f.seekg(0, std::ios::beg); // reset the count reader

    int buf;
    BitWriter bits;
    while ((buf = f.get()) != EOF) {
        bits.bitStorer(o, table[buf]);
    }
    bits.flushBitWriter(o);

    o.close(); f.close();
    return;
}

void decompress (std::string fileName) {
    std::ofstream ot ("decompressed " + fileName,        std::ios::binary);
    std::ifstream c  ("compressed " + fileName + ".bin", std::ios::binary);
    if (!c) {
        std::cout << "Error: Could not open the comperssed file\n";
        return;
    }
    if (!ot) {
        std::cout << "Error: could not open a file for decompression\n";
        return;
    }

    char header[3];
    c.read(header, 3);
    if (header[0] != 'H' || header[1] != 'U' || header[2] != 'F') {
        std::cout << "Error: This file is not huffman encoded";
        return;
    }

    int fileSizeBeforeCompression;
    std::unordered_map <uint64_t, int> table = {};
    readHeader(c, table, fileSizeBeforeCompression);

    BitReader bitsR;
    uint8_t chunk;
    while (fileSizeBeforeCompression > 0) {
        c.read(reinterpret_cast<char*>(&chunk), 1);
        bitsR.readBits(ot, chunk, table, fileSizeBeforeCompression);
    }
    // each symbol decoded is a byte from the original size
    // and this method decrements it until it has decoded
    // the whole file and thus file size is 0, that's why we don't
    // need an EOF
}

std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> getCount(std::ifstream &f, int &fileSize) {
    std::unordered_map <int, int> counter = {};
    int buffer;
    // get the count (weight) of each letter
    while ((buffer = f.get()) != EOF) {
        fileSize++;
        counter[buffer]++;
    }

    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq;
    for (auto element : counter) 
        pq.push(std::make_shared <TreeNode> (element.first, element.second));
    return pq;
}

std::shared_ptr <TreeNode> makeHuffTree(std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq) {
    while (pq.size() > 1) {

        // make a node out of the least used leaves
        // (the lowest weight) and add that node to our pq
        std::shared_ptr <TreeNode> a = pq.top(); pq.pop();
        std::shared_ptr <TreeNode> b = pq.top(); pq.pop();
        pq.push(std::make_shared <TreeNode> (0, a->weightOfChar + b->weightOfChar, a, b));
    }
    return pq.top();
}

void makeTable (std::shared_ptr <TreeNode> head, int i, int depth, std::unordered_map <int, encodedChars> &table) {
    if (!head) return;
    if (isItALeaf(head)) {
        table[head->charValueInInt].n = i;
        table[head->charValueInInt].len = depth;

        return;
    }

    // 0 for left
    // 1 for right
    makeTable(head->left,  (i << 1),     depth + 1, table);
    makeTable(head->right, (i << 1) + 1, depth + 1, table);
}

void writeHeader (std::ofstream &o, std::unordered_map <int, encodedChars> &table, int &fileSize) {
    /*
        ============================================
        FILE STRUCTURE: (14 bytes)
        HUF (The header) (3 bytes)
        File size before compression (4 bytes)
        number of unique symbols (2 bytes)
        encoding table:
            character value in ascii (1 byte)
            its encoded value or bits (4 bytes)
            the length of bytes because its hard to use bits in c++ (1 byte)
        compressed file
        ============================================
    */

    o.write("HUF", 3); // signature
    writeBytes<int>(o, fileSize); 
    uint16_t uniqueSymbols = static_cast<uint16_t> (table.size());
    writeBytes<uint16_t>(o, uniqueSymbols);
    for (auto &v : table) {
        writeBytes<uint8_t> (o, v.first);       // ascii character's value
        writeBytes<uint32_t>(o, v.second.n);    // the encoded bits
        writeBytes<uint8_t> (o, v.second.len);  // the length of the actual bits
    }
}

void readHeader (std::ifstream &c, std::unordered_map <uint64_t, int> &table, int &fileSizeBeforeCompression) {
    c.read(reinterpret_cast<char*>(&fileSizeBeforeCompression), 4);

    uint16_t uniqueSymbols;
    c.read(reinterpret_cast<char*>(&uniqueSymbols), 2);
    
    // reading the encoding table
    for (int _ = 0; _ < uniqueSymbols; _++) {
        uint8_t character;
        uint32_t bits;
        uint8_t bitLen;
        c.read(reinterpret_cast<char*>(&character), 1);
        c.read(reinterpret_cast<char*>(&bits), 4);
        c.read(reinterpret_cast<char*>(&bitLen), 1);
        encodedChars c;
        c.n = bits;
        c.len = bitLen;
        table[c.getId()] = character;
    }
}