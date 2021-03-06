#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>
#include <time.h>
#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#include <SDKDDKVer.h>
#include "Engine.h"
#include "Input.h"
#include "Include_ImGui.h"
#include "Time.h"
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
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SETFOCUS:
			Engine::GetHundle(message, wParam, lParam);
			break;
		case WM_KILLFOCUS:
			Engine::GetHundle(message, wParam, lParam);
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
	// ウィンドウ生成
	TCHAR szWindowClass[] = TEXT("Game_Engine");
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	HWND hWnd;
	hWnd = CreateWindow(szWindowClass,
		szWindowClass,
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight(),
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

	// デバイス初期化
	if (FAILED((HRESULT)DxSystem::Initialize(hWnd, DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight())))
	{
		return 0;
	}

	Engine* engine = new Engine();
	Time::timeScale = 1.0f;

	//メインループ
	MSG hMsg = { 0 };
	float Interval = 1.0f;
	ULONGLONG before = GetTickCount64();
	int fps = 0;

	while (hMsg.message != WM_QUIT)
	{
		if (PeekMessage(&hMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
		else
		{
			Time::deltaTime = //1/60.0f;
				(GetTickCount64() - before) * 0.001f * Time::timeScale;

			before = GetTickCount64();
			float mspf = 1000.0f / fps;

			Interval -= Time::deltaTime;
			fps++;
			if (Interval < 0)
			{
				std::ostringstream outs;
				outs.precision(6);
				outs << "Game_Engine    " << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
				SetWindowTextA(hWnd, outs.str().c_str());
				Interval += 1.0f;
				fps = 0;
			}

			// 更新・描画
			engine->Update();
		}
	}
	SAFE_DELETE(engine);
	return 0;
}