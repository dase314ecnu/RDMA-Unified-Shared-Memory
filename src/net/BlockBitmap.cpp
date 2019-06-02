/**
 * Create by weixing 20190330
 * The implementation of blockbitmap
 */

#include "BlockBitmap.h"
#include "debug.hpp"
#include "common.hpp"
#include <boost/thread/lock_guard.hpp>

using namespace std;

int BlockBitmap::set(uint64_t k){
    if(k > 8 * N){
        Debug::notifyError("The value %lu exceeds the total index number %lu", k, N);
        return ERROR;
    }
    bits_[k>>3] |= (0x80 >> (k & 0x07));
    return SUCCESS;
}

int BlockBitmap::clear(uint64_t k){
    if(k > 8 * N){
        Debug::notifyError("The value %lu exceeds the total index number %lu", k, N);
        return ERROR;
    }
    bits_[k>>3] &= ~(0x80 >> (k & 0x07));
    return SUCCESS;
}

int BlockBitmap::getAvailableBlocks(uint64_t num, uint64_t *lists){
    if(num > MAX_ADDR_NUM){
        Debug::notifyError("The request block %lu exceeds the MAX_ADDR_NUM %lu",num,MAX_ADDR_NUM);
        return ERROR;
    }
    boost::lock_guard<boost::detail::spinlock> lock(this->bits_lock_);
    uint64_t count_ = 0;
    int pos = 0;
    for(int i = 0; i < num; i++){
        if(!(bits_[i]^0xFF)){
            continue;
        }
        else{
            while(pos < 8){
                if(!(bits_[i] & (1 << (7-pos)))){
                    bits_[i] |= (1<<pos);
                    lists[count_] = i*8+pos;
                    pos++;
                    this->set(lists[count_]);
                    count_++;
                    if(count_ == (num - 1)){
                        boost::lock_guard<boost::detail::spinlock> unlock(this->bits_lock_);
                        return SUCCESS;
                    }
                }
            }
        }
    }
    if(count_ < num){
        Debug::notifyError("Failed to allocate enough memory blocks! request: %lu; allocated: %lu",num,count_);
        boost::lock_guard<boost::detail::spinlock> unlock(this->bits_lock_);
        return ERROR;
    }
}


