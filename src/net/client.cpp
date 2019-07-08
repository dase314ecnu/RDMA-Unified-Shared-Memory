#include "client.h"
//#include "TxManager.hpp"

Client::Client(Configuration *_conf, RdmaSocket *_socket, MemoryManager *_mem, uint64_t _mm)
:conf(_conf), socket(_socket), mem(_mem), mm(_mm) {
    isServer = true;
    taskID  = 1;
    //add:xurui
    addr_size = 0;
    path_length = 0;
    //add:e
}

Client::Client() {
    isServer = false;
    taskID = 1;
    //add:xurui
    addr_size = 0;
    path_length = 0;
    //add:e
    mm = (uint64_t)malloc(sizeof(char) * (1024 * 4 + 1024 * 1024 * 4));
    conf = new Configuration();
    socket = new RdmaSocket(1, mm, (1024 * 4 + 1024 * 1024 * 4), conf, false, 0);
    socket->RdmaConnect();
    //add:xurui
    DmfsDataOffset =  CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
    DmfsDataOffset += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * conf->getServerCount();
    DmfsDataOffset += METADATA_SIZE;
    //add:e
}

Client::~Client() {
    Debug::notifyInfo("Stop Client.");
    if (!isServer) {
        delete conf;
        delete socket;
        free((void *)mm);
    }
    Debug::notifyInfo("Client is closed successfully.");
}

RdmaSocket* Client::getRdmaSocketInstance() {
    return socket;
}

Configuration* Client::getConfInstance() {
    return conf;
}

//add:xurui
bool Client::Write(uint64_t SourceBuffer, uint64_t size, char* start_key, char* end_key)
{
     uint64_t address = BlockWrite(SourceBuffer,size,-1,0,1);

     cout<<"address:"<<address<<" "<<endl;
     IndexWrite(start_key,end_key,address);
}

uint64_t Client::BlockWrite(uint64_t SourceBuffer, uint64_t BufferSize, uint32_t imm, int TaskID, uint16_t NodeID){
  if(addr_size==0){
      BlockAlloc(10); //BAlloc->BlockAlloc
      printf("addr_size==0\n");
  }

  uint64_t DesBuffer = addr[addr_size-1];
  if(socket->OutboundHamal(0, SourceBuffer, NodeID, DesBuffer, BufferSize))
  //if(getRdmaSocketInstance()->RdmaWrite(NodeID, SourceBuffer, DesBuffer, BufferSize, imm, TaskID))
  {
    addr_size--;
    printf("getRdmaSocketInstance()->RdmaWrite(NodeID, SourceBuffer, DesBuffer, BufferSize, imm, TaskID)\n");
    return DesBuffer;
  }
  printf("addr_size==0\n");
  return -1;
}

bool Client::BlockAlloc(int size){
    if(addr_size!=0){
        return false;
    }

    char recieve_buffer[CLIENT_MESSAGE_SIZE];
    memset(recieve_buffer,0,CLIENT_MESSAGE_SIZE);
    uint64_t SendPoolAddr = mm;
    GeneralRequestBuffer *send = (GeneralRequestBuffer*)SendPoolAddr;
    send->message = MESSAGE_MALLOC;
    send->size = size;

    cout<<"GeneralRequestBuffer_size"<<sizeof(GeneralRequestBuffer)<<endl;

    bool fal = RdmaCall(1, (char*)send, (uint64_t)CLIENT_MESSAGE_SIZE,recieve_buffer, (uint64_t)CLIENT_MESSAGE_SIZE);
//    RdmaCall(1, (char*)send, size, value, size);
    if(fal==1){
        GeneralRequestBuffer *receive = (GeneralRequestBuffer*)recieve_buffer;
        if(receive->message==MESSAGE_RESPONSE){
            addr_size = receive->size;
            for(int i=0;i<addr_size;i++){
                addr[i] = receive->addr[i];
                printf("MESSAGE_MALLOC:%lu,%lu\n",addr[i],addr_size);
            }

            return true;
        }
    }
    return false;
}

bool Client::IndexWrite(char * start_key, char* end_key, uint64_t address){//uint64_t BufferSize = CLIENT_MESSAGE_SIZE->uint64_t BufferSize
    char recieve_buffer[CLIENT_MESSAGE_SIZE];
    uint64_t SendPoolAddr = mm;
    GeneralRequestBuffer *send = (GeneralRequestBuffer*)SendPoolAddr;
    send->message = MESSAGE_INSERT;
    memcpy(send->range[0].start_key,start_key,KEY_LENGTH);
    memcpy(send->range[0].end_key,end_key,KEY_LENGTH);
    //send->range[0].start_key[0]='1';
    //send->range[0].end_key[0]='5';
    send->range[0].address = address;
    send->size = 1;
    send->flag = false;
    bool fal = RdmaCall(1, (char*)SendPoolAddr, (uint64_t)CLIENT_MESSAGE_SIZE, recieve_buffer, (uint64_t)CLIENT_MESSAGE_SIZE);
    if(fal){
        GeneralRequestBuffer *receive = (GeneralRequestBuffer*)recieve_buffer;
        if(receive->message==SUCCESS){
            printf("MESSAGE_INSERT:%d,%d\n",receive->message,receive->size);
            return true;
        }
    }
    return false;
}

