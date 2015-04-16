// hashtable.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of hashtable class.

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

struct HashEntry
{
    HashEntry *prev;
    HashEntry *next;
    
    int fd;
    int page_num;
    int slot;
};

class HashTable
{
public:
    HashTable(int buckets);
    ~HashTable();

    int find(int fd, int page_num);
    void insert(int fd, int page_num, int slot);
    void remove(int fd, int page_num);

private:
    int hash(int fd, int page_num) {
        return (fd + page_num) % n_buckets;
    }
    int n_buckets;
    HashEntry ** hash_entries;
};

#endif
