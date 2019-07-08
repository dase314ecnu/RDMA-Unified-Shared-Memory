 #ifndef SC_SKIPLIST_H
#define SC_SKIPLIST_H

#include <stdint.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <atomic>
#include "skiplist.h"
#include "global.h"

#define MAX_VERSION_NUM 1024
using namespace std;


struct Key
{
    char start_key[KEY_LENGTH];\

    Key()
    {
//       printf("1\n");
    }

    Key(const Key &y)
    {
//        printf("2\n");
        strcpy(start_key,y.start_key);
    }

    void operator = (const Key &y)
    {
//        printf("3\n");
        strcpy(start_key,y.start_key);
    }

    bool operator < (const Key &y)
    {
        return (strcmp(start_key,y.start_key)<0);
    }

    bool operator == (const Key &y)
    {
        return (strcmp(start_key,y.start_key)==0);
    }

    void print()
    {
        cout<<start_key<<endl;
    }
};


struct VersionInfo
{
    int64_t version;
    uint64_t address;
    atomic<int> ref_count;

    VersionInfo()
    {
        version = 0;;
        address = -1;
        ref_count = 0;
    }

    VersionInfo(const VersionInfo &y)
    {
        int ref = 0;
        version = y.version;
        address = y.address;
//        ref_count = y.ref_count;
        ref = y.ref_count;
        ref_count = ref;
    }

    void operator = (const VersionInfo &y)
    {
        int ref = 0;
        version = y.version;
        address = y.address;
        ref = y.ref_count;
        ref_count = ref;
    }

    void print()
    {
        cout<<version<<":"<<address<<":"<<ref_count<<endl;
    }
};
struct Value
{
    char end_key[KEY_LENGTH];
    VersionInfo version_infos[MAX_VERSION_NUM];
    int64_t version_count;
    int64_t latest_version_index;
    int64_t oldest_version_index;
    bool valid;

    Value()
    {
        version_count = 0;
        latest_version_index = 0;
        oldest_version_index = 0;
        valid = true;
    }

    Value(const Value &y)
    {
        strcpy(end_key,y.end_key);
        version_count = y.version_count;
        for(int i=y.oldest_version_index; i<y.latest_version_index; i++)
        {
//            printf("%d\n",i);
//            printf("end_key=%s",y.end_key);
//            printf("version=%ld",y.version_infos[i].version);
            version_infos[i] = y.version_infos[i];

        }
        latest_version_index = y.latest_version_index;
        oldest_version_index = y.oldest_version_index;
        valid = y.valid;
    }

    void operator = (const Value &y)
    {
        strcpy(end_key,y.end_key);
        version_count = y.version_count;
        for(int i=y.oldest_version_index; i<y.latest_version_index; i++)
        {
            version_infos[i] = y.version_infos[i];
        }
        latest_version_index = y.latest_version_index;
        oldest_version_index = y.oldest_version_index;
        valid = y.valid;
    }
};

//struct RangeInformation
//{
//    char start_key[KEY_LENGTH];
//    char end_key[KEY_LENGTH];
//    uint64_t address;
//    int64_t version; //版本索引
// //   int size;
//};


class SC_Skiplist
{
    public:
        SC_Skiplist();
 //       virtual ~sc_skiplist();
        bool Insert(RangeInformation* block_list, uint8_t block_num);
        int Scan(char* start, char* end, RangeInformation* block_list, uint8_t &block_num);
        int Get(char* key, RangeInformation* block_list);
        int release(char* start, char* end, RangeInformation* block_list, uint8_t block_num);
        int Delete(char* start, char* end);

    protected:

    private:
        SkipList<Key,Value> skiplist_;
};

#endif // SC_SKIPLIST_H
