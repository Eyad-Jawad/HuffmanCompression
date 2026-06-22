#include "../include/inc.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Proper use: ./zipper comp/decomp fileName outputFileName\ncomp for compress and decomp for decompress\nPlease try again\n";
        return 1; 
    }

    std::ifstream f (argv[2], std::ios::binary);
    if (!f) {
        std::cout << "Error: the file is empty!\n";
        return 1;
    }

    auto runTime = std::chrono::steady_clock::now();
    int fileSizeBeforeComp = 0;
    int fileSizeAfterComp  = 0;
    std::string mode = argv[1];

    if (mode == "comp") {
        compress(f, argv[3], fileSizeBeforeComp, fileSizeAfterComp);
    }

    auto compTime = std::chrono::steady_clock::now() - runTime;
    float savedSpace = (fileSizeBeforeComp - fileSizeAfterComp) * 100.0 / fileSizeBeforeComp;
    
    if (mode ==  "decomp") {
        decompress(argv[2], argv[3]);
    }
    
    auto decompTime = std::chrono::steady_clock::now() - runTime - compTime;
    benchMarks(compTime, decompTime, fileSizeBeforeComp, fileSizeAfterComp, savedSpace);

    return 0;
}


void benchMarks(const std::chrono::steady_clock::duration &compTime, const std::chrono::steady_clock::duration &decompTime, const int &fileSizeBeforeComp, const int &fileSizeAfterComp, const float &savedSpace) {
    using namespace std::chrono;
    auto compTimeInMs   = duration_cast <milliseconds> (compTime);
    auto decompTimeInMs = duration_cast <milliseconds> (decompTime);
    auto totalTimeInMs = compTimeInMs + decompTimeInMs;
    std::cout << "Compression time:   " << "          "; 
    std::cout << compTimeInMs.count() << "ms\n";
    std::cout << "decompression time: " << "          ";
    std::cout << decompTimeInMs.count() << "ms\n";
    std::cout << "Total Time:         " << "          ";
    std::cout << totalTimeInMs.count() << "ms\n\n";

    std::cout << "File size before compression: " << byteToKB(fileSizeBeforeComp) << "kb\n";
    std::cout << "File size after compression:  " << byteToKB(fileSizeAfterComp) << "kb\n";
    std::cout << "Saved space:                  " << savedSpace << "%\n";
}

void compress (std::ifstream &f, std::string fileName, int &fileSizeBeforeComp, int &fileSizeAfterComp) {
    int fileSize = 0;
    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> vals = getCount(f, fileSize);
    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> valsCopy = vals;

    std::shared_ptr <TreeNode> head = makeHuffTree(vals);

    std::unordered_map <int, encodedChars> table = {};
    makeTable(head, 0, 0, table);

    std::ofstream o (fileName, std::ios::binary);

    if (!o) {
        std::cout << "Error: Could not open a file for compression\n";
        return;
    }

    writeHeader(o, valsCopy, fileSize, head);
    fileSizeBeforeComp = fileSize;

    f.clear();
    f.seekg(0, std::ios::beg); // reset the count reader

    int buf;
    BitWriter bits;
    while ((buf = f.get()) != EOF) {
        bits.bitStorer(o, table[buf]);
    }
    fileSizeAfterComp = bits.flushBitWriter(o);

    o.close(); f.close();
    return;
}

void decompress (const std::string fileName, const std::string outputFileName) {
    std::ifstream c  (fileName,       std::ios::binary);
    std::ofstream ot (outputFileName, std::ios::binary);
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
    std::shared_ptr <TreeNode> head = readHeader(c, fileSizeBeforeCompression);

    BitReader bitsR(head);
    std::array <uint8_t, CHUNK_SIZE> chunk;
    while (fileSizeBeforeCompression > 0) {
        c.read(reinterpret_cast<char*>(&chunk), CHUNK_SIZE);
        bitsR.readBits(ot, chunk, head, fileSizeBeforeCompression);
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

std::shared_ptr <TreeNode> makeHuffTree(std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> &pq) {
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
    if (head->isItALeaf()) {
        table[head->charValueInInt].n = i;
        table[head->charValueInInt].len = depth;

        return;
    }

    // 0 for left
    // 1 for right
    makeTable(head->left,  (i << 1),     depth + 1, table);
    makeTable(head->right, (i << 1) + 1, depth + 1, table);
}

void writeHeader (std::ofstream &o, std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> &vals, const int &fileSize, std::shared_ptr <TreeNode> head) {
    /*
        ============================================
        FILE STRUCTURE: (14 bytes)
        HUF (The header) (3 bytes)
        File size before compression (4 bytes)
        number of unique symbols (2 bytes)
        encoding table:
            character value in ascii (1 byte)
            its weight (4 bytes)
        compressed file
        ============================================
    */

    o.write("HUF", 3); // signature
    writeBytes<int>(o, fileSize); 
    uint16_t uniqueSymbols = static_cast<uint16_t> (vals.size());
    writeBytes<uint16_t>(o, uniqueSymbols);

    for (int i = 0; i < uniqueSymbols; i++) {
        // writing the cahracters in an ascending order
        writeBytes<uint8_t>(o, vals.top()->charValueInInt);
        writeBytes<int>    (o, vals.top()->weightOfChar); vals.pop();
    }
}

std::shared_ptr <TreeNode> readHeader (std::ifstream &c, int &fileSizeBeforeCompression) {
    c.read(reinterpret_cast<char*>(&fileSizeBeforeCompression), 4);

    uint16_t uniqueSymbols;
    c.read(reinterpret_cast<char*>(&uniqueSymbols), 2);

    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq;
    // reading the encoding table
    for (int i = 1; i <= uniqueSymbols; i++) {
        uint8_t character;
        int weight;
        c.read(reinterpret_cast<char*>(&character), 1);
        c.read(reinterpret_cast<char*>(&weight), 4);
        pq.push(std::make_shared <TreeNode> (character, weight));
    }
    return makeHuffTree(pq);
}