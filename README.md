# Hermes

Light packaging engine for readonly files. Solution contains three projects: HermesCore, PakUtils and HermesReadExample.
HermesCore provides structures, enums and few functions used for processing data. PakUtils is utility to build archive from manifest file and HermesReadExample is a simple demo that shows how you could implement archive reader in your engine.

## Description

Engine pack files into two files: .hhs (Hermes Header File) and .haf (Hermes Archive File).

Hermes Header File contain lookup table for data in Hermes Archive File. HAF file is plain data processed earlier by PakUtils and combained into big file. Offset, sizes(with and without compression), hash and flags are stored in HHF lookup table. PakUtils at the end of packagin process sort the whole table by path to allow usage of binary search during reading.

### PakUtils Manifest
PakUtils manifest is XML file with root element `manifest` that have attribute `Name`, which is filename of archive(extensions will be added automaticly). All subelements are `File` and must contain all of these elements:
`path` - relative path from manifest path
`compression` - compression method. Currently only support None or LZ4
`output` - name which will be used in lookup table in runtime

### HermesCore API
#### HermesHeaderFile.h
`HERMES_HEADER_SIGNATURE` definition - contain a magic value for HHF header. Default "MAMBAATS"
`HHFHeader` struct - have 64 bit signature, 32 bit hash of lookup table and 32 bit number of entries
`HHFEntry` struct - have 64 bit hash of path, 64 bit hash of data, 32 bit size of data, 32 bit size of data without compression, 32 bit offest and 32 bit flags
16 bits in flags are reserved for compression

#### Hash.h
`uint32 Hash32(uint8* memStart, uint8* memEnd)` provides 32 bit hash of data
`uint64 Hash64(uint8* memStart, uint8* memEnd)` provides 64 bit hash of data

#### Compression.h
`enum Compressor` - provides avaliable compressors
`uint32 GetCompressionMaxSize(uint16 compressor, uint32 srcSize)` the biggest possible size after compression(usually much lower than this number, but it is used to allocate buffer)
`uint32 Compress(uint16 compressor, uint8* src, uint8* dest, uint32 srcSize, uint32 destSize)` compress using selected method. Buffers must be allocated. Returns number of used bytes in destBuffer
`uint32 Decompress(uint16 compressor, uint8* src, uint8* dest, uint32 srcSize, uint32 destSize);` analog to the previous one, but insted of compressing decompress


## Installing
Before building project you have to install dependencies via [vcpkg](https://github.com/microsoft/vcpkg). There are three packages to install: xxHash, LZ4 and TinyXML2.
