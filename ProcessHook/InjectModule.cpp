#include <stdio.h>
#include <Windows.h>
#include "InjectModule.h"
#include "PETools.h"
#include "IATHook.h"
#include "InlineHook.h"

/************************************
	被注入程序的地址(仅在Debug模式下编译通过的程序才能正确Hook)
************************************/
wchar_t pFilePath[] = L"C:/Users/12269/source/repos/Test/Debug/Test.exe";

// 远程注入代码入口处
DWORD entry(void* pImageBuffer) {
	// 修复IAT表
	repairIatImage(pImageBuffer);
	// 初始化IAT Hook
	initIATHook();
	// 初始化Inline Hook
	initInlineHook();

	//创建FileMapping对象					
	HANDLE hMapObject = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 10, TEXT("shared"));
	//将FileMapping对象映射到自己的进程					
	HANDLE hMapView = MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);

	while (true) {
		switch (*(char*)hMapView) {
		case HOOK_MESSAGE_BOX:
			setIATHook();
			break;

		case UNHOOK_MESSAGE_BOX:
			unSetIATHook();
			break;

		case HOOK_SUM:
			setInlineHook();
			break;

		case UNHOOK_SUM:
			unsetInlineHook();
			break;

		case CALL_MESSAGE_BOX:
			callMessageBox();
			break;

		case CALL_SUM:
			callSum();
			break;
		}

		*(char*)hMapView = 0;
		Sleep(100);
	}

	return 0;
}

void injectModule() {
	// 获取ImageBase
	HMODULE imageBase = GetModuleHandle(NULL);
	// 获取SizeOfImage
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader32 = getOptionalHeader32((void*)imageBase);
	size_t sizeOfImage = pOptionalHeader32->SizeOfImage;

	// 复制自身到新内存
	void* pImageBuffer = malloc(sizeOfImage);
	memcpy(pImageBuffer, (void*)imageBase, sizeOfImage);

	// 启动要注入的进程
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	CreateProcess(pFilePath, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	// 申请远程进程内存
	size_t newImageBase = (size_t)VirtualAllocEx(pi.hProcess, NULL, sizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// 修复重定位表
	repairRelocationImage(pImageBuffer, newImageBase);

	// 写入远程进程
	WriteProcessMemory(pi.hProcess, (void*)newImageBase, pImageBuffer, sizeOfImage, NULL);

	// 启动远程线程，入口为entry，参数为newImageBase
	CreateRemoteThread(pi.hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)((size_t)entry - (size_t)imageBase + newImageBase), (void*)newImageBase, 0, NULL);

	free(pImageBuffer);
}