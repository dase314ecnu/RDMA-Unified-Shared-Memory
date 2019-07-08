#include "server.h"


int main()
{
    Server *server =new Server(2);
    char *p = (char *)server->getMemoryManagerInstance()->getDataAddress();
    while (true) {
        getchar();
        printf("storage addr = %lx\n", (long)p);
        for (int i = 0; i < 12; i++) {
            printf("%c", p[i]);
        }
        printf("\n");
        uint64_t mm=server->getRdmaSocketInstance()->get_mem();
        char test[1024];
        memcpy((void *)(test), (void *)(mm+2*4096),1024);
        Debug::notifyError(test);
        server->getRdmaSocketInstance()->RdmaReceive(0,server->getMemoryManagerInstance()->getDataAddress(),1024);
    }

    return 0;
}
