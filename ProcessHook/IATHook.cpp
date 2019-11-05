#include <stdio.h>
#include <Windows.h>
#include "PETools.h"
#include "IATHook.h"

typedef int (WINAPI* pMessageBox)(HWND, LPCWSTR, LPCWSTR, UINT);

size_t oldProcAddr = 0;
size_t newProcAddr = 0;
size_t* hookAddr = 0;

int WINAPI HookMessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
	printf("***************************IAT Hook***************************\n");
	wprintf(L"args: %d %s %s %d\n", (size_t)hWnd, lpText, lpCaption, uType);
	int ret = ((pMessageBox)oldProcAddr)(hWnd, lpText, lpCaption, uType);
	printf("ret: %d\n", ret);
	printf("***************************IAT Hook***************************\n");
	return ret;
}

// 在被注入进程内部初始化
void initIATHook() {
	oldProcAddr = (size_t)MessageBox;
	newProcAddr = (size_t)HookMessageBox;

	HANDLE pImageBuffer = GetModuleHandle(NULL);

	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader32 = getOptionalHeader32(pImageBuffer);
	size_t dataDirectoryRva = pOptionalHeader32->DataDirectory[1].VirtualAddress;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((size_t)pImageBuffer + dataDirectoryRva);
	if (!pImportDescriptor) {
		printf("没有导入表\n");
		return;
	}

	while (pImportDescriptor->FirstThunk) {
		size_t* thunk = (size_t*)((size_t)pImageBuffer + pImportDescriptor->FirstThunk);
		while (*thunk) {
			if (*thunk == oldProcAddr) {
				hookAddr = thunk;
				// 设置内存读写属性，不设置oldProtext则无法正确执行VirtualProtect
				DWORD oldProtext = 0;
				VirtualProtect(thunk, 4, PAGE_EXECUTE_READWRITE, &oldProtext);
				return;
			}
			thunk++;
		}
		pImportDescriptor++;
	}
}

void setIATHook() {
	*hookAddr = newProcAddr;
}

void unSetIATHook() {
	*hookAddr = oldProcAddr;
}

void callMessageBox() {
	((pMessageBox)*hookAddr)(0, 0, 0, 0);
}
