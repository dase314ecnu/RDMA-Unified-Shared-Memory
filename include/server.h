#ifndef SERVER
#define SERVER

#include <thread>
#include <unordered_map>
#include <vector>
#include "RdmaSocket.hpp"
#include "Configuration.hpp"
#include "client.h"
#include "MemoryManager.h"
#include "global.h"
#include "common.hpp"
//#include "filesystem.hpp"
//#include "TxManager.hpp"

using namespace std;

typedef unordered_map<uint32_t, int> Thread2ID;

typedef struct {
    uint64_t send;
    uint16_t NodeID;
    uint16_t offset;
} RPCTask;

class Server {
private:
    thread *wk;
    Configuration *conf;
    RdmaSocket *socket;
    MemoryManager *mem;
    uint64_t mm;
    Client *client;
    int ServerCount;
    int cqSize;
    Thread2ID th2id;
    vector<RPCTask*> tasks;
    bool UnlockWait;
    void Worker(int id);
    void ProcessRequest(GeneralSendBuffer *send, uint16_t NodeID, uint16_t offset);
    // add by weixing [20190327]:b
    void ProcessLocalRequest(GeneralReceiveBuffer *send, uint16_t NodeID, uint16_t offset, GAddr* list);
    // add e
public:
    Server(int cqSize);
    RdmaSocket* getRdmaSocketInstance();
    MemoryManager* getMemoryManagerInstance();
    Client* getRPCClientInstance();
    uint64_t ContractReceiveBuffer(GeneralSendBuffer *send, GeneralReceiveBuffer *recv);
    void RequestPoller(int id);
    int getIDbyTID();
    ~Server();
};

#endif // SERVER

