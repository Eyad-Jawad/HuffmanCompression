#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>

using namespace std;

class TreeNode {
    public:
        int val, weight;
        TreeNode *right, *left; // unique_ptr<TreeNode> ??
        TreeNode (int v, int w, TreeNode *l = nullptr, TreeNode *r = nullptr) {
            val = v;
            weight = w;
            left = l;
            right = r;

        }
        void delTreeNode(TreeNode *head) {
            if (head == nullptr) return;
            delTreeNode(head->left);
            delTreeNode(head->right);
            delete head;
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

        void bitStorer (ofstream &f, uint16_t i) {
            int lenInBits = -1;
            uint16_t temp = i;
            while (temp > 0) {
                lenInBits++;
                temp >>= 1;
            }

            lengthOfBits += lenInBits;
            i ^= (1 << lenInBits);
            bits = ((bits << lenInBits) | i);

            while (lengthOfBits >= bufferSizeInBits) {
                bitWriter(f);
            }
        }

        void bitWriter (ofstream &f) {
            int shift = lengthOfBits - bufferSizeInBits;
            buffer = (bits >> (shift));
            f.write((reinterpret_cast<char*> (&buffer)), bufferSizeInBytes);

            bits ^= ((bits >> shift) << shift);
            lengthOfBits -= bufferSizeInBits;
            buffer = 0;
        }

        void flushBitWriter (ofstream &f) {
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

        bool readBits(ofstream &f, uint8_t chunk, map <int, int> &table) {
            int bufferSizeInBits = sizeof(chunk) * 8;
            for (int i = 0; i < bufferSizeInBits; i++) {
                buffer <<= 1;
                buffer += ((chunk >> (bufferSizeInBits - 1 - i)) & 1);

                if (table.find(buffer) != table.end()) {
                    if (table[buffer] == -1) return false; // our EOF, it returns false to signal the reader to stop
        
                    char c = table[buffer];
                    f.write(&c, 1);
                    buffer = 1;
                }
            }
            return true;
        }
};


vector <TreeNode*> getCount(ifstream &f);
vector <TreeNode*> bubbleSort(vector <TreeNode*> vals);
TreeNode* makeHuffTree(vector <TreeNode*> vals);
void makeTable (TreeNode *head, int s, map <int, int> &table);


int main(int argc, char *argv[]) {
    clock_t programTime = clock();
    if (argc != 2) {
        cout << "Proper use: ./zipper fileName\nPlease try again\n";
        return 1; 
    }
    ifstream f (argv[1], ios::binary);
    // ifstream f ("test.txt", ios::binary);
    if (!f) {
        cout << "Error: Could not read the file\n";
        return 1;
    }
    
    cout << "Reading the file...\n";
    vector <TreeNode*> vals = getCount(f);
    f.close();
    clock_t readingFile = clock() - programTime;
    cout << "Done reading the file!: " << readingFile/1000.0 << "\n\n";

    cout << "Making the encoding tree...\n";
    TreeNode *head = makeHuffTree(vals);

    cout << "Making the encoding table...\n";
    map <int, int> table = {};
    makeTable(head, 1, table); // 1 = 00000001 in binary
    head->delTreeNode(head);

    clock_t treeAndTableTime = clock() - programTime - readingFile;
    cout << "Done making the huffman tree and table!: " << treeAndTableTime/1000.0 << "\n\n";

    ofstream o ("output.bin", ios::binary);

    ifstream d(argv[1], ios::binary);
    // ifstream d("test.txt", ios::binary);
    if (!d || !o) {
        cout << "Error: Could not read the file\n";
        return 1;
    }

    cout << "Writing the encoded file...\n";
    int sizeBeforeComp = 0;
    unsigned int buf;
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
    cout << "Done writing the encoded file!: " << encodignTime/1000.0 << "\n\n";
    
    map <int, int> invTable;
    for (auto v : table) {
        invTable[v.second] = v.first;
    }

    ofstream ot ("output.txt", ios::binary);
    ifstream c ("output.bin", ios::binary);
    if (!ot || !c) {
        cout << "Error: Could not open the file\n";
        return 1;
    }
    
    cout << "Writing the decoded file...\n";
    BitReader bitsR;
    int sizeAfterComp = 0;
    uint8_t readingBuf;
    while (c.read(reinterpret_cast<char*>(&readingBuf), 1)) {
        sizeAfterComp++;
        bool state = bitsR.readBits(ot, readingBuf, invTable);
        if (!state) break;
    }
    clock_t decodingTime = clock() - programTime - readingFile - treeAndTableTime - encodignTime;
    cout << "Done writing the decoded file!: " << decodingTime/1000.0 << "\n\n";

    cout << string(40, '*') << '\n' << string(18, ' ') << "DONE!\n" << string(40, '*') << "\n\n";
    cout << "It took the program " << (clock() - programTime)/1000.0 << "s to zip and unzip the file " << argv[1];
    cout << " which is of size: " << (sizeBeforeComp/1024.0) << "KB before compression, and of size: " << (sizeAfterComp/1024.0) << "KB after cpmpression";
    cout << "saving " << ((sizeBeforeComp - sizeAfterComp)/ (float) sizeBeforeComp) * 100 << "%\n";
    return 0;
}

vector <TreeNode*> getCount(ifstream &f)
{
    map <int, int> counter = {};
    int buffer;
    while (buffer = f.get()) {
        if (buffer == EOF) break;
        counter[buffer]++;
    }
    
    vector <TreeNode*> vals;
    for (auto element : counter)
        vals.push_back(new TreeNode (element.first, element.second));
    return bubbleSort(vals);
}

vector <TreeNode*> bubbleSort(vector <TreeNode*> vals)
{
    int N = vals.size(), last = N;
    bool changes = 0;
    do {
        changes = 0;
        for (int i = 0; i < last - 1; i++) {
            if (vals[i]->weight < vals[i + 1]->weight) {
                swap(vals[i], vals[i + 1]);
                changes = 1;
            }
        }
        last--;
    } while (changes && last >= 1);
    return vals;
}

TreeNode* makeHuffTree(vector <TreeNode*> vals)
{
    vals.push_back(new TreeNode(-1, 1));
    while (vals.size() > 1) {
        TreeNode *smallestVal = vals.back();
        vals.pop_back();
        
        TreeNode *secondSmallestVal = vals.back();
        vals.pop_back();

        TreeNode *node = new TreeNode(-1, smallestVal->weight + secondSmallestVal->weight, smallestVal, secondSmallestVal);

        int r = vals.size() - 1, l = 0;
        while (l < r) {
            int m = l + (r - l)/2;
            if (vals[m]->weight > node->weight)
                l = m + 1;
            else 
                r = m;
        }
        vals.insert(vals.begin() + l, node);
    }
    return vals[0];
}

void makeTable (TreeNode *head, int i, map <int, int> &table)
{
    if (!head) return;
    if (!head->left && !head->right) {
            table[head->val] = i;
            return;
        }
    makeTable(head->left,  (i << 1),     table);
    makeTable(head->right, (i << 1) + 1, table);
}
