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

//     cout<<"address:"<<address<<" "<<endl;
     IndexWrite(start_key,end_key,address);
}

uint64_t Client::BlockWrite(uint64_t SourceBuffer, uint64_t BufferSize, uint32_t imm, int TaskID, uint16_t NodeID){
  if(addr_size==0){
      BlockAlloc(10); //BAlloc->BlockAlloc
//      printf("addr_size==0\n");
  }

  uint64_t DesBuffer = addr[addr_size-1];
//  cout<< "The client des addr is %ld " << DesBuffer << endl;

  //Desbuffer Contains 16-bit NodeID + 48-bit Address
  NodeID = (uint16_t)(DesBuffer >> 48);
//  cout<< "The client  NodeID  is %ld " << NodeID << endl;
  DesBuffer = DesBuffer & 0x0000FFFFFFFFFFFF;
//  cout<< "The client true addr  is %ld " << DesBuffer << endl;

  if(socket->OutboundHamal(0, SourceBuffer, NodeID, DesBuffer, BufferSize))
  //if(getRdmaSocketInstance()->RdmaWrite(NodeID, SourceBuffer, DesBuffer, BufferSize, imm, TaskID))
  {
    addr_size--;
//    printf("getRdmaSocketInstance()->RdmaWrite(NodeID, SourceBuffer, DesBuffer, BufferSize, imm, TaskID)\n");
//    printf("getRdmaSocketInstance()->RdmaWrite(%d, %d, %d, %d, %d, %d)\n",NodeID,SourceBuffer,DesBuffer, BufferSize, imm, TaskID);
    //add:modified by xr
    return addr[addr_size];
    //add:e
  }
//  printf("addr_size==0\n");
  return -1;
}

bool Client::BlockAlloc(int size){
    if(addr_size!=0){
        return false;
    }
    //choose which server. ID is random integer mod ServerCount add by qi 20191015 :b
    int server_id = 1;
    server_id = rand() % getConfInstance()->getServerCount() + 1;
//    printf("Block allocated server id is %d \n", server_id);

    char recieve_buffer[CLIENT_MESSAGE_SIZE];
    memset(recieve_buffer,0,CLIENT_MESSAGE_SIZE);
    uint64_t SendPoolAddr = mm;
    GeneralRequestBuffer *send = (GeneralRequestBuffer*)SendPoolAddr;
    send->message = MESSAGE_MALLOC;
    send->size = size;

//    cout<<"GeneralRequestBuffer_size"<<sizeof(GeneralRequestBuffer)<<endl;

    bool fal = RdmaCall(server_id, (char*)send, (uint64_t)CLIENT_MESSAGE_SIZE,recieve_buffer, (uint64_t)CLIENT_MESSAGE_SIZE);
//    RdmaCall(1, (char*)send, size, value, size);
    if(fal==1){
        GeneralRequestBuffer *receive = (GeneralRequestBuffer*)recieve_buffer;
        if(receive->message==MESSAGE_RESPONSE){
            addr_size = receive->size;
            for(int i=0;i<addr_size;i++){
                addr[i] = receive->addr[i];
//                printf("MESSAGE_MALLOC:%lu,%lu\n",addr[i],addr_size);
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
//            printf("MESSAGE_INSERT:%d,%d\n",receive->message,receive->size);
            return true;
        }
    }
    return false;
}

//bool Client::Read(uint64_t value, uint64_t size, char* start_key, char* end_key)
//{
//    char recieve[CLIENT_MESSAGE_SIZE];
//    memset(recieve,0,CLIENT_MESSAGE_SIZE);
//    uint64_t SendPoolAddr = mm;
//    GeneralRequestBuffer *send = (GeneralRequestBuffer*)SendPoolAddr;
//    send->message = MESSAGE_SCAN;
//    memcpy(send->range[0].start_key,start_key,KEY_LENGTH);
//    memcpy(send->range[0].end_key,end_key,KEY_LENGTH);
//    //send->range[0].start_key[0]='1';
//    //send->range[0].end_key[0]='5';
//    send->range[0].address = 0;
//    send->size = 0;
//    send->flag = false;
//    if(!RdmaCall(1, (char*)SendPoolAddr, (uint64_t)CLIENT_MESSAGE_SIZE, (char*)recieve, (uint64_t)CLIENT_MESSAGE_SIZE))
//    {
//        return false;
//    }
//    else
//    {
//        GeneralRequestBuffer *rec = (GeneralRequestBuffer*)recieve;
//        printf("RdmaCall:%d,%d,%lu,%ld\n",rec->message,rec->flag,rec->range[0].address,
//                rec->range[0].version);
//        if(!socket->InboundHamal(0,value,1,rec->range[0].address,size))
//        {
//            return false;
//        }
//        else
//        {
//            printf("Read:%s\n",value);
//            return true;
//        }
//    }
//    //sleep(1);
//}
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
//		printf("RdmaCall:%d,%d,%lu,%ld\n",rec->message,rec->flag,rec->range[0].address,
//				rec->range[0].version);
				
		//add:modified by xr
		//Desbuffer Contains 16-bit NodeID + 48-bit Address
		uint16_t NodeID = (uint16_t)(rec->range[0].address >> 48);
//		cout<< "The client  NodeID  is %ld " << NodeID << endl;
		uint64_t address = rec->range[0].address & 0x0000FFFFFFFFFFFF;
//		cout<<"nodeid "<<rec->range[0].address<<"address "<<address<<endl;

        if(!socket->InboundHamal(0,value,NodeID,address,size))
//        if(!socket->RemoteRead(value,NodeID,address,size))
		//add:e
		{
			return false;
		}
		else
		{
			//test for GetNextRow
			//rec->range[0].GetNextRow();
//            printf("Read:%s\n",value);
			return true;
		}
	}
