#pragma once
#include "inc.h"

class TreeNode {
    public:
        uint8_t charValueInInt;
        int weightOfChar; 
        /* 
        the first one is simply the ascii value of the character, 
        with the exception of the EOF which is hard-codded to -1.
        for now, this is actually a char, but in the future it'll be a
        string of bits that reperesent any data.

        the second one is the number of times the character was used in the file.
        */

        std::shared_ptr <TreeNode> right;
        std::shared_ptr <TreeNode> left;
        // for the Huffman tree

        // the constructer:
        TreeNode (uint8_t v, int w, 
            std::shared_ptr <TreeNode> l = nullptr, 
            std::shared_ptr <TreeNode> r = nullptr) {

            charValueInInt = v;
            weightOfChar = w;

            left  = std::move(l);
            right = std::move(r);
        }
        // there's no need for destructer since we are using smart pointers
    
        void trv(std::shared_ptr <TreeNode> head) {
            if (!head) {
                std::cout << "Null\n";
                return;
            }
            std::cout << (int) head->charValueInInt;
            std::cout << "\nLeft: ";
            head->trv(head->left);
            std::cout << "Right: ";
            head->trv(head->right);
        }
};
