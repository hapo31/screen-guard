#pragma once
#include"defs.h"
#include"resource.h"
#include"wndproc.h"

#include<Windows.h>
#include<string>

HWND processHwnd = NULL;
DWORD processPid = 0;

bool autorun_flag = false;

int mainScreenX, mainScreenY;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc = { 0 };

	std::string cmdline = lpCmdLine;

	HWND executed_hwnd = FindWindow(WNDCLASSNAME, NULL);

	if (executed_hwnd != NULL)
	{
		SetActiveWindow(executed_hwnd);
		return 0;
	}

	wc.hInstance = hInstance;
	wc.lpfnWndProc = wndproc;
	wc.lpszClassName = WNDCLASSNAME;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH) (COLOR_3DDKSHADOW + 1);

	//コマンドライン引数に-aオプションがある場合はオートランモード
	//(メニュー非表示、自動で起動及び再起動)
	if (cmdline.find("-a") != std::string::npos)
	{
		::autorun_flag = true;
		wc.lpszMenuName = NULL;
	}
	else
	{
		::autorun_flag = false;
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	}
	
	if (FAILED(RegisterClass(&wc))) return 1;

	int scx = 5  + GetSystemMetrics(SM_CXSCREEN);
	int scy = 30 + GetSystemMetrics(SM_CXSCREEN);

	::mainScreenX = scx;
	::mainScreenY = scy;

	auto hwnd = CreateWindow(wc.lpszClassName, TEXT("ScreenGuard"), WS_OVERLAPPEDWINDOW, -5, -20, scx, scy, 0, 0, hInstance, 0);
	if (hwnd == NULL) return 1;
	MSG msg;

	ShowWindow(hwnd, SW_MAXIMIZE);

	while (GetMessage(&msg,NULL,0,0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}