bool Client::Read(uint64_t value, uint64_t size, char* start_key, char* end_key)
{
    char recieve[CLIENT_MESSAGE_SIZE];
    memset(recieve,0,CLIENT_MESSAGE_SIZE);
    uint64_t SendPoolAddr = mm;
    GeneralRequestBuffer *send = (GeneralRequestBuffer*)SendPoolAddr;
    send->message = MESSAGE_SCAN;
    memcpy(send->range[0].start_key,start_key,KEY_LENGTH);
    memcpy(send->range[0].end_key,end_key,KEY_LENGTH);
    //send->range[0].start_key[0]='1';
    //send->range[0].end_key[0]='5';
    send->range[0].address = 0;
    send->size = 0;
    send->flag = false;
    if(!RdmaCall(1, (char*)SendPoolAddr, (uint64_t)CLIENT_MESSAGE_SIZE, (char*)recieve, (uint64_t)CLIENT_MESSAGE_SIZE))
    {
        return false;
    }
    else
    {
        GeneralRequestBuffer *rec = (GeneralRequestBuffer*)recieve;
        printf("RdmaCall:%d,%d,%lu,%ld\n",rec->message,rec->flag,rec->range[0].address,
                rec->range[0].version);
        if(!socket->InboundHamal(0,value,1,rec->range[0].address,size))
        {
            return false;
        }
        else
        {
            //printf("Read:%s\n",value);
            return true;
        }
    }
    //sleep(1);
}
//add:e

//bool Client::RdmaCall(uint16_t DesNodeID, char *bufferSend, uint64_t lengthSend, char *bufferReceive, uint64_t lengthReceive) {
//    uint32_t ID = __sync_fetch_and_add( &taskID, 1 ), temp;
//    uint64_t sendBuffer, receiveBuffer, remoteRecvBuffer;
//    uint16_t offset = 0;
//    uint32_t imm = (uint32_t)socket->getNodeID();
//    struct  timeval startt, endd;
//    unsigned long diff, tempCount = 0;
//    GeneralRequestBuffer *send = (GeneralRequestBuffer*)bufferSend;
//    lengthReceive -= ContractSendBuffer(send);
//    send->taskID = ID;
//    send->sourceNodeID = socket->getNodeID();
//    send->sizeReceiveBuffer = lengthReceive;
//    if (isServer) {
//        offset = mem->getServerSendAddress(DesNodeID, &sendBuffer);
//        // printf("offset = %d\n", offset);
//        receiveBuffer = mem->getServerRecvAddress(socket->getNodeID(), offset);
//        remoteRecvBuffer = receiveBuffer - mm;
//    } else {
//        sendBuffer = mm;
//        receiveBuffer = mm;
//        remoteRecvBuffer = (socket->getNodeID() - conf->getServerCount() - 1) * CLIENT_MESSAGE_SIZE;
//    }
//    GeneralRequestBuffer *recv = (GeneralRequestBuffer*)receiveBuffer;
//    if (isServer)
//        recv->message = MESSAGE_INVALID;
//    memcpy((void *)sendBuffer, (void *)bufferSend, lengthSend);
//    _mm_clflush(recv);
//    asm volatile ("sfence\n" : : );
//    temp = (uint32_t)offset;
//    imm = imm + (temp << 16);
//    Debug::notifyError("sendBuffer = %lx, receiveBuffer = %lx, remoteRecvBuffer = %lx, ReceiveSize = %d",
//        sendBuffer, receiveBuffer, remoteRecvBuffer, lengthReceive);

//    socket->_RdmaBatchWrite(DesNodeID, sendBuffer, remoteRecvBuffer, lengthSend, imm, 1);
//    if (isServer) {
//        while (recv->message == MESSAGE_INVALID || recv->message != MESSAGE_RESPONSE)
//            ;
//    } else {
////        gettimeofday(&startt,NULL);
//        while (recv->message != MESSAGE_RESPONSE) {
////            gettimeofday(&endd,NULL);
////            diff = 1000000 * (endd.tv_sec - startt.tv_sec) + endd.tv_usec - startt.tv_usec;
////            if (diff > 1000000) {
////                Debug::notifyError("Send the Fucking Message Again.");
////                ExtentWriteSendBuffer *tempsend = (ExtentWriteSendBuffer *)sendBuffer;
////                tempsend->offset = (uint64_t)tempCount;
////                tempCount += 1;
////                socket->_RdmaBatchWrite(DesNodeID, sendBuffer, remoteRecvBuffer, lengthSend, imm, 1);
////                gettimeofday(&startt,NULL);
////                diff = 0;
////            }
//        }
//    }


