#include <algorithm>
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

        void trv(TreeNode *head) {
            if (head == nullptr) {
                cout << "Null\n";
                return;
            }
            string c = ""; 
            if (head->val != -1) c = (char) head->val;
            cout << head->val << ' ' << c << " : " << head->weight << endl;
            cout << "left: ";
            trv(head->left);
            cout << "right: ";
            trv(head->right);
            return;
        }
        void delTreeNode(TreeNode *head) {
            if (head == nullptr) return;
            delTreeNode(head->left);
            delTreeNode(head->right);
            delete head;
        }

    private:
};

class BitWriter {
    private:
        string bits;
        int lengthOfBits;
        uint8_t buffer;
        int bufferSizeInBytes;
        int bufferSizeInBits;
    public:
        BitWriter () {
            bits = "";
            lengthOfBits = 0;
            buffer = 0;
            bufferSizeInBytes = sizeof(buffer);
            bufferSizeInBits = bufferSizeInBytes * 8;
        }

        void bitStorer (ofstream &f, string s) {
            bits += s;
            lengthOfBits += s.size();
            while (lengthOfBits >= bufferSizeInBits) {
                bitWriter(f);
            }
        }

        void bitWriter (ofstream &f) {
            for (int i = 0; i < bufferSizeInBits; i++) {
                buffer <<= 1;
                if (bits[i] == '1') buffer++;
            }
            f.write((reinterpret_cast<char*> (&buffer)), bufferSizeInBytes);

            bits.erase(bits.begin(), bits.begin() + bufferSizeInBits);
            buffer = 0;
            lengthOfBits -= bufferSizeInBits;
        }

        void flushBitWriter (ofstream &f) {
            if (lengthOfBits > 0) {
                for (int i = 0; i < lengthOfBits; i++) {
                    buffer <<= 1;
                    if (bits[i] == '1') buffer++;
                }
                buffer <<= (bufferSizeInBits - lengthOfBits);
                f.write((reinterpret_cast<char*> (&buffer)), bufferSizeInBytes);

                bits = "";
                lengthOfBits = 0;
                buffer = 0;
            }
        }
        bool bitReader (ofstream &f, uint8_t chunk, map <string, int> &table) {
            for (int i = 0; i < bufferSizeInBits; i++) {
                uint8_t tChunk = (chunk << i);
                tChunk = (tChunk >> (bufferSizeInBits - 1));
                char c = '0' + tChunk;
                bits += c;
                if ((table.find(bits) != table.end())) {
                    if (table[bits] == -1) return false;
                    char ch = table[bits];
                    f.write(&ch, 1);
                    bits = "";
                }
            }
            return true;
        }
};



vector <TreeNode*> getCount(ifstream &f);
vector <TreeNode*> bubbleSort(vector <TreeNode*> vals);
TreeNode* makeHuffTree(vector <TreeNode*> vals);
void makeTable (TreeNode *head, string s, map<int, string> &table);


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
    
    vector <TreeNode*> vals = getCount(f);
    f.close();
    TreeNode *head = makeHuffTree(vals);

    map <int, string> table = {};
    makeTable(head, "", table);
    head->delTreeNode(head);

    ofstream o ("output.bin", ios::binary);
    

    ifstream d(argv[1], ios::binary);
    // ifstream d("test.txt", ios::binary);
    if (!d || !o) {
        cout << "Error: Could not read the file\n";
        return 1;
    }

    
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
    
    map <string, int> invTable;
    for (auto v : table) {
        invTable[v.second] = v.first;
        cout << v.second << " : " << v.first << endl;
    }

    ofstream ot ("output.txt", ios::binary);
    ifstream c ("output.bin", ios::binary);
    if (!ot || !c) {
        cout << "Error: Could not open the file\n";
        return 1;
    }
    
    int sizeAfterComp = 0;
    uint8_t readingBuf;
    while (true) {
        sizeAfterComp++;
        readingBuf = c.get();
        bool state = bits.bitReader(ot, readingBuf, invTable);
        if (!state) break;
    }

    cout << "It took the program " << (clock() - programTime)/1000 << "s to zip and unzip the file " << argv[1];
    cout << " which is of size: " << (sizeBeforeComp/1024.0)/1024.0 << "MB before compression, and of size: " << (sizeAfterComp/1024.0)/1024.0 << "MB after cpmpression";
    cout << "saving " << ((sizeBeforeComp - sizeAfterComp)/ (float) sizeBeforeComp) * 100 << "%\n";
    return 0;
}

vector <TreeNode*> getCount(ifstream &f)
{
    map <int, int> counter = {};
    int buffer;
    while (true) {
        buffer = f.get();
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

void makeTable (TreeNode *head, string s, map<int, string> &table)
{
    if (!head) return;
    if (!head->left && !head->right) {
            table[head->val] = s;
            return;
        }
    makeTable(head->left,  s + '0', table);
    makeTable(head->right, s + '1', table);
}
