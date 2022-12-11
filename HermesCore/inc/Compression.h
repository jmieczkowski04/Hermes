#pragma once

#include "GenericTypes.h"

enum class Compressor : uint16
{
	None = 0x0,
	LZ4 = 0x1
};

uint32 GetCompressionMaxSize(uint16 compressor, uint32 srcSize);

uint32 Compress(uint16 compressor, uint8* src, uint8* dest, uint32 srcSize, uint32 destSize);

uint32 Decompress(uint16 compressor, uint8* src, uint8* dest, uint32 srcSize, uint32 destSize);