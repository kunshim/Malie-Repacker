#pragma once
#include "Repacker.h"
#include "camellia.c"

/*
리패키징 과정 요약
1. 기존 .dat 파일에서 데이터 영역 전까지 복호화
2. 기존 .dat 파일에서 헤더 복사
3. 기존 .dat 파일에서 존재하는 파일 목록에서 오프셋 순으로 정렬
4. 오프셋 테이블 수정 및 데이터 추가
5. 암호화 
*/
void Repacker::repack(const char* path)
{
	char buf[0x80];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s.repack.dat", path);
	this->path = (char*)path;
	printf("Start repack %s\n", path);
	repacked = fopen(buf, "wb");
	if (repacked == nullptr)
	{
		printf("Failed to make file %s\n", buf);
		return;
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s.dat", path);
	FILE* original = fopen(buf, "rb");
	if (original == nullptr)
	{
		printf("Failed to open original data file %s\n", buf);
		return;
	}
	data = new uint8_t[0x100];
	fread(data, 1, 0x100, original);
	
}

void Repacker::decryptBlock(uint32_t start, uint32_t end) //decrypt data
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

