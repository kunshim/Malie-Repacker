#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Unpacker.h"
#include "Entry.h"
#include "camellia.c"
#include <windows.h>
#include <functional>
#include <direct.h>	
/*
언패키징 과정 요약
1. .dat 파일 복호화 (camellia-128)
2. 파일 개수 및, 엔트리 개수 추출
3. 데이터 영역 위치 찾기
4. 엔트리 및 파일 추출(재귀)
*/
inline char* getFileName(const char* path)
{
	int len = strlen(path);
	char* name = (char*)path;
	for (int i = 0; i < len; i++)
	{
		if (path[i] == '\\')
			name = (char*)&path[i];
		if (path[i] == '.' )
			break;
	}
	return name;
}

uint8_t* Unpacker::getDataOffset()
{
	return dataOffset;
}

int* Unpacker::getOffsetTable()
{
	return offsetTable;
}
void Unpacker::unpack(const char* path)
{
	FILE* fp = fopen(path, "rb");
	//FILE* second = fopen(path, "w");
	if (fp == nullptr)
	{
		printf("Can't find file %s\n", path);
		return;
	}
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	data = new(std::nothrow)uint8_t[fileSize];
	if (data == nullptr)
	{
		printf("Failed to allocate memory (%#x bytes require)\n", fileSize);
		return;
	}
	printf("Read %s into memory\n", path);
	fread(data, 1, fileSize, fp);
	decrypt();
	extract(getFileName(path));
	delete[] data;
	fclose(fp);
}

uint8_t* Unpacker::unpackOnlyData(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (fp == nullptr)
	{
		printf("Can't find file %s\n", path);
		return;
	}
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	data = new(std::nothrow)uint8_t[fileSize];
	if (data == nullptr)
	{
		printf("Failed to allocate memory (%#x bytes require)\n", fileSize);
		return;
	}
	printf("Read %s into memory\n", path);
	fread(data, 1, fileSize, fp);
	decrypt();
	return data;
}

