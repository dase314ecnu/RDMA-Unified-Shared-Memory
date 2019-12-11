
#include "client.h"


int main()
{
////  1.basic testcase
//    Client *client = new Client();
////    uint64_t DmfsDataOffset;
////    DmfsDataOffset =  CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
////    DmfsDataOffset += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * client->getConfInstance()->getServerCount();
////    DmfsDataOffset += METADATA_SIZE;

//    char value[BLOCK_SIZE];
//    memset(value,0,BLOCK_SIZE);

//    //write value
//    char alphavalue[BLOCK_SIZE];
//    memset(alphavalue,'b',BLOCK_SIZE);
//    char start_key[KEY_LENGTH];
//    char end_key[KEY_LENGTH];
//    int* start = (int*)(start_key);
//    int* end = (int*)(end_key);

//    for(int i=0; i<1;i++)
//    {
//        *start = i;
//        *end = i+5;
//        printf("start_key：%s ",start_key);
//        printf("end_key：%s ",end_key);
//        client->Write((uint64_t)alphavalue,BLOCK_SIZE,start_key,end_key);
//    }


//    for(int i=0; i<1;i++)
//    {
//        *start = i;
//        *end = i+5;
//        client->Read((uint64_t)value,BLOCK_SIZE,start_key,end_key);
//    }

//    printf("value:\n");
//    for(int i=0; i<BLOCK_SIZE; i++)
//    {
//        printf("%c",value[i]);
//    }

//    return 0;
//}
    //2. 2M read testcase
//    Client *client = new Client();
////    uint64_t DmfsDataOffset;
////    DmfsDataOffset =  CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
////    DmfsDataOffset += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * client->getConfInstance()->getServerCount();
////    DmfsDataOffset += METADATA_SIZE;

//    char value[BLOCK_SIZE];
//    memset(value,0,BLOCK_SIZE);

//    //2.write value
//    char alphavalue[BLOCK_SIZE];
////    memset(alphavalue,'b',BLOCK_SIZE);
//    //printf("Read:%s\n",alphavalue);
//    //memcpy((void *)SendPoolAddr, (void *)(alphavalue), BLOCK_SIZE);
//    char start_key[KEY_LENGTH];
//    char end_key[KEY_LENGTH];
//    int* start = (int*)(start_key);
//    int* end = (int*)(end_key);
//    timeval tstart,tend;
////    int test_size = 1*1024*1024;
//    int test_size = 2*1024*1024;


////    int m =65;
////    for(int j=0;j<1024;j++)
////    {
////        for(int i=0; i<2048; i++)
////        {
////            alphavalue[i+j*2048] = m;
//////            printf("%c ",alphavalue[i]);
////        }
////        m++;
////    }

////    sleep(1);
//    while(true)
//    {
//        for(int i=0; i<1;i++)
//        {
//    //        Client *client = new Client();
//            //start_key[0] = 'b';
//            //end_key[0] = 'e';
//            printf("start_key：%s ",start_key);
//            printf("end_key：%s ",end_key);
//            clock_t begin = clock();
//            client->Write((uint64_t)alphavalue,test_size,start_key,end_key);
//            clock_t end = clock();
//            printf("write %d bytes in %d us\n", test_size, (int)(end-begin));
//        }
//    //    sleep(2);

//        for(int i=0; i<1;i++)
//        {
//    //        Client *client = new Client();
//            //start_key[0] = 'b';
//            //end_key[0] = 'e';
//            memset(value,0,BLOCK_SIZE);
//            gettimeofday(&tstart,NULL);
//    //        clock_t begin = clock();
//            client->Read((uint64_t)value,test_size,start_key,end_key);
//    //        clock_t end = clock();
//            gettimeofday(&tend,NULL);
//            cout << "read "<< test_size << " bytes in " << tend.tv_sec-tstart.tv_sec << "s" << tend.tv_usec-tstart.tv_usec<<"us"<<endl;
//    //        printf("read %d bytes in %d us\n", test_size, (int)(end-begin));
//            if(memcmp(alphavalue, value, test_size) == 0) {
//                        printf("read success, the same\n");
//            }
//            else {
//                int same_size = 0;
//                int first_not_same = 0;
//                int last_not_same = 0;
//                //for(same_size = 0; same_size < test_size; same_size++) if(alphavalue[same_size] != value[same_size]) break;
//                //for(same_size = 0; same_size < test_size; same_size++) if(alphavalue[same_size] != value[same_size]) printf("%d,", same_size);
//                // write to file
//                ofstream written, readed;
//                written.open ("write.txt");
//                written << string(alphavalue, test_size);
//                written.close();
//                readed.open ("read.txt");
//                readed << string(value, test_size);
//                readed.close();
//                for(int k = 0; k < test_size; k++) {
//                    if(alphavalue[k] != value[k]) {
//                        if(first_not_same == 0) first_not_same = k;
//                        last_not_same = k;
//                    }
//                    else same_size++;
//                }
//                printf("\n\n read failed, not the same, same_size=%d, first_not_same=%d, last_not_same=%d\n", same_size, first_not_same, last_not_same);
//            }
//    //        sleep(1);
//        }
//    }
    //3. test case for random of start_key
    char value[BLOCK_SIZE];
    memset(value,0,BLOCK_SIZE);

    //2.write value
    char alphavalue[BLOCK_SIZE];
    memset(alphavalue,'b',BLOCK_SIZE);
    srand(time(NULL));
    int test_size = 2*1024*1024;
    //for(int i=0; i<test_size; i++) alphavalue[i] = char(rand() % 127);
    //printf("Read:%s\n",alphavalue);
    //memcpy((void *)SendPoolAddr, (void *)(alphavalue), BLOCK_SIZE);
    char start_key[KEY_LENGTH];
    char end_key[KEY_LENGTH];
    memset(start_key, 0, KEY_LENGTH);
    memset(end_key, 0, KEY_LENGTH);
    printf("BLOCK_SIZE=%d\n", BLOCK_SIZE);

    clock_t start, end;
    Client *client = new Client();

    while(true) {
        usleep(1);
        for(int i=0; i<test_size; i++) alphavalue[i] = char(rand() % 127);
        for(int i=0; i<KEY_LENGTH-2; i++) start_key[i] = end_key[i] = '0' + rand() % 10;
        start_key[KEY_LENGTH-2] = 'a';
        end_key[KEY_LENGTH-2] = 'b';
        sleep(1);
        for(int i=0; i<1;i++)
        {
            //Client c; Client* client = &c;
            auto t_start = std::chrono::high_resolution_clock::now();
            //Client *client = new Client();
            printf("start_key：%s ",start_key);
            printf("end_key：%s ",end_key);


            client->Write((uint64_t)alphavalue,test_size,start_key,end_key);
            auto t_end = std::chrono::high_resolution_clock::now();

            std::cout << std::fixed << std::setprecision(2)
                      << "write Wall clock time passed: "
                      << std::chrono::duration<double, std::milli>(t_end-t_start).count()
                      << " ms\n";
        }

        for(int i=0; i<1;i++)
        {
            //Client c; Client* client = &c;
            auto t_start = std::chrono::high_resolution_clock::now();
            //Client *client = new Client();

            client->Read((uint64_t)value,test_size,start_key,end_key);
            auto t_end = std::chrono::high_resolution_clock::now();

            std::cout << std::fixed << std::setprecision(2)
                      << "read Wall clock time passed: "
                      << std::chrono::duration<double, std::milli>(t_end-t_start).count()
                    << " ms\n";
        }
        if(memcmp(alphavalue, value, test_size) == 0) {
            printf("\n\n read success, the same\n");
        }
        else {
            printf("\n\n read failed, not the same\n");
        }
    }

    return 0;
}

