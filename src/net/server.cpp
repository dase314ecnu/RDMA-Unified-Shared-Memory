#include "server.h"

Server::Server(int _cqSize) :cqSize(_cqSize) {
    mm = 0;
    UnlockWait = false;
    conf = new Configuration();
    mem = new MemoryManager(mm, conf->getServerCount(), 2);
    mm = mem->getDmfsBaseAddress();
    Debug::notifyInfo("DmfsBaseAddress = %lx, DmfsTotalSize = %lx",
        mem->getDmfsBaseAddress(), mem->getDmfsTotalSize());
    ServerCount = conf->getServerCount();
    socket = new RdmaSocket(cqSize, mm, mem->getDmfsTotalSize(), conf, true, 0);
    Debug::notifyInfo("RdmaSocket");
    client = new Client(conf, socket, mem, (uint64_t)mm);
    Debug::notifyInfo("RPCClient");
    socket->RdmaListen();
    Debug::notifyInfo("RdmaListen");
    wk = new thread[cqSize]();
    for (int i = 0; i < cqSize; i++)
        wk[i] = thread(&Server::Worker, this, i);
}
Server::~Server() {
    Debug::notifyInfo("Stop Server.");
    delete conf;
    for (int i = 0; i < cqSize; i++) {
        wk[i].detach();
    }
    delete mem;
    delete wk;
    delete socket;
    Debug::notifyInfo("Server is closed successfully.");
}

RdmaSocket* Server::getRdmaSocketInstance() {
    return socket;
}

MemoryManager* Server::getMemoryManagerInstance() {
    return mem;
}

Client* Server::getRPCClientInstance() {
    return client;
}

void Server::Worker(int id) {
    uint32_t tid = gettid();
    // gettimeofday(&startt, NULL);
    Debug::notifyInfo("Worker %d, tid = %d", id, tid);
    th2id[tid] = id;
    mem->setID(id);
    while (true) {
        RequestPoller(id);
    }
}

void Server::RequestPoller(int id) {
    struct ibv_wc wc[1];
    uint16_t NodeID;
    uint16_t offset;
    int ret = 0, count = 0;
    uint64_t bufferRecv;
    // unsigned long diff;
    ret = socket->PollOnce(id, 1, wc);
    if (ret <= 0) {
        return;
    }
    else if (wc[0].opcode == IBV_WC_RECV_RDMA_WITH_IMM)
    {
        NodeID = wc[0].imm_data >> 20;
        if (NodeID == 0XFFF) {
            /* Unlock request, process it directly. */
            // uint64_t hashAddress = wc[0].imm_data & 0x000FFFFF;
            // fs->unlockWriteHashItem(0, 0, hashAddress);
            return;
        }
        NodeID = (uint16_t)(wc[0].imm_data << 16 >> 16);
        offset = (uint16_t)(wc[0].imm_data >> 16);
        Debug::notifyError("NodeID = %d, offset = %d", NodeID, offset);
        count += 1;
        Debug::notifyError("ServerCount%d\n",ServerCount);
        if (NodeID > 0 && NodeID <= ServerCount)
        {
            Debug::notifyError("NodeID"+NodeID);
            /* Recv Message From Other Server. */
            bufferRecv = mem->getServerRecvAddress(NodeID, offset);
        }
        else if (NodeID > ServerCount)
        {
            /* Recv Message From Client. */
            bufferRecv = mem->getClientMessageAddress(NodeID);
        }
        Debug::notifyError("bufferRecv:%lu\n",bufferRecv);
        char test[1024];
        memcpy((void *)test, (void *)bufferRecv, 1024);
        Debug::notifyError("Server%s\n",test);
        printf("bufferRecv:%lu\n",bufferRecv);

        GeneralSendBuffer *send = (GeneralSendBuffer*)bufferRecv;
        Debug::notifyError("send->message%d\n",send->message);
        memcpy((void *)test, send->path, 10);
        Debug::notifyError("Server path%s\n",test);
        switch (send->message)
        {
            case MESSAGE_MALLOC: {
                break;
            }
            case MESSAGE_SCAN: {
                break;
            }
            case MESSAGE_EXPIRE: {
                break;
            }
            default: {

                ProcessRequest(send, NodeID, offset);
            }
        }

    }
}

// add by weixing [20190327]:b
void Server::ProcessLocalRequest(GeneralReceiveBuffer *send, uint16_t NodeID, uint16_t offset, GAddr *list){
    char receiveBuffer[CLIENT_MESSAGE_SIZE];
    uint64_t bufferRecv = (uint64_t)send;
    return;
}
// add e

