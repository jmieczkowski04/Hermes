#include "Hash.h"
#include "xxhash.h"


uint32 Hash32(uint8* memStart, uint8* memEnd)
{
	size_t size = memEnd - memStart;

	static XXH32_hash_t seed = 0;
	XXH32_hash_t hash = XXH32(memStart, size, seed);
	uint32 out = hash;
	return out;
}

uint64 Hash64(uint8* memStart, uint8* memEnd)
{
	size_t size = memEnd - memStart;

	XXH64_hash_t hash = XXH3_64bits(memStart, size);
	uint64 out = hash;
	return out;
}