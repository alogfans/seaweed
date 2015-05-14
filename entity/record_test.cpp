// record_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive storage structure in /inc/record.h

#include "generic.h"
#include "record.h"
#include "storage.h"
#include <iostream>
#include "gtest/gtest.h"
using namespace std;

TEST(entity, test1)
{
    EntityProperty pro;
    pro.type = 1;
    pro.length = 4;

    Storage stor;
    stor.create_file("test.db");
    stor.open_file("test.db");

    Record rec(stor);
    rec.init_entity(1, &pro);
    for (int i = 0; i <= 2000; i++)
    {
        RID rid = rec.insert_record(&i);
        EXPECT_EQ(rid.page_num, i / 988);
        EXPECT_EQ(rid.slot_num, i % 988);
    }
    for (int i = 0; i <= 2000; i++)
    {
        RID rid(i / 988, i % 988);
        EXPECT_EQ(*(int *)rec.attain_record(rid), i);
    }
    for (int i = 0; i <= 2000; i++)
    {
        RID rid(i / 988, i % 988);
        rec.remove_record(rid);
        //cout << "removed " << i << endl;
    }
    for (int i = 0; i <= 2000; i++)
    {
        RID rid = rec.insert_record(&i);
        EXPECT_EQ(rid.page_num, i / 988);
        EXPECT_EQ(rid.slot_num, i % 988);
    }
    stor.close_file();
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
