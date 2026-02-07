#pragma once
#include "inc.h"

struct encodedChars {
    uint32_t n;
    uint8_t len;

    uint64_t getId () {
        
        return (static_cast <uint64_t> (len) << 32 |
                static_cast <uint64_t> (n));
    }
    bool operator() (
        const int &Len1,
        const int &Len2
    )   const {
        return Len1 > Len2;
    }
};