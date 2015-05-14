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
#include "entity.h"
#include "btree.h"

struct Record
{
	Record * next;
	int row;
	void * data;
};

void delete_record_structure(Record * record);

class Index
{
public:
	Index(Storage &stor);
	~Index();

    void create_table(int n_property, Property * properties, int key_id);
    void create_index(int table_id, int key_id);
    void drop_table(int table_id);
    void drop_index(int table_id, int index_id);

    void open_table(int table_id);
    void close_table(int table_id);

    int count();
    void insert_record(void * data);
    void delete_record(int key_id, void * key);
    Record * select_record(int operand, int key_id, void * key);
    void modify_record(Records * record, void * data);

private:
	Storage &stor;
	Entity entity;
	BTree * btree;
};

#endif