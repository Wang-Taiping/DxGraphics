#include "DxGraphicsExC.h"

// ע�ᴰ���� (ANSI �汾)
ATOM DXG_API RegisterWndClassA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCSTR lpszMenuName)
{
    return RegisterWndClassExA(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

// ע�ᴰ���� (Unicode �汾)
ATOM DXG_API RegisterWndClassW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCWSTR lpszMenuName)
{
    return RegisterWndClassExW(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

// ���þ���4302���ض�ת��
#pragma warning(disable: 4302)

// ע����չ������ (ANSI �汾)
ATOM DXG_API RegisterWndClassExA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCSTR lpszMenuName)
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
ATOM DXG_API RegisterWndClassExW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCWSTR lpszMenuName)
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
    IDLEPARAM IdleParam;      // ���в���
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
int DXG_API StartThreadMessageLoop(MSGLOOPINIT InitCallback, INITPARAM InitParam, MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam)
{
    if (ThreadMessageLoopStatus) return -1; // �����Ϣѭ���Ѿ��������򷵻�
    ThreadMessageLoopStatus = true; // ������Ϣѭ��״̬
    ThreadHWNDAttributes = new NativeHWNDAttributes[MAX_HWNDATTRIBUTES_NUMBER]; // ���䴰����������
    memset(ThreadHWNDAttributes, 0, sizeof(NativeHWNDAttributes) * MAX_HWNDATTRIBUTES_NUMBER); // ��ʼ������
    ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle; // ���ÿ��лص�����
    ThreadMessageLoopIdleInformation.IdleParam = IdleParam; // ���ÿ��в���

    BOOL InitResult = false;
    if (InitCallback) InitResult = InitCallback(InitParam.hInstance, InitParam.hPrevInstance, InitParam.lpCmdline, InitParam.nCmdShow, InitParam.lpUserParam); // ���ó�ʼ���ص�����
    if (!InitResult) return -1; // �����ʼ��ʧ�ܣ��򷵻�

    MSG Msg;
    while (true) {
    QueryMessage:
        if (PeekMessageW(&Msg, nullptr, 0, 0, PM_REMOVE)) {
            if (Msg.message == WM_QUIT) break; // ����յ�WM_QUIT��Ϣ�����˳�ѭ��
            // TODO: ������Ϣ����
            for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
                if (ThreadHWNDAttributes[i].hWnd == Msg.hwnd && ThreadHWNDAttributes[i].hAccel && TranslateAcceleratorW(ThreadHWNDAttributes[i].hWnd, ThreadHWNDAttributes[i].hAccel, &Msg))
                    goto QueryMessage; // ������ټ���Ϣ
            }
            TranslateMessage(&Msg); // ������Ϣ
            DispatchMessageW(&Msg); // �ַ���Ϣ
        }
        else ThreadMessageLoopIdleInformation.IdleCallback(ThreadMessageLoopIdleInformation.IdleParam.hInstance, ThreadMessageLoopIdleInformation.IdleParam.hPrevInstance, ThreadMessageLoopIdleInformation.IdleParam.lpCmdline, ThreadMessageLoopIdleInformation.IdleParam.nCmdShow, ThreadMessageLoopIdleInformation.IdleParam.lpUserParam); // ���ÿ��лص�����
    }

    delete[] ThreadHWNDAttributes; // �ͷŴ�����������
    ThreadMessageLoopStatus = false; // ������Ϣѭ��״̬
    return int(Msg.wParam); // ������Ϣѭ�����
}

// �����߳�Ĭ�Ͽ��еȴ�ʱ��
void DXG_API SetThreadDefaultIdleWaitTime(DWORD dwMilliseconds)
{
    ThreadDefaultIdleWaitTime = dwMilliseconds;
}

// �����߳���Ϣѭ�����лص�����
void DXG_API SetThreadMessageLoopIdleCallback(MSGLOOPIDLE IdleCallback)
{
    if (!ThreadMessageLoopStatus) return;
    ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
}

// �����߳���Ϣѭ�����в���
void DXG_API SetThreadMessageLoopIdleParam(IDLEPARAM IdleParam)
{
    if (!ThreadMessageLoopStatus) return;
    ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

// �����߳���Ϣѭ��������Ϣ
void DXG_API SetThreadMessageLoopIdleInformation(MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam)
{
    if (!ThreadMessageLoopStatus) return;
    ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
    ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

// ע�ᴰ�����Ե��߳�
BOOL DXG_API RegisterHWNDAttributesToThread(HWND hWnd)
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
BOOL DXG_API UnregisterHWNDAttributesFromThread(HWND hWnd)
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
BOOL DXG_API BindAcceleratorToThreadHWNDAttributes(HWND hWnd, HACCEL hAccel)
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