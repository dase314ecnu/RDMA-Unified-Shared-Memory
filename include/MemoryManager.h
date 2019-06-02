#ifndef MEMORYMANAGER
#define MEMORYMANAGER

#include <unordered_map>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <debug.hpp>
#include "global.h"
#include "common.hpp"
#include "BlockBitmap.h"
#define SHARE_MEMORY_KEY 78

typedef unordered_map<uint32_t, int> Thread2ID;
class MemoryManager {
private:
    uint64_t ServerCount;
    uint64_t MemoryBaseAddress;
    uint64_t ClientBaseAddress;
    uint64_t ServerSendBaseAddress;
    uint64_t ServerRecvBaseAddress;
    uint64_t MetadataBaseAddress;
    uint64_t DataBaseAddress;
    uint8_t *SendPoolPointer;
    uint64_t DMFSTotalSize;
    uint64_t LocalLogAddress;
    uint64_t DistributedLogAddress;
    uint64_t BlockSize; // the default conf added by weixing
    int shmid;
    Thread2ID th2id;
    BlockBitmap *BB_;   // the bitmap that record the usage of memory allocation

public:
    MemoryManager(uint64_t mm, uint64_t ServerCount, int DataSize);
    ~MemoryManager();
    int allocateMemoryBlocks(uint64_t num, GAddr* addrList); //allocate memory blocks added by weixing [20190329]
    int freeMemoryBlocks(uint64_t num, GAddr* addrList);    // free up memory blocks added by weixing [20190331]
    uint64_t getDmfsBaseAddress();
    uint64_t getDmfsTotalSize();
    uint64_t getMetadataBaseAddress();
    uint64_t getDataAddress();
    uint64_t getServerSendAddress(uint16_t NodeID, uint64_t *buffer);
    uint64_t getServerRecvAddress(uint16_t NodeID, uint16_t offset);
    uint64_t getClientMessageAddress(uint16_t NodeID);
    uint64_t getLocalLogAddress();
    uint64_t getDistributedLogAddress();
    void setID(int ID);
};



#endif // MEMORYMANAGER

