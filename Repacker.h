#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <thread>
using namespace std;
class Repacker
{
public:
	Repacker()
	{
		processorCount = thread::hardware_concurrency() - 2;
		if (processorCount % 2 != 0)
		{
			processorCount - 1;
		}
	}
	void repack(const char* path);
private:
	void decryptBlock(uint32_t start, uint32_t end);
	FILE* repacked;
	char* path;
	uint8_t* data;
	int processorCount;
	//void encrypt();
	//void encryptRange(uint32_t start, uint32_t end, int threadId);
	//void merge();
	//void mergeInner();
	uint32_t key[0x34] = { 0x37723369,0x42676D2B,0x7A323621,0x71793961,0x525CEF2C,0xD9F91F97,0x1B99C5F1,0xFD26B8D0,0x19B4A133,0xB695BD19,0x1B10B8BC,0x9CB09BB9,0x77966CFC,0x8FCB8DCC,0xE2F8FE93,0x5C68292E,0x367E47E5,0xC6E6717C,0x7F49AE34,0x14973BCB,0xEDA56F46,0x46C42E2F,0x272C26EE,0x466D284C,0x23F2E373,0x38BE3FA4,0x13772336,0x942676D2,0x71B99C5F,0x1FD26B8D,0x0525CEF2,0xCD9F91F9,0x46C42E2F,0x272C26EE,0x466D284C,0xEDA56F46,0x5C5E4E58,0x4DDC8CDA,0x5099DB4A,0xDE8C8D88,0x7F49AE34,0x14973BCB,0x367E47E5,0xC6E6717C,0x9CB09BB9,0x19B4A133,0xB695BD19,0x1B10B8BC,0x5C68292E,0x77966CFC,0x8FCB8DCC,0xE2F8FE93 };
};