//#include "client.h"
//#include "Configuration.hpp"
//#include <dirent.h>

//#include <cstdio>
//#include <cerrno>
//#include <atomic>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <cstdlib>
//#include <cstdint>
//#include <ctime>
//#include <stdlib.h>

////add:xurui
////#define CLK_TCK  CLOCKS_PER_SEC
////#define CLOCKS_PER_SEC 1000
//////add:e
//#define random(x) (rand()%x)

//constexpr uint32_t REPORT = 10000;

//int test_fal = 1;
//const char* ip_master = "10.11.6.193";
//const char* ip_worker = "localhost";
//int port_master = 5967;
//int port_worker = 5967;
//int no_thread = 1;
//int no_client = 1;
//int scan_ratio = 100;
//int val_size = 1024*1024*2;
//int total_scan = 1000;
//int total_range = 100*1000;
//const char *log_dir = "./rdma_client.out";

//pthread_t *threads;
//pthread_mutex_t cnt_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
//int ccount = 0;

//Configuration conf;
//char* value;

//// performance counters
//std::atomic<uint64_t> scan_latency(0);
//std::atomic<uint64_t> write_latency(0);
//std::atomic<uint64_t> scan_finished(0);
//std::atomic<uint64_t> write_finished(0);
//std::atomic<uint64_t> finished(1);


