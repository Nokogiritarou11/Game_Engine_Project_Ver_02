#include <Windows.h>
#include <memory>
#include <cassert>
#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#include "Engine.h"
#include "Input.h"
#include "Include_ImGui.h"
#include "Time_Engine.h"
#include <sstream>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
		case WM_ACTIVATEAPP:
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SETFOCUS:
			BeastEngine::Engine::Get_Handle(message, wParam, lParam);
			break;
		case WM_KILLFOCUS:
			BeastEngine::Engine::Get_Handle(message, wParam, lParam);
			break;
			/*
			case WM_CREATE:
				break;
			case WM_KEYDOWN:
				break;
			case WM_KEYUP:
				break;
			case WM_INPUT:
				Input::ProcessMessage(message, wParam, lParam);
				break;
			case WM_MOUSEMOVE:
				break;
			case WM_LBUTTONDOWN:
				break;
			case WM_LBUTTONUP:
				break;
			case WM_RBUTTONDOWN:
				break;
			case WM_RBUTTONUP:
				break;
			*/
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return(DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// �E�B���h�E����
	TCHAR szWindowClass[] = TEXT("Game_Engine");
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	const HWND hwnd = CreateWindow(szWindowClass,
	                         szWindowClass,
	                         WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	                         0, 0, BeastEngine::DxSystem::Get_Screen_Width(),
	                         BeastEngine::DxSystem::Get_Screen_Height(),
	                         nullptr,
	                         nullptr,
	                         hInstance,
	                         nullptr);

	ShowWindow(hwnd, nCmdShow);

	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

	// �f�o�C�X������
	if (!BeastEngine::DxSystem::Initialize(hwnd, BeastEngine::DxSystem::Get_Screen_Width(), BeastEngine::DxSystem::Get_Screen_Height()))
	{
		return 0;
	}

	std::unique_ptr<BeastEngine::Engine> engine = std::make_unique<BeastEngine::Engine>();
	BeastEngine::Time::time_scale = 1.0f;

	//���C�����[�v
	MSG hmsg = { nullptr };
	float Interval = 1.0f;
	ULONGLONG before = GetTickCount64();
	int fps = 0;

	while (hmsg.message != WM_QUIT)
	{
		if (PeekMessage(&hmsg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&hmsg);
			DispatchMessage(&hmsg);
		}
		else
		{
			BeastEngine::Time::delta_time = static_cast<float>(GetTickCount64() - before) * 0.001f * BeastEngine::Time::time_scale;

			before = GetTickCount64();
			const float mspf = 1000.0f / static_cast<float>(fps);

			Interval -= BeastEngine::Time::delta_time;
			++fps;
			if (Interval < 0)
			{
				std::ostringstream outs;
				outs.precision(6);
				outs << "Game_Engine    " << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
				SetWindowTextA(hwnd, outs.str().c_str());
				Interval += 1.0f;
				fps = 0;
			}

			// �X�V�E�`��
			engine->Update();
		}
	}
	engine.reset();
	return 0;
}