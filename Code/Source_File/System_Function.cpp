#include "System_Function.h"
#include "DxSystem.h"
#include <clocale>
#include <tchar.h>
#include <Windows.h>
#pragma comment(lib, "rpcrt4.lib")
using namespace std;
using namespace BeastEngine;

//�J���t�@�C���̃p�X�擾
string System_Function::Get_Open_File_Name(const string& extension, const string& initial_dir)
{
	static OPENFILENAME ofn;
	static TCHAR        sz_path[MAX_PATH];
	static TCHAR        sz_file[MAX_PATH];
	static string       str_path;
	static string       current_path;
	static size_t       current_path_size;

	if (sz_path[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, sz_path);
		char char_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, sz_path, -1, char_path, MAX_PATH, nullptr, nullptr);
		current_path = char_path;
		current_path_size = current_path.size();
	}

	const string path = current_path + initial_dir;

	setlocale(LC_ALL, "japanese");
	wchar_t w_path[MAX_PATH];
	size_t value = 0;
	mbstowcs_s(&value, w_path, path.c_str(), _TRUNCATE);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = DxSystem::hwnd;
	ofn.lpstrInitialDir = w_path; // �����t�H���_�ʒu
	ofn.lpstrFile = sz_file;       // �I���t�@�C���i�[
	ofn.nMaxFile = MAX_PATH;

	//�g���q����t�B���^���쐬����
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
	else if (extension == "controller")
	{
		ofn.lpstrFilter = TEXT("Controller�t�@�C��(*.controller*)\0*.controller*\0");
	}
	else if (extension == "mat")
	{
		ofn.lpstrFilter = TEXT("Material�t�@�C��(*.mat*)\0*.mat*\0");
	}

	ofn.lpstrTitle = TEXT("�t�@�C���I��");
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn))
	{
		char c_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, sz_file, -1, c_path, MAX_PATH, nullptr, nullptr);
		str_path = c_path;
		str_path = str_path.substr(current_path_size + 1, str_path.size());
		ZeroMemory(sz_file, sizeof(sz_file));
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
	static OPENFILENAME     ofn;
	static TCHAR            sz_path[MAX_PATH];
	static TCHAR            sz_file[MAX_PATH];
	static string           str_path;
	static string			current_path;
	static size_t           current_path_size;

	if (sz_path[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, sz_path);
		char char_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, sz_path, -1, char_path, MAX_PATH, nullptr, nullptr);
		current_path = char_path;
		current_path_size = current_path.size();
	}

	const string path = current_path + initial_dir;

	setlocale(LC_ALL, "japanese");
	wchar_t w_path[MAX_PATH];
	size_t value = 0;
	mbstowcs_s(&value, w_path, path.c_str(), _TRUNCATE);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = DxSystem::hwnd;
	ofn.lpstrInitialDir = w_path; // �����t�H���_�ʒu
	ofn.lpstrFile = sz_file;       // �I���t�@�C���i�[
	ofn.nMaxFile = MAX_PATH;

	//�g���q����t�B���^���쐬����
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
	else if (extension == "controller")
	{
		ofn.lpstrFilter = TEXT("Controller�t�@�C��(*.controller*)\0*.controller*\0");
	}
	else if (extension == "mat")
	{
		ofn.lpstrFilter = TEXT("Material�t�@�C��(*.mat*)\0*.mat*\0");
	}

	ofn.lpstrTitle = TEXT("�t�@�C���I��");
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = TEXT("���O�����ĕۑ�");
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn))
	{
		char char_path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, sz_file, -1, char_path, MAX_PATH, nullptr, nullptr);
		str_path = char_path;
		str_path = str_path.substr(current_path_size + 1, str_path.size());
	}
	else
	{
		str_path = "";
	}
	return str_path;
}

string System_Function::Create_Id()
{
	//UUID(��΂ɔ��Ȃ��ŗL�̕�����)���쐬����
	UUID uuid = {};
	string guid;

	(void)UuidCreate(&uuid);

	RPC_CSTR sz_uuid = nullptr;
	if (UuidToStringA(&uuid, &sz_uuid) == RPC_S_OK)
	{
		guid = reinterpret_cast<char*>(sz_uuid);
		RpcStringFreeA(&sz_uuid);
	}

	return guid;
}