//void basic_test(){
//    Client *client = new Client();
//    uint64_t DmfsDataOffset;
//    DmfsDataOffset =  CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
//    DmfsDataOffset += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * client->getConfInstance()->getServerCount();
//    DmfsDataOffset += METADATA_SIZE;


////    if(client->BlockAlloc(0)){
////        printf("BlcokAlloc!\n");
////    }
////    else {
////        printf("Failed!\n");
////    }

//    //1.read null
//    char value[CLIENT_MESSAGE_SIZE];
//    memset(value,0,CLIENT_MESSAGE_SIZE);
//    uint64_t size=CLIENT_MESSAGE_SIZE;
//    uint64_t SendPoolAddr = client->mm + 4 * CLIENT_MESSAGE_SIZE;
//    client->getRdmaSocketInstance()->RdmaRead(1,SendPoolAddr,0+DmfsDataOffset,size,0);

////    GeneralSendBuffer *send = (GeneralSendBuffer*)SendPoolAddr;
////    send->message = MESSAGE_MALLOC;

////    client->BlockWrite(SendPoolAddr,1024*1024*2,-1,0,1);

//    //2.write value
////    Begin = clock();
//    for(int i=0;i<CLIENT_MESSAGE_SIZE;i++){
//        value[i]='0'+i%10;
//    }
//    memcpy((void *)SendPoolAddr, (void *)(value), size);
//    memset(value,0,1024);
//    client->getRdmaSocketInstance()->RdmaWrite(1, SendPoolAddr,DmfsDataOffset, size,-1,0);
//    printf("Write\n");
//    //add:xurui
////    End = clock();
////    duration = double(End - Begin) / CLK_TCK;
////    printf("%ld\n",duration);
//    //add:e

//    //3.test message
//    uint64_t ReceivePoolAddr = client->mm + 6 * CLIENT_MESSAGE_SIZE;
//    GeneralSendBuffer *send = (GeneralSendBuffer*)SendPoolAddr;
//    send->message = MESSAGE_TEST;
//    memcpy(send->path,"1234",5);
//    client->RdmaCall(1, (char*)send, size, value, size);
//    GeneralSendBuffer *receive = (GeneralSendBuffer*)value;
//    printf("RdmaCall:%d,%s\n",receive->message,receive->path);

//    //4.read value
//    client->getRdmaSocketInstance()->RdmaRead(1,SendPoolAddr,0+DmfsDataOffset,size,0);
//    memcpy((void *)(value), (void *)SendPoolAddr, size);
//    printf("Read:%s\n",value);
//}

////add:xurui
//void performance_test(){
//    MemoryManager *mem;
//    struct ibv_wc wc[1];
////    uint16_t NodeID;
//    uint64_t bufferRecv;
//    Client *client = new Client();
//    uint64_t DmfsDataOffset;
//    uint16_t NodeID = (uint16_t)(wc[0].imm_data << 16 >> 16);
//    uint16_t offset = (uint16_t)(wc[0].imm_data >> 16);
//    uint64_t size=CLIENT_MESSAGE_SIZE;

//    bufferRecv = mem->getServerRecvAddress(NodeID, offset);
////    DmfsDataOffset = bufferRecv;
//    DmfsDataOffset =  CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
//    DmfsDataOffset += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * client->getConfInstance()->getServerCount();
//    DmfsDataOffset += METADATA_SIZE;
////    GeneralRequestBuffer *send = (GeneralRequestBuffer*) bufferRecv;
////    Debug::notifyError("Requested Block Num: %u",send->size);
////    recv->flag = mem->allocateMemoryBlocks(send->size,recv->addr);
////    recv->size = send->size;


