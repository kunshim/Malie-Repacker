#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <thread>
#include <atomic>
#include "Entry.h"
using namespace std;
class Unpacker
{
public:
	Unpacker()
	{
		processorCount = thread::hardware_concurrency() - 2;
		if (processorCount % 2 != 0) 
		{
			processorCount - 1;
		}
	}
	void decryptBlock(uint8_t* buf, uint32_t start, uint32_t end);
	void unpack(const char* path); //unpack single file
	uint8_t* unpackOnlyData(const char* path);
	uint8_t* getDataOffset();
	int* getOffsetTable();
private:
	void decryptRange(uint32_t start, uint32_t end);
	int* offsetTable;
	uint8_t* dataPtr;
	uint8_t* dataOffset;
	atomic<uint32_t> decryptedSize;
	uint32_t key[0x34] = { 0x37723369,0x42676D2B,0x7A323621,0x71793961,0x525CEF2C,0xD9F91F97,0x1B99C5F1,0xFD26B8D0,0x19B4A133,0xB695BD19,0x1B10B8BC,0x9CB09BB9,0x77966CFC,0x8FCB8DCC,0xE2F8FE93,0x5C68292E,0x367E47E5,0xC6E6717C,0x7F49AE34,0x14973BCB,0xEDA56F46,0x46C42E2F,0x272C26EE,0x466D284C,0x23F2E373,0x38BE3FA4,0x13772336,0x942676D2,0x71B99C5F,0x1FD26B8D,0x0525CEF2,0xCD9F91F9,0x46C42E2F,0x272C26EE,0x466D284C,0xEDA56F46,0x5C5E4E58,0x4DDC8CDA,0x5099DB4A,0xDE8C8D88,0x7F49AE34,0x14973BCB,0x367E47E5,0xC6E6717C,0x9CB09BB9,0x19B4A133,0xB695BD19,0x1B10B8BC,0x5C68292E,0x77966CFC,0x8FCB8DCC,0xE2F8FE93 };
	unsigned int fileSize;
	uint8_t* data;
	
	vector<bool> elementsBit;
	unsigned int processorCount;
	void decrypt();
	void extract(const char* name);
	void extractInner(Entry* entry);
	uint32_t getBlockSize()
	{
		uint32_t blockSize = fileSize / processorCount;
		if (blockSize % 0x100 == 0)
		{
			return blockSize;
		}
		else
		{
			return ((blockSize + 0x1000) >> 8) << 8; // padding 0x100
		}
	}

};