//        if(Cover(start_key,end_key)){
//            printf("Read:%s\n",value);
//            return true;
//        }
//        else
//            return false;
    

    //sleep(1);
}

//test for interface Client::Cover
bool Client::Cover(char* start_key, char* end_key){
    char value[BLOCK_SIZE];
    memset(value,0,BLOCK_SIZE);
    uint64_t size = BLOCK_SIZE;

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
        if(!socket->InboundHamal(0,(uint64_t)value,1,rec->range[0].address,size))
        {
            return false;
        }
        else
        {
            printf("Range is covered!\n",value);
            return true;
        }
    }
}

int Client::GetNextRow(GeneralRequestBuffer *rec, int i, const DSMRow *&row){
//    if(Firsttime){
//        //receive  Read()
//    }
//    else{
//        //while(block_endkey == last key)
//        //BlockIterator
//    }

      DSMRow cur_row_;
      int ret = SUCCESS;
//      ret = rec->range[0].get_next_row(cur_row_);   //rec->range[i]:row_store
//      if (DSM_ITER_END == ret)
//      {
//        printf("end of iteration!\n");
//      }
//      else if (SUCCESS != ret)
//      {
//        printf("fail to get next row from row store.\n");
//      }
//      else
//      {
//        row = &cur_row_;
//      }
      return ret;
}
//add:e

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
        //拿到server分配好的地址
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
//    Debug::notifyError("sendBuffer = %lx, receiveBuffer = %lx, remoteRecvBuffer = %lx, ReceiveSize = %d",
//        sendBuffer, receiveBuffer, remoteRecvBuffer, lengthReceive);
    if (send->message == MESSAGE_DISCONNECT
        || send->message == MESSAGE_UPDATEMETA
        || send->message == MESSAGE_EXTENTREADEND) {
        // socket->_RdmaBatchWrite(DesNodeID, sendBuffer, remoteRecvBuffer, lengthSend, imm, 1);
        // socket->PollCompletion(DesNodeID, 1, &wc);
        return true;
    }
    char test[1024];
    memcpy((void *)test, send->path, 10);
//    Debug::notifyError("Client path%s\n",test);
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
