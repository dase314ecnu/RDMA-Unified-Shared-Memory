#include "MemoryManager.h"

MemoryManager::MemoryManager(uint64_t _mm, uint64_t _ServerCount,  int _DataSize)
: ServerCount(_ServerCount), MemoryBaseAddress(_mm) {
    void *shmptr;
    if (_mm == 0) {
        /* Open Shared Memory. */
        /* Add Data Storage. */
        DMFSTotalSize = (uint64_t)_DataSize;
        DMFSTotalSize = DMFSTotalSize * 1024 * 1024;
        //DMFSTotalSize = DMFSTotalSize * 1024; //deleted by weixing
        /* Add Metadata Storage. */
        DMFSTotalSize += METADATA_SIZE;
        /* Add Client Message Pool. */
        DMFSTotalSize += CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
        /* Add Server Message Pool. */
        DMFSTotalSize += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * ServerCount;
        Debug::notifyError("total_size=%lu",DMFSTotalSize + LOCALLOGSIZE + DISTRIBUTEDLOGSIZE);
        shmid = shmget(SHARE_MEMORY_KEY, DMFSTotalSize + LOCALLOGSIZE + DISTRIBUTEDLOGSIZE, IPC_CREAT);
        //shmid = shmget(SHARE_MEMORY_KEY,2048, IPC_CREAT);
        if (shmid == -1) {
            Debug::notifyError("21::shmget error");
        }
        shmptr = shmat(shmid, 0, 0);
        Debug::notifyError("shmptr = %p",shmptr);
        if (shmptr == (void *)(-1)) {
            Debug::notifyError("25::shmat error");
        }
        MemoryBaseAddress = (uint64_t)shmptr;
        memset((void *)MemoryBaseAddress, '\0', DMFSTotalSize + LOCALLOGSIZE + DISTRIBUTEDLOGSIZE);

        //memset((void *)MemoryBaseAddress, '\0', 1024);
    }
    ClientBaseAddress = MemoryBaseAddress;
    ServerSendBaseAddress = MemoryBaseAddress + CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
    ServerRecvBaseAddress = ServerSendBaseAddress + SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * ServerCount;
    MetadataBaseAddress = ServerRecvBaseAddress + SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * ServerCount;
    DataBaseAddress = ServerRecvBaseAddress + METADATA_SIZE;
    memset((void *)DataBaseAddress, 'f', _DataSize*1024*1024);
    //deleted by weixing [20190329]:b
//    LocalLogAddress = _DataSize;
//    LocalLogAddress *= (1024);
//    LocalLogAddress += DataBaseAddress;
//    Debug::debugItem("LocalLogAddress = %lx", LocalLogAddress);
//    DistributedLogAddress = LocalLogAddress + LOCALLOGSIZE;
    //delete e
    SendPoolPointer = (uint8_t *)malloc(sizeof(uint8_t) * ServerCount);
    memset((void *)SendPoolPointer, '\0', sizeof(uint8_t) * ServerCount);
    // add by weixing [20190330]:b
    uint64_t num = ((uint64_t)_DataSize * 1024 * 1024)/BLOCK_SIZE;
    Debug::notifyError("Allocated Num:%lu",num);
    blockBitmap = new BlockBitmap(num);
    Debug::notifyError("Allocated Num:%lu",num);
    // add e
}

MemoryManager::~MemoryManager() {
    Debug::notifyInfo("Stop MemoryManager.");
    free(SendPoolPointer);
    shmctl(shmid, IPC_RMID , 0);
    Debug::notifyInfo("MemoryManager is closed successfully.");
}

uint64_t MemoryManager::getDmfsBaseAddress() {
    return MemoryBaseAddress;
}

uint64_t MemoryManager::getDmfsTotalSize() {
    return DMFSTotalSize;
}

uint64_t MemoryManager::getMetadataBaseAddress() {
    return MetadataBaseAddress;
}

uint64_t MemoryManager::getDataAddress() {
    return DataBaseAddress;
}

uint64_t MemoryManager::getServerSendAddress(uint16_t NodeID, uint64_t *buffer) {
    // uint8_t temp = __sync_fetch_and_add( &SendPoolPointer[NodeID - 1], 1 );
    // temp = temp % SERVER_MASSAGE_NUM;
    uint32_t tid = gettid();
    uint64_t offset = (uint64_t)th2id[tid];
    *buffer = (ServerSendBaseAddress +
        (NodeID - 1) * SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM
        + offset * SERVER_MASSAGE_SIZE);
    return offset;
}

