// storage_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive storage structure in /inc/storage.h.

#include "storage.h"
#include <iostream>
#include "gtest/gtest.h"
using namespace std;

TEST(storage_test, bitmap)
{
    // function test
    Slot slot;
    for (int i = 0; i < 8; i++)
    {
        set_slot_property(slot, i);
        EXPECT_EQ(get_slot_property(slot, i), 1);
        clear_slot_property(slot, i);
        EXPECT_EQ(get_slot_property(slot, i), 0);        
    }
}

TEST(storage_test, header)
{
    EXPECT_EQ(sizeof(StorageHeader), SizeOfStorageHeader);
}

TEST(storage_test, basic)
{
    Storage storage;
    //storage.destory_file("test.db");
    storage.create_file("test.db");
    EXPECT_THROW(storage.create_file("test.db"), logic_error);
    storage.open_file("test.db");
    // check the intergrity
    EXPECT_STREQ("Seaweed DB File", storage.shadow.magic);
    for (int i = 0; i < 1000; i++)
        EXPECT_EQ(storage.bitmap[i], 0);

    // Manually hack some bit and save and reopen to check it
    storage.shadow.bitmap_checksum = 0xdeaddead;
    set_slot_property(storage.bitmap[0], 7);
    storage.dirty = true;

    storage.close_file();
    storage.open_file("test.db");
    EXPECT_EQ(storage.shadow.bitmap_checksum, 0xdeaddead);
    EXPECT_EQ(get_slot_property(storage.bitmap[0], 7), true);
    storage.close_file();
    storage.destory_file("test.db");
}

TEST(storage_test, page_ops)
{
    Storage storage;
    storage.create_file("test.db");
    storage.open_file("test.db");

    int n1 = storage.acquire_page();
    EXPECT_EQ(n1, 0);
    int n2 = storage.acquire_page();
    EXPECT_EQ(n2, 1);
    EXPECT_EQ(storage.shadow.allot_pages, 2);
    EXPECT_EQ(storage.shadow.used_pages, 2);
    storage.release_page(n2);
    EXPECT_EQ(storage.shadow.allot_pages, 2);
    EXPECT_EQ(storage.shadow.used_pages, 1);
    n2 = storage.acquire_page();
    EXPECT_EQ(storage.shadow.allot_pages, 2);
    EXPECT_EQ(storage.shadow.used_pages, 2);
    EXPECT_EQ(n2, 1);

    byte *ptr = storage.get_page_content(0);
    strcpy(ptr, "Hello World!");

    storage.update_page_content(n1, ptr);
    delete ptr;
    storage.release_page(n2);
    storage.close_file();

    storage.open_file("test.db");
    ptr = storage.get_page_content(n1);
    EXPECT_STREQ(ptr, "Hello World!");
    EXPECT_THROW(storage.get_page_content(n2), logic_error);
    EXPECT_THROW(storage.get_page_content(155), domain_error);
    storage.close_file();

    storage.destory_file("test.db");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