void gotoxy(int x, int y) {
	//x, y 좌표 설정
	COORD pos = { x,y };
	//커서 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void Unpacker::decrypt()
{
	printf("Decrypt start\n");
	thread **threadList = new thread*[processorCount];
	uint32_t blockSize = getBlockSize();
	for (int i = 0; i < processorCount; i++)
	{
		thread* t;
		if (i != processorCount - 1)
		{
			auto func = std::bind(&Unpacker::decryptRange, this, i * blockSize, (i + 1) * blockSize);
			if (i != processorCount)
				t = new thread(func);
			else
				t = new thread(func);
		}
		else
		{
			auto func = std::bind(&Unpacker::decryptRange, this, i * blockSize, fileSize);
			if (i != processorCount)
				t = new thread(func);
			else
				t = new thread(func);
		}
		
		threadList[i] = t;
	}
	for (int i = 0; i < processorCount; i++)
	{
		threadList[i]->join();
		delete threadList[i];
	}
	delete[]threadList;
	printf("Decrypt end\n");
}


void Unpacker::decryptRange(uint32_t start, uint32_t end) //decrypt data
{
	int idx = 0x10;
	uint8_t* plaintext = new uint8_t[0x10];
	uint8_t* dataPtr = &data[start];
	uint8_t* endPtr = &data[end];
	while (dataPtr != endPtr)
	{
		*(uint32_t*)dataPtr = _rotl(*(uint32_t*)dataPtr, idx);
		*(uint32_t*)(dataPtr + 4) = _rotr(*(uint32_t*)(dataPtr + 4), idx);
		*(uint32_t*)(dataPtr + 8) = _rotl(*(uint32_t*)(dataPtr + 8), idx);
		*(uint32_t*)(dataPtr + 12) = _rotr(*(uint32_t*)(dataPtr + 12), idx);
		idx++;
		if (idx == 0x20)
			idx = 0x10;
		Camellia_DecryptBlock(128, dataPtr, key, plaintext);
		memcpy(dataPtr, plaintext, 0x10);
		dataPtr += 0x10;
	}
}

void Unpacker::decryptBlock(uint8_t* buf,uint32_t start, uint32_t end) //decrypt data
{
	int idx = 0x10;
	uint8_t* plaintext = new uint8_t[0x10];
	uint8_t* dataPtr = &buf[start];
	uint8_t* endPtr = &buf[end];
	while (dataPtr != endPtr)
	{
		*(uint32_t*)dataPtr = _rotl(*(uint32_t*)dataPtr, idx);
		*(uint32_t*)(dataPtr + 4) = _rotr(*(uint32_t*)(dataPtr + 4), idx);
		*(uint32_t*)(dataPtr + 8) = _rotl(*(uint32_t*)(dataPtr + 8), idx);
		*(uint32_t*)(dataPtr + 12) = _rotr(*(uint32_t*)(dataPtr + 12), idx);
		idx++;
		if (idx == 0x20)
			idx = 0x10;
		Camellia_DecryptBlock(128, dataPtr, key, plaintext);
		memcpy(dataPtr, plaintext, 0x10);
		dataPtr += 0x10;
	}
}

int readInt(uint8_t** ptr)
{
	int result = **(int**)ptr;
	*ptr += 4;
	return result;
}



void Unpacker::extract(const char* name)
{
	Entry* topEntry = new Entry(nullptr);
	char temp[0x20];
	strncpy(temp, name + 1, 15);
	temp[strlen(temp) - 4] = 0;
	topEntry->name = temp;
	printf("Start extract %s\n",temp);
	_mkdir(temp);
	dataPtr = data;
	if (strncmp((char*)dataPtr, "LIBP", 4) != 0)
	{
		printf("Invalid magic number\n");
		return;
	}
	dataPtr += 4;
	int elementCount = readInt(&dataPtr);
	int fileCount = readInt(&dataPtr);
	elementsBit = vector<bool>(elementCount, false);
	offsetTable = (int*)(0x10 + elementCount * 0x20 + data);
	dataOffset = (uint8_t*)(0x10 + elementCount * 0x20) + fileCount * 4;
	dataOffset = (uint8_t*)(((int)dataOffset + 0x3ff) & (~0x3ff)); //align 0x400 
	dataPtr = 0x30 + data;
	uint8_t* savedDataPtr;
	for (int i = 0; i < elementCount; i++)
	{
		int idx = ((dataPtr - data) - 0x30) / 0x20;
		if (elementsBit[idx])
			continue;
		elementsBit[idx] = true;
		Entry* child = new Entry(dataPtr);
		child->parent = topEntry;
		dataPtr += 0x20;
		savedDataPtr = dataPtr;
		if (!child->isFile)
		{
			char* path = child->getFullPath();
			_mkdir(path);
			delete path;
			extractInner(child);
		}
		delete child;
		dataPtr = savedDataPtr;
	}
	delete topEntry;
	printf("Extract end\n");
}

void Unpacker::extractInner(Entry* parent)
{
	uint8_t* savedDataPtr;
	int originalLen = 0;
	char path[0x80];
	char* tempPath = parent->getFullPath(); 
	strncpy(path, tempPath, sizeof(path));
	originalLen = strlen(path);
	delete tempPath;
	dataPtr = parent->offset * 0x20 + 0x10 + data;
	int idx = ((dataPtr - data) - 0x30) / 0x20;
	if (elementsBit[idx])
		return;
	elementsBit[idx] = true;
	for (int i = 0; i < parent->childCount; i++)
	{
		idx = ((dataPtr - data) - 0x30) / 0x20;
		elementsBit[idx] = true;
		Entry* child = new Entry(dataPtr);
		child->parent = parent;
		dataPtr += 0x20;
		savedDataPtr = dataPtr;
		parent->childs[i] = child;
		path[originalLen] = 0; 
		strcat(path, child->name); //이후 다시 경로 제작 
		if (!child->isFile)
		{
			_mkdir(path);
			extractInner(child);
		}
		else
		{
			FILE* fp = fopen(path, "wb");
			child->offset = (offsetTable[child->offset] << 10) + (int)dataOffset;
			fwrite(data + child->offset, 1, child->size, fp);
			fclose(fp);
		}
		dataPtr = savedDataPtr;
		delete child;

	}
}