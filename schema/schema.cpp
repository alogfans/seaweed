// index.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implemetation file of index.


#include "schema.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
using namespace std;

Schema::Schema(Storage &stor) : stor(stor)
{
	index = new Index(stor);
	opened = false;
	columns = 0;
	buffer = 0;
}

Schema::~Schema()
{
	delete index;
}


void Schema::initize_schema()
{
	// first build header table content
	EntityProperty schema_properties[3];

	// table name
	schema_properties[0].type = CharType;
	schema_properties[0].length = 32;

	// root page
	schema_properties[1].type = IntegerType;
	schema_properties[1].length = 4;

	// sql statement
	schema_properties[2].type = CharType;
	schema_properties[2].length = 255;

	stor.shadow.schema_table = index->create_table(3, schema_properties);
	stor.shadow.schema_index = index->create_index(stor.shadow.schema_table, CharType, 32);
}

void Schema::create_table(const char * tb_name, int n_columns, Column * columns)
{
	if (sizeof(Column) * n_columns + 8 > SizeOfPage)
		throw logic_error("too many columns");

	table_root = stor.acquire_page();

	TableInfo table_info;
	memset(&table_info, 0, sizeof(table_info));
	strncpy(table_info.title, tb_name, 32);
	table_info.root_page = table_root;

	index->open_table(stor.shadow.schema_table, stor.shadow.schema_index);
	index->insert_record(&table_info, table_info.title);
	index->close_table(stor.shadow.schema_table, stor.shadow.schema_index);

	buffer = stor.get_page_content(table_root);

	EntityProperty * entity_properties = new EntityProperty[n_columns];
	for (int i = 0; i < n_columns; i++)
	{		
		entity_properties[i].type = columns[i].type;
		entity_properties[i].length = columns[i].length;
		columns[i].index_root = -1;
	}

	int record_root = index->create_table(n_columns, entity_properties);
//cout << record_root << endl;
	delete [] entity_properties;

	memcpy(buffer, &record_root, 4);
	memcpy(buffer + 4, &n_columns, 4);
	memcpy(buffer + 8, columns, sizeof(Column) * n_columns);

	stor.update_page_content(table_root, buffer);
	stor.unpin_page(table_root);
}

void Schema::create_index(const char * tb_name, const char * column_title)
{
	construct(tb_name);

	int property_id = -1;
	int current_index = -1;
	int offset_property = 0;
	int offset_current = 0;

	for (int i = 0; i < n_columns && property_id < 0; i++)
	{		
		if (strncmp(columns[i].title, column_title, 32) == 0)
			property_id = i;
		else
			offset_property += columns[i].length;
	}

	for (int i = 0; i < n_columns && current_index < 0; i++)
	{		
		if (columns[i].index_root >= 0)
			current_index = i;
		else
			offset_current += columns[i].length;
	}
//cout << offset_property << "\t" << offset_current << endl;
	if (property_id >= 0 && columns[property_id].index_root < 0)
	{
		columns[property_id].index_root = index->create_index(record_root, 
			columns[property_id].type, columns[property_id].length);
//cout << columns[property_id].index_root << endl;
		if (current_index >= 0)
		{
			//cout << column_title << endl;
			vector<void *> vecs;

			Index index_reference(stor);
			index_reference.open_table(record_root, columns[current_index].index_root);
			vecs = index_reference.select_record(OP_AL, NULL);
			index_reference.close_table(record_root, columns[current_index].index_root);

			BTree btree(stor, columns[property_id].index_root);
			BTree current_btree(stor, columns[current_index].index_root);

			for (int i = 0; i < vecs.size(); i++)
			{
				RID rid = current_btree.search((void *)((byte *)vecs[i] + offset_current));
				//cout << rid.page_num << endl;
				btree.insert((void *)((byte *)vecs[i] + offset_property), rid);
			}
			columns[property_id].index_root = btree.get_root();
//cout << columns[property_id].index_root << endl;
		}
	}
	deconstruct();
}

