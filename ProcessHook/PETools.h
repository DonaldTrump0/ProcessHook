#pragma once

PIMAGE_DOS_HEADER getDosHeader(void* pFileBuffer);
PIMAGE_NT_HEADERS getNTHeader(void* pFileBuffer);
PIMAGE_FILE_HEADER getFileHeader(void* pFileBuffer);
PIMAGE_OPTIONAL_HEADER32 getOptionalHeader32(void* pFileBuffer);
PIMAGE_SECTION_HEADER getFirstSectionHeader(void* pFileBuffer);
PIMAGE_SECTION_HEADER getLastSectionHeader(void* pFileBuffer);

void dbgPrintf(const wchar_t* format, ...);

// rva转换为fa
void* rvaToFa(void* pFileBuffer, size_t rva);

// 获取对应的数据目录表
void* getDataDirectory(void* pFileBuffer, size_t index);

// 读文件
size_t readFile(const wchar_t* pFilePath, void** ppFileBuffer);

// 拉伸
size_t copyFileBufferToImageBuffer(void* pFileBuffer, void** ppImageBuffer);

// 修复拉伸后的重定位表
bool repairRelocationImage(void* pImageBuffer, size_t newImageBase);

// 修复拉伸后的IAT表
bool repairIatImage(void* pImageBuffer);

