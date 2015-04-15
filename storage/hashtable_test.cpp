// hashtable_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive hashtable class.

#include "hashtable.h"
#include "gtest/gtest.h"
#include "stdexcept"
using namespace std;

TEST(hashtable_test, simple_condition)
{
    HashTable *hashtable = new HashTable(4);
    EXPECT_EQ(hashtable->find(1, 5), -1);
    hashtable->insert(1, 5, 7);
    EXPECT_EQ(hashtable->find(1, 5), 7);
    hashtable->insert(2, 4, 8);
    hashtable->remove(1, 5);
    EXPECT_EQ(hashtable->find(1, 5), -1);
    delete hashtable;
}

TEST(hashtable_test, invaild_buckets)
{
    ASSERT_THROW(new HashTable(-1), domain_error);
}

TEST(hashtable_test, multiple_insert)
{
    HashTable *hashtable = new HashTable(4);
    hashtable->insert(1, 5, 8);
    // Attempt to duplicate input, will throw exception
    EXPECT_THROW(hashtable->insert(1, 5, 15), logic_error);
    // Attempt to delete invaild item, will throw exception
    EXPECT_THROW(hashtable->remove(2, 4), logic_error);
    delete hashtable;
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


