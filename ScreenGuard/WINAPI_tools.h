#pragma once
#include<Windows.h>
#include<string>

//�w�肵���v���Z�XID�̃E�C���h�E�n���h�����擾
HWND GetWindowHandleByPID(DWORD pid);
//�^�X�N�o�[�̕\��
BOOL ShowTaskBar(HWND tray, HWND start);
//�^�X�N�o�[�̔�\��
BOOL HideTaskBar(HWND tray, HWND start);

DWORD WaitProcess(HWND hwMain, const char* name, const char* cmdline);

//RECT�T�C�Y�̃E�B���h�E���AHWND�Ɏw�肵���E�B���h�E�̒��S�ɗ���悤�ȍ��W���擾����
//���������̊֐��Ŏ擾����RECT�����̂܂�SetRect����̂͂悭�Ȃ��̂ŁAtop��left�̂ݎg�p����悤�ɂ��邱��
bool GetCenterFromOwnerWindow(HWND owner, LPCRECT childWindowRect, LPRECT newChildWindowRect);

//�t�@�C���I�[�v���_�C�A���O�Ńt�@�C���p�X���擾����
bool GetFilenameByOpenDialog(HWND ownerHwnd, const char* ext, std::string& path_buf, std::string& name_buf);
//�t�@�C���Z�[�u�_�C�A���O�Ńt�@�C���p�X���擾����
bool GetFilenameBySaveDialog(HWND ownerHwnd, const char* ext, std::string& path_buf, std::string& name_buf);

//������ǂ�������
HWND create_window(HWND owner, LPCTSTR classname, LPCTSTR caption, int style, int x, int y, int nWidth, int nHeight, HMENU Id , LPARAM lp = 0);
HWND create_window(HWND owner, LPCSTR classname, LPCSTR caption, int style, int x, int y, int nWidth, int nHeight, HMENU Id, LPARAM lp = 0);

//�{�^�������
HWND create_button(HWND owner, LPCTSTR caption , int x,int y,int nWidth,int nHeight, int Id);

//�e�L�X�g�{�b�N�X�����
HWND create_textbox(HWND owner, int x, int y, int nWidth, int nHeight, int Id);
HWND create_textbox(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int Id);
HWND create_textbox(HWND owner, LPCSTR caption, int x, int y, int nWidth, int nHeight, int Id);

//�`�F�b�N�{�b�N�X�����
HWND create_checkbox(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int id);

//�X�^�e�B�b�N�R���g���[��(������)�����
HWND create_static(HWND owner, LPCTSTR caption, int x, int y, int nWidth, int nHeight, int id, int style = 0);