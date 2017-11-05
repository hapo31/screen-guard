#pragma once
#include"config_man.h"
#include"wndproc.h"
#include"WINAPI_tools.h"

void SetCurrentDir()
{
	//�J�����g�f�B���N�g����ۑ�
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

//�R���t�B�O�t�@�C���𐶐�����(�㏑���ۑ�)
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
		else //�f�t�H���g�Ȃ�true
			ofs << true;

		ofs << std::endl;

		return setcode(code::CREATE_FILE);
	}
}

//�R���t�B�O�t�@�C������͂���
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

	//�ݒ�t�@�C����ǂݍ���
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
			OutputDebugStringA(std::string(item_name + std::string("�̐ݒ荀�ڂ���ł�\n")).c_str());
		}

		switch (count)
		{
		case( 0 ) :
		{
			{
				char buf[128];
				std::string myDir; //���̃v���O�����̃f�B���N�g��
				std::string prDir; //�N�����悤�Ƃ��Ă���v���O�����̃f�B���N�g��
				std::string exename;

				exename.resize(128);
				myDir.resize(128);
				prDir.resize(128);

				//���̃v���O�����̃f�B���N�g�����擾
				GetModuleFileNameA(GetModuleHandle(NULL), buf, sizeof(buf));
				_splitpath_s(buf, nullptr, 0, &myDir[0], myDir.size(), nullptr, 0, nullptr, 0);

				//�w��v���O�����̃f�B���N�g�����擾
				_splitpath_s(item_value.c_str(), nullptr, 0, &prDir[0], prDir.size(), &exename[0], exename.size(), nullptr, 0);

				//�t�@�C���p�X����Ȃ����̂����͂���Ă�����G���[
				if (prDir.length() == 0 && exename.length() == 0)
					return setcode(code::INVALID_ITEM);

				//�t�H���_���Ⴄ�ꍇ�̓J�����g�f�B���N�g����ύX
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
			CreateWindow(wc.lpszClassName, TEXT("�v���t�@�C���̕ҏW"), WS_OVERLAPPED
			, 0, 0, Width, Height, ownerHwnd, NULL, GetModuleHandle(NULL), NULL);

		if (hwnd == NULL) return false;

		this->controls["filepath_title"] =
			create_static(
			hwnd, TEXT("�t�@�C���p�X"),
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
			hwnd, TEXT("�R�}���h���C������"),
			30, 65, 320, 25,
			TEXT_ID::CMDLINE_SS);
		this->controls["cmdline_textb"] =
			create_textbox(
			hwnd, this->lanch_process_cmdline.c_str(),
			30, 90, 320, 25,
			TEXT_ID::CMDLINE);

		this->controls["save_button"] =
			create_button(
			hwnd, TEXT("�ۑ�"),
			140, base_buttonY,
			bwidth, bheight,
			BUTTON_ID::SAVE
			);
		this->controls["cancel_button"] =
			create_button(
			hwnd, TEXT("�L�����Z��"),
			250, base_buttonY,
			bwidth, bheight,
			BUTTON_ID::CANCEL
			);

		this->controls["checkbox"] =
			create_checkbox(
			hwnd,TEXT("�ċN���m�F�_�C�A���O���o��"),
			30, 120, 320, 30,
			CHECKBOX_ID::CONTINUE);

		this->set_window_fromConfig();

		this->window_created = true;
		//���̃E�C���h�E�̃v���V�[�W���ɐe�E�C���h�E�̃n���h����������
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