//    //1.allocate addr
//    //return addr[addr_size-1]
//    uint64_t SendPoolAddr = client->mm + 4 * CLIENT_MESSAGE_SIZE;
//    SendPoolAddr = client->BlockWrite(SendPoolAddr,1024*1024*2,-1,0,1);

//    //2.write value in SendPoolAddr buffer
//    char value[CLIENT_MESSAGE_SIZE];
//    memset(value,0,CLIENT_MESSAGE_SIZE);

//    for(int i=0;i<CLIENT_MESSAGE_SIZE;i++){
//        value[i]='0'+i%10;
//    }

//    memcpy((void *)SendPoolAddr, (void *)(value), size);
//    memset(value,0,1024);
////    PeerSockData *peer = peers[1];
////    DmfsDataOffset= SendPoolAddr + peer->RegisteredMemory;
//    client->getRdmaSocketInstance()->RdmaWrite(1, SendPoolAddr,DmfsDataOffset, size,-1,0);
//    printf("Write\n");


////    if(client->BlockAlloc(10)){
////        printf("BlcokAlloc!\n");
////    }
////    else {
////        printf("Failed!\n");
////    }

////    char value[CLIENT_MESSAGE_SIZE];
////    memset(value,0,CLIENT_MESSAGE_SIZE);


////    memset(value,0,1024);
////    client->getRdmaSocketInstance()->RdmaWrite(1, SendPoolAddr,DmfsDataOffset, size,-1,0);

////    for(int i=0;i<CLIENT_MESSAGE_SIZE;i++){
////        value[i]='0'+i%10;
////    }
////    memcpy((void *)SendPoolAddr, (void *)(value), size);
////    memset(value,0,1024);
////    client->getRdmaSocketInstance()->RdmaWrite(1, SendPoolAddr,DmfsDataOffset, size,-1,0);
////    printf("Write\n");

////    //4.read value
////    client->getRdmaSocketInstance()->RdmaRead(1,SendPoolAddr,0+DmfsDataOffset,size,0);
////    memcpy((void *)(value), (void *)SendPoolAddr, size);
////    printf("Read:%s\n",value);

//}
////add:e

//static uint64_t nstime(void) {
//  struct timespec time;
//  uint64_t ust;

//  clock_gettime(CLOCK_MONOTONIC, &time);

//  ust = ((uint64_t)time.tv_sec)*1000000000;
//  ust += time.tv_nsec;
//  return ust;
//}

//static uint64_t mstime(void) {
//  return nstime()/1000000;
//}

//void populate(Client* client) {
//  value = new char[val_size];
//  value[0] = 's';
//  for (int i = 1; i < val_size; i++)
//    value[i] = '0'+i%10;
//  value[val_size - 1] = 0;

//  uint64_t start = mstime();
//  printf("start to populate\n");


//  uint64_t SendPoolAddr = client->mm + 4 * 1024; //?
//  memcpy((void *)SendPoolAddr, (void *)(value), val_size);

//  //prepare index
//  //uint64_t SendPoolAddr = mm + 4 * 1024;
//  GeneralRequestBuffer *send = (GeneralRequestBuffer*)SendPoolAddr;
//  send->message = MESSAGE_INSERT;
//  send->size = total_range/100;
//  //insert data
//  for(int i=0; i<total_range/100; i++){
//    //generate block id
//    int number = i;
//    char num[10];
//    int len=0;
//    while(number>0){
//      num[i]=number%10+'0';
//      number/=10;
//      len++;
//    }
//    num[len]='\0';
//    strcat(num," data:");
//    for(int j=0; j<len/2; j++){
//      char change = num[j];
//      num[j] = num[len-j];
//      num[len-j] = change;
//    }
//    memcpy((void *)SendPoolAddr, (void *)(num), len+6);
//    //prepare index
////    char *itoa(int p);
////    send->range[i].start_key=*itoa(i*100);
////    send->range[i].end_key=*iota((i+1)*100-1);
//    send->range[i].path = client->BlockWrite(SendPoolAddr);

//  }
//  double duration = mstime() - start;
//  printf("%f",duration);

//  //insert index
//  client->IndexWrite((char*)send);

//  //4.read value
////  client->getRdmaSocketInstance()->RdmaRead(1,SendPoolAddr,0+DmfsDataOffset,size,0);
////  memcpy((void *)(value), (void *)SendPoolAddr, size);
//  printf("Read:%s\n",send->range);
//}


