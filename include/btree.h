// btree.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of btree node componment.

#ifndef __BTREE_H__
#define __BTREE_H__

#include "generic.h"
#include "btnode.h"

// The Btree will maintain a instance of index, remember that
// the data is provided by entity level, which will be integrated
// on Table class above it.

const int IndexListEnd = -1;

struct RID
{
    uint32_t page_num;
    uint32_t slot_num;
};

class BTree
{
public:
    BTree(int root);                        // if the page existed
    BTree(int key_type, int key_sizeof);    // if need allot new btree.

    ~BTree();
    
    RID search(void * key);
    void insert(void * key, RID loc);
    void remove(void * key);
    void destory();

private:
    uint32_t key_type;
    uint32_t key_sizeof;
    bool opened;
    int root;
};


#endif 