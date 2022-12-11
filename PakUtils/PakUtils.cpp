#include <iostream>
#include <stdio.h>
#include <filesystem>
#include "tinyxml2.h"

#include "HermesHeaderFile.h"
#include "Compression.h"
#include "Hash.h"

namespace tx =  tinyxml2;

namespace fs = std::filesystem;

uint16 GetCompression(const char* method);
int CompareEntries(const void* a, const void* b);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Error: expected argument format \nPakUtils.exe <manifest.xml>";
        return 1;
    }

    FILE* manifest;
    fopen_s(&manifest, argv[1], "rb");

    if (!manifest)
    {
        std::cout << "There was problem opening " << argv[1] << ". Make sure file exist!";
        return 1;
    }


    tx::XMLDocument xmlMan;
    if (xmlMan.LoadFile(manifest) != tx::XML_SUCCESS)
    {
        std::cout << "Error parsing XML structure!";
        fclose(manifest);
        return 1;
    }
    
    fclose(manifest);

    tx::XMLElement* root = xmlMan.RootElement();
    
    const tx::XMLAttribute * nameAttr = root->FindAttribute("Name");

    const char* fileName = nameAttr->Value();

    /*
    Before parsing do some path setup
    */

    fs::path manifestRoot(argv[1]);
    manifestRoot.remove_filename();

    fs::path hhfPath(manifestRoot);
    hhfPath.replace_filename(fileName);
    hhfPath.replace_extension(".hhf");
    

    fs::path hafPath(hhfPath);
    hafPath.replace_extension(".haf");

    FILE* hafFile;
    fopen_s(&hafFile, hafPath.string().c_str(), "wb");
    if (!hafFile)
    {
        std::cout << "Error creating file: " << hafPath << " !";
        return 1;
    }

    /*
    Process every element
    */

    std::vector<HHFEntry> entries;

    for (tx::XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {   
        tx::XMLElement* orginalPath = child->FirstChildElement("path");
        tx::XMLElement* compression = child->FirstChildElement("compression");
        tx::XMLElement* outputPath = child->FirstChildElement("output");
        if (!(orginalPath && compression && outputPath))
        {
            std::cout << "Error parsing file: make sure every file has complete entry!";
            fclose(hafFile);
            return 1;
        }

        const char* cstrOrginalPath = orginalPath->GetText();
        const char* cstrCompression = compression->GetText();
        const char* cstrOutputPath = outputPath->GetText();

        if (!(cstrOrginalPath && cstrCompression && cstrOutputPath))
        {
            std::cout << "Error parsing file: make sure every file has complete entry!";
            fclose(hafFile);
            return 1;
        }
        
        //read data

        fs::path lPath(manifestRoot);
        lPath.replace_filename(cstrOrginalPath);

        FILE* f;
        fopen_s(&f, lPath.string().c_str(), "rb");

        if (!f)
        {
            std::cout << "Error opening file: " << lPath;
            fclose(hafFile);
            return 1;
        }
        fseek(f, 0, SEEK_END);

        int32 size = ftell(f);

        fseek(f, 0, SEEK_SET);
        
        uint8* data = new uint8[size];

        fread(data, sizeof(uint8), size, f);

        fclose(f);

        //calculate and compress data

        uint16 compressor = GetCompression(cstrCompression);

        std::string stringPath(cstrOutputPath);
        uint64 path = Hash64((uint8*)stringPath.data(), (uint8*)stringPath.data() + stringPath.size());

        uint32 compressedSize = GetCompressionMaxSize(compressor, size);

        uint8* targetData = new uint8[compressedSize];

        uint32 realSize = Compress(compressor, data, targetData, size, compressedSize);

        delete[] data;

        uint32 hafPos = ftell(hafFile);

        uint64 dataHash = Hash64(targetData, targetData + realSize);

        uint32 flags = compressor;

        HHFEntry entry = {};
        entry.flags = flags;
        entry.hash = dataHash;
        entry.offset = hafPos;
        entry.path = path;
        entry.size = realSize;
        entry.sizeNoCompression = size;

        fwrite(targetData, sizeof(uint8), realSize, hafFile);

        entries.push_back(entry);
    }

    fclose(hafFile);

    uint32 entriesCount = entries.size();

    HHFEntry* entriesTable = new HHFEntry[entriesCount];

    memcpy(entriesTable, entries.data(), sizeof(HHFEntry) * entriesCount);

    qsort(entriesTable, entriesCount, sizeof(HHFEntry), CompareEntries);

    HHFHeader header = {};
    header.entries = entriesCount;
    header.entryTableHash = Hash32((uint8*)entriesTable, (uint8*)(entriesTable + entriesCount));
    header.signature = HERMES_HEADER_SIGNATURE;

    FILE* hhfFile;

    fopen_s(&hhfFile, hhfPath.string().c_str(), "wb");

    if (!hhfFile)
    {
        std::cout << "Error creating hhf file: " << hhfPath;
        return 1;
    }

    fwrite(&header, sizeof(header), 1, hhfFile);
    fwrite(entriesTable, sizeof(HHFEntry), entriesCount, hhfFile);

    fclose(hhfFile);

    return 0;
}


uint16 GetCompression(const char* method)
{
    std::string m(method);
    uint32 h = Hash32((uint8*)m.data(), (uint8*)m.data() + m.size());
    if (h == 0x93fd699b)
        return (uint16)Compressor::LZ4;
    else
        return (uint16)Compressor::None;
}

int CompareEntries(const void* a, const void* b)
{
    const HHFEntry* x = (HHFEntry*)a;
    const HHFEntry* y = (HHFEntry*)b;

    if (x->path > y->path)
        return 1;
    else if (x->hash < y->hash)
        return -1;
    return 0;
}