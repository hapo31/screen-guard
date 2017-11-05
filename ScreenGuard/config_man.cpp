#pragma once
#include"config_man.h"
#include"wndproc.h"
#include"WINAPI_tools.h"

void SetCurrentDir()
{
	//カレントディレクトリを保存
	std::string newfilename;
	char fullpath[256];
	char drive[16];
	char dir[128];

	GetModuleFileNameA(GetModuleHandle(NULL), fullpath, sizeof(fullpath));
	_splitpath_s(fullpath, drive, sizeof(drive), &dir[0], sizeof(dir), nullptr, 0, nullptr, 0);

	size_t len = strnlen_s(drive, 256) + strnlen_s(dir, 256);
	newfilename.resize(len + 1);

	sprintf_s(&newfilename[0], newfilename.size(), "%s%s",drive,dir);

	SetCurrentDirectoryA(newfilename.c_str());
}

//コンフィグファイルを生成する(上書き保存)
config::code config::create_ConfigFile(const char* filename,bool defaultFlag)
{
	defaultFlag = false;
	::SetCurrentDir();

	if (window_created)
		this->set_config_fromWindow();

	std::ofstream ofs(filename, std::ios::trunc);
	if (!ofs)
	{
		return setcode(code::FAILED_FILEOPEN);
	}
	else
	{
		if (!defaultFlag)
			this->set_config_fromWindow();

		ofs << "process_path=";
		if (!defaultFlag)
			ofs << this->lanch_process_name.c_str();
		ofs << std::endl;
		ofs << "process_cmdline=";
		if (!defaultFlag)
			ofs << this->lanch_process_cmdline.c_str();
		ofs << std::endl;
		ofs.setf(std::ios::boolalpha);
		ofs << "select_continue=";
		if (!defaultFlag)
			ofs << this->select_box;
		else //デフォルトならtrue
			ofs << true;

		ofs << std::endl;

		return setcode(code::CREATE_FILE);
	}
}

//コンフィグファイルを解析する
config::code config::loadConfig(const char* filename)
{
	::SetCurrentDir();

	std::ifstream ifs(filename);
	std::string item_name;
	std::string item_value;

	int count = 0;

	if (!ifs)
	{
		return setcode(code::NOTFOUND_FILE);
	}

	item_name.resize(CONFIGFILE_LINEMAX);
	item_value.resize(CONFIGFILE_LINEMAX);

	//設定ファイルを読み込む
	do
	{
		ifs.getline(&item_name[0], item_name.size(), '=');
		ifs.getline(&item_value[0], item_value.size());

		if (item_name.length() == 0)
		{
			return setcode(code::INVALID_ITEM);
		}
		else if (item_value.size() == 0)
		{
			OutputDebugStringA(std::string(item_name + std::string("の設定項目が空です\n")).c_str());
		}

		switch (count)
		{
		case( 0 ) :
		{
			{
				char buf[128];
				std::string myDir; //このプログラムのディレクトリ
				std::string prDir; //起動しようとしているプログラムのディレクトリ
				std::string exename;

				exename.resize(128);
				myDir.resize(128);
				prDir.resize(128);

				//このプログラムのディレクトリを取得
				GetModuleFileNameA(GetModuleHandle(NULL), buf, sizeof(buf));
				_splitpath_s(buf, nullptr, 0, &myDir[0], myDir.size(), nullptr, 0, nullptr, 0);

				//指定プログラムのディレクトリを取得
				_splitpath_s(item_value.c_str(), nullptr, 0, &prDir[0], prDir.size(), &exename[0], exename.size(), nullptr, 0);

				//ファイルパスじゃないものが入力されていたらエラー
				if (prDir.length() == 0 && exename.length() == 0)
					return setcode(code::INVALID_ITEM);

				//フォルダが違う場合はカレントディレクトリを変更
				if (myDir != prDir)
				{
					//prDir[prDir.find_last_of('\\')] = '\0';
					SetCurrentDirectoryA(prDir.c_str());
				}
			}
		}
			lanch_process_name = item_value;
			break;
		case( 1 ):
			lanch_process_cmdline = item_value;
			break;
		case( 2 ):
			if (item_value.find("true") != std::string::npos || item_value.find("TRUE") != std::string::npos)
			{
				this->select_box = true;
			}
			else if (item_value.find("false") != std::string::npos || item_value.find("FALSE") != std::string::npos )
			{
				this->select_box = false;
			}
		}
		++count;

	} while (!ifs.eof());

	return setcode(code::SUCCESS);
}

