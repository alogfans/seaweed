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
typedef unsigned int uint64_t;
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
const uint32_t UnknownType = 0;
const uint32_t IntegerType = 1;
const uint32_t RealType = 2;	// double
const uint32_t CharType = 3;

// Page Type
const uint32_t EntityPage = 1;
const uint32_t OverflowPage = 2;
const uint32_t InternalPage = 3;
const uint32_t LeafPage = 4;

// Operand
const uint32_t OP_NA = 0;
const uint32_t OP_EQ = 1;	// equal, include string type
const uint32_t OP_LT = 2;	// less than
const uint32_t OP_GT = 3;	// great than
const uint32_t OP_LE = 4;	// less or equal
const uint32_t OP_GE = 5;	// great or equal
const uint32_t OP_NE = 6;	// not equal

// If using buffer componment, set this def.
// Remove this define will use direct-write technique.
#define CONFIG_USING_BUFFER

#endif
