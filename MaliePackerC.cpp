// MaliePackerC.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include "Unpacker.h"
#include "Repacker.h"
int main(int argc, wchar_t** argv)
{
    char path[0x80];

    Unpacker unpacker;
    if (argc >= 2)
    {
        DWORD result = GetFileAttributesW(argv[1]);
        if (result == FILE_ATTRIBUTE_DIRECTORY)
        {
            puts("Repack");
            //Repack
        }
        else
        {
            puts("Unpack");
            unpacker.unpack((char*)argv[1]);
        }
        return 0;
    }
    //unpacker.unpack("C:\\Users\\kunsh\\Desktop\\data\\msvcrt.dll");
    getchar();
    Repacker repacker;
    repacker.repack("C:\\Users\\kunsh\\Desktop\\data\\data8");
}


