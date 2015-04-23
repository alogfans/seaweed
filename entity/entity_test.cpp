// entity_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive storage structure in /inc/entity.h.

#include "entity.h"
#include <iostream>
#include "gtest/gtest.h"
using namespace std;

TEST(entity, test1)
{
    EntityProperty pro;
    pro.type = 1;
    pro.length = 4;
    byte * mem = new byte[4096];
    Entity et;
    et.init_entity(1, &pro, mem);
    int data;
    for (int i = 0; i < 989; i++)
    {
        data = i;
        int loc = et.insert_record(&data);
        EXPECT_EQ(loc, i);

        int *rec = (int *) et.attain_record(loc);
        EXPECT_EQ(*rec, i);
    }
    data = 5555555;
    et.replace_record(105, &data);
    int *rec = (int *) et.attain_record(105);
    EXPECT_EQ(*rec, data);

    et.remove_record(10);
    {
        data = 555;
        int loc = et.insert_record(&data);
        EXPECT_EQ(loc, 10);
    }
    ASSERT_THROW(et.insert_record(&data), logic_error);
}

TEST(entity, test2)
{
    // file ops. mem mgr not used carefully!
    EntityProperty pro;
    pro.type = 1;
    pro.length = 4;
    byte * mem = new byte[4096];
    Entity et;
    et.init_entity(1, &pro, mem);
    int data;
    for (int i = 0; i < 989; i ++)
    {
        data = i;
        int loc = et.insert_record(&data);
        EXPECT_EQ(loc, i);

        int *rec = (int *) et.attain_record(loc);
        EXPECT_EQ(*rec, i);
    }
    et.remove_record(988);
    
    et.close_entity();
    et.open_entity(mem);
    for (int i = 0; i < 987; i++)
    {
        int *rec = (int *) et.attain_record(i);
        EXPECT_EQ(*rec, i);
    }    
    ASSERT_THROW(et.attain_record(988), logic_error);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
