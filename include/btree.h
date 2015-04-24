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
    RID(int page_num = Invalid, int slot_num = Invalid) : 
        page_num(page_num), slot_num(slot_num);

    bool operator==(const RID &rhs) const 
    {
        return (page_num == rhs.page_num) && (slot_num == rhs.slot_num);
    }

    int page_num;
    int slot_num;
};

class BTree
{
public:
    BTree(int root);                        // if the page existed
    BTree(int key_type, int key_sizeof);    // if need allot new btree.

    ~BTree();
    
    RID search(void * key);
    void insert(void * key, RID &loc);
    void remove(void * key);
    void destory();

private:
    uint32_t key_type;
    uint32_t key_sizeof;
    bool opened;
    int root;
    int next_root;

    int compare(void * lhs, void * rhs);
    RID find(void * key);
    BTNode * find_leaf(void * key);
    void initize_root(void * key, RID &loc);
    void insert_leaf(BTNode * leaf, void * key, RID &loc);
    void insert_leaf_spilt(BTNode * leaf, void * key, RID &loc);
    void insert_into_parent(BTNode * left, BTNode * right, void * key);
    void insert_into_new_root(BTNode * left, BTNode * right, void * key);
    void insert_node(BTNode * parent, int left_index, void * key, BTNode * right);
    void insert_node_split(BTNode * old_node, int left_index, void * key, BTNode * right);
    int get_child_indexof(BTNode * parent, BTNode * child);

    
};


#endif 