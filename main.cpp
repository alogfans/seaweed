// main.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The file for seaweed console app, implemented a
// simple SQL-like interface

#include "seaweed.h"
#include <vector>
#include <iostream>
#include <exception>

using namespace std;

int main()
{
	Seaweed::create_db("hello.db");
	Seaweed inst("hello.db");

	vector<TableProperty> property;
	property.push_back(TableProperty("EmployeeId", IntegerType, true));
	property.push_back(TableProperty("Name", CharType, false, 32));
	property.push_back(TableProperty("Score", RealType, true));

	inst.create_table("empolyee", property);
	for (int i = 0; i < 400; i++)
	{
		//cout << "Success filling." << endl;
		vector<Value> values;
		values.push_back(Value(i));
		char text[] = "BEI BAO YAN LA! BEI BAO YAN LA!";
		values.push_back(Value(text, 32));
		values.push_back(Value(i * (float)1.0));
		inst.insert_record("empolyee", values);
	}

	vector<Row> feedback;
	Condition cond("EmployeeId", OP_GT, Value(300));
	int affected = inst.select_record("empolyee", cond, feedback);
	cout << "Affected rows = " << affected << endl;
	return 0;
}