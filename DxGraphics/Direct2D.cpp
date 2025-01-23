// 本文件为DxGraphics库的示例代码，用于展示如何使用DxGraphics库创建一个窗口并绘制内容
// 本文件的代码将创建一个窗口并使用Direct2D绘制内容，仅供参考
// 本文件的代码不参与编译，不会生成可执行文件

#include "DxGraphics.h"
#include "DxGraphicsExC.h"

// Direct2D 工厂对象
DxFactory2D d2dFactory;
// Direct2D 渲染目标对象
DxTarget2D d2dTarget;
//DxWriteFactory dwriteFactory;
//DxFontCollection fontCollection;
//DxImagingFactory imagingFactory;
//DxImage image;
HACCEL hAccel = nullptr;

// 窗口过程函数声明
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

// 初始化函数声明
BOOL DXG_CALLBACK InitFunction(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPVOID lpCmdLine, int nCmdShow, LPVOID lpUserParam);

// 空闲函数声明
BOOL DXG_CALLBACK IdleFunction(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPVOID lpCmdLine, int nCmdShow, LPVOID lpUserParam);

// 应用程序入口点
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdline, _In_ int nShowCmd)
{
	// 消息循环参数
	MSGLOOPPROCPARAM param = { hInstance, hPrevInstance, lpCmdline, nShowCmd, nullptr };

	// 初始化 Direct2D 工厂
	d2dFactory.Initialize();
	//CoInitialize(nullptr);
	//imagingFactory.Initialize();

	// 启动消息循环
	int ret = DxGStartThreadMessageLoop(InitFunction, &param, IdleFunction, &param);

	// 反初始化 Direct2D 工厂
	//imagingFactory.Uninitialize();
	//CoUninitialize();
	d2dFactory.Uninitialize();

	return ret;
}

// 窗口过程函数定义
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CREATE:
		// 初始化 Direct2D 渲染目标
		d2dTarget.Initialize(d2dFactory, hWnd);
		// hAccel = LoadAcceleratorsW(nullptr, MAKEINTRESOURCEW(IDC_ACCELERATOR)); // 加载快捷键表
		DxGRegisterHWNDAttributesToThread(hWnd); // 注册窗口属性
		DxGBindAcceleratorToThreadHWNDAttributes(hWnd, hAccel); // 绑定快捷键表
		break;
	case WM_PAINT:
		// 开始绘制
		d2dTarget.BeginDraw();
		// 清除背景
		d2dTarget.Clear();

		// TODO: 绘制内容代码...

		// 结束绘制
		d2dTarget.EndDraw();
		break;
	case WM_COMMAND:
		// 快捷键处理
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_SIZE:
		// 调整渲染目标大小
		d2dTarget.Resize(DxGetSizeFromlParam(lParam));
		break;
	case WM_DESTROY:
		DxGUnregisterHWNDAttributesFromThread(hWnd);
		// 反初始化 Direct2D 渲染目标
		d2dTarget.Uninitialize();
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

// 初始化函数定义
// 返回值意义
//      TRUE: 初始化成功，消息泵将会启动
//      FALSE: 初始化失败，DxGStartThreadMessageLoop不会启动消息泵，并返回-1
BOOL DXG_CALLBACK InitFunction(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPVOID lpCmdLine, int nCmdShow, LPVOID lpUserParam)
{
	// 注册窗口类
	ATOM atom = DxGRegisterClassExW(
		CS_RECOMMEND,
		hInstance,
		L"DxGraphics",
		WndProc,
		nullptr, nullptr, nullptr, nullptr, nullptr
	);

	if (!atom) return FALSE; // 注册失败，返回FALSE，消息泵不会启动

	// 创建窗口
	HWND hWnd = CreateCentralWindowExW(
		CS_RECOMMEND,
		MAKEINTATOM(atom),
		L"DxGraphics",
		WS_OVERLAPPEDWINDOW,
		1280, 720,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd) return FALSE; // 创建失败，返回FALSE，消息泵不会启动

	// 显示并更新窗口
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE; // 返回TRUE以启动消息泵	
}

// 空闲函数定义
// lpUserParam应指向一段可以访问的内存，如用户自定义结构体，数据可以经此传入或传出
BOOL DXG_CALLBACK IdleFunction(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPVOID lpCmdLine, int nCmdShow, LPVOID lpUserParam)
{
	// TODO: 空闲函数代码...
	return TRUE; // 事实上程序不关心Idle函数的返回值，可以返回任意值
}
