#pragma once
class Entry
{
public:
	char* name;
	bool isFile;
	uint32_t offset;
	uint32_t size;
	uint32_t childCount;
	Entry** childs;
	Entry* parent;
	Entry(uint8_t* data)
	{
		childs = nullptr;
		parent = nullptr;
		if (data == nullptr)
			return;
		name = (char*)data;
		isFile = data[0x16] == 1;
		offset = ((uint32_t*)data)[0x18/4];
		if (isFile)
		{
			childCount = 0;
			size = ((uint32_t*)data)[0x1c/4];
		}
		else
		{
			childCount = ((uint32_t*)data)[0x1c/4];
			childs = (Entry**)malloc(sizeof(Entry*) * childCount);
			size = 0;
		}

	}
	~Entry()
	{
		free(childs);
	}
	char* getFullPath()
	{
		char* buffer = new char[0x80];
		char* parents[0x10];
		buffer[0] = 0;
		Entry* entry = this;
		int i = 0;
		for (i = 0; entry != nullptr; i++, entry = entry->parent)
		{
			parents[i] = entry->name;
		}
		for (int j = i - 1 ; j >= 0; j--)
		{
			strcat(buffer, parents[j]);
			strcat(buffer,"\\");
		}
		return buffer;
	}

	

};