void Server::ProcessRequest(GeneralSendBuffer *send, uint16_t NodeID, uint16_t offset) {
    char receiveBuffer[CLIENT_MESSAGE_SIZE];
    uint64_t bufferRecv = (uint64_t)send;
    //GeneralReceiveBuffer *recv = (GeneralReceiveBuffer*)receiveBuffer;
    GeneralSendBuffer *recv = (GeneralSendBuffer*)receiveBuffer;
    recv->taskID = send->taskID;
    recv->message = MESSAGE_RESPONSE;
    memcpy(recv->path, "4321", 5);
    uint64_t size = send->sizeReceiveBuffer;
    if (send->message == MESSAGE_DISCONNECT)
    {
        //rdma->disconnect(send->sourceNodeID);
        return;
    }
    else if (send->message == MESSAGE_TEST)
    {
        //fs->parseMessage((char*)send, receiveBuffer);
        //fs->recursivereaddir("/", 0);
        Debug::notifyError("Contract Receive Buffer, size = %d.", size);
              //size -= ContractReceiveBuffer(send, recv);

              if (send->message == MESSAGE_RAWREAD)
              {
                  ExtentReadSendBuffer *bufferSend = (ExtentReadSendBuffer *)send;
                  uint64_t *value = (uint64_t *)mem->getDataAddress();
                  // printf("rawread size = %d\n", (int)bufferSend->size);
                  *value = 1;
                  socket->RdmaWrite(NodeID, mem->getDataAddress(), 2 * 4096, bufferSend->size, -1, 1);
              }
              else if (send->message == MESSAGE_RAWWRITE)
              {
                  ExtentWriteSendBuffer *bufferSend = (ExtentWriteSendBuffer *)send;
                  // printf("rawwrite size = %d\n", (int)bufferSend->size);
                  uint64_t *value = (uint64_t *)mem->getDataAddress();
                  *value = 0;
                  socket->RdmaRead(NodeID, mem->getDataAddress(), 2 * 4096, bufferSend->size, 1); // FIX ME.
                  while (*value == 0);
              }
              Debug::notifyError("Copy Reply Data, size = %d.", size);
              memcpy((void *)send, receiveBuffer, size);
              Debug::notifyError("Select Buffer.");
              if (NodeID > 0 && NodeID <= ServerCount) {
                  // Recv Message From Other Server.
                  bufferRecv = bufferRecv - mm;
              } else if (NodeID > ServerCount) {
                  // Recv Message From Client.
                  bufferRecv = 0;
              }
              Debug::notifyError("send = %lx, recv = %lx", send, bufferRecv);
                  socket->_RdmaBatchWrite(NodeID, (uint64_t)send, bufferRecv, size, 0, 1);
              // socket->_RdmaBatchReceive(NodeID, mm, 0, 2);
              socket->RdmaReceive(NodeID, mm + NodeID * 4096, 0);
              // printf("process end\n");
    }
    else if (send->message == MESSAGE_UPDATEMETA)
    {
        /* Write unlock. */
        // UpdateMetaSendBuffer *bufferSend = (UpdateMetaSendBuffer *)send;
        // fs->unlockWriteHashItem(bufferSend->key, NodeID, bufferSend->offset);
        return;
    }
    else if (send->message == MESSAGE_EXTENTREADEND)
    {
        /* Read unlock */
        // ExtentReadEndSendBuffer *bufferSend = (ExtentReadEndSendBuffer *)send;
        // fs->unlockReadHashItem(bufferSend->key, NodeID, bufferSend->offset);
        return;
    }
    else
    {
  //      fs->parseMessage((char*)send, receiveBuffer);
        // fs->recursivereaddir("/", 0);
  /*      Debug::debugItem("Contract Receive Buffer, size = %d.", size);
        size -= ContractReceiveBuffer(send, recv);

        if (send->message == MESSAGE_RAWREAD)
        {
            ExtentReadSendBuffer *bufferSend = (ExtentReadSendBuffer *)send;
            uint64_t *value = (uint64_t *)mem->getDataAddress();
            // printf("rawread size = %d\n", (int)bufferSend->size);
            *value = 1;
            socket->RdmaWrite(NodeID, mem->getDataAddress(), 2 * 4096, bufferSend->size, -1, 1);
        }
        else if (send->message == MESSAGE_RAWWRITE)
        {
            ExtentWriteSendBuffer *bufferSend = (ExtentWriteSendBuffer *)send;
            // printf("rawwrite size = %d\n", (int)bufferSend->size);
            uint64_t *value = (uint64_t *)mem->getDataAddress();
            *value = 0;
            socket->RdmaRead(NodeID, mem->getDataAddress(), 2 * 4096, bufferSend->size, 1); // FIX ME.
            while (*value == 0);
        }
        Debug::debugItem("Copy Reply Data, size = %d.", size);
        memcpy((void *)send, receiveBuffer, size);
        Debug::debugItem("Select Buffer.");
        if (NodeID > 0 && NodeID <= ServerCount) {
            // Recv Message From Other Server.
            bufferRecv = bufferRecv - mm;
        } else if (NodeID > ServerCount) {
            // Recv Message From Client.
            bufferRecv = 0;
        }
        Debug::debugItem("send = %lx, recv = %lx", send, bufferRecv);
            socket->_RdmaBatchWrite(NodeID, (uint64_t)send, bufferRecv, size, 0, 1);
        // socket->_RdmaBatchReceive(NodeID, mm, 0, 2);
        socket->RdmaReceive(NodeID, mm + NodeID * 4096, 0);
        // printf("process end\n");
   */
    }
}