uint64_t MemoryManager::getServerRecvAddress(uint16_t NodeID, uint16_t offset) {
    uint64_t buffer;
    buffer = (ServerRecvBaseAddress +
        (NodeID - 1) * SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM
        + offset * SERVER_MASSAGE_SIZE);
    return buffer;
}

uint64_t MemoryManager::getClientMessageAddress(uint16_t NodeID) {
    return ClientBaseAddress + (NodeID - ServerCount  - 1) * CLIENT_MESSAGE_SIZE;
}

uint64_t MemoryManager::getLocalLogAddress() {
    return LocalLogAddress;
}

uint64_t MemoryManager::getDistributedLogAddress() {
    return DistributedLogAddress;
}

void MemoryManager::setID(int ID) {
    uint32_t tid = gettid();
    th2id[tid] = ID;
}

// add by weixing [20190329]:b
int MemoryManager::allocateMemoryBlocks(uint64_t num, GAddr *addrList){
    if(num > MAX_ADDR_NUM){
        Debug::notifyError("The requested num %lu exceeds the MAX_ADDR_NUM %lu",num,MAX_ADDR_NUM);
        return ERROR;
    }
    if(NULL == blockBitmap){
        Debug::notifyError("Failed to allocate memory blocks! BB_ is Nullptr!");
        return ERROR;
    }else if(NULL != blockBitmap){
        if(SUCCESS != (blockBitmap->getAvailableBlocks(num,addrList))){
            Debug::notifyError("Failed to get available blocks!");
            return ERROR;
        }else{
            //printf("Get All Addresses!\n");
            for(uint64_t i = 0; i < num; i++){
                addrList[i] = DataBaseAddress + addrList[i] * BLOCK_SIZE;
//                uint64_t flag = 0xFFFFFF;

//                addrList[i] = ;

            }
            return SUCCESS;
        }

    }else{
        return ERROR;
    }
}

int MemoryManager::allocateMemoryBlocks(uint16_t NodeID, uint64_t num, GAddr *addrList){
    if(num > MAX_ADDR_NUM){
        Debug::notifyError("The requested num %lu exceeds the MAX_ADDR_NUM %lu",num,MAX_ADDR_NUM);
        return ERROR;
    }
    if(NULL == blockBitmap){
        Debug::notifyError("Failed to allocate memory blocks! BB_ is Nullptr!");
        return ERROR;
    }else if(NULL != blockBitmap){
        if(SUCCESS != (blockBitmap->getAvailableBlocks(num,addrList))){
            Debug::notifyError("Failed to get available blocks!");
            return ERROR;
        }else{
            //printf("Get All Addresses!\n");
            for(uint64_t i = 0; i < num; i++){
                GAddr temp = DataBaseAddress + addrList[i] * BLOCK_SIZE;

                printf("The temp address is  %ld!\n", temp);

                temp = temp & 0xFFFFFFFFFFFFFFFF;
                uint64_t flag = (uint64_t)NodeID << 48 | 0x0000FFFFFFFFFFFF & temp;
                printf("The NodeID is 2 %ld!\n", flag >> 48);
                addrList[i] = flag;
                printf("The address is 3 %ld!\n", addrList[i]);

//                int addr = flag >> 48;
//                printf("The nodeid is 4 %ld!\n", addr);

//                printf("The nodeid is 5 %ld!\n", flag & 0x0000FFFFFFFFFFFF);

            }
            return SUCCESS;
        }

    }else{
        return ERROR;
    }

}
int MemoryManager::freeMemoryBlocks(uint64_t num, GAddr *addrList){
    if(NULL == blockBitmap){
        Debug::notifyError("Failed to free up memory blocks! BB_ is Nullptr!");
        return ERROR;
    }else if(0 == num || NULL ==  addrList){
        Debug::notifyError("Failed to free up memory blocks! Parameter is NULL!");
        return ERROR;
    }else{
        bool tag = false;
        for(uint64_t i = 0; i < num; i++){
            if(SUCCESS != (blockBitmap->clear(addrList[i]))){
                Debug::notifyError("Failed to free up memory block:%lu",addrList[i]);
                tag = true;
                continue;
            }
        }
        if(tag){
            return ERROR;
        }else{
            return SUCCESS;
        }
    }
}
// add e




