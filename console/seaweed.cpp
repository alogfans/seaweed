// seaweed.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The file for seaweed console app, implemented a
// simple SQL-like interface

#include "seaweed.h"
#include <iostream>
using namespace std;

Seaweed::Seaweed(const char * db_name)
{
	stor.open_file(db_name);
	schema = new Schema(stor);	
}


Seaweed::~Seaweed()
{
	stor.close_file();
	delete schema;
}


void Seaweed::create_db(const char * db_name)
{
	Storage static_stor;
	static_stor.create_file(db_name);
	static_stor.open_file(db_name);
	Schema static_schema(static_stor);
	static_schema.initize_schema();
	// flush
	static_stor.dirty = true;
	static_stor.close_file();
}

void Seaweed::unlink_db(const char * db_name)
{
	Storage static_stor;
	static_stor.destory_file(db_name);
}


void Seaweed::create_table(const char * tb_name, std::vector<TableProperty> &properties)
{
	int n_columns = properties.size();

	Column * columns = new Column[n_columns];
	for (int i = 0; i < n_columns; i++)
	{
		strncpy(columns[i].title, properties[i].title, 32);
		columns[i].type = properties[i].type;
		columns[i].length = properties[i].length;
	}
	schema->create_table(tb_name, n_columns, columns);

	for (int i = 0; i < n_columns; i++)
		if (properties[i].is_index)
    		schema->create_index(tb_name, properties[i].title);

    delete [] columns;
}

void Seaweed::get_table_schema(const char * tb_name, vector<TableProperty> &properties)
{
	for (int i = 0; i < schema->n_columns; i++)
	{
		properties.push_back(TableProperty(schema->columns[i].title, schema->columns[i].type, 
			schema->columns[i].index_root >= 0, schema->columns[i].length));
	}
}

void Seaweed::drop_table(const char * tb_name)
{
	drop_table(tb_name);
}

byte * Seaweed::marshall(vector<Value> &values)
{
	byte * packet;
	int length = 0;
	for (int i = 0; i < values.size(); i++)
		length += values[i].length;
	packet = new byte[length];

	int offset = 0;
	for (int i = 0; i < values.size(); i++)
	{
		memcpy(packet + offset, values[i].v, values[i].length);
		offset += values[i].length;
	}

	return packet;
}

void Seaweed::unmarshall(byte * packet, vector<Value> &values)
{
	int offset = 0;
	for (int i = 0; i < schema->n_columns; i++)
	{
		switch (schema->columns[i].type)
		{
		case IntegerType:
			values.push_back(Value(*(int *) (packet + offset)));
			break;
		case RealType:
			values.push_back(Value(*(float *) (packet + offset)));
			break;
		case CharType:
			values.push_back(Value(packet + offset, (int) schema->columns[i].length));
			break;		
		}
		offset += schema->columns[i].length;
	}
}

void Seaweed::insert_record(const char * tb_name, std::vector<Value> &values)
{
	schema->open_table(tb_name);
	byte * packet = marshall(values);
	schema->insert_record(packet);
	schema->close_table();
	delete [] packet;
}


int Seaweed::delete_record(const char * tb_name, Condition &condition)
{
	vector<Row> rows;
	int affected = select_record(tb_name, condition, rows);
	schema->open_table(tb_name);
	for (int i = 0; i < affected; i++)
	{
		byte * packet = marshall(rows[i]);
		schema->delete_record(packet);
		delete [] packet;
	}
	schema->close_table();
	return affected;
}


int Seaweed::select_record(const char * tb_name, Condition &condition, vector<Row> &rows)
{
    //std::vector<void *> select_record(int operand, const char * column_title, void * key);
	int affected = 0;
	schema->open_table(tb_name);
	//byte * packet = marshall(condition.compared_value);
	vector<void *> raw_returned = schema->select_record(condition.operand,
		condition.property_name, condition.compared_value.v);
	affected = raw_returned.size();
	for (int i = 0; i < affected; i++)
	{
		vector<Value> dummy;
		unmarshall((byte *)raw_returned[i], dummy);
		rows.push_back(dummy);
	}
	schema->close_table();
	//delete [] packet;
	return affected;
}

void Seaweed::replace_record(const char * tb_name, std::vector<Value> &origin_values, 
	std::vector<Value> &modified_values)
{
	schema->open_table(tb_name);

	byte * packet = marshall(origin_values);
	schema->delete_record(packet);
	delete [] packet;

	packet = marshall(modified_values);
	schema->insert_record(packet);
	delete [] packet;

	schema->close_table();
}
