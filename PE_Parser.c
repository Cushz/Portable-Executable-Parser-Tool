#include <stdio.h>
#include <windows.h>

BOOL ReadPEFile(LPCSTR lpFileName, PBYTE *pointerToPE, SIZE_T *sizeOfPE)
{
    HANDLE hFile = NULL;
    PBYTE allocatedBuffer = NULL;
    DWORD fileSize = NULL;
    DWORD numberOfBytesRead = NULL;
    // Using CreateFile to get the handle to the desired file
    hFile = CreateFileA(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile)
    {
        printf("failed to open file:%d\n", GetLastError());
        return FALSE;
    }
    // Getting file size in order to allocate memory
    fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE)
    {
        printf("failed to get the size of the file:%d\n", GetLastError());
        return FALSE;
    }

    // allocating memory to copy the contents of the file
    allocatedBuffer = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
    if (allocatedBuffer == NULL)
    {
        printf("failed to allocate memory:%d\n", GetLastError());
        return FALSE;
    }

    // Reading file and copying its contents to allocated buffer
    if (!ReadFile(hFile, allocatedBuffer, fileSize, &numberOfBytesRead, NULL) || numberOfBytesRead != fileSize)
    {
        printf("failed to read the file:%d\n", GetLastError());
        return FALSE;
    }

    *pointerToPE = allocatedBuffer;
    *sizeOfPE = fileSize;
    return TRUE;
}

int main()
{

    PBYTE pPE;
    SIZE_T sPE;
    char full_path[MAX_PATH];
    printf("                    _    __   _           _____    ______     _____\n");
    printf("     /\\            (_)  / _| ( )         |  __ \\  |  ____|   |  __ \\\n");
    printf("    /  \\     _ __   _  | |_  |/   ___    | |__) | | |__      | |__) |   __ _   _ __   ___    ___   _ __\n");
    printf("   / /\\ \\   | '__| | | |  _|     / __|   |  ___/  |  __|     |  ___/   / _` | | '__| / __|  / _ \\ | '__|\n");
    printf("  / ____ \\  | |    | | | |       \\__ \\   | |      | |____    | |      | (_| | | |    \\__ \\ |  __/ | |\n");
    printf(" /_/    \\_\\ |_|    |_| |_|       |___/   |_|      |______|   |_|       \\__,_| |_|    |___/  \\___| |_|\n");
    printf("\n\n\n");
    printf("Please enter the full executable path:");
    scanf("%s", full_path);
    // Reading the file
    ReadPEFile(full_path, &pPE, &sPE);

    // Reading DOS Header by typcasting pointer to dos header
    PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pPE;
    if (pImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        printf("image DOS signature is invalid:%d\n", GetLastError());
        return 1;
    }

    // Reading NT header by using lfanew + pPE(base address)
    PIMAGE_NT_HEADERS pImageNTHeader = (PIMAGE_NT_HEADERS)(pPE + pImageDosHeader->e_lfanew);
    if (pImageNTHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        printf("image NT signature is invalid:%d\n", GetLastError());
        return 1;
    }

    // Reading File header (as it is member of nt header, we will use struct itself instead of pointer to the struct)
    printf("\n---------------FILE HEADER------------------\n");
    IMAGE_FILE_HEADER ImageFileHeader = pImageNTHeader->FileHeader;
    printf("Size of Optional Header:%hu Bytes\n", ImageFileHeader.SizeOfOptionalHeader);
    printf("Number of Sections:%hu\n", ImageFileHeader.NumberOfSections);
    printf("Image architecture:%s\n", ImageFileHeader.Machine == IMAGE_FILE_MACHINE_I386 ? "x32" : "x64");

    // Reading Optional Header
    printf("\n---------------OPTIONAL HEADER------------------\n");
    IMAGE_OPTIONAL_HEADER ImageOptionalHeader = pImageNTHeader->OptionalHeader;
    if (ImageOptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
    {
        printf("image optional signature is invalid:%d\n", GetLastError());
        return 1;
    }
    printf("Size of Code:%lu\n", ImageOptionalHeader.SizeOfCode);
    printf("Major and Minor Linker version:%d AND %d\n", ImageOptionalHeader.MajorLinkerVersion, ImageOptionalHeader.MinorLinkerVersion);
    printf("Size of Initialized data section:%lu\n", ImageOptionalHeader.SizeOfInitializedData);
    printf("SIze of Uninitialized data section:%lu\n", ImageOptionalHeader.SizeOfUninitializedData);
    printf("Address to Entry point in RVA:%lu(in decimals)\n", ImageOptionalHeader.AddressOfEntryPoint);
    printf("Address of Entry point in VA:0x%p\n", pPE + ImageOptionalHeader.AddressOfEntryPoint);
    printf("Address of the code section in RVA:%lu\n", ImageOptionalHeader.BaseOfCode);
    printf("Address of the code section in VA:0x%p\n", pPE + ImageOptionalHeader.BaseOfCode);
    printf("Address of the ImageBase:0x%p\n", ImageOptionalHeader.ImageBase);
    printf("Major and Minor Operating system version:%d AND %d\n", ImageOptionalHeader.MajorOperatingSystemVersion, ImageOptionalHeader.MinorOperatingSystemVersion);

    // Getting list of directories
    printf("\n-------------------DATA DIRECTORIES--------------------------\n");
    printf("EXPORT DIRECTORY\n");
    // 1. Export directory
    PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(pPE + ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    printf("Export directory Address:0x%p\n", pImageExportDirectory);
    printf("Export directory size:%d\n", ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
    printf("Export directory RVA Address:0x%0.8X\n", ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    PIMAGE_IMPORT_DESCRIPTOR IATable = (PIMAGE_IMPORT_DESCRIPTOR)(pPE + ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    printf("Import directory Address:0x%p\n", IATable);
    printf("Import directory size:%d\n", ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size);
    printf("Import directory RVA Address:0x%0.8X\n", ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    printf("\n---------------------SECTIONS----------------------------------\n");
    // In here we are starting to calculate from the beginning of the NT headers section
    PIMAGE_SECTION_HEADER pImageSectionHeader = (PIMAGE_SECTION_HEADER)((PBYTE)pImageNTHeader + sizeof(IMAGE_NT_HEADERS));
    for (size_t i = 0; i < ImageFileHeader.NumberOfSections; i++)
    {
        printf("[#] Section Name:%s\n", (char *)pImageSectionHeader->Name);
        printf("[#] Virtual Address:%p\n", (PVOID)pPE + pImageSectionHeader->VirtualAddress);
        pImageSectionHeader = (PIMAGE_SECTION_HEADER)((PBYTE)pImageSectionHeader + (DWORD)sizeof(IMAGE_SECTION_HEADER));
    }

    return 0;
}