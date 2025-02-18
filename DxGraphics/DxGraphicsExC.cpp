#include "DxGraphicsExC.h"

// ע�ᴰ���� (ANSI �汾)
ATOM DXG_API DxGRegisterClassA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCSTR lpszMenuName)
{
	return DxGRegisterClassExA(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

// ע�ᴰ���� (Unicode �汾)
ATOM DXG_API DxGRegisterClassW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCWSTR lpszMenuName)
{
	return DxGRegisterClassExW(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

// ���þ���4302���ض�ת��
#pragma warning(disable: 4302)

// ע����չ������ (ANSI �汾)
ATOM DXG_API DxGRegisterClassExA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCSTR lpszMenuName)
{
	// ��ʼ��WNDCLASSEXA�ṹ
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

// ע����չ������ (Unicode �汾)
ATOM DXG_API DxGRegisterClassExW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCWSTR lpszMenuName)
{
	// ��ʼ��WNDCLASSEXW�ṹ
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

// �������þ���4302
#pragma warning(default: 4302)

// ��󴰿���������
#define MAX_HWNDATTRIBUTES_NUMBER    4096

// �ֲ߳̾��洢����Ϣѭ��������Ϣ
thread_local struct
{
	MSGLOOPIDLE IdleCallback; // ���лص�����
	PIDLEPARAM IdleParam;     // ���в���
} ThreadMessageLoopIdleInformation;

// �ֲ߳̾��洢����Ϣѭ��״̬
thread_local bool ThreadMessageLoopStatus = false;

// ԭ���������Խṹ
struct NativeHWNDAttributes
{
	HWND hWnd;      // ���ھ��
	HACCEL hAccel;  // ���ټ���
};

// �ֲ߳̾��洢����������ָ��
thread_local NativeHWNDAttributes* ThreadHWNDAttributes = nullptr;
// �ֲ߳̾��洢��Ĭ�Ͽ��еȴ�ʱ��
thread_local DWORD ThreadDefaultIdleWaitTime = 10;

// Ĭ����Ϣѭ�����д�����
static BOOL DXG_CALLBACK DefaultMsgLoopIdle(HINSTANCE, HINSTANCE, LPVOID, int, LPVOID)
{
	Sleep(ThreadDefaultIdleWaitTime); // ����һ��ʱ��
	return 0;
}

// �����߳���Ϣѭ��
int DXG_API DxGStartThreadMessageLoop(MSGLOOPINIT InitCallback, PINITPARAM InitParam, MSGLOOPIDLE IdleCallback, PIDLEPARAM IdleParam)
{
	if (ThreadMessageLoopStatus) return -1; // �����Ϣѭ���Ѿ��������򷵻�
	ThreadMessageLoopStatus = true; // ������Ϣѭ��״̬
	ThreadHWNDAttributes = new NativeHWNDAttributes[MAX_HWNDATTRIBUTES_NUMBER]; // ���䴰����������
	memset(ThreadHWNDAttributes, 0, sizeof(NativeHWNDAttributes) * MAX_HWNDATTRIBUTES_NUMBER); // ��ʼ������
	ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle; // ���ÿ��лص�����
	if (IdleParam) ThreadMessageLoopIdleInformation.IdleParam = IdleParam; // ���ÿ��в���
	else ThreadMessageLoopIdleInformation.IdleParam = { 0 }; // ��ʼ�����в���
	INITPARAM DefaultInitParam = { 0 }; // ��ʼ��Ĭ�ϳ�ʼ������
	if (InitParam) DefaultInitParam = *InitParam; // ���ó�ʼ������

	BOOL InitResult = false;
	if (InitCallback) InitResult = InitCallback(DefaultInitParam.hInstance, DefaultInitParam.hPrevInstance, DefaultInitParam.lpCmdline, DefaultInitParam.nCmdShow, DefaultInitParam.lpUserParam); // ���ó�ʼ���ص�����
	if (!InitResult) return -1; // �����ʼ��ʧ�ܣ��򷵻�

	MSG Msg;
	while (true) {
	QueryMessage:
		if (PeekMessageW(&Msg, nullptr, 0, 0, PM_REMOVE)) {
			if (Msg.message == WM_QUIT) break; // ����յ�WM_QUIT��Ϣ�����˳�ѭ��
			// TODO: ������Ϣ����
			for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
				if (ThreadHWNDAttributes[i].hWnd == Msg.hwnd &&
					ThreadHWNDAttributes[i].hAccel &&
					TranslateAcceleratorW(
						ThreadHWNDAttributes[i].hWnd,
						ThreadHWNDAttributes[i].hAccel,
						&Msg)
					) goto QueryMessage; // ��ȡ���ټ���Ϣ
			}
			TranslateMessage(&Msg); // ������Ϣ
			DispatchMessageW(&Msg); // �ַ���Ϣ
		}
		else if (ThreadMessageLoopIdleInformation.IdleParam)
			ThreadMessageLoopIdleInformation.IdleCallback(
				ThreadMessageLoopIdleInformation.IdleParam->hInstance,
				ThreadMessageLoopIdleInformation.IdleParam->hPrevInstance,
				ThreadMessageLoopIdleInformation.IdleParam->lpCmdline,
				ThreadMessageLoopIdleInformation.IdleParam->nCmdShow,
				ThreadMessageLoopIdleInformation.IdleParam->lpUserParam
			); // ���ÿ��лص�����
		else ThreadMessageLoopIdleInformation.IdleCallback(nullptr, nullptr, nullptr, 0, nullptr);
	}

	delete[] ThreadHWNDAttributes; // �ͷŴ�����������
	ThreadMessageLoopStatus = false; // ������Ϣѭ��״̬
	return int(Msg.wParam); // ������Ϣѭ�����
}

// �����߳�Ĭ�Ͽ��еȴ�ʱ��
void DXG_API DxGSetThreadDefaultIdleWaitTime(DWORD dwMilliseconds)
{
	ThreadDefaultIdleWaitTime = dwMilliseconds;
}

// �����߳���Ϣѭ�����лص�����
void DXG_API DxGSetThreadMessageLoopIdleCallback(MSGLOOPIDLE IdleCallback)
{
	if (!ThreadMessageLoopStatus) return;
	ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
}

// �����߳���Ϣѭ�����в���
void DXG_API DxGSetThreadMessageLoopIdleParam(PIDLEPARAM IdleParam)
{
	if (!ThreadMessageLoopStatus) return;
	ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

// �����߳���Ϣѭ��������Ϣ
void DXG_API DxGSetThreadMessageLoopIdleInformation(MSGLOOPIDLE IdleCallback, PIDLEPARAM IdleParam)
{
	if (!ThreadMessageLoopStatus) return;
	ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
	ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

// ע�ᴰ�����Ե��߳�
BOOL DXG_API DxGRegisterHWNDAttributesToThread(HWND hWnd)
{
	if (!ThreadMessageLoopStatus) return false;
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (ThreadHWNDAttributes[i].hWnd == hWnd) return false; // ����Ѿ�ע������򷵻�false
	}
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (!ThreadHWNDAttributes[i].hWnd) {
			ThreadHWNDAttributes[i].hWnd = hWnd; // ע�ᴰ������
			return true;
		}
	}
	return false; // ��������������򷵻�false
}

// ���߳�ע����������
BOOL DXG_API DxGUnregisterHWNDAttributesFromThread(HWND hWnd)
{
	if (!ThreadMessageLoopStatus) return false;
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (ThreadHWNDAttributes[i].hWnd == hWnd) {
			memset(&ThreadHWNDAttributes[i], 0, sizeof(NativeHWNDAttributes)); // �����������
			return true;
		}
	}
	return false; // ���δ�ҵ����򷵻�false
}

// �󶨼��ټ����̴߳�������
BOOL DXG_API DxGBindAcceleratorToThreadHWNDAttributes(HWND hWnd, HACCEL hAccel)
{
	if (!ThreadMessageLoopStatus) return false;
	for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
		if (ThreadHWNDAttributes[i].hWnd == hWnd) {
			ThreadHWNDAttributes[i].hAccel = hAccel; // �󶨼��ټ���
			return true;
		}
	}
	return false; // ���δ�ҵ����ھ�����򷵻�false
}