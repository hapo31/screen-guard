#pragma once
#include<Windows.h>
#include<unordered_map>
#include<vector>
#include<string>

namespace winapi
{
	//�_���t�H���g���b�p
	class clogfont
	{
	private:
		typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> tstring;

		HFONT myhandle;
		tstring fontname;
	public:
		explicit clogfont(){ this->myhandle = 0; }
		//LOGFONT�\���̂��琶��
		explicit clogfont(LPLOGFONT lf)
		{ 
			this->create(lf);
		}
		//�t�H���g������͂��Đ����i�t�H���g���̂ݕK�{
		explicit clogfont(TCHAR* fontname, int height = 0, int width = 0, bool bold = false , bool italic = false, bool underline = false, bool strikeOut = false)
		{
			this->create(fontname, height, width, bold, italic, underline, strikeOut);
		}
		~clogfont()
		{
			this->Delete();
		}
		//�t�H���g�𐶐�����
		void create(LPLOGFONT lf)
		{
			this->fontname = lf->lfFaceName;
			this->myhandle = ::CreateFontIndirect(lf);
		}
		//�t�H���g�𐶐�����
		void create(TCHAR* fontname, int height = 0, int width = 0,bool bold = false, bool italic = false, bool underline = false, bool strikeOut = false)
		{
			this->fontname = fontname;
			this->myhandle = ::CreateFont(
				height, width, 0, 0,
				bold ? FW_BOLD : FW_THIN,	//�t�H���g��Bold���������邩�ǂ���
				italic,
				underline,
				strikeOut,
				SHIFTJIS_CHARSET,	//�����Z�b�g��ShiftJIS
				OUT_TT_ONLY_PRECIS,
				0,
				DEFAULT_QUALITY,
				0,
				fontname
				);
		}
		//�t�H���g�̍폜
		void Delete()
		{
			DeleteObject( this->myhandle );  
		}
		
		const TCHAR* name(){ return this->fontname.c_str(); }

		operator HFONT() const { return this->myhandle; }
		operator WPARAM() const { return WPARAM(this->myhandle); }
		operator HFONT*() { return &this->myhandle; }


	};
};