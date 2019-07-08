#include "skiplist.h"

/*
bool SkipList<DataType>::Delete(Range range)
{
    Node* start_node=head_;
    Node* end_node=head_;
    Node* start_update[level_];
    Node* end_update[level_];
    int start_key_level=0;
    int end_key_level=0;

    for(int i=level_-1;i>=0;i--)
    {
        while(start_node->next[i]!=NULL)
        {
            if(start_node->next[i]->keyvalue->first < range.start)
            {
                start_node=start_node->next[i];
            }
            else if(start_node->next[i]->keyvalue->first == range.start)
            {
                //计算key节点next指针一共有多少层
                start_key_level=sizeof(start_node->next[i]->next)/sizeof(Node*);
                break;
            }
            else
            {
                break;
            }
        }
        start_update[i]=start_node;
    }

    for(int i=level_-1;i>=0;i--)
    {
        while(end_node->next[i]!=NULL)
        {
            if(end_node->next[i]->keyvalue->first < range.end)
            {
                end_node=end_node->next[i];
            }
            else if(end_node->next[i]->keyvalue->first == range.end)
            {
                //计算key节点next指针一共有多少层
                end_key_level=sizeof(end_node->next[i]->next)/sizeof(Node*);
                break;
            }
            else
            {
                break;
            }
        }
        end_update[i]=end_node;
    }

    if(start_key_level == 0 || end_key_level == 0)
    {
        return false;  //没有找到节点
    }
    else if(start_key_level>=end_key_level)
    {
        Node* p = start_update[0]->next[0];
        for(int i=0; i<end_key_level; i++)
        {
            start_update[i]->next[i]=end_update[i]->next[i]->next[i];
        }
        for(int i=end_key_level; i<start_key_level; i++)
        {
            start_update[i]->next[i]=start_update[i]->next[i]->next[i];
        }
        memory_manager_->release_block_space((void*)p);
    }
    else if(start_key_level<end_key_level)
    {
        Node* p = start_update[0]->next[0];
        for(int i=0; i<start_key_level; i++)
        {
            start_update[i]->next[i]=end_update[i]->next[i]->next[i];
        }
        for(int i=end_key_level; i<start_key_level; i++)
        {
            end_update[i]->next[i]=end_update[i]->next[i]->next[i];
        }
        memory_manager_->release_block_space((void*)p);
    }

    return true;
}*/