//void* do_work(void *arg) {
//  Client* client = new Client();
//  uint64_t start = mstime();
//  char start_key[KEY_LENGTH];
//  char end_key[KEY_LENGTH];
//  srand((int)time(0));
//  uint64_t t1, t2;
//  uint64_t last_report = start, current;
//  int cnt = 0;
//  while(cnt++ < total_scan) {
//      printf("print scan performance1!\n");
//    if (random(100) < scan_ratio) {
//      //scan
//      t1 = nstime();
//      //client->
//      if(client->RangeCover(start_key,KEY_LENGTH,1) ){
//      t2 = nstime();
//      scan_latency += (t2 - t1);
//      scan_finished++;
//      printf("print scan performance2!\n");
//      }
//      else {
//          printf("print scan performance3!\n");
//      }
//    } else {
//      //write
//      t1 = nstime();
//      //client->BlockWrite(start_key, (uint64_t) 1024*1024*2, -1, 0, 1);
//      t2 = nstime();
//      write_latency += (t2 - t1);
//      write_finished++;
//      printf("print scan performance3!\n");
//    }

//    if (finished.fetch_add(1, std::memory_order_relaxed) % REPORT == 0) {
//      current = mstime();
//      printf("%.2f\n", (1000.0 * REPORT)/(current - last_report));
//      last_report = current;
//    }
//  }

//  double duration = mstime() - start;
//  double gets = scan_finished.load(), sets = write_finished.load();
//  double glat = scan_latency.load(), slat = write_latency.load();
//  printf("%lu, %.2f", finished - 1, (finished-1) * 1000/duration);
//  if (gets > 0) printf(", %.2f", glat/gets);
//  else printf(", -");
//  if (sets > 0) printf(", %.2f", slat/sets);
//  else printf(", -");
//  printf("\n");
//  Debug::notifyError("performance 5!");
//  return ((void *)0);
//}


//  int main(int argc, char* argv[]){

//    for(int i = 1; i < argc; i++) {
//      if(strcmp(argv[i], "--ip_master") == 0) {
//        ip_master = argv[++i];
//      } else if(strcmp(argv[i], "--ip_worker") == 0) {
//        ip_worker = argv[++i];
//      } else if (strcmp(argv[i], "--port_master") == 0) {
//        port_master = atoi(argv[++i]);
//      } else if (strcmp(argv[i], "--port_worker") == 0) {
//        port_worker = atoi(argv[++i]);
//      } else if (strcmp(argv[i], "--no_thread") == 0) {
//        no_thread = atoi(argv[++i]);
//      } else if (strcmp(argv[i], "--no_client") == 0) {
//        no_client = atoi(argv[++i]);
//      } else if (strcmp(argv[i], "--scan_ratio") == 0) {
//        scan_ratio = atoi(argv[++i]); //0..100
//      } else if (strcmp(argv[i], "--val_size") == 0) {
//        val_size = atoi(argv[++i]);
//      } else if (strcmp(argv[i], "--log_dir") == 0) {
//        log_dir = argv[++i];
//      } else if (strcmp(argv[i], "--total_scan") == 0) {
//        total_scan = atoi(argv[++i]);
//      } else if (strcmp(argv[i], "--test_fal") == 0) {
//          test_fal = atoi(argv[++i]);
//      } else {
//        fprintf(stderr, "Unrecognized option %s\n", argv[i]);
//      }
//    }

//    if(test_fal==1){
//       basic_test();
////        performance_test();
//       exit(1);
//    }

//    //step.0	set conf
//    //conf.loglevel = LOG_WARNING;

//    //step.1 	log file/dir
//    /*struct dirent * ent = NULL;
//    DIR* dir = opendir(log_dir);
//    if (!dir) {
//      perror("opendir:");
//      exit(1);
//    }*/

//    //step.2	write initial data
//    Client *client = new Client();
//    populate(client);


//    //step.3  test scan
//    threads = new pthread_t[no_thread];
//    int i = 0;
//    for (i = 0; i < no_thread; ++i)
//    {
//       pthread_create(&threads[i], NULL, do_work, NULL);
//    }

//    //step.4 stop
//    for (i--; i >= 0; i--)
//      pthread_join(threads[i], NULL);

//    return 0;
//}
