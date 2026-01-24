#include <inc.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Proper use: ./zipper fileName outputFileName\nPlease try again\n";
        return 1; 
    }

    std::ifstream f (argv[1], std::ios::binary);
    if (!f) {
        std::cout << "Error: Could not read the file\n";
        return 1;
    }
    
    std::unordered_map <int, int> 
    table = compress(f, argv[1]);

    decompress(table, argv[2]);
    return 0;
}



std::unordered_map <int, int> compress (std::ifstream &f, std::string fileName) {
    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> vals = getCount(f);

    std::shared_ptr <TreeNode> head = makeHuffTree(vals);

    std::unordered_map <int, int> table = {};
    makeTable(head, 1, table); // 1 = 00000001 in binary, it's called a sentinel bit

    std::ofstream o ("output.bin", std::ios::binary);

    if (!o) {
        std::cout << "Error: Could not read the file\n";
        return table;
    }
    f.clear();
    f.seekg(0, std::ios::beg); // reset the count reader

    int buf;
    BitWriter bits;
    while ((buf = f.get()) != EOF) {
        bits.bitStorer(o, table[buf]);
    }
    bits.bitStorer(o, table[-1]); // the hard-codded EOF
    bits.flushBitWriter(o);

    o.close(); f.close();
    return table;
}

void decompress (std::unordered_map <int, int> table, std::string fileName) {
    
    std::unordered_map <int, int> invTable;
    for (const auto &v : table) {
        invTable[v.second] = v.first;
    }

    std::ofstream ot (fileName, std::ios::binary);
    std::ifstream c ("output.bin", std::ios::binary);
    if (!ot || !c) {
        std::cout << "Error: Could not open the file\n";
        return;
    }
    
    BitReader bitsR;
    uint8_t readingBuf;
    while (c.read(reinterpret_cast<char*>(&readingBuf), 1)) {
        bool state = bitsR.readBits(ot, readingBuf, invTable);
        if (!state) break; // reached the EOF
    }
}

std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> getCount(std::ifstream &f) {
    std::unordered_map <int, int> counter = {};
    int buffer;
    // get the count (weight) of each letter
    while ((buffer = f.get()) != EOF) {
        counter[buffer]++;
    }

    std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq;
    for (auto element : counter) 
        pq.push(std::make_shared <TreeNode> (element.first, element.second));
    return pq;
}

std::shared_ptr <TreeNode> makeHuffTree(std::priority_queue <std::shared_ptr <TreeNode>, std::vector <std::shared_ptr <TreeNode>>, PQComp> pq) {
    pq.push(std::make_shared <TreeNode> (-1, 1)); // hard-codded EOF
    while (pq.size() > 1) {

        // make a node out of the least used leaves
        // (the lowest weight) and add that node to our pq
        std::shared_ptr <TreeNode> a = pq.top(); pq.pop();
        std::shared_ptr <TreeNode> b = pq.top(); pq.pop();
        pq.push(std::make_shared <TreeNode> (-1, a->weightOfChar + b->weightOfChar, a, b));
    }
    return pq.top();
}

void makeTable (std::shared_ptr <TreeNode> head, int i, std::unordered_map <int, int> &table) {
    if (!head) return;
    if (isItALeaf(head)) {
        table[head->charValueInInt] = i;
        return;
    }

    // 0 for left
    // 1 for right
    makeTable(head->left,  (i << 1),     table);
    makeTable(head->right, (i << 1) + 1, table);
}