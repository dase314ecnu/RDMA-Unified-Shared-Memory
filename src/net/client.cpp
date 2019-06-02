/*
 * add dhc
 * Balloc:block allock
 */
#include "client.h"
//#include "TxManager.hpp"

Client::Client(Configuration *_conf, RdmaSocket *_socket, MemoryManager *_mem, uint64_t _mm)
:conf(_conf), socket(_socket), mem(_mem), mm(_mm) {
    isServer = true;
    taskID  = 1;
    addr_size = 0;
    path_length = 0;
}

Client::Client() {
    isServer = false;
    taskID = 1;
    addr_size = 0;
    path_length = 0;
    mm = (uint64_t)malloc(sizeof(char) * (1024 * 4 + 1024 * 1024 * 4));
    conf = new Configuration();
    socket = new RdmaSocket(1, mm, (1024 * 4 + 1024 * 1024 * 4), conf, false, 0);
    socket->RdmaConnect();
    DmfsDataOffset =  CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
    DmfsDataOffset += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * conf->getServerCount();
    DmfsDataOffset += METADATA_SIZE;
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

uint64_t Client::BlockWrite(uint64_t SourceBuffer, uint64_t BufferSize, uint32_t imm, int TaskID, uint16_t NodeID){
  if(addr_size==0){
	  BAlloc(MAX_ADDR_LENGTH);
  }
  uint64_t DesBuffer = addr[addr_size-1];
  if(getRdmaSocketInstance()->RdmaWrite(NodeID, SourceBuffer, DesBuffer, BufferSize, imm, TaskID))
  {
	addr_size--;
	return addr[addr_size-1];
  }
  return -1;
}

bool Client::BlockAlloc(int size){
	if(addr_size!=0){
		return false;
	}
	uint64_t SendPoolAddr = mm + 4 * 1024;
	GeneralRequestBuffer *send = (GeneralRequestBuffer*)SendPoolAddr;
	send->message = MESSAGE_MALLOC;
	send->size = DEFAULTMALLOCSIZE;
	char value[CLIENT_MESSAGE_SIZE];
	bool fal = RdmaCall(1, (char*)send, (uint64_t)CLIENT_MESSAGE_SIZE, value, (uint64_t)CLIENT_MESSAGE_SIZE);
	if(fal){
		GeneralRequestBuffer *receive = (GeneralRequestBuffer*)value;
		if(receive->message==SUCCESS){
			addr_size = receive->size;
			for(int i=0;i<addr_size;i++){
				addr[i] = receive->addr[i];
			}
			printf("MESSAGE_MALLOC:%d,%d\n",receive->message,receive->size);
			return true;
		}
	}
	return false;
}

bool Client::IndexWrite(char * SourceBuffer, uint64_t BufferSize = CLIENT_MESSAGE_SIZE, uint16_t DesNodeID){
	char value[CLIENT_MESSAGE_SIZE];
	bool fal = RdmaCall(DesNodeID, SourceBuffer, (uint64_t)CLIENT_MESSAGE_SIZE, value, (uint64_t)CLIENT_MESSAGE_SIZE);
	if(fal){
		GeneralRequestBuffer *receive = (GeneralRequestBuffer*)value;
		if(receive->message==SUCCESS){
			printf("MESSAGE_INSERT:%d,%d\n",receive->message,receive->size);
			return true;
		}
	}
	return false;
}

bool Client::RangeCover(char * SourceBuffer, uint64_t BufferSize = CLIENT_MESSAGE_SIZE, uint16_t DesNodeID){
	char value[CLIENT_MESSAGE_SIZE];
	bool fal = RdmaCall(DesNodeID, SourceBuffer, (uint64_t)CLIENT_MESSAGE_SIZE, value, (uint64_t)CLIENT_MESSAGE_SIZE);
	if(fal){
		GeneralRequestBuffer *receive = (GeneralRequestBuffer*)value;
		if(receive->message==SUCCESS){
			addr_size = receive->size;
			for(int i=0;i<addr_size;i++){
				addr[i] = receive->addr[i];
			}
			printf("MESSAGE_INSERT:%d,%d\n",receive->message,receive->size);
			return true;
		}
	}
	return false;
}

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
            gettimeofday(&endd,NULL);
            diff = 1000000 * (endd.tv_sec - startt.tv_sec) + endd.tv_usec - startt.tv_usec;
            if (diff > 1000000) {
                Debug::notifyError("Send the Fucking Message Again.");
                ExtentWriteSendBuffer *tempsend = (ExtentWriteSendBuffer *)sendBuffer;
                tempsend->offset = (uint64_t)tempCount;
                tempCount += 1;
                socket->_RdmaBatchWrite(DesNodeID, sendBuffer, remoteRecvBuffer, lengthSend, imm, 1);
                gettimeofday(&startt,NULL);
                diff = 0;
            }
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
