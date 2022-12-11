#include "HermesRead.h"

HermesReader::HermesReader(FILE* file1, FILE* file2) : hhf(file1), haf(file2), entries(0)
{
	HHFHeader header = {};
	fread(&header, sizeof(HHFHeader), 1, hhf);
	if (header.signature != HERMES_HEADER_SIGNATURE)
		throw new std::runtime_error("Wrong format");
	entryCount = header.entries;

	entries = new HHFEntry[entryCount];
	fread(entries, sizeof(HHFEntry), entryCount, hhf);
	if (Hash32((uint8*)entries, (uint8*)(entries + entryCount)) != header.entryTableHash)
		throw new std::runtime_error("Header file is corupted!");
}

void HermesReader::LoadFile(std::string name, uint8** pointer)
{
	uint64 fileName = Hash64((uint8*)name.data(), (uint8*)name.data() + name.size());
	int32 entry = LookForEntry(fileName);
	if (entry == -1)
	{
		return;
	}

	uint32 offset = entries[entry].offset;
	uint32 realSize = entries[entry].size;

	uint8* bytes = new uint8[realSize];

	fseek(haf, offset, SEEK_SET);
	fread(bytes, sizeof(uint8), realSize, haf);

	uint16 c = entries[entry].flags;
	uint32 size = entries[entry].sizeNoCompression;
	(*pointer) = new uint8[size];

	uint32 sizeNC = Decompress(c, bytes, *pointer, realSize, size);
	if (sizeNC != size)
	{
		throw new std::runtime_error("Something wrong with compression");
	}
	delete[] bytes;
}


int32 HermesReader::LookForEntry(uint64 name)
{
	int32 l = 0;
	int32 r = entryCount;
	while (l <= r)
	{
		int32 m = l + (r - l) / 2;
		if (entries[m].path == name)
		{
			return m;
		}


		if (entries[m].path < name)
			l = m + 1;
		else
			r = m - 1;
	}
	return -1;
}

