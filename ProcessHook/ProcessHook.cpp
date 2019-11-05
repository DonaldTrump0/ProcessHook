#include <stdio.h>
#include <Windows.h>
#include <locale>
#include "resource.h"
#include "InjectModule.h"

bool isMessageBoxMonitored = false;
bool isSumMonitored = false;
char* hMapView = NULL;

BOOL CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;

	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_MESSAGE_BOX_MONITOR:
			isMessageBoxMonitored = !isMessageBoxMonitored;
			if (isMessageBoxMonitored) {
				*hMapView = HOOK_MESSAGE_BOX;
				SetWindowText(GetDlgItem(hDlg, IDC_BUTTON_MESSAGE_BOX_MONITOR), L"Close Monitor");
			}
			else {
				*hMapView = UNHOOK_MESSAGE_BOX;
				SetWindowText(GetDlgItem(hDlg, IDC_BUTTON_MESSAGE_BOX_MONITOR), L"Open Monitor");
			}
			return TRUE;

		case IDC_BUTTON_SUM_MONITOR:
			isSumMonitored = !isSumMonitored;
			if (isSumMonitored) {
				*hMapView = HOOK_SUM;
				SetWindowText(GetDlgItem(hDlg, IDC_BUTTON_SUM_MONITOR), L"Close Monitor");
			}
			else {
				*hMapView = UNHOOK_SUM;
				SetWindowText(GetDlgItem(hDlg, IDC_BUTTON_SUM_MONITOR), L"Open Monitor");
			}
			return TRUE;

		case IDC_BUTTON_MESSAGE_BOX_CALL:
			*hMapView = CALL_MESSAGE_BOX;
			return TRUE;

		case IDC_BUTTON_SUM_CALL:
			*hMapView = CALL_SUM;
			return TRUE;
		}
	}
	return FALSE;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// 设置本地字符集
	setlocale(LC_ALL, "");

	// 通过内存写入方式注入模块，实现模块隐藏
	injectModule();

	// 创建FileMapping对象
	HANDLE hMapObject = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 10, TEXT("shared"));
	// 将FileMapping对象映射到自己的进程
	hMapView = (char*)MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);

	DialogBox(hInstance, (LPCWSTR)IDD_DIALOG_MAIN, NULL, DialogProc);
	return 0;
}