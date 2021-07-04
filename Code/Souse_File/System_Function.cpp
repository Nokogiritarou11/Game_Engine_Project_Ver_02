#include "System_Function.h"
#include "DxSystem.h"
#include <clocale>
#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "rpcrt4.lib")
using namespace std;
using namespace BeastEngine;

//�J���t�@�C���̃p�X�擾
string System_Function::Get_Open_File_Name(string extension, string initial_dir)
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

	string path = current_path + initial_dir;

	setlocale(LC_ALL, "japanese");
	wchar_t w_path[MAX_PATH];
	size_t value = 0;
	mbstowcs_s(&value, w_path, path.c_str(), _TRUNCATE);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = DxSystem::hwnd;
	ofn.lpstrInitialDir = w_path; // �����t�H���_�ʒu
	ofn.lpstrFile = szFile;       // �I���t�@�C���i�[
	ofn.nMaxFile = MAX_PATH;
	if (extension.empty())
	{
		ofn.lpstrFilter = TEXT("���ׂẴt�@�C��(*.*)\0*.*\0");
	}
	else if (extension == "anim")
	{
		ofn.lpstrFilter = TEXT("Animation�t�@�C��(*.anim*)\0*.anim*\0");
	}
	else if (extension == "fbx")
	{
		ofn.lpstrFilter = TEXT("FBX�t�@�C��(*.fbx*)\0*.fbx*\0");
	}
	else if (extension == "bin")
	{
		ofn.lpstrFilter = TEXT("Scene�t�@�C��(*.bin*)\0*.bin*\0");
	}
	else if (extension == "wav")
	{
		ofn.lpstrFilter = TEXT("WAV�t�@�C��(*.wav*)\0*.wav*\0");
	}
	else if (extension == "prefab")
	{
		ofn.lpstrFilter = TEXT("Prefab�t�@�C��(*.prefab*)\0*.prefab*\0");
	}
	else if (extension == "png")
	{
		ofn.lpstrFilter = TEXT("PNG�t�@�C��(*.png*)\0*.png*\0");
	}
	ofn.lpstrTitle = TEXT("�t�@�C���I��");
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn))
	{
		char c_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, c_path, MAX_PATH, NULL, NULL);
		str_path = c_path;
		str_path = str_path.substr(current_path_size + 1, str_path.size());
		ZeroMemory(szFile, sizeof(szFile));
	}
	else
	{
		str_path = "";
	}

	return str_path;
}

//�ۑ���p�X�擾
string System_Function::Get_Save_File_Name(string extension, string initial_dir)
{
	OPENFILENAME     ofn;
	TCHAR            szPath[MAX_PATH];
	TCHAR            szFile[MAX_PATH];
	string           str_path;
	size_t           current_path_size;

	GetCurrentDirectory(MAX_PATH, szPath);
	char char_path[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, szPath, -1, char_path, MAX_PATH, NULL, NULL);
	string current_path = char_path;
	current_path_size = current_path.size();

	current_path += initial_dir;

	string filter;
	if (extension.empty())
	{
		filter = "���ׂẴt�@�C��(*.*)\0 * .*\0";
	}
	else
	{
		filter = extension + "�t�@�C��" + "(*." + extension + "*)\0" + "*." + extension + "*\0";
	}
	setlocale(LC_ALL, "japanese");
	wchar_t w_filter[MAX_PATH];
	wchar_t w_path[MAX_PATH];
	size_t value = 0;
	mbstowcs_s(&value, w_filter, filter.c_str(), _TRUNCATE);
	mbstowcs_s(&value, w_path, current_path.c_str(), _TRUNCATE);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = DxSystem::hwnd;
	ofn.lpstrInitialDir = w_path; // �����t�H���_�ʒu
	ofn.lpstrFile = szFile;       // �I���t�@�C���i�[
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = w_filter;
	ofn.lpstrTitle = TEXT("���O�����ĕۑ�");
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

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

string System_Function::Create_ID()
{
	UUID uuid = { 0 };
	string guid;

	UuidCreate(&uuid);

	RPC_CSTR szUuid = NULL;
	if (UuidToStringA(&uuid, &szUuid) == RPC_S_OK)
	{
		guid = (char*)szUuid;
		RpcStringFreeA(&szUuid);
	}

	return guid;
}