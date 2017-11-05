#pragma once
#include<Windows.h>
#include<unordered_map>
#include<vector>
#include<string>

namespace winapi
{
	//論理フォントラッパ
	class clogfont
	{
	private:
		typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> tstring;

		HFONT myhandle;
		tstring fontname;
	public:
		explicit clogfont(){ this->myhandle = 0; }
		//LOGFONT構造体から生成
		explicit clogfont(LPLOGFONT lf)
		{ 
			this->create(lf);
		}
		//フォント情報を入力して生成（フォント名のみ必須
		explicit clogfont(TCHAR* fontname, int height = 0, int width = 0, bool bold = false , bool italic = false, bool underline = false, bool strikeOut = false)
		{
			this->create(fontname, height, width, bold, italic, underline, strikeOut);
		}
		~clogfont()
		{
			this->Delete();
		}
		//フォントを生成する
		void create(LPLOGFONT lf)
		{
			this->fontname = lf->lfFaceName;
			this->myhandle = ::CreateFontIndirect(lf);
		}
		//フォントを生成する
		void create(TCHAR* fontname, int height = 0, int width = 0,bool bold = false, bool italic = false, bool underline = false, bool strikeOut = false)
		{
			this->fontname = fontname;
			this->myhandle = ::CreateFont(
				height, width, 0, 0,
				bold ? FW_BOLD : FW_THIN,	//フォントにBold装飾をするかどうか
				italic,
				underline,
				strikeOut,
				SHIFTJIS_CHARSET,	//文字セットはShiftJIS
				OUT_TT_ONLY_PRECIS,
				0,
				DEFAULT_QUALITY,
				0,
				fontname
				);
		}
		//フォントの削除
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