//    memcpy((void*)bufferReceive, (void *)receiveBuffer, lengthReceive);
//    return true;
//}

bool Client::RdmaCall(uint16_t DesNodeID, char *bufferSend, uint64_t lengthSend, char *bufferReceive, uint64_t lengthReceive) {
    uint32_t ID = __sync_fetch_and_add( &taskID, 1 ), temp;
    uint64_t sendBuffer, receiveBuffer, remoteRecvBuffer;
    uint16_t offset = 0;
    uint32_t imm = (uint32_t)socket->getNodeID();
    struct  timeval startt, endd;
    unsigned long diff, tempCount = 0;
    GeneralSendBuffer *send = (GeneralSendBuffer*)bufferSend;
    lengthReceive -= ContractSendBuffer(send);
    send->taskID = ID;
    send->sourceNodeID = socket->getNodeID();
    send->sizeReceiveBuffer = lengthReceive;
    if (isServer) {
        offset = mem->getServerSendAddress(DesNodeID, &sendBuffer);
        // printf("offset = %d\n", offset);
        receiveBuffer = mem->getServerRecvAddress(socket->getNodeID(), offset);
        remoteRecvBuffer = receiveBuffer - mm;
    } else {
        sendBuffer = mm;
        receiveBuffer = mm;
        remoteRecvBuffer = (socket->getNodeID() - conf->getServerCount() - 1) * CLIENT_MESSAGE_SIZE;
    }
    GeneralReceiveBuffer *recv = (GeneralReceiveBuffer*)receiveBuffer;
    if (isServer)
        recv->message = MESSAGE_INVALID;
    memcpy((void *)sendBuffer, (void *)bufferSend, lengthSend);
    _mm_clflush(recv);
    asm volatile ("sfence\n" : : );
    temp = (uint32_t)offset;
    imm = imm + (temp << 16);
    Debug::notifyError("sendBuffer = %lx, receiveBuffer = %lx, remoteRecvBuffer = %lx, ReceiveSize = %d",
        sendBuffer, receiveBuffer, remoteRecvBuffer, lengthReceive);
    if (send->message == MESSAGE_DISCONNECT
        || send->message == MESSAGE_UPDATEMETA
        || send->message == MESSAGE_EXTENTREADEND) {
        // socket->_RdmaBatchWrite(DesNodeID, sendBuffer, remoteRecvBuffer, lengthSend, imm, 1);
        // socket->PollCompletion(DesNodeID, 1, &wc);
        return true;
    }
    char test[1024];
    memcpy((void *)test, send->path, 10);
    Debug::notifyError("Client path%s\n",test);
    socket->_RdmaBatchWrite(DesNodeID, sendBuffer, remoteRecvBuffer, lengthSend, imm, 1);
    if (isServer) {
        while (recv->message == MESSAGE_INVALID || recv->message != MESSAGE_RESPONSE)
            ;
    } else {
        gettimeofday(&startt,NULL);
        while (recv->message != MESSAGE_RESPONSE) {
 /*           gettimeofday(&endd,NULL);
            diff = 1000000 * (endd.tv_sec - startt.tv_sec) + endd.tv_usec - startt.tv_usec;
            if (diff > 1000000) {
                Debug::notifyError("Send the Fucking Message Again.");
                ExtentWriteSendBuffer *tempsend = (ExtentWriteSendBuffer *)sendBuffer;
                tempsend->offset = (uint64_t)tempCount;
                tempCount += 1;
                socket->_RdmaBatchWrite(DesNodeID, sendBuffer, remoteRecvBuffer, lengthSend, imm, 1);
                gettimeofday(&startt,NULL);
                diff = 0;*
            }*/
        }
    }
    memcpy((void*)bufferReceive, (void *)receiveBuffer, lengthReceive);
    return true;
}

uint64_t Client::ContractSendBuffer(GeneralSendBuffer *send) {
    uint64_t length = 0;
    switch (send->message) {
        case MESSAGE_MKNODWITHMETA: {
            MakeNodeWithMetaSendBuffer *bufferSend =
                    (MakeNodeWithMetaSendBuffer *)send;
                    length = (MAX_FILE_EXTENT_COUNT - bufferSend->metaFile.size) * sizeof(FileMetaTuple);
            length = 0;
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
