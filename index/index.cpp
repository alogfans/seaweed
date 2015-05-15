// index.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implemetation file of index.


#include "index.h"
#include <iostream>
#include <stdexcept>
using namespace std;

Index::Index(Storage &stor) : stor(stor), opened(false)
{

}

int Index::create_table(int n_property, EntityProperty * properties)
{
    Record rec(stor);
    rec.init_entity(n_property, properties);
    return rec.get_first_page();
}

int Index::create_index(int table_id, int key_type, int key_sizeof)
{
    BTree btree(stor, key_type, key_sizeof);
    return btree.allocate_root();
}

void Index::drop_table(int table_id)
{
	//cout << table_id << endl;
    Record rec(stor, table_id);
    rec.destroy();
}

void Index::drop_index(int index_id)
{
    BTree btree(stor, index_id);
    btree.destroy();
}

void Index::open_table(int table_id, int index_id)
{
	if (opened == true)
		throw logic_error("table opened");
	opened = true;

	op_record = new Record(stor, table_id);
	op_btree = new BTree(stor, index_id);
}

void Index::close_table(int &table_id, int &index_id)
{
	if (opened == false)
		throw logic_error("table not opened");
	opened = false;

	table_id = op_record->get_first_page();
	index_id = op_btree->get_root();

	delete op_record;
	delete op_btree;
}

void Index::insert_record(void * data, void * key)
{
	if (opened == false)
		throw logic_error("table not opened");
	RID rid = op_record->insert_record(data);
	op_btree->insert(key, rid);
}

void Index::delete_record(void * key)
{
	if (opened == false)
		throw logic_error("table not opened");
	RID rid = op_btree->search(key);
	op_record->remove_record(rid);
	op_btree->remove(key);
}

vector<void *> Index::select_record(int operand, void * key)
{
	if (opened == false)
		throw logic_error("table not opened");

	vector<RID> rids = op_btree->selects(operand, key);
	vector<void *> record_vector;

	for (int i = 0; i < rids.size(); i++)
		record_vector.push_back(op_record->attain_record(rids[i]));
	return record_vector;
}
