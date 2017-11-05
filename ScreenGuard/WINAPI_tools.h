#pragma once
#include<Windows.h>
#include<string>

//指定したプロセスIDのウインドウハンドルを取得
HWND GetWindowHandleByPID(DWORD pid);
//タスクバーの表示
BOOL ShowTaskBar(HWND tray, HWND start);
//タスクバーの非表示
BOOL HideTaskBar(HWND tray, HWND start);

DWORD WaitProcess(HWND hwMain, const char* name, const char* cmdline);

//RECTサイズのウィンドウが、HWNDに指定したウィンドウの中心に来るような座標を取得する
//ただしこの関数で取得したRECTをそのままSetRectするのはよくないので、topとleftのみ使用するようにすること
bool GetCenterFromOwnerWindow(HWND owner, LPCRECT childWindowRect, LPRECT newChildWindowRect);

//ファイルオープンダイアログでファイルパスを取得する
bool GetFilenameByOpenDialog(HWND ownerHwnd, const char* ext, std::string& path_buf, std::string& name_buf);
//ファイルセーブダイアログでファイルパスを取得する
bool GetFilenameBySaveDialog(HWND ownerHwnd, const char* ext, std::string& path_buf, std::string& name_buf);

//ういんどうをつくる
HWND create_window(HWND owner, LPCTSTR classname, LPCTSTR caption, int style, int x, int y, int nWidth, int nHeight, HMENU Id , LPARAM lp = 0);
HWND create_window(HWND owner, LPCSTR classname, LPCSTR caption, int style, int x, int y, int nWidth, int nHeight, HMENU Id, LPARAM lp = 0);

//ボタンを作る
HWND create_button(HWND owner, LPCTSTR caption , int x,int y,int nWidth,int nHeight, int Id);

//テキストボックスを作る
HWND create_textbox(HWND owner, int x, int y, int nWidth, int nHeight, int Id);
HWND create_textbox(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int Id);
HWND create_textbox(HWND owner, LPCSTR caption, int x, int y, int nWidth, int nHeight, int Id);

//チェックボックスを作る
HWND create_checkbox(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int id);

//スタティックコントロール(文字列)を作る
HWND create_static(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int id, int style = 0);