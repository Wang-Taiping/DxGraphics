#ifndef DXGRAPHICSEXC_H
#define DXGRAPHICSEXC_H

#include "DxGBase.h"
#include <wtypes.h>

/**
 * @brief 使用 CreateWindowExA 创建居中窗口的宏。
 *
 * @param lpClassName 窗口类名。
 * @param lpWindowName 窗口名称。
 * @param dwStyle 窗口样式。
 * @param nWidth 窗口宽度。
 * @param nHeight 窗口高度。
 * @param hWndParent 父窗口句柄。
 * @param hMenu 菜单句柄。
 * @param hInstance 实例句柄。
 * @param lpParam 附加参数。
 */
#define CreateCentralWindowA(lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(0L, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

 /**
  * @brief 使用带扩展样式的 CreateWindowExA 创建居中窗口的宏。
  *
  * @param dwExStyle 窗口扩展样式。
  * @param lpClassName 窗口类名。
  * @param lpWindowName 窗口名称。
  * @param dwStyle 窗口样式。
  * @param nWidth 窗口宽度。
  * @param nHeight 窗口高度。
  * @param hWndParent 父窗口句柄。
  * @param hMenu 菜单句柄。
  * @param hInstance 实例句柄。
  * @param lpParam 附加参数。
  */
#define CreateCentralWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

  /**
   * @brief 使用 CreateWindowExW 创建居中窗口的宏。
   *
   * @param lpszClassName 窗口类名。
   * @param lpszWindowName 窗口名称。
   * @param dwStyle 窗口样式。
   * @param nWidth 窗口宽度。
   * @param nHeight 窗口高度。
   * @param hWndParent 父窗口句柄。
   * @param hMenu 菜单句柄。
   * @param hInstance 实例句柄。
   * @param lpParam 附加参数。
   */
