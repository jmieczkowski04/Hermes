#pragma once

#include "GenericTypes.h"

#define HERMES_HEADER_SIGNATURE 0x53544141424D414D // "MAMBAATS"

struct HHFHeader
{
	uint64 signature;
	uint32 entryTableHash;
	uint32 entries;
};

struct HHFEntry
{
	uint64 path;
	uint64 hash;
	uint32 size;
	uint32 sizeNoCompression;
	uint32 offset;
	uint32 flags;
};