bool config::create_config_window(HWND ownerHwnd,bool re_create_flag)
{
	
	const int Width = 420;
	const int Height = 230;

	const int bwidth = 100;
	const int bheight = 30;
	const int base_buttonY = 150;

	if (!window_created || re_create_flag)
	{
		this->window_created = false;

		WNDCLASS wc = {0};
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = TEXT("config_window");
		wc.lpfnWndProc = configWndProc;
		wc.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
		wc.style = CS_HREDRAW | CS_VREDRAW;

		if (FAILED(RegisterClass(&wc))) return false;

		HWND hwnd = this->controls["window"] =
			CreateWindow(wc.lpszClassName, TEXT("プロファイルの編集"), WS_OVERLAPPED
			, 0, 0, Width, Height, ownerHwnd, NULL, GetModuleHandle(NULL), NULL);

		if (hwnd == NULL) return false;

		this->controls["filepath_title"] =
			create_static(
			hwnd, TEXT("ファイルパス"),
			30, 5, 320, 25,
			TEXT_ID::FILEPATH_SS);
		this->controls["filepath_textb"] =
			create_textbox(
			hwnd,this->lanch_process_name.c_str(),
			30, 30, 320, 25,
			TEXT_ID::FILEPATH);	
		this->controls["dialog_button"] =
			create_button(
			hwnd, TEXT("..."), 351, 30, 30, 25,
			BUTTON_ID::FILESAVE_DIALOG);

		this->controls["cmdline_title"] =
			create_static(
			hwnd, TEXT("コマンドライン引数"),
			30, 65, 320, 25,
			TEXT_ID::CMDLINE_SS);
		this->controls["cmdline_textb"] =
			create_textbox(
			hwnd, this->lanch_process_cmdline.c_str(),
			30, 90, 320, 25,
			TEXT_ID::CMDLINE);

		this->controls["save_button"] =
			create_button(
			hwnd, TEXT("保存"),
			140, base_buttonY,
			bwidth, bheight,
			BUTTON_ID::SAVE
			);
		this->controls["cancel_button"] =
			create_button(
			hwnd, TEXT("キャンセル"),
			250, base_buttonY,
			bwidth, bheight,
			BUTTON_ID::CANCEL
			);

		this->controls["checkbox"] =
			create_checkbox(
			hwnd,TEXT("再起動確認ダイアログを出す"),
			30, 120, 320, 30,
			CHECKBOX_ID::CONTINUE);

		this->set_window_fromConfig();

		this->window_created = true;
		//このウインドウのプロシージャに親ウインドウのハンドルを教える
		SendMessage(hwnd, WM_REGISTOWNER, (WPARAM)this, (LPARAM) ownerHwnd);
	}
	return true;
}

HRESULT CALLBACK configWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	using CNTROL_MAP = std::unordered_map<std::string, HWND>;

	static HWND owner = nullptr;
	static config* cfg;

	switch (msg)
	{
	case WM_CREATE:
		return 0;
	case WM_REGISTOWNER:
		owner = (HWND) lp;
		cfg = reinterpret_cast<config*>(wp);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wp))
		{

			return 0;
		case BUTTON_ID::SAVE:
			SendMessage(owner, WM_SAVE_PROFILE, 0, 0);
			return 0;
		case BUTTON_ID::CANCEL:
			SendMessage(owner, WM_CANCEL_PROFILE, 0, 0);
			return 0;
		case BUTTON_ID::FILESAVE_DIALOG:
		{
			{
				std::string path;
				std::string filename;
				std::string bef;
				bef.resize(128);
				GetWindowTextA(cfg->get_control("filepath_textb"), &bef[0], bef.size());
				if(GetFilenameByOpenDialog(hwnd, "exe", path, filename))
					SetWindowTextA(cfg->get_control("filepath_textb"), &path[0]);
				else
					SetWindowTextA(cfg->get_control("filepath_textb"), &bef[0]);

				UpdateWindow(hwnd);
			}										   
		}
			return 0;
		default:
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}