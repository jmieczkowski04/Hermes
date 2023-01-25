#include <iostream>
#include "HermesRead.h"

int main()
{
    const char* h1 = ""; //name of hhf
    const char* h2 = ""; //name of haf
    FILE* hhf;
    FILE* haf;
    fopen_s(&hhf, h1, "rb");
    
    if (!hhf)
    {
        std::cout << "Failed to open " << h1 << " !";
        return 1;
    }

    fopen_s(&haf, h2, "rb");

    if (!haf)
    {
        std::cout << "Failed to open " << h2 << " !";
        fclose(hhf);
        return 1;
    }

    //Ok, we've got files

    HermesReader reader(hhf, haf);

    uint8* file;
    std::string fName(""); // name of file to search
    reader.LoadFile(fName, file);
    if (file == 0)
        std::cout << "File " << fName << " not found";


    fclose(hhf);
    fclose(haf);
}


