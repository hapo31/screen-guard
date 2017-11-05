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
	
	//���̃v���O�����̏I���t���O
	static bool program_closing = false;
	//�N���v���Z�X�̏I���t���O
	static bool process_closing = false;

	//�E�B���h�E���Z���^�[�Ɉړ��ς݂��ۂ�
	static bool set_center = false;
	static std::string loaded_filename;

	//�N���v���Z�X�̃v���Z�XID
	static DWORD s_processPid;
	//�N���v���Z�X�̃v���Z�X�n���h��
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

			//�őO�ʂɈړ��@�^�X�N�o�[�̏�ɕ������Ԃ���悤�ɃE�C���h�E���[�h��ύX
			SetWindowLong(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
			//SetWindowLong(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUPWINDOW);

			//�R���t�B�O�p���̍쐬
			cfg.create_config_window(hwnd);

			//�I���p�z�b�g�L�[�̓o�^
			RegisterHotKey(hwnd, HOTKEY_CODE, MOD_CONTROL, 'M');

			//�e��V�X�e���L�[�𖳌���
			RegisterHotKey(hwnd, HOTKEY_CODE + 1, MOD_ALT, VK_TAB);
			RegisterHotKey(hwnd, HOTKEY_CODE + 2, MOD_WIN, 0);
			RegisterHotKey(hwnd, HOTKEY_CODE + 3, MOD_ALT, VK_F4);

			//�R���t�B�O�t�@�C���̓ǂݍ���	
			if (cfg.get_errcode() == config::code::NO_WORK) cfg.loadConfig(loaded_filename.c_str());
			//�����������Ȃ�������
			if (cfg.get_errcode() != config::code::SUCCESS)
			{
				switch (cfg.get_errcode())
				{
				case config::code::NOTFOUND_FILE:
					cfg.create_ConfigFile(loaded_filename.c_str(),true);

					//�t�@�C����V��������			
					if (cfg.get_errcode() == config::code::CREATE_FILE)
					{
						MessageBox(hwnd, TEXT("config�t�@�C����������Ȃ��������߁A�V�����������܂����B"), WNDCLASSNAME, MB_ICONINFORMATION);
					}
					//�t�@�C���I�[�v���G���[
					else if (cfg.get_errcode() == config::code::FAILED_FILEOPEN)
					{
						MessageBox(hwnd, TEXT("�V����config�t�@�C���𐶐����悤�Ƃ��܂������A�t�@�C�����I�[�v���o���܂���ł����B"), WNDCLASSNAME, MB_ICONINFORMATION);
						DestroyWindow(hwnd);
					}

					break;
					//�����G���[
				case config::code::INVALID_ITEM:
					MessageBox(hwnd, TEXT("config�t�@�C�����s���ł��B�������m�F���ĉ������B"), WNDCLASSNAME, MB_ICONINFORMATION);
					DestroyWindow(hwnd);
					break;
				default:
					MessageBox(hwnd, TEXT("�s���ȃG���[�ł� �J���҂ɖ₢���킹�Ă�������"), WNDCLASSNAME, MB_ICONINFORMATION);
					DestroyWindow(hwnd);
					break;
				}
			}
			//���폈��
			else
			{
				HideTaskBar(trayHwnd, startHwnd);
				if (::autorun_flag)
					SendMessage(hwnd, WM_RUN_PROCESS, 0, 0);
			}

		}
		return 0;

		//���g���A�N�e�B�u�����ꂽ��N���v���Z�X���őO�ʂɂ���
	case WM_ACTIVATE:
		if (state != STATE::WAIT)
		{
			HWND hw = GetWindowHandleByPID(s_processPid);
			//SetWindowText(hw, TEXT("<(^o^)>�����f,;�f;�ߎO[SiON���̓z��]�O�߁f,;�f;����<(^o^)>"));
			if (s_processPid == 0)
			{
				OutputDebugStringA("processHwnd��NULL\n");
			}
			else
			{
				//�e�E�C���h�E�������݂�

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
				//�v���t�@�C���ǂݍ���
			case (ID_40001) :
				MessageBoxA(hwnd, "��(�K�ÁB)��������", WNDCLASSNAMEA, MB_OK);
				break;
				//�ݒ�...
			case (ID_40002) :
				cfg.visible_config_window(true);
				break;
				//�㏑���ۑ�
			case(ID_40003) :
				MessageBoxA(hwnd, "��(�K�ÁB)��������", WNDCLASSNAMEA, MB_OK);
				break;
				//�v���O�������N������
			case(ID_40004) :
				SendMessage(hwnd, WM_RUN_PROCESS, 0, 0);
				break;
				//�I��
			case(ID_40005) :

				if (MessageBoxA(hwnd, "�I�����܂����H", WNDCLASSNAMEA, MB_YESNO) == IDYES)
				{
					DestroyWindow(hwnd);
				}
				break;

				//�o�[�W�������\��
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

		//�w�肳�ꂽ�z�b�g�L�[�������ꂽ��I��
	case WM_HOTKEY:
		if (wp == HOTKEY_CODE) DestroyWindow(hwnd);
		return 0;

		//�v���O�������J�n���ꂽ
	case WM_ULUNCHPROCESS_START:
		//ShowWindow(hwnd, SW_MINIMIZE);
		InvalidateRect(hwnd, nullptr, false);
		state = STATE::PROCESS_RUNNING;
		s_processPid = (DWORD) lp;
		s_processHandle = (HANDLE) wp;

		//�e�E�C���h�E�������݂�
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

		//�v���Z�X�̏I�������m������ċN�����邩�ǂ����𕷂�
	case WM_ULUNCHPROCESS_CLOSE:
		
		state = STATE::WAIT;
		if (th.joinable()) th.join();

		//���������̃v���O�����������悤�Ƃ��Ă����珈�����Ȃ�
		if (!program_closing)
		{
			BOOL key = GetAsyncKeyState(VK_SHIFT) & 0x8000;

			if ( !::autorun_flag && (( cfg.get_selectbox() || key != 0) && MessageBoxA(hwnd, ("�v���O�����������܂����B�ċN�����܂����H"), WNDCLASSNAMEA, MB_YESNO) == IDNO ))
			{
				SendMessage(hwnd, WM_NULL, 0, 0);
			}
			else
			{
				SendMessage(hwnd, WM_RUN_PROCESS, 0, 0);
			}
		}
		return 0;


		//�N���G���[
	case WM_ULUNCHPROCESS_ERROR:
		if (th.joinable()) th.join();
		MessageBox(hwnd, TEXT("�v���Z�X�̋N���Ɏ��s���܂����B\n�w�肵���p�X�Ɏ��s�t�@�C�������݂��邩�m�F���ĉ������B"), WNDCLASSNAME, MB_ICONERROR);
		state = STATE::WAIT;
		return 0;

		//�v���t�@�C�����Z�[�u����
	case WM_SAVE_PROFILE:
		cfg.create_ConfigFile(loaded_filename.c_str());
		if (cfg.get_errcode() != config::code::CREATE_FILE)
		{
			MessageBox(hwnd, TEXT("�v���t�@�C���̕ۑ��Ɏ��s���܂���"), WNDCLASSNAME, MB_ICONINFORMATION);
		}
		
		cfg.visible_config_window(false);
		return 0;

		//�ύX��j��
	case WM_CANCEL_PROFILE:
		cfg.visible_config_window(false);
		return 0;

		//�v���Z�X�̋N��
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

		//�v���Z�X���܂��I�����Ă��Ȃ������狭���I������
		if (!process_closing)
		{
			HWND hw = GetWindowHandleByPID(s_processPid);
			SetParent(hw, NULL);
			//TerminateProcess(s_processHandle,EXIT_SUCCESS);
		}
		

		//�z�b�g�L�[�̐ݒ������
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