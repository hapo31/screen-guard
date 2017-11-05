#pragma once
#include"wndproc.h"
#include"defs.h"
#include"config_man.h"
#include"WINAPI_tools.h"
#include"resource.h"

#include<string>
#include<thread>

using tstring = std::basic_string<TCHAR>;

extern HWND processHwnd;
extern DWORD processPid;

extern int mainScreenX;
extern int mainScreenY;

extern bool autorun_flag;

HRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static config cfg;
	static std::thread th;
	static HWND trayHwnd = FindWindow(TEXT("Shell_TrayWnd"), NULL);
	static HWND startHwnd = FindWindowEx(trayHwnd, NULL, TEXT("Button"), NULL);

	static STATE state = WAIT;
	
	//このプログラムの終了フラグ
	static bool program_closing = false;
	//起動プロセスの終了フラグ
	static bool process_closing = false;

	//ウィンドウをセンターに移動済みか否か
	static bool set_center = false;
	static std::string loaded_filename;

	//起動プロセスのプロセスID
	static DWORD s_processPid;
	//起動プロセスのプロセスハンドル
	static HANDLE s_processHandle;

	switch (msg)
	{

	case WM_CREATE:
		if (((LPCREATESTRUCT) (lp))->hwndParent == NULL)
		{
			{
				//std::ifstream fs("data.dat", std::ios::binary | std::ios::in);
				//BYTE byte;
				loaded_filename = "config.ini";
		             		//while ( fs && byte = fs.get())

			}
			SetCursor(LoadCursor(nullptr, IDC_ARROW));

			//最前面に移動　タスクバーの上に覆いかぶさるようにウインドウモードを変更
			SetWindowLong(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
			//SetWindowLong(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUPWINDOW);

			//コンフィグ用窓の作成
			cfg.create_config_window(hwnd);

			//終了用ホットキーの登録
			RegisterHotKey(hwnd, HOTKEY_CODE, MOD_CONTROL, 'M');

			//各種システムキーを無効化
			RegisterHotKey(hwnd, HOTKEY_CODE + 1, MOD_ALT, VK_TAB);
			RegisterHotKey(hwnd, HOTKEY_CODE + 2, MOD_WIN, 0);
			RegisterHotKey(hwnd, HOTKEY_CODE + 3, MOD_ALT, VK_F4);

			//コンフィグファイルの読み込み	
			if (cfg.get_errcode() == config::code::NO_WORK) cfg.loadConfig(loaded_filename.c_str());
			//もし成功しなかったら
			if (cfg.get_errcode() != config::code::SUCCESS)
			{
				switch (cfg.get_errcode())
				{
				case config::code::NOTFOUND_FILE:
					cfg.create_ConfigFile(loaded_filename.c_str(),true);

					//ファイルを新しく生成			
					if (cfg.get_errcode() == config::code::CREATE_FILE)
					{
						MessageBox(hwnd, TEXT("configファイルが見つからなかったため、新しく生成しました。"), WNDCLASSNAME, MB_ICONINFORMATION);
					}
					//ファイルオープンエラー
					else if (cfg.get_errcode() == config::code::FAILED_FILEOPEN)
					{
						MessageBox(hwnd, TEXT("新しくconfigファイルを生成しようとしましたが、ファイルがオープン出来ませんでした。"), WNDCLASSNAME, MB_ICONINFORMATION);
						DestroyWindow(hwnd);
					}

					break;
					//書式エラー
				case config::code::INVALID_ITEM:
					MessageBox(hwnd, TEXT("configファイルが不正です。書式を確認して下さい。"), WNDCLASSNAME, MB_ICONINFORMATION);
					DestroyWindow(hwnd);
					break;
				default:
					MessageBox(hwnd, TEXT("不明なエラーです 開発者に問い合わせてください"), WNDCLASSNAME, MB_ICONINFORMATION);
					DestroyWindow(hwnd);
					break;
				}
			}
			//正常処理
			else
			{
				HideTaskBar(trayHwnd, startHwnd);
				if (::autorun_flag)
					SendMessage(hwnd, WM_RUN_PROCESS, 0, 0);
			}

		}
		return 0;

		//自身がアクティブ化されたら起動プロセスを最前面にする
	case WM_ACTIVATE:
		if (state != STATE::WAIT)
		{
			HWND hw = GetWindowHandleByPID(s_processPid);
			//SetWindowText(hw, TEXT("<(^o^)>┌┛’,;’;≡三[SiON氏はホモ]三≡’,;’;┗┐<(^o^)>"));
			if (s_processPid == 0)
			{
				OutputDebugStringA("processHwndがNULL\n");
			}
			else
			{
				//親ウインドウ化を試みる

				if (GetParent(hw) != hwnd)
				{
					SetParent(hw, hwnd);
					RECT rc, nrc;
					GetWindowRect(hw, &rc);
					if (GetCenterFromOwnerWindow(hwnd, &rc, &nrc))
						SetWindowPos(hw, HWND_TOP, nrc.left, nrc.top, 0, 0, SWP_NOSIZE);
				}
				UpdateWindow(hw);
			}
		}
		return 0;
	case WM_COMMAND:
		if (lp == 0)
		{
			switch (LOWORD(wp))
			{
				//プロファイル読み込み
			case (ID_40001) :
				MessageBoxA(hwnd, "＜(゜ε。)＞未実装", WNDCLASSNAMEA, MB_OK);
				break;
				//設定...
			case (ID_40002) :
				cfg.visible_config_window(true);
				break;
				//上書き保存
			case(ID_40003) :
				MessageBoxA(hwnd, "＜(゜ε。)＞未実装", WNDCLASSNAMEA, MB_OK);
				break;
				//プログラムを起動する
			case(ID_40004) :
				SendMessage(hwnd, WM_RUN_PROCESS, 0, 0);
				break;
				//終了
			case(ID_40005) :

				if (MessageBoxA(hwnd, "終了しますか？", WNDCLASSNAMEA, MB_YESNO) == IDYES)
				{
					DestroyWindow(hwnd);
				}
				break;

				//バージョン情報表示
			case (ID_ABOUT_SCREENGUARD40006) :
			{
				char buf[128];
				sprintf_s(buf, sizeof(buf), "Screen Guard ver %s\n%s",GET_VERSION,GET_FOOTER );
				MessageBoxA(hwnd, buf, WNDCLASSNAMEA, MB_ICONINFORMATION);
			}
				break;

			default:
				break;
			}


		}
		return 0;
	case WM_PAINT:
	{
					 PAINTSTRUCT pt;
					 HBRUSH brush = (HBRUSH) GetStockObject(BLACK_BRUSH);
					 HDC hdc = BeginPaint(hwnd, &pt);
					 Rectangle(hdc, 0, 0, ::mainScreenX, ::mainScreenY);
					 EndPaint(hwnd, &pt);
	}
		return 0;

		//指定されたホットキーが押されたら終了
	case WM_HOTKEY:
		if (wp == HOTKEY_CODE) DestroyWindow(hwnd);
		return 0;

		//プログラムが開始された
	case WM_ULUNCHPROCESS_START:
		//ShowWindow(hwnd, SW_MINIMIZE);
		InvalidateRect(hwnd, nullptr, false);
		state = STATE::PROCESS_RUNNING;
		s_processPid = (DWORD) lp;
		s_processHandle = (HANDLE) wp;

		//親ウインドウ化を試みる
		{
			HWND hw = GetWindowHandleByPID(s_processPid);
			if (GetParent(hw) != hwnd)
			{
				SetParent(hw, hwnd);
				RECT rc, nrc;
				GetWindowRect(hw, &rc);
				if (GetCenterFromOwnerWindow(hwnd, &rc, &nrc))
					SetWindowPos(hw, HWND_TOP, nrc.left, nrc.top, 0, 0, SWP_NOSIZE);
			}
			UpdateWindow(hw);
		}

		return 0;

		//プロセスの終了を検知したら再起動するかどうかを聞く
	case WM_ULUNCHPROCESS_CLOSE:
		
		state = STATE::WAIT;
		if (th.joinable()) th.join();

		//ただしこのプログラムが閉じられようとしていたら処理しない
		if (!program_closing)
		{
			BOOL key = GetAsyncKeyState(VK_SHIFT) & 0x8000;

			if ( !::autorun_flag && (( cfg.get_selectbox() || key != 0) && MessageBoxA(hwnd, ("プログラムが閉じられました。再起動しますか？"), WNDCLASSNAMEA, MB_YESNO) == IDNO ))
			{
				SendMessage(hwnd, WM_NULL, 0, 0);
			}
			else
			{
				SendMessage(hwnd, WM_RUN_PROCESS, 0, 0);
			}
		}
		return 0;


		//起動エラー
	case WM_ULUNCHPROCESS_ERROR:
		if (th.joinable()) th.join();
		MessageBox(hwnd, TEXT("プロセスの起動に失敗しました。\n指定したパスに実行ファイルが存在するか確認して下さい。"), WNDCLASSNAME, MB_ICONERROR);
		state = STATE::WAIT;
		return 0;

		//プロファイルをセーブする
	case WM_SAVE_PROFILE:
		cfg.create_ConfigFile(loaded_filename.c_str());
		if (cfg.get_errcode() != config::code::CREATE_FILE)
		{
			MessageBox(hwnd, TEXT("プロファイルの保存に失敗しました"), WNDCLASSNAME, MB_ICONINFORMATION);
		}
		
		cfg.visible_config_window(false);
		return 0;

		//変更を破棄
	case WM_CANCEL_PROFILE:
		cfg.visible_config_window(false);
		return 0;

		//プロセスの起動
	case WM_RUN_PROCESS:
		
		th = std::move(std::thread(WaitProcess, hwnd, cfg.get_processName().c_str(), cfg.get_processCmdLine().c_str()));
		state = STATE::PROCESS_RUNNING;
		process_closing = false;
		set_center = false;
		return 0;

	case WM_CLOSE:
		if (th.joinable()) th.join();
		program_closing = true;
		return 0;
	case WM_DESTROY:

		//プロセスがまだ終了していなかったら強制終了する
		if (!process_closing)
		{
			HWND hw = GetWindowHandleByPID(s_processPid);
			SetParent(hw, NULL);
			//TerminateProcess(s_processHandle,EXIT_SUCCESS);
		}
		

		//ホットキーの設定を解除
		UnregisterHotKey(hwnd, HOTKEY_CODE);
		UnregisterHotKey(hwnd, HOTKEY_CODE + 1);
		UnregisterHotKey(hwnd, HOTKEY_CODE + 2);
		UnregisterHotKey(hwnd, HOTKEY_CODE + 3);

		if (th.joinable()) th.join();

		ShowTaskBar(trayHwnd, startHwnd);
		PostQuitMessage(0);

		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}