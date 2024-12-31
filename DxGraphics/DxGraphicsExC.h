
#ifndef DXGRAPHICSEXC_H
#define DXGRAPHICSEXC_H

#include "DxGBase.h"
#include <wtypes.h>

#define CreateCentralWindowA(lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(0L, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateCentralWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateCentralWindowW(lpszClassName, lpszWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(0L, lpszClassName, lpszWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateCentralWindowExW(dwExStyle, lpszClassName, lpszWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(dwExStyle, lpszClassName, lpszWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#ifdef UNICODE
#define CreateCentralWindow		CreateCentralWindowW
#define CreateCentralWindowEx	CreateCentralWindowExW
#else
#define CreateCentralWindow		CreateCentralWindowA
#define CreateCentralWindowEx	CreateCentralWindowExA
#endif // !UNICODE

#define CS_RECOMMEND	0x0003

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	typedef BOOL(DXG_CALLBACK* MSGLOOPPROC)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPVOID lpCmdLine, int nCmdShow, LPVOID lpUserParam);
	typedef MSGLOOPPROC MSGLOOPINIT, MSGLOOPIDLE;
	struct MSGLOOPPROCPARAM {
		HINSTANCE hInstance;
		HINSTANCE hPrevInstance;
		LPVOID lpCmdline;
		int nCmdShow;
		LPVOID lpUserParam;
	};
	typedef struct MSGLOOPPROCPARAM INITPARAM, IDLEPARAM;

	DXG_EXPORT ATOM DXG_API RegisterWndClassA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCSTR lpszMenuName);
	DXG_EXPORT ATOM DXG_API RegisterWndClassW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCWSTR lpszMenuName);
	DXG_EXPORT ATOM DXG_API RegisterWndClassExA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCSTR lpszMenuName);
	DXG_EXPORT ATOM DXG_API RegisterWndClassExW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCWSTR lpszMenuName);

	DXG_EXPORT int DXG_API StartThreadMessageLoop(MSGLOOPINIT InitCallback, INITPARAM InitParam, MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam);
	DXG_EXPORT void DXG_API SetThreadDefaultIdleWaitTime(DWORD dwMilliseconds);
	DXG_EXPORT void DXG_API SetThreadMessageLoopIdleCallback(MSGLOOPIDLE IdleCallback);
	DXG_EXPORT void DXG_API SetThreadMessageLoopIdleParam(IDLEPARAM IdleParam);
	DXG_EXPORT void DXG_API SetThreadMessageLoopIdleInformation(MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam);
	DXG_EXPORT BOOL DXG_API RegisterHWNDAttributesToThread(HWND hWnd);
	DXG_EXPORT BOOL DXG_API UnregisterHWNDAttributesFromThread(HWND hWnd);
	DXG_EXPORT BOOL DXG_API BindAcceleratorToThreadHWNDAttributes(HWND hWnd, HACCEL hAccel);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !DXGRAPHICSEXC_H
