#pragma once

#include "HermesHeaderFile.h"
#include "Hash.h"
#include "Compression.h"

#include <stdio.h>
#include <stdexcept>
#include <string>


class HermesReader
{
private:
	FILE* haf;
	FILE* hhf;
	HHFEntry* entries;
	uint32 entryCount;

public:
	HermesReader(FILE* file1, FILE* file2);

	void LoadFile(std::string name, uint8** pointer);
private:
	int32 LookForEntry(uint64 name);

};