int Server::getIDbyTID() {
    uint32_t tid = gettid();
    return th2id[tid];
}
uint64_t Server::ContractReceiveBuffer(GeneralSendBuffer *send, GeneralReceiveBuffer *recv) {
    uint64_t length;
    switch (send->message) {
        case MESSAGE_GETATTR: {
            GetAttributeReceiveBuffer *bufferRecv =
            (GetAttributeReceiveBuffer *)recv;
            if (bufferRecv->attribute.count >= 0 && bufferRecv->attribute.count < MAX_FILE_EXTENT_COUNT)
                length = (MAX_FILE_EXTENT_COUNT - bufferRecv->attribute.count) * sizeof(FileMetaTuple);
            else
                length = sizeof(FileMetaTuple) * MAX_FILE_EXTENT_COUNT;
            break;
        }
        case MESSAGE_READDIR: {
            ReadDirectoryReceiveBuffer *bufferRecv =
            (ReadDirectoryReceiveBuffer *)recv;
            if (bufferRecv->list.count >= 0 && bufferRecv->list.count <= MAX_DIRECTORY_COUNT)
                length = (MAX_DIRECTORY_COUNT - bufferRecv->list.count) * sizeof(DirectoryMetaTuple);
            else
                length = MAX_DIRECTORY_COUNT * sizeof(DirectoryMetaTuple);
            break;
        }
        case MESSAGE_EXTENTREAD: {
            ExtentReadReceiveBuffer *bufferRecv =
            (ExtentReadReceiveBuffer *)recv;
            if (bufferRecv->fpi.len >= 0 && bufferRecv->fpi.len <= MAX_MESSAGE_BLOCK_COUNT)
                length = (MAX_MESSAGE_BLOCK_COUNT - bufferRecv->fpi.len) * sizeof(file_pos_tuple);
            else
                length = MAX_MESSAGE_BLOCK_COUNT * sizeof(file_pos_tuple);
            break;
        }
        case MESSAGE_EXTENTWRITE: {
            ExtentWriteReceiveBuffer *bufferRecv =
            (ExtentWriteReceiveBuffer *)recv;
            if (bufferRecv->fpi.len >= 0 && bufferRecv->fpi.len <= MAX_MESSAGE_BLOCK_COUNT)
                length = (MAX_MESSAGE_BLOCK_COUNT - bufferRecv->fpi.len) * sizeof(file_pos_tuple);
            else
                length = MAX_MESSAGE_BLOCK_COUNT * sizeof(file_pos_tuple);
            break;
        }
        case MESSAGE_READDIRECTORYMETA: {
            ReadDirectoryMetaReceiveBuffer *bufferRecv =
            (ReadDirectoryMetaReceiveBuffer *)recv;
            if (bufferRecv->meta.count >= 0 && bufferRecv->meta.count <= MAX_DIRECTORY_COUNT)
                length = (MAX_DIRECTORY_COUNT - bufferRecv->meta.count) * sizeof(DirectoryMetaTuple);
            else
                length = MAX_DIRECTORY_COUNT * sizeof(DirectoryMetaTuple);
            break;
        }
        default: {
            length = 0;
            break;
        }
    }
    // printf("contract length = %d", (int)length);
    return length;
}

