#pragma once
#include "inc.h"

struct encodedChars {
    uint32_t n;
    uint8_t len;
    int c;

    uint64_t getId () {
        
        return (static_cast <uint64_t> (len) << 32 |
                static_cast <uint64_t> (n));
    }
};