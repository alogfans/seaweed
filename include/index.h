// index.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of index definitions.

#ifndef __INDEX_H__
#define __INDEX_H__

#include "generic.h"
#include "storage.h"
#include "record.h"
#include "btree.h"
#include <vector>

class Index
{
public:
	Index(Storage &stor);
	//~Index();

    int create_table(int n_property, EntityProperty * properties);
    int create_index(int table_id, int key_type, int key_sizeof);
    void drop_table(int table_id);
    void drop_index(int index_id);


    void open_table(int table_id, int index_id);
    void close_table(int &table_id, int &index_id);

    void insert_record(void * data, void * key);
    void delete_record(void * key);
    std::vector<void *> select_record(int operand, void * key);

private:
	Storage &stor;

    bool opened;
    Record * op_record;
    BTree * op_btree;
};

#endif