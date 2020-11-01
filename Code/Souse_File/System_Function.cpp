#include "System_Function.h"
#include "DxSystem.h"
#include <clocale>
#include <tchar.h>
using namespace std;

//開くファイルのパス取得
string System_Function::Get_Open_File_Name()
{
	static OPENFILENAME     ofn;
	static TCHAR            szPath[MAX_PATH];
	static TCHAR            szFile[MAX_PATH];
	static string           str_path;
	static string           current_path;
	static size_t           current_path_size;

	if (szPath[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, szPath);
		char char_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szPath, -1, char_path, MAX_PATH, NULL, NULL);
		current_path = char_path;
		current_path_size = current_path.size();
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
		char char_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, char_path, MAX_PATH, NULL, NULL);
		str_path = char_path;
		str_path = str_path.substr(current_path_size + 1, str_path.size());
	}
	else
	{
		str_path = "";
	}
	return str_path;
}

//保存先パス取得
string System_Function::Get_Save_File_Name()
{
	static OPENFILENAME     ofn;
	static TCHAR            szPath[MAX_PATH];
	static TCHAR            szFile[MAX_PATH];
	static string           str_path;
	static size_t           current_path_size;

	if (szPath[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, szPath);
		char char_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szPath, -1, char_path, MAX_PATH, NULL, NULL);
		string current_path = char_path;
		current_path_size = current_path.size();
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
		char char_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, char_path, MAX_PATH, NULL, NULL);
		str_path = char_path;
		str_path = str_path.substr(current_path_size + 1, str_path.size());
	}
	else
	{
		str_path = "";
	}
	return str_path;
}