#ifndef COMMON_HEADER
#define COMMON_HEADER
#include <stdint.h>
#include <time.h>

typedef int nrfs;
typedef char* nrfsFile;

#define MAX_MESSAGE_BLOCK_COUNT 10      /* Max count of block index in a message. */

typedef struct 
{
	uint16_t node_id;
	uint64_t offset;
	uint64_t size;
} file_pos_tuple;

struct file_pos_info
{
	uint32_t len;
	file_pos_tuple tuple[MAX_MESSAGE_BLOCK_COUNT];
};

/* getattr */
struct nrfsfileattr
{
	uint32_t mode;	/* 0 - file, 1 - directory */
	uint64_t size;
	uint32_t time;
};



/** Definitions. **/
#define MAX_PATH_LENGTH 255             /* Max length of path. */
//add by dhc :s
#define KEY_LENGTH 64             /* Length of key. */
#define DEFAULTMALLOCSIZE  10     /* Default size of malloc. */
#define MAX_ADDR_LENGTH 1024      /* Max length of addr. */
#define MAX_RANGE_LENGTH 1024      /* Max length of range. */
#define ERROR 0
#define SUCCESS 1
#define INSUFFICIENT_SPACE 2
#define DISCONTINUOUS_RANGE 3
//add by dhc:e
// add by weixing [20190325]:b
#define MAX_ADDR_NUM 64
// add e

// add by weixing [20190330]:b
#define GET_BIT0(x) ((x&0x01)==0x01?1:0)
#define GET_BIT1(x) ((x&0x02)==0x02?1:0)
#define GET_BIT2(x) ((x&0x04)==0x04?1:0)
#define GET_BIT3(x) ((x&0x08)==0x08?1:0)
#define GET_BIT4(x) ((x&0x10)==0x10?1:0)
#define GET_BIT5(x) ((x&0x20)==0x20?1:0)
#define GET_BIT6(x) ((x&0x40)==0x40?1:0)
#define GET_BIT7(x) ((x&0x80)==0x80?1:0)
// add e

/** Definitions. **/
#define MAX_FILE_EXTENT_COUNT 20        /* Max extent count in meta of a file. */
//#define BLOCK_SIZE (1 * 1024 * 1024)  /* Current block size in bytes. */
#define BLOCK_SIZE (2 * 1024)
#define MAX_FILE_NAME_LENGTH 50         /* Max file name length. */
#define MAX_DIRECTORY_COUNT 60          /* Max directory count. */

/** Classes and structures. **/
typedef uint64_t NodeHash;              /* Node hash. */

// add by weixing [20190325]:b
typedef uint64_t GAddr;
typedef long Align;     /* alignment boundary */

typedef struct
{
    int num;    /* record the number of allocated blocks*/
    GAddr addr[MAX_ADDR_NUM];   /* the array of returned addresses*/
} ResponseAddrList;
// add e

// add by weixing [20190328]:b
union defHeader{
    struct info{
        char* const addrBlock_;
        uint64_t size_;
    };
    Align X; /*force the memory alignment*/
};

typedef union defHeader BlockHeader;
// add e

typedef struct 
{
	NodeHash hashNode; /* Node hash array of extent. */
    uint32_t indexExtentStartBlock; /* Index array of start block in an extent. */
    uint32_t countExtentBlock; /* Count array of blocks in an extent. */
} FileMetaTuple;

typedef struct                          /* File meta structure. */
{
    time_t timeLastModified;        /* Last modified time. */
    uint64_t count;                 /* Count of extents. (not required and might have consistency problem with size) */
    uint64_t size;                  /* Size of extents. */
    FileMetaTuple tuple[MAX_FILE_EXTENT_COUNT];
} FileMeta;

typedef struct {
    char names[MAX_FILE_NAME_LENGTH];
} DirectoryMetaTuple;

typedef struct                          /* Directory meta structure. */
{
    uint64_t count;                 /* Count of names. */
    DirectoryMetaTuple tuple[MAX_DIRECTORY_COUNT];
} DirectoryMeta;

typedef DirectoryMeta nrfsfilelist;


static inline void NanosecondSleep(struct timespec *preTime, uint64_t diff) {
	struct timespec now;
	uint64_t temp;
	temp = 0;
	while (temp < diff) {
		clock_gettime(CLOCK_MONOTONIC, &now);
		temp = (now.tv_sec - preTime->tv_sec) * 1000000000 + now.tv_nsec - preTime->tv_nsec;
		temp = temp / 1000;
	}
}

#endif
