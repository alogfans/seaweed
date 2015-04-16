// hashtable.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation file of hashtable class.

#include "hashtable.h"
#include <stdexcept>
using namespace std;

HashTable::HashTable(int buckets) : n_buckets(buckets)
{
	if (n_buckets < 0)
		throw domain_error("Invaild buckets");
    hash_entries = new HashEntry * [n_buckets];
    for (int i = 0; i < n_buckets; i++)
    {
        hash_entries[i] = NULL;
    }
}

HashTable::~HashTable()
{
    for (int i = 0; i < n_buckets; i++)
    {
        HashEntry *entry = hash_entries[i];
        while (entry != NULL)
        {
            HashEntry *next = entry->next;
            delete entry;
            entry = next;
        }
    }
    delete [] hash_entries;
}

// Find: according to fd and page_num given by user, try to find whether
// a proper slot is vaild. If not found, will return -1.
int HashTable::find(int fd, int page_num)
{
    int key = hash(fd, page_num);
    // key is in region between 0 to n_buckets - 1, so overflow is impossible
    HashEntry *entry = hash_entries[key];
    while (entry != NULL)
    {
        if (entry->fd == fd && entry->page_num == page_num)
            return entry->slot;

        entry = entry->next;
    }
    // finally nothing is found.
    return -1;
}

void HashTable::insert(int fd, int page_num, int slot)
{
    int key = hash(fd, page_num);

    if (find(fd, page_num) != -1)
        throw logic_error("fd page_num pair existed");
    
    HashEntry * entry = new HashEntry;
    if (entry == NULL)
        throw overflow_error("out of memory");
    
    entry->prev = NULL;
    entry->next = hash_entries[key];
    entry->fd = fd;
    entry->page_num = page_num;
    entry->slot = slot;
    if (hash_entries[key] != NULL)
        hash_entries[key]->prev = entry;

    hash_entries[key] = entry;
}

void HashTable::remove(int fd, int page_num)
{
    int key = hash(fd, page_num);

    HashEntry *entry = hash_entries[key];
    while (entry != NULL)
    {
        if (entry->fd == fd && entry->page_num == page_num)
            break;
            
        entry = entry->next;
    }

    if (entry == NULL)
        throw logic_error("entry not found");

    if (entry == hash_entries[key])
        hash_entries[key] = entry->next;
    if (entry->prev != NULL)
        entry->prev->next = entry->next;
    if (entry->next != NULL)
        entry->next->prev = entry->prev;
    delete entry;
}


