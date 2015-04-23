// btree.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of btree node componment.

#ifndef __BT_NODE_H__
#define __BT_NODE_H__

#include "generic.h"

struct BTNodeMarshall
{
    bool        is_leaf;
	uint32_t    key_type;
	uint32_t    key_sizeof;
	uint32_t    order;
    uint32_t    parent;
    uint32_t    num_keys;
    uint32_t    next;
};

class BTNode
{
public:
	BTNode();
	~BTNode();

    void create_block(bool is_leaf, uint32_t key_type, uint32_t key_sizeof);
	void marshall_to(byte * buffer);
	void unmarshall_from(byte * buffer);
    void dispose();

	void marshall_block(BTNodeMarshall * head_block);
	void unmarshall_block(BTNodeMarshall * head_block);

    uint32_t    page_num;

	uint32_t    key_type;
	uint32_t    key_sizeof;
	uint32_t    order;
    byte  	 *  keys;				// [Order - 1];
    uint32_t    parent;
    bool        is_leaf;
    uint32_t    num_keys;
    uint32_t    next;
    uint32_t *  pointers;			// [Order];
    uint32_t *  slots;				// [Order], available if it's leaf;
};

struct Record 
{
	Record * next;
	void * value;
};

class BTree
{
public:
    BTree();
    ~BTree();
    Record * find(void * key);
    void insert(void * key, void * value);
    //void delete(void * key);
    void print_tree();
    void destroy();
private:
    int root_page;
    int key_type;
    int key_sizeof;
};

// relative functions that helps btree routine...

void destroy_record(Record * record);


#endif 