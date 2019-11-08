#ifndef CLIENT
#define CLIENT

//#include <thread>
#include "RdmaSocket.hpp"
#include "Configuration.hpp"
#include "MemoryManager.h"
#include "global.h"

using namespace std;
class Client {
private:
    Configuration *conf;
    RdmaSocket *socket;
    MemoryManager *mem;
    bool isServer;
    uint32_t taskID;
    //add:xu
    int addr_size;
    int path_length;
    uint64_t addr[MAX_ADDR_LENGTH];
    RangeInformation range_path[MAX_RANGE_LENGTH];
    uint64_t DmfsDataOffset;
    //add:e
public:
    uint64_t mm;
    Client(Configuration *conf, RdmaSocket *socket, MemoryManager *mem, uint64_t mm);
    Client();
    ~Client();
    RdmaSocket* getRdmaSocketInstance();
    Configuration* getConfInstance();
    bool RdmaCall(uint16_t DesNodeID, char *bufferSend, uint64_t lengthSend, char *bufferReceive, uint64_t lengthReceive);
    //add:xu
    bool Write(uint64_t SourceBuffer, uint64_t size,char* start_key, char* end_key);
    bool BlockAlloc(int size = MAX_ADDR_LENGTH);
    uint64_t BlockWrite(uint64_t SourceBuffer, uint64_t BufferSize = 1024*1024*2, uint32_t imm = -1, int TaskID = 0, uint16_t NodeID = 1);
    bool IndexWrite(char * start_key, char* end_key, uint64_t address);
    bool Read(uint64_t value, uint64_t size, char* start_key, char* end_key);
    bool RangeCover(char * SourceBuffer, uint64_t BufferSize = CLIENT_MESSAGE_SIZE, uint16_t DesNodeID = 1);
    virtual int GetNextRow(GeneralRequestBuffer *rec, int i, const DSMRow *&row);
    bool Cover(char* start_key, char* end_key);
    //add:e
    uint64_t ContractSendBuffer(GeneralSendBuffer *send);
};

#endif // CLIENT