void Schema::drop_table(const char * tb_name)
{
	construct(tb_name);

	for (int i = 0; i < n_columns; i++)
		if (columns[i].index_root >= 0)
			index->drop_index(columns[i].index_root);

	deconstruct();

	index->drop_table(record_root);
	stor.release_page(table_root);
}

void Schema::drop_index(const char * tb_name, const char * column_title)
{
	construct(tb_name);

	int i;
	for (i = 0; i < n_columns; i++)
		if (strncmp(columns[i].title, column_title, 32) == 0)
			break;
	if (i != n_columns)
	{
		index->drop_index(columns[i].index_root);
		columns[i].index_root = -1;		
	}

	deconstruct();
}

void Schema::construct(const char * tb_name)
{
	index->open_table(stor.shadow.schema_table, stor.shadow.schema_index);
	vector<void *> tables = index->select_record(OP_EQ, (char *) tb_name);
	index->close_table(stor.shadow.schema_table, stor.shadow.schema_index);
	if (tables.size() != 1)
		throw logic_error("table non existed");
	table_root = (*(TableInfo *) tables[0]).root_page;

	buffer = stor.get_page_content(table_root);

	memcpy(&record_root, buffer, 4);
	memcpy(&n_columns, buffer + 4, 4);

	columns = new Column[n_columns];
	memcpy(columns, buffer + 8, sizeof(Column) * n_columns);
}

void Schema::deconstruct()
{
	memcpy(buffer + 8, columns, sizeof(Column) * n_columns);
	delete [] columns;
	columns = 0;
	stor.update_page_content(table_root, buffer);
	stor.unpin_page(table_root);
	//delete [] buffer;
	buffer = 0;
}

void Schema::open_table(const char * tb_name)
{
	if (opened == true)
		throw logic_error("table opened");
	opened = true;

	construct(tb_name);
}

void Schema::close_table()
{
	if (opened == false)
		throw logic_error("table not opened");
	opened = false;

	deconstruct();
}


void Schema::insert_record(void * data)
{
	if (opened == false)
		throw logic_error("table not opened");

	Record record(stor, record_root);
	RID rid = record.insert_record(data);

	int offset = 0;
	for (int i = 0; i < n_columns; i++)
	{
		if (columns[i].index_root >= 0)
		{
			BTree btree(stor, columns[i].index_root);
			btree.insert((void *)((byte *)data + offset), rid);
			columns[i].index_root = btree.get_root();
		}
		offset += columns[i].length;
	}
}

void Schema::delete_record(void * data)
{
	if (opened == false)
		throw logic_error("table not opened");
	RID rid;

	int offset = 0;
	for (int i = 0; i < n_columns; i++)
	{
		if (columns[i].index_root >= 0)
		{
			BTree btree(stor, columns[i].index_root);

			if (rid == RID())
				rid = btree.search((void *)((byte *)data + offset));

			btree.remove((void *)((byte *)data + offset));
			columns[i].index_root = btree.get_root();
		}
		offset += columns[i].length;
	}

	Record record(stor, record_root);
	record.remove_record(rid);
}

std::vector<void *> Schema::select_record(int operand, const char * column_title, void * key)
{
	if (opened == false)
		throw logic_error("table not opened");

	vector<void *> vecs;

	int offset = 0;
	for (int i = 0; i < n_columns; i++)
	{
		if (strncmp(columns[i].title, column_title, 32) == 0)
		{
			if (columns[i].index_root < 0)
				return vecs;

			index->open_table(record_root, columns[i].index_root);
			vecs = index->select_record(operand, key);
			index->close_table(record_root, columns[i].index_root);
			return vecs;
		}
		offset += columns[i].length;
	}
	return vecs;
}

void Schema::replace_record(void * origin_data, void * modified_data)
{
	//
}

int Schema::count()
{
	if (opened == false)
		throw logic_error("table not opened");

	int i;
	for (i = 0; i < n_columns; i++)
		if (columns[i].index_root >= 0)
			break;

	if (i != n_columns)
		return select_record(OP_AL, columns[i].title, NULL).size();
	return -1;
}
