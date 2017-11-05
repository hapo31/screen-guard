#pragma once
#include"defs.h"

#include<fstream>
#include<Windows.h>
#include<unordered_map>

class config
{
public:
	enum code
	{
		NO_WORK = 0,
		SUCCESS = 1,
		CREATE_FILE = 2,
		NOTFOUND_FILE = 3,
		EXIST_FILE = 4,
		INVALID_ITEM = -1,
		FAILED_FILEOPEN = -2,
		UNKNOWN_ERROR = -3
	};
private:
	//クラス自身が使用するフィールド
	std::string filename;
	
	bool newfileFlag = true;
	std::unordered_map<std::string, HWND> controls;

	//読み込んだデータ用フィールド
	const int CONFIGFILE_LINEMAX = 128;
	std::string lanch_process_name = "";
	std::string lanch_process_cmdline = "";
	code returnCode = NO_WORK;
	bool select_box = true;
	bool window_created = false;

private:

	code setcode(code c){ return returnCode = c; }
	
	bool get_checkbox()
	{ 
		auto t = SendMessage(controls["checkbox"], BM_GETCHECK, 0, 0);
		return t == BST_CHECKED;
	}
	bool set_checkbox(bool check){ return SendMessage(controls["checkbox"], BM_SETCHECK, check ? BST_CHECKED : BST_UNCHECKED, 0) == TRUE; }

	BOOL set_textbox(const char* name, const std::string& str){ return SetWindowTextA(controls[name], str.c_str()); }
	BOOL get_textbox(const char* name, std::string buf){ return GetWindowTextA(controls[name], &buf[0], buf.size()); }

	
	//ウィンドウの入力状態を内部フィールドに適用
	void set_config_fromWindow()
	{
		if (window_created)
		{
			lanch_process_name.resize(128);
			lanch_process_cmdline.resize(128);
			GetWindowTextA(controls["filepath_textb"], &lanch_process_name[0], lanch_process_name.size());
			GetWindowTextA(controls["cmdline_textb"], &lanch_process_cmdline[0], lanch_process_cmdline.size());
			/*
			get_textbox("filepath_textb", lanch_process_name);
			get_textbox("cmdline_textb", lanch_process_cmdline);
			*/
			this->select_box = get_checkbox();
		}
	}
	//内部フィールドの状態をウィンドウに適用
	void set_window_fromConfig()
	{
		if (window_created)
		{
			set_textbox("filepath_textb", lanch_process_name);
			set_textbox("cmdline_textb", lanch_process_cmdline);
			set_checkbox(this->select_box);
		}
	}
public:
	config(){  }
	config(const char* filename){ this->create_ConfigFile(filename, false); }

	code create_ConfigFile(const char* filename,bool defaultFlag = false);
	code loadConfig(const char* filename);

	const std::string& get_processName() const { return lanch_process_name; }
	const std::string& get_processCmdLine() const { return lanch_process_cmdline; }
	const std::string& get_filename() const { return this->filename; }

	code get_errcode() const { return this->returnCode; }

	bool get_selectbox() const { return select_box; }
	void set_selectbox(bool value){ this->select_box = value; }
	HWND get_control(const char* name){ return controls[name]; }

	bool create_config_window(HWND ownerHwnd, bool re_create_flag = false);
	//表示するときはウィンドウの状態を内部フィールドから適用する
	bool visible_config_window(bool flag){ if( flag )this->set_window_fromConfig(); if (window_created) return ShowWindow(controls["window"], flag ? SW_SHOW : SW_HIDE) == TRUE; else return false; }
};