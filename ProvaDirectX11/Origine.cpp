#include <Windows.h>
#include "GPU.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,LPSTR cmdLine, int showCmd)
{

	int larghezza = 1280, altezza = 720;
	HWND hwnd = 0;
	MSG msg;  memset(&msg, 0, sizeof(MSG));

	

	WNDCLASS wnd;
	memset(&wnd, 0, sizeof(WNDCLASS));

	wnd.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = L"ProvaDirectX11";

	if (!RegisterClass(&wnd))
	{
		MessageBox(hwnd, L"errore creazione finestra", L"Errore", 0);
		return -1;
	}

	hwnd = CreateWindow(L"ProvaDirectX11", L"DirectX11", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, larghezza,altezza, 0, 0, hInstance, 0);

	if (hwnd == NULL)
	{
		MessageBox(hwnd, L"errore creazione finestra", L"Errore", 0);
		return -1;
	}
	ShowWindow(hwnd, showCmd);
	UpdateWindow(hwnd);

	GPU* gpu = new GPU(larghezza, altezza, hwnd);
	gpu->Inizializza();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

			gpu->Renderizza();
			gpu->Update();
	}

	delete gpu;

	return (int)msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_QUIT:
		{
			PostQuitMessage(0);
			break;
		}

		default:
		{
			return DefWindowProc(hwnd, message, wparam, lparam);
		}
	}
	
	return DefWindowProc(hwnd, message, wparam, lparam);
}