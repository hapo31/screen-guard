#pragma once
#include<Windows.h>

#define VERSION_STR( v ) #v

#define GET_VERSION (VERSION_STR( 0.10 ))

#define GET_FOOTER ("Created by @happou31")

#define WNDCLASSNAMEA "ScreenGuard"
#define WNDCLASSNAME TEXT("ScreenGuard")

#define HOTKEY_CODE 0x01

//�Ǝ��E�C���h�E���b�Z�[�W
enum WM_USER_IDs
{
	//�v���Z�X�}�l�[�W�����瑗���郁�b�Z�[�W
	WM_ULUNCHPROCESS_CLOSE = (WM_USER + 1),
	WM_ULUNCHPROCESS_ERROR,
	WM_ULUNCHPROCESS_START,
	
	//
	WM_RUN_PROCESS,

	//�R���t�B�O�E�C���h�E���瑗���郁�b�Z�[�W
	WM_SAVE_PROFILE,
	WM_CANCEL_PROFILE,

	//�R���t�B�O�E�C���h�E�ɑ����郁�b�Z�[�W
	WM_REGISTOWNER,

};



//�e�E�C���h�E�̃R���g���[���͂��͈̔͂�ID���g�p����
#define PARENT_CONTROL_ID_MIN 0x001
#define PARENT_CONTROL_ID_MAX 0x0FF

//�q�E�C���h�E�̃R���g���[���͂��͈̔͂�ID���g�p����
#define CHILD_CONTROL_ID_MIN 0x0F00
#define CHILD_CONTROL_ID_MAX 0x0FFF

#define IS_PARENT_ID( id ) bool( PARENT_CONTROL_ID_MIN <= id && PARENT_CONTROL_ID_MAX >= id )
#define IS_CHILD_ID( id ) bool( CHILD_CONTROL_ID_MIN <= id && CHILD_CONTROL_ID_MAX >= id )

enum STATE
{
	WAIT,
	PROCESS_RUNNING,
};
enum TEXT_ID
{
	FILEPATH = CHILD_CONTROL_ID_MIN,
	CMDLINE,
	FILEPATH_SS,
	CMDLINE_SS,
	TEXT_ID_MAX
};
enum BUTTON_ID
{
	SAVE = TEXT_ID_MAX + 1,
	CANCEL,
	FILESAVE_DIALOG,
	BUTTON_ID_MAX
};

enum CHECKBOX_ID
{
	CONTINUE = BUTTON_ID_MAX + 1,
};