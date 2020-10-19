#include "System_Function.h"
#include "DxSystem.h"
using namespace std;

//開くファイルのパス取得
string System_Function::Get_Open_File_Name()
{
	static OPENFILENAME     ofn;
	static TCHAR            szPath[MAX_PATH];
	static TCHAR            szFile[MAX_PATH];
	static string           str_pass;
	static string           current_pass;
	static size_t           current_pass_size;

	if (szPath[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, szPath);
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szPath, -1, char_pass, MAX_PATH, NULL, NULL);
		current_pass = char_pass;
		current_pass_size = current_pass.size();
	}
	if (ofn.lStructSize == 0)
	{
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = DxSystem::hwnd;
		ofn.lpstrInitialDir = szPath;       // 初期フォルダ位置
		ofn.lpstrFile = szFile;       // 選択ファイル格納
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("すべてのファイル(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("ファイル選択");
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	}
	if (GetOpenFileName(&ofn))
	{
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, char_pass, MAX_PATH, NULL, NULL);
		str_pass = char_pass;
		str_pass = str_pass.substr(current_pass_size + 1, str_pass.size());
	}
	else
	{
		str_pass = "";
	}
	return str_pass;
}

//保存先パス取得
string System_Function::Get_Save_File_Name()
{
	static OPENFILENAME     ofn;
	static TCHAR            szPath[MAX_PATH];
	static TCHAR            szFile[MAX_PATH];
	static string           str_pass;
	static size_t           current_pass_size;

	if (szPath[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, szPath);
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szPath, -1, char_pass, MAX_PATH, NULL, NULL);
		string current_pass = char_pass;
		current_pass_size = current_pass.size();
	}
	if (ofn.lStructSize == 0)
	{
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = DxSystem::hwnd;
		ofn.lpstrInitialDir = szPath;       // 初期フォルダ位置
		ofn.lpstrFile = szFile;       // 選択ファイル格納
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("すべてのファイル(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("名前をつけて保存");
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	}
	if (GetSaveFileName(&ofn))
	{
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, char_pass, MAX_PATH, NULL, NULL);
		str_pass = char_pass;
		str_pass = str_pass.substr(current_pass_size + 1, str_pass.size());
	}
	else
	{
		str_pass = "";
	}
	return str_pass;
}