#define CreateCentralWindowW(lpszClassName, lpszWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(0L, lpszClassName, lpszWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

   /**
	* @brief 使用带扩展样式的 CreateWindowExW 创建居中窗口的宏。
	*
	* @param dwExStyle 窗口扩展样式。
	* @param lpszClassName 窗口类名。
	* @param lpszWindowName 窗口名称。
	* @param dwStyle 窗口样式。
	* @param nWidth 窗口宽度。
	* @param nHeight 窗口高度。
	* @param hWndParent 父窗口句柄。
	* @param hMenu 菜单句柄。
	* @param hInstance 实例句柄。
	* @param lpParam 附加参数。
	*/
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

	/**
	 * @brief 消息循环回调函数类型定义。
	 */
	typedef BOOL(DXG_CALLBACK* MSGLOOPPROC)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPVOID lpCmdLine, int nCmdShow, LPVOID lpUserParam);

	typedef MSGLOOPPROC MSGLOOPINIT, MSGLOOPIDLE;

	/**
	 * @brief 保存消息循环回调函数参数的结构体。
	 */
	struct MSGLOOPPROCPARAM {
		HINSTANCE hInstance; ///< 实例句柄。
		HINSTANCE hPrevInstance; ///< 前一个实例句柄。
		LPVOID lpCmdline; ///< 命令行参数。
		int nCmdShow; ///< 显示命令。
		LPVOID lpUserParam; ///< 用户定义参数。
	};

	typedef struct MSGLOOPPROCPARAM INITPARAM, * PINITPARAM, IDLEPARAM, * PIDLEPARAM;

	/**
	 * @brief 使用 ANSI 字符串注册窗口类。
	 *
	 * @param style 窗口类的样式。
	 * @param hInstance 实例句柄。
	 * @param lpszClassName 类名。
	 * @param lpfnWndProc 窗口过程。
	 * @param hIcon 图标句柄。
	 * @param hCursor 光标句柄。
	 * @param lpszMenuName 菜单名。
	 * @return 唯一标识类的原子值。
	 */
	DXG_EXPORT ATOM DXG_API DxGRegisterClassA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCSTR lpszMenuName);

	/**
	 * @brief 使用 Unicode 字符串注册窗口类。
	 *
	 * @param style 窗口类的样式。
	 * @param hInstance 实例句柄。
	 * @param lpszClassName 类名。
	 * @param lpfnWndProc 窗口过程。
	 * @param hIcon 图标句柄。
	 * @param hCursor 光标句柄。
	 * @param lpszMenuName 菜单名。
	 * @return 唯一标识类的原子值。
	 */
	DXG_EXPORT ATOM DXG_API DxGRegisterClassW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCWSTR lpszMenuName);

	/**
	 * @brief 使用 ANSI 字符串注册带扩展属性的窗口类。
	 *
	 * @param style 窗口类的样式。
	 * @param hInstance 实例句柄。
	 * @param lpszClassName 类名。
	 * @param lpfnWndProc 窗口过程。
	 * @param hIcon 图标句柄。
	 * @param hIconSm 小图标句柄。
	 * @param hCursor 光标句柄。
	 * @param hbrBackground 背景画刷句柄。
	 * @param lpszMenuName 菜单名。
	 * @return 唯一标识类的原子值。
	 */
	DXG_EXPORT ATOM DXG_API DxGRegisterClassExA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCSTR lpszMenuName);

	/**
	 * @brief 使用 Unicode 字符串注册带扩展属性的窗口类。
	 *
	 * @param style 窗口类的样式。
	 * @param hInstance 实例句柄。
	 * @param lpszClassName 类名。
	 * @param lpfnWndProc 窗口过程。
	 * @param hIcon 图标句柄。
	 * @param hIconSm 小图标句柄。
	 * @param hCursor 光标句柄。
	 * @param hbrBackground 背景画刷句柄。
	 * @param lpszMenuName 菜单名。
	 * @return 唯一标识类的原子值。
	 */
	DXG_EXPORT ATOM DXG_API DxGRegisterClassExW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCWSTR lpszMenuName);

	/**
	 * @brief 启动新线程上的消息循环。
	 *
	 * 如果传入 nullptr 作为 IdleCallback，程序使用默认的 idle 回调函数，该函数唯一的功能是等待一段时间（默认为 10 毫秒）。
	 * 可以通过 DxGSetThreadDefaultIdleWaitTime 更改等待时间。线程在等待期间不会检查消息。
	 * 如果 idle 回调函数迅速返回，程序会频繁检查线程消息，可能会导致 CPU 占用率升高。
	 * 如果 idle 回调函数返回时间过长，即使有新的线程消息到达，也会等到 idle 返回后才会检查新消息。
	 *
	 * @param InitCallback 初始化回调函数。
	 * @param InitParam 初始化参数。
	 * @param IdleCallback 空闲回调函数。
	 * @param IdleParam 空闲参数。
	 * @return 状态码。
	 */
	DXG_EXPORT int DXG_API DxGStartThreadMessageLoop(MSGLOOPINIT InitCallback, PINITPARAM InitParam, MSGLOOPIDLE IdleCallback, PIDLEPARAM IdleParam);

	/**
	 * @brief 设置线程消息循环的默认空闲等待时间。
	 *
	 * @param dwMilliseconds 空闲等待时间（毫秒）。
	 */
	DXG_EXPORT void DXG_API DxGSetThreadDefaultIdleWaitTime(DWORD dwMilliseconds);

	/**
	 * @brief 设置线程消息循环的空闲回调函数。
	 *
	 * @param IdleCallback 空闲回调函数。
	 */
	DXG_EXPORT void DXG_API DxGSetThreadMessageLoopIdleCallback(MSGLOOPIDLE IdleCallback);

	/**
	 * @brief 设置空闲回调函数的参数。
	 *
	 * @param IdleParam 空闲参数。
	 */
	DXG_EXPORT void DXG_API DxGSetThreadMessageLoopIdleParam(PIDLEPARAM IdleParam);

	/**
	 * @brief 设置线程消息循环的空闲回调函数及参数。
	 *
	 * @param IdleCallback 空闲回调函数。
	 * @param IdleParam 空闲参数。
	 */
	DXG_EXPORT void DXG_API DxGSetThreadMessageLoopIdleInformation(MSGLOOPIDLE IdleCallback, PIDLEPARAM IdleParam);

	/**
	 * @brief 将窗口属性注册到当前线程。
	 *
	 * @param hWnd 窗口句柄。
	 * @return 如果成功返回 TRUE，否则返回 FALSE。
	 */
	DXG_EXPORT BOOL DXG_API DxGRegisterHWNDAttributesToThread(HWND hWnd);

	/**
	 * @brief 从当前线程注销窗口属性。
	 *
	 * @param hWnd 窗口句柄。
	 * @return 如果成功返回 TRUE，否则返回 FALSE。
	 */
	DXG_EXPORT BOOL DXG_API DxGUnregisterHWNDAttributesFromThread(HWND hWnd);

	/**
	 * @brief 将加速表绑定到当前线程的窗口属性。
	 *
	 * @param hWnd 窗口句柄。
	 * @param hAccel 加速表句柄。
	 * @return 如果成功返回 TRUE，否则返回 FALSE。
	 */
	DXG_EXPORT BOOL DXG_API DxGBindAcceleratorToThreadHWNDAttributes(HWND hWnd, HACCEL hAccel);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !DXGRAPHICSEXC_H