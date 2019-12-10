#include "sc_skiplist.h"

typedef SkipListNode<Key,Value> Node;

SC_Skiplist::SC_Skiplist()
{
    //ctor
}

//sc_skiplist::~sc_skiplist()
//{
    //dtor
//}


bool SC_Skiplist::Insert(RangeInformation* block_list, uint8_t block_num)
{
    Key key;
    Node* node = NULL;
    int64_t latest_index;

    for(uint8_t i=0; i< block_num; i++)
    {
        /*key*/
        strcpy(key.start_key,block_list[i].start_key);

//        if(NULL == (node = skiplist_.Get(key)))
        if(NULL == (node = skiplist_.Find(key)))
        {
            Value value;
//            pair<Key,Value> keyvalue;

            /*value*/
            strcpy(value.end_key,block_list[i].end_key);

            value.version_infos[value.latest_version_index].address = block_list[i].address;
            value.version_infos[value.latest_version_index].version = 1; //version start from 1
            value.latest_version_index++;
            value.version_count++;
            value.valid = true;
            printf("startkey:%s,endkey:%s\n",key.start_key,value.end_key);
            printf("address:%lu,version:%ld\n",value.version_infos[value.latest_version_index-1].address,
                    value.version_infos[value.latest_version_index-1].version);

//            printf("8\n");
//            pair<int,int> test=make_pair(1,1);
//            printf("9\n");
//            printf("version=%ld\n",value.version_infos[0].version);
            /*insert*/
            pair<Key,Value> keyvalue = make_pair(key,value);


            skiplist_.Insert(keyvalue);
//            printf("10\n");

            node = NULL;
            node = skiplist_.Get(key);
            if(node == NULL)
            {
                printf("no find!\n");
            }
            else
            {
                printf("find!\n");
                printf("endkey=%ld",node->keyvalue.second.version_count);
            }
        }
        else
        {
            latest_index = node->keyvalue.second.latest_version_index;
            node->keyvalue.second.version_infos[latest_index].address = block_list[i].address;
            node->keyvalue.second.version_infos[latest_index].version = node->keyvalue.second.version_infos[latest_index-1].version+1;
            node->keyvalue.second.latest_version_index++;
            node->keyvalue.second.version_count++;
            node->keyvalue.second.valid = true;
            printf("duplicated endkey=%ld",node->keyvalue.second.version_count);
        }

    }

    return true;
}


//success:1,fail:0
int SC_Skiplist::Scan(char* start, char* end, RangeInformation* block_list, uint8_t &block_num)
{
    Key start_key;
    Key end_key;
    vector<Node*> nodes;
    int64_t latest_index;
    int status = 0; // record for success or not

    strcpy(start_key.start_key,start);
    strcpy(end_key.start_key,end);
//    printf("1\n");
    skiplist_.Scan(start_key, end_key, nodes);
//    printf("2\n");
    for(int i=0; i<(int)(nodes.size()); i++)
    {
//        printf("3\n");
        latest_index = nodes[i]->keyvalue.second.latest_version_index;
        if(nodes[i]->keyvalue.second.valid)
        {
            strcpy(block_list[i].start_key,nodes[i]->keyvalue.first.start_key);
            strcpy(block_list[i].end_key,nodes[i]->keyvalue.second.end_key);
            block_list[i].address = nodes[i]->keyvalue.second.version_infos[latest_index-1].address;
            block_list[i].version = latest_index;

            /*จน1*/
            nodes[i]->keyvalue.second.version_infos[latest_index-1].ref_count++;
            block_num++;
        }
        status = 1;
    }
    return status;
}


int SC_Skiplist::Get(char* key, RangeInformation* block_list)
{
    Key key_;
    Node* node;
    int64_t latest_index;

    strcpy(key_.start_key,key);
    node = skiplist_.Get(key_);
    latest_index = node->keyvalue.second.latest_version_index;

    if(node->keyvalue.second.valid)
    {
        strcpy(block_list[0].start_key,node->keyvalue.first.start_key);
        strcpy(block_list[0].end_key,node->keyvalue.second.end_key);
        block_list[0].address = node->keyvalue.second.version_infos[latest_index-1].address;
        block_list[0].version = latest_index;

        /*จน1*/
        node->keyvalue.second.version_infos[latest_index-1].ref_count++;
        return 1;
    }
    else
    {
        return 0;
    }

    return 0;
}


int SC_Skiplist::release(char* start, char* end, RangeInformation* block_list, uint8_t block_num)
{
    Key start_key;
    Key end_key;
    vector<Node*> nodes;
    int j=0;
    int k=0;

    strcpy(start_key.start_key,start);
    strcpy(end_key.start_key,end);

    skiplist_.Scan(start_key, end_key, nodes);


    for(int i=0; i<(int)(nodes.size()); i++)
    {
        if(j<block_num && nodes[i]->keyvalue.first.start_key == block_list[j].start_key)
        {
            k = block_list[j].version;
            nodes[i]->keyvalue.second.version_infos[k].ref_count--;
        }
        else if(j >= block_num)
        {
            return 1;
        }
        else
        {
            j++;
        }
    }
    return 0;
}



int SC_Skiplist::Delete(char* start, char* end)
{
    Key start_key;
    Key end_key;
    vector<Node*> nodes;

    strcpy(start_key.start_key,start);
    strcpy(end_key.start_key,end);

    skiplist_.Scan(start_key, end_key, nodes);

    for(int i=0; i<(int)(nodes.size()); i++)
    {
        nodes[i]->keyvalue.second.valid=false;
    }
    return 0;
}

