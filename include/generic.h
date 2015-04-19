// generic.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of generic definitions.

#ifndef __GENERIC_H__
#define __GENERIC_H__

// DO NOT INCLUDE ANY OTHER FILES!
typedef unsigned int uint32_t;
typedef char byte;

#ifndef NULL
#define NULL 0
#endif

const uint32_t SizeOfPage = 4096;
const uint32_t SizeOfStorageHeader = 64;
const char MagicString[] = "Seaweed DB File";

const uint32_t SizeOfBitmap = 1024;
typedef byte Slot;

// Supported the following data type
const uint32_t Unknown = 0;
const uint32_t Integer = 1;
const uint32_t Real = 2;	// double
const uint32_t Char = 3;

// If using buffer componment, set this def.
// Remove this define will use direct-write technique.
#define CONFIG_USING_BUFFER

#endif
