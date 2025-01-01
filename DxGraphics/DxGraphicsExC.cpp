#include "DxGraphicsExC.h"

// 注册窗口类 (ANSI 版本)
ATOM DXG_API RegisterWndClassA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCSTR lpszMenuName)
{
    return RegisterWndClassExA(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

// 注册窗口类 (Unicode 版本)
ATOM DXG_API RegisterWndClassW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCWSTR lpszMenuName)
{
    return RegisterWndClassExW(style, hInstance, lpszClassName, lpfnWndProc, hIcon, nullptr, hCursor, nullptr, lpszMenuName);
}

// 禁用警告4302：截断转换
#pragma warning(disable: 4302)

// 注册扩展窗口类 (ANSI 版本)
ATOM DXG_API RegisterWndClassExA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCSTR lpszMenuName)
{
    // 初始化WNDCLASSEXA结构
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

// 注册扩展窗口类 (Unicode 版本)
ATOM DXG_API RegisterWndClassExW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCWSTR lpszMenuName)
{
    // 初始化WNDCLASSEXW结构
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

// 重新启用警告4302
#pragma warning(default: 4302)

// 最大窗口属性数量
#define MAX_HWNDATTRIBUTES_NUMBER    4096

// 线程局部存储：消息循环空闲信息
thread_local struct
{
    MSGLOOPIDLE IdleCallback; // 空闲回调函数
    IDLEPARAM IdleParam;      // 空闲参数
} ThreadMessageLoopIdleInformation;

// 线程局部存储：消息循环状态
thread_local bool ThreadMessageLoopStatus = false;

// 原生窗口属性结构
struct NativeHWNDAttributes
{
    HWND hWnd;      // 窗口句柄
    HACCEL hAccel;  // 加速键表
};

// 线程局部存储：窗口属性指针
thread_local NativeHWNDAttributes* ThreadHWNDAttributes = nullptr;
// 线程局部存储：默认空闲等待时间
thread_local DWORD ThreadDefaultIdleWaitTime = 10;

// 默认消息循环空闲处理函数
static BOOL DXG_CALLBACK DefaultMsgLoopIdle(HINSTANCE, HINSTANCE, LPVOID, int, LPVOID)
{
    Sleep(ThreadDefaultIdleWaitTime); // 休眠一段时间
    return 0;
}

// 启动线程消息循环
int DXG_API StartThreadMessageLoop(MSGLOOPINIT InitCallback, INITPARAM InitParam, MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam)
{
    if (ThreadMessageLoopStatus) return -1; // 如果消息循环已经启动，则返回
    ThreadMessageLoopStatus = true; // 设置消息循环状态
    ThreadHWNDAttributes = new NativeHWNDAttributes[MAX_HWNDATTRIBUTES_NUMBER]; // 分配窗口属性数组
    memset(ThreadHWNDAttributes, 0, sizeof(NativeHWNDAttributes) * MAX_HWNDATTRIBUTES_NUMBER); // 初始化数组
    ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle; // 设置空闲回调函数
    ThreadMessageLoopIdleInformation.IdleParam = IdleParam; // 设置空闲参数

    BOOL InitResult = false;
    if (InitCallback) InitResult = InitCallback(InitParam.hInstance, InitParam.hPrevInstance, InitParam.lpCmdline, InitParam.nCmdShow, InitParam.lpUserParam); // 调用初始化回调函数
    if (!InitResult) return -1; // 如果初始化失败，则返回

    MSG Msg;
    while (true) {
    QueryMessage:
        if (PeekMessageW(&Msg, nullptr, 0, 0, PM_REMOVE)) {
            if (Msg.message == WM_QUIT) break; // 如果收到WM_QUIT消息，则退出循环
            // TODO: 处理消息翻译
            for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
                if (ThreadHWNDAttributes[i].hWnd == Msg.hwnd && ThreadHWNDAttributes[i].hAccel && TranslateAcceleratorW(ThreadHWNDAttributes[i].hWnd, ThreadHWNDAttributes[i].hAccel, &Msg))
                    goto QueryMessage; // 翻译加速键消息
            }
            TranslateMessage(&Msg); // 翻译消息
            DispatchMessageW(&Msg); // 分发消息
        }
        else ThreadMessageLoopIdleInformation.IdleCallback(ThreadMessageLoopIdleInformation.IdleParam.hInstance, ThreadMessageLoopIdleInformation.IdleParam.hPrevInstance, ThreadMessageLoopIdleInformation.IdleParam.lpCmdline, ThreadMessageLoopIdleInformation.IdleParam.nCmdShow, ThreadMessageLoopIdleInformation.IdleParam.lpUserParam); // 调用空闲回调函数
    }

    delete[] ThreadHWNDAttributes; // 释放窗口属性数组
    ThreadMessageLoopStatus = false; // 重置消息循环状态
    return int(Msg.wParam); // 返回消息循环结果
}

// 设置线程默认空闲等待时间
void DXG_API SetThreadDefaultIdleWaitTime(DWORD dwMilliseconds)
{
    ThreadDefaultIdleWaitTime = dwMilliseconds;
}

// 设置线程消息循环空闲回调函数
void DXG_API SetThreadMessageLoopIdleCallback(MSGLOOPIDLE IdleCallback)
{
    if (!ThreadMessageLoopStatus) return;
    ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
}

// 设置线程消息循环空闲参数
void DXG_API SetThreadMessageLoopIdleParam(IDLEPARAM IdleParam)
{
    if (!ThreadMessageLoopStatus) return;
    ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

// 设置线程消息循环空闲信息
void DXG_API SetThreadMessageLoopIdleInformation(MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam)
{
    if (!ThreadMessageLoopStatus) return;
    ThreadMessageLoopIdleInformation.IdleCallback = IdleCallback ? IdleCallback : DefaultMsgLoopIdle;
    ThreadMessageLoopIdleInformation.IdleParam = IdleParam;
}

// 注册窗口属性到线程
BOOL DXG_API RegisterHWNDAttributesToThread(HWND hWnd)
{
    if (!ThreadMessageLoopStatus) return false;
    for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
        if (ThreadHWNDAttributes[i].hWnd == hWnd) return false; // 如果已经注册过，则返回false
    }
    for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
        if (!ThreadHWNDAttributes[i].hWnd) {
            ThreadHWNDAttributes[i].hWnd = hWnd; // 注册窗口属性
            return true;
        }
    }
    return false; // 如果数组已满，则返回false
}

// 从线程注销窗口属性
BOOL DXG_API UnregisterHWNDAttributesFromThread(HWND hWnd)
{
    if (!ThreadMessageLoopStatus) return false;
    for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
        if (ThreadHWNDAttributes[i].hWnd == hWnd) {
            memset(&ThreadHWNDAttributes[i], 0, sizeof(NativeHWNDAttributes)); // 清除窗口属性
            return true;
        }
    }
    return false; // 如果未找到，则返回false
}

// 绑定加速键表到线程窗口属性
BOOL DXG_API BindAcceleratorToThreadHWNDAttributes(HWND hWnd, HACCEL hAccel)
{
    if (!ThreadMessageLoopStatus) return false;
    for (size_t i = 0; i < MAX_HWNDATTRIBUTES_NUMBER; i++) {
        if (ThreadHWNDAttributes[i].hWnd == hWnd) {
            ThreadHWNDAttributes[i].hAccel = hAccel; // 绑定加速键表
            return true;
        }
    }
    return false; // 如果未找到窗口句柄，则返回false
}