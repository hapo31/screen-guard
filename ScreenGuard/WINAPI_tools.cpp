#pragma once
#include"WINAPI_tools.h"
#include"defs.h"

#include<cstdio>
#include<string>

extern HWND processHwnd;
extern DWORD processPid;

extern int mainScreenX;
extern int mainScreenY;

extern bool autorun_flag;

//指定したプロセスIDのウインドウハンドルを取得
HWND GetWindowHandleByPID(DWORD pid)
{
	typedef struct pid_and_hwnd
	{
		//IN
		DWORD pid;
		//OUT
		HWND hwnd = NULL;
		pid_and_hwnd(DWORD pid) :pid(pid){}
	}
	PIDANDHWND, *LPPIDANDHWND;

	PIDANDHWND ph = pid;
	
	auto enumproc = [](HWND hwnd, LPARAM lp) -> BOOL
	{
		//無理やりLPARAMに変換した構造体へのアドレスをキャストして分かっているpidを取得する
		DWORD pid_ = ((LPPIDANDHWND) lp)->pid;
		DWORD dwId;

		GetWindowThreadProcessId(hwnd, &dwId);

		//目的のpidを持つハンドルが見つかった
		if (pid_ == dwId)
		{
			//構造体に無理やり変換してハンドルを渡す
			((LPPIDANDHWND) lp)->hwnd = hwnd;
			return false;
		}
		return true;
	};//lambda
	EnumWindows(enumproc, (LPARAM) &ph);
	return ph.hwnd;
}

//RECTの大きさのウィンドウが、HWNDに指定したウィンドウの中心に来るような座標を取得する
bool GetCenterFromOwnerWindow(HWND owner, LPCRECT in_cwr, LPRECT out_cwr)
{
	RECT oRect;
	
	if (GetWindowRect(owner, &oRect) == FALSE) return false;

	long owidth = oRect.right - oRect.left;
	long oheight = oRect.bottom - oRect.top;

	long width = in_cwr->right - in_cwr->left;
	long height = in_cwr->bottom - in_cwr->top;

	//大きさが取れなかったらお帰り願う
	if (width == 0 || height == 0) return false;

	out_cwr->left = owidth / 2 - width / 2;
	out_cwr->right = out_cwr->left + width;
	out_cwr->top = oheight / 2 - height / 2;
	out_cwr->bottom = out_cwr->top + height;

	return true;
}

BOOL ShowTaskBar(HWND tray, HWND start)
{
	ShowWindow(tray, SW_RESTORE);
	ShowWindow(start, SW_RESTORE);

	return TRUE;
}

BOOL HideTaskBar(HWND tray, HWND start)
{
	ShowWindow(tray, SW_HIDE);
	ShowWindow(start, SW_HIDE);

	return TRUE;
}

DWORD WaitProcess(HWND hwMain, const char* name, const char* cmdline)
{

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	{
		char drive[6];
		char dir[128];
		char path[256];
		_splitpath_s(name, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
		sprintf_s(path, sizeof(path), "%s%s", drive, dir);
		SetCurrentDirectoryA(path);
	}
	if (!CreateProcessA(name, const_cast<char*>(cmdline), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS,
		NULL, NULL, &si, &pi))
	{
		//CreateProcessに失敗するとWM_ULUNCHPROCESS_ERRORを投げる
		PostMessage(hwMain, WM_ULUNCHPROCESS_ERROR, 0, 0);
		return 0;
	}

	::processPid = pi.dwProcessId;
	HWND hwnd = ::processHwnd = GetWindowHandleByPID(pi.dwProcessId);

	PostMessage(hwMain, WM_ULUNCHPROCESS_START, (WPARAM)pi.hProcess, (LPARAM) pi.dwProcessId);
	CloseHandle(pi.hThread);

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);

	ClipCursor(NULL);
	::processHwnd = NULL;
	::processPid = 0;
	PostMessage(hwMain, WM_ULUNCHPROCESS_CLOSE, 0, 0);

	return 0;
}

//オープンダイアログに必要な情報のセット
bool SetOPENFILENAME(OPENFILENAMEA &ofn, HWND ownerHwnd,char *filter_buf,size_t filter_size,  const char* ext, std::string& path_buf, std::string& name_buf)
{
	path_buf.resize(128);
	name_buf.resize(128);

	//\0入ってるとちゃんと出来ないっぽいので\\をあとから\0に置き換える感じで
	sprintf_s(filter_buf, filter_size, "%sファイル(*.%s)\\*.%s\\\\", ext, ext, ext);
	for (size_t i = 0; i < filter_size; ++i)
	{
		if (filter_buf[i] == '\\') filter_buf[i] = '\0';
	}

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ownerHwnd;
	ofn.lpstrFilter = filter_buf;
	ofn.lpstrDefExt = ext;

	ofn.Flags = OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST;

	ofn.lpstrFile = &path_buf[0];
	ofn.nMaxFile = path_buf.size();
	ofn.lpstrFileTitle = &name_buf[0];
	ofn.nMaxFileTitle = name_buf.size();

	return true;
}

//ファイルオープンダイアログでファイルパスを取得する
bool GetFilenameByOpenDialog(HWND ownerHwnd,  const char* ext, std::string& path_buf, std::string& name_buf)
{
	OPENFILENAMEA ofn = {0};
	char fill[128];
	SetOPENFILENAME(ofn, ownerHwnd, fill, sizeof(fill), ext, path_buf, name_buf);
	ofn.lpstrTitle = "ファイルを開く";
	return GetOpenFileNameA(&ofn) == TRUE;
}

//ファイルセーブダイアログでファイルパスを取得する
bool GetFilenameBySaveDialog(HWND ownerHwnd, const char* ext, std::string& path_buf, std::string& name_buf)
{
	OPENFILENAMEA ofn = {0};
	char fill[128];
	SetOPENFILENAME(ofn, ownerHwnd, fill, sizeof(fill), ext, path_buf, name_buf);
	ofn.lpstrTitle = "ファイルを保存";
	return GetSaveFileNameA(&ofn) == TRUE;
}

HWND create_window(HWND owner, LPCTSTR caption, LPCTSTR classname, int style, int x, int y, int nWidth, int nHeight, HMENU id, LPARAM lp)
{
	return CreateWindow(classname, caption, style, x, y, nWidth, nHeight, owner, (HMENU)(INT_PTR)id, GetModuleHandle(NULL),NULL);
}

HWND create_window(HWND owner, LPCSTR caption, LPCSTR classname, int style, int x, int y, int nWidth, int nHeight, HMENU id, LPARAM lp)
{
	return CreateWindowA(classname, caption, style, x, y, nWidth, nHeight, owner, (HMENU) (INT_PTR) id, GetModuleHandle(NULL), NULL);
}

HWND create_button(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int id)
{
	return create_window(owner, caption, TEXT("BUTTON"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, nWidth, nHeight, (HMENU) id);
}

HWND create_textbox(HWND owner, int x, int y, int nWidth, int nHeight, int id)
{
	return create_window(owner, nullptr, TEXT("EDIT"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, x, y, nWidth, nHeight, (HMENU) id);
}

HWND create_textbox(HWND owner, LPCTSTR caption,int x, int y, int nWidth, int nHeight, int id)
{
	return create_window(owner, caption, TEXT("EDIT"), WS_CHILD | WS_VISIBLE | WS_BORDER |  ES_AUTOHSCROLL | ES_LEFT, x, y, nWidth, nHeight, (HMENU) id);
}

HWND create_textbox(HWND owner, LPCSTR caption, int x, int y, int nWidth, int nHeight, int id)
{
	return create_window(owner, caption, "EDIT", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, x, y, nWidth, nHeight, (HMENU) id);
}

HWND create_checkbox(HWND owner,  LPCTSTR caption ,int x, int y, int nWidth, int nHeight, int id)
{
	return create_window(owner, caption, TEXT("BUTTON"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, nWidth, nHeight, (HMENU)id);
}

HWND create_static(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int id, int style)
{
	return create_window(owner, caption, TEXT("STATIC"), WS_CHILD | WS_VISIBLE | style, x, y, nWidth, nHeight, (HMENU) id);
}

