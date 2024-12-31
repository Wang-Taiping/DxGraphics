#include "DxGraphicsExC.h"

ATOM DXG_API RegisterWndClassA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCSTR lpszMenuName)
{
	return RegisterWndClassExA(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

ATOM DXG_API RegisterWndClassW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCWSTR lpszMenuName)
{
	return RegisterWndClassExW(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

#pragma warning(disable: 4302)

ATOM DXG_API RegisterWndClassExA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCSTR lpszMenuName)
{
	WNDCLASSEXA wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEXA);
	wcex.style = style;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = lpszClassName;
	wcex.lpfnWndProc = lpfnWndProc;
	wcex.hIcon = hIcon;
	wcex.hIconSm = hIconSm;
	wcex.hCursor = hCursor ? hCursor : LoadCursorW(nullptr, MAKEINTRESOURCEW(IDC_ARROW));
	wcex.hbrBackground = hbrBackground ? hbrBackground : (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = lpszMenuName;
	return RegisterClassExA(&wcex);
}

ATOM DXG_API RegisterWndClassExW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCWSTR lpszMenuName)
{
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = style;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = lpszClassName;
	wcex.lpfnWndProc = lpfnWndProc;
	wcex.hIcon = hIcon;
	wcex.hIconSm = hIconSm;
	wcex.hCursor = hCursor ? hCursor : LoadCursorW(nullptr, MAKEINTRESOURCEW(IDC_ARROW));
	wcex.hbrBackground = hbrBackground ? hbrBackground : (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = lpszMenuName;
	return RegisterClassExW(&wcex);
}

#pragma warning(default: 4302)

#define MAX_HWNDATTRIBUTES_NUMBER	4096

thread_local struct
{
	MSGLOOPIDLE IdleCallback;
	IDLEPARAM IdleParam;
} ThreadMessageLoopIdleInformation;

thread_local bool ThreadMessageLoopStatus = false;

struct NativeHWNDAttributes
{
	HWND hWnd;
	HACCEL hAccel;
	//WNDPROC hHook;
};

thread_local NativeHWNDAttributes* ThreadHWNDAttributes = nullptr;
thread_local DWORD ThreadDefaultIdleWaitTime = 10;

static BOOL DXG_CALLBACK DefaultMsgLoopIdle(HINSTANCE, HINSTANCE, LPVOID, int, LPVOID)
{
	Sleep(ThreadDefaultIdleWaitTime);
	return 0;
}

int DXG_API StartThreadMessageLoop(MSGLOOPINIT InitCallback, INITPARAM InitParam, MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam)
{
	if (ThreadMessageLoopStatus) return -1;
	ThreadMessageLoopStatus = true;
	ThreadHWNDAttributes = new NativeHWNDAttributes[MAX_HWNDATTRIBUTES_NUMBER];
	memset(ThreadHWNDAttributes, 0, sizeof(NativeHWNDAttributes) * MAX_HWNDATTRIBUTES_NUMBER);
	ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
	ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
	BOOL InitResult = false;
	if (InitCallback) InitResult = InitCallback(InitParam.hInstance, InitParam.hPrevInstance, InitParam.lpCmdline, InitParam.nCmdShow, InitParam.lpUserParam);
	if (!InitResult) return -1;
	MSG Msg;
	while (true) {
	QueryMessage:
		if (PeekMessageW(&Msg, nullptr, 0, 0, PM_REMOVE)) {
			if (Msg.message == WM_QUIT) break;
			// TODO: Translate...
			for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
				if (ThreadHWNDAttributes[i].hWnd == Msg.hwnd && ThreadHWNDAttributes[i].hAccel && TranslateAcceleratorW(ThreadHWNDAttributes[i].hWnd, ThreadHWNDAttributes[i].hAccel, &Msg))
					goto QueryMessage;
			}
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		else ThreadMessageLoopIdleInformation.IdleCallback(ThreadMessageLoopIdleInformation.IdleParam.hInstance, ThreadMessageLoopIdleInformation.IdleParam.hPrevInstance, ThreadMessageLoopIdleInformation.IdleParam.lpCmdline, ThreadMessageLoopIdleInformation.IdleParam.nCmdShow, ThreadMessageLoopIdleInformation.IdleParam.lpUserParam);
	}
	delete[] ThreadHWNDAttributes;
	ThreadMessageLoopStatus = false;
	return int(Msg.wParam);
}

void DXG_API SetThreadDefaultIdleWaitTime(DWORD dwMilliseconds)
{
	ThreadDefaultIdleWaitTime = dwMilliseconds;
}

void DXG_API SetThreadMessageLoopIdleCallback(MSGLOOPIDLE IdleCallback)
{
	if (!ThreadMessageLoopStatus) return;
	ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
}

void DXG_API SetThreadMessageLoopIdleParam(IDLEPARAM IdleParam)
{
	if (!ThreadMessageLoopStatus) return;
	ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

void DXG_API SetThreadMessageLoopIdleInformation(MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam)
{
	if (!ThreadMessageLoopStatus) return;
	ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
	ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

BOOL DXG_API RegisterHWNDAttributesToThread(HWND hWnd)
{
	if (!ThreadMessageLoopStatus) return false;
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (ThreadHWNDAttributes[i].hWnd == hWnd) return false;
	}
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (!ThreadHWNDAttributes[i].hWnd) ThreadHWNDAttributes[i].hWnd = hWnd;
		return true;
	}
	return false;
}

BOOL DXG_API UnregisterHWNDAttributesFromThread(HWND hWnd)
{
	if (!ThreadMessageLoopStatus) return false;
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (ThreadHWNDAttributes[i].hWnd == hWnd) {
			memset(&ThreadHWNDAttributes[i], 0, sizeof(NativeHWNDAttributes));
			return true;
		}
	}
	return false;
}

BOOL DXG_API BindAcceleratorToThreadHWNDAttributes(HWND hWnd, HACCEL hAccel)
{
	if (!ThreadMessageLoopStatus) return false;
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (ThreadHWNDAttributes[i].hWnd == hWnd) ThreadHWNDAttributes[i].hAccel = hAccel;
		return true;
	}
	return false;
}
