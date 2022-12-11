#include "Compression.h"
#include "lz4.h"

#include <cstring>

uint32 GetCompressionMaxSize(uint16 compressor, uint32 srcSize)
{
	switch (static_cast<Compressor>(compressor))
	{
	case Compressor::None:
		return srcSize;
		break;
	case Compressor::LZ4:
		return LZ4_compressBound(srcSize);
		break;
	default:
		return 0;
	} 
	return 0;
}

uint32 Compress(uint16 compressor, uint8* src, uint8* dest, uint32 srcSize, uint32 destSize)
{
	switch (static_cast<Compressor>(compressor))
	{
	case Compressor::None:
		memcpy(dest, src, srcSize);
		return srcSize;
	case Compressor::LZ4:
		return LZ4_compress_default((const char*)src, (char*)dest, srcSize, destSize);
		break;
	default:
		return 0;
	}
}

uint32 Decompress(uint16 compressor, uint8* src, uint8* dest, uint32 srcSize, uint32 destSize)
{
	switch (static_cast<Compressor>(compressor))
	{
	case Compressor::None:
		memcpy(dest, src, srcSize);
		return srcSize;
	case Compressor::LZ4:
		return LZ4_decompress_safe((const char*)src, (char*)dest, srcSize, destSize);
		break;
	default:
		return 0;
	}
}