#ifndef BLOCKBITMAP_H
#define BLOCKBITMAP_H
/**
 *  Created by Weixing
 *  @date 2019-03-29
 *  Manager the memory blocks
 *
 */

#pragma warning(disable : 4996 4800)
#include <memory.h>
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <boost/smart_ptr/detail/spinlock.hpp>


class BlockBitmap {
private:
    char* bits_;
    int N;
    int offset_;
    //boost::detail::spinlock bits_lock_;
protected:
    void init(uint64_t n){
        N = n/8 + 1;
        bits_ = (char*)malloc(N);    // allocated memory
        memset(bits_,0,N);  // init the bitsets
        offset_ = 0;
    }
public:
    BlockBitmap(uint64_t num){
        init(num);
    }
    ~BlockBitmap(){
        delete [] bits_;
        bits_ = NULL;
    }
    int set(uint64_t k);

    int clear(uint64_t k);

    int getAvailableBlocks(uint64_t num, uint64_t* lists);
};

#endif // BLOCKBITMAP_H
