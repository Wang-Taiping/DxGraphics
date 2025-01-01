#ifndef DXGRAPHICSEXC_H
#define DXGRAPHICSEXC_H

#include "DxGBase.h"
#include <wtypes.h>

/**
 * @brief ʹ�� CreateWindowExA �������д��ڵĺꡣ
 *
 * @param lpClassName ����������
 * @param lpWindowName �������ơ�
 * @param dwStyle ������ʽ��
 * @param nWidth ���ڿ�ȡ�
 * @param nHeight ���ڸ߶ȡ�
 * @param hWndParent �����ھ����
 * @param hMenu �˵������
 * @param hInstance ʵ�������
 * @param lpParam ���Ӳ�����
 */
#define CreateCentralWindowA(lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(0L, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

 /**
  * @brief ʹ�ô���չ��ʽ�� CreateWindowExA �������д��ڵĺꡣ
  *
  * @param dwExStyle ������չ��ʽ��
  * @param lpClassName ����������
  * @param lpWindowName �������ơ�
  * @param dwStyle ������ʽ��
  * @param nWidth ���ڿ�ȡ�
  * @param nHeight ���ڸ߶ȡ�
  * @param hWndParent �����ھ����
  * @param hMenu �˵������
  * @param hInstance ʵ�������
  * @param lpParam ���Ӳ�����
  */
#define CreateCentralWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

  /**
   * @brief ʹ�� CreateWindowExW �������д��ڵĺꡣ
   *
   * @param lpszClassName ����������
   * @param lpszWindowName �������ơ�
   * @param dwStyle ������ʽ��
   * @param nWidth ���ڿ�ȡ�
   * @param nHeight ���ڸ߶ȡ�
   * @param hWndParent �����ھ����
   * @param hMenu �˵������
   * @param hInstance ʵ�������
   * @param lpParam ���Ӳ�����
   */
#define CreateCentralWindowW(lpszClassName, lpszWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(0L, lpszClassName, lpszWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

   /**
	* @brief ʹ�ô���չ��ʽ�� CreateWindowExW �������д��ڵĺꡣ
	*
	* @param dwExStyle ������չ��ʽ��
	* @param lpszClassName ����������
	* @param lpszWindowName �������ơ�
	* @param dwStyle ������ʽ��
	* @param nWidth ���ڿ�ȡ�
	* @param nHeight ���ڸ߶ȡ�
	* @param hWndParent �����ھ����
	* @param hMenu �˵������
	* @param hInstance ʵ�������
	* @param lpParam ���Ӳ�����
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
	 * @brief ��Ϣѭ���ص��������Ͷ��塣
	 */
	typedef BOOL(DXG_CALLBACK* MSGLOOPPROC)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPVOID lpCmdLine, int nCmdShow, LPVOID lpUserParam);

	typedef MSGLOOPPROC MSGLOOPINIT, MSGLOOPIDLE;

	/**
	 * @brief ������Ϣѭ���ص����������Ľṹ�塣
	 */
	struct MSGLOOPPROCPARAM {
		HINSTANCE hInstance; ///< ʵ�������
		HINSTANCE hPrevInstance; ///< ǰһ��ʵ�������
		LPVOID lpCmdline; ///< �����в�����
		int nCmdShow; ///< ��ʾ���
		LPVOID lpUserParam; ///< �û����������
	};

	typedef struct MSGLOOPPROCPARAM INITPARAM, IDLEPARAM;

	/**
	 * @brief ʹ�� ANSI �ַ���ע�ᴰ���ࡣ
	 *
	 * @param style ���������ʽ��
	 * @param hInstance ʵ�������
	 * @param lpszClassName ������
	 * @param lpfnWndProc ���ڹ��̡�
	 * @param hIcon ͼ������
	 * @param hCursor �������
	 * @param lpszMenuName �˵�����
	 * @return Ψһ��ʶ���ԭ��ֵ��
	 */
	DXG_EXPORT ATOM DXG_API RegisterWndClassA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCSTR lpszMenuName);

	/**
	 * @brief ʹ�� Unicode �ַ���ע�ᴰ���ࡣ
	 *
	 * @param style ���������ʽ��
	 * @param hInstance ʵ�������
	 * @param lpszClassName ������
	 * @param lpfnWndProc ���ڹ��̡�
	 * @param hIcon ͼ������
	 * @param hCursor �������
	 * @param lpszMenuName �˵�����
	 * @return Ψһ��ʶ���ԭ��ֵ��
	 */
	DXG_EXPORT ATOM DXG_API RegisterWndClassW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HCURSOR hCursor, LPCWSTR lpszMenuName);

	/**
	 * @brief ʹ�� ANSI �ַ���ע�����չ���ԵĴ����ࡣ
	 *
	 * @param style ���������ʽ��
	 * @param hInstance ʵ�������
	 * @param lpszClassName ������
	 * @param lpfnWndProc ���ڹ��̡�
	 * @param hIcon ͼ������
	 * @param hIconSm Сͼ������
	 * @param hCursor �������
	 * @param hbrBackground ������ˢ�����
	 * @param lpszMenuName �˵�����
	 * @return Ψһ��ʶ���ԭ��ֵ��
	 */
	DXG_EXPORT ATOM DXG_API RegisterWndClassExA(UINT style, HINSTANCE hInstance, LPCSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCSTR lpszMenuName);

	/**
	 * @brief ʹ�� Unicode �ַ���ע�����չ���ԵĴ����ࡣ
	 *
	 * @param style ���������ʽ��
	 * @param hInstance ʵ�������
	 * @param lpszClassName ������
	 * @param lpfnWndProc ���ڹ��̡�
	 * @param hIcon ͼ������
	 * @param hIconSm Сͼ������
	 * @param hCursor �������
	 * @param hbrBackground ������ˢ�����
	 * @param lpszMenuName �˵�����
	 * @return Ψһ��ʶ���ԭ��ֵ��
	 */
	DXG_EXPORT ATOM DXG_API RegisterWndClassExW(UINT style, HINSTANCE hInstance, LPCWSTR lpszClassName, WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor, HBRUSH hbrBackground, LPCWSTR lpszMenuName);

	/**
	 * @brief �������߳��ϵ���Ϣѭ����
	 *
	 * ������� nullptr ��Ϊ IdleCallback������ʹ��Ĭ�ϵ� idle �ص��������ú���Ψһ�Ĺ����ǵȴ�һ��ʱ�䣨Ĭ��Ϊ 10 ���룩��
	 * ����ͨ�� SetThreadDefaultIdleWaitTime ���ĵȴ�ʱ�䡣�߳��ڵȴ��ڼ䲻������Ϣ��
	 * ��� idle �ص�����Ѹ�ٷ��أ������Ƶ������߳���Ϣ�����ܻᵼ�� CPU ռ�������ߡ�
	 * ��� idle �ص���������ʱ���������ʹ���µ��߳���Ϣ���Ҳ��ȵ� idle ���غ�Ż�������Ϣ��
	 *
	 * @param InitCallback ��ʼ���ص�������
	 * @param InitParam ��ʼ��������
	 * @param IdleCallback ���лص�������
	 * @param IdleParam ���в�����
	 * @return ״̬�롣
	 */
	DXG_EXPORT int DXG_API StartThreadMessageLoop(MSGLOOPINIT InitCallback, INITPARAM InitParam, MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam);

	/**
	 * @brief �����߳���Ϣѭ����Ĭ�Ͽ��еȴ�ʱ�䡣
	 *
	 * @param dwMilliseconds ���еȴ�ʱ�䣨���룩��
	 */
	DXG_EXPORT void DXG_API SetThreadDefaultIdleWaitTime(DWORD dwMilliseconds);

	/**
	 * @brief �����߳���Ϣѭ���Ŀ��лص�������
	 *
	 * @param IdleCallback ���лص�������
	 */
	DXG_EXPORT void DXG_API SetThreadMessageLoopIdleCallback(MSGLOOPIDLE IdleCallback);

	/**
	 * @brief ���ÿ��лص������Ĳ�����
	 *
	 * @param IdleParam ���в�����
	 */
	DXG_EXPORT void DXG_API SetThreadMessageLoopIdleParam(IDLEPARAM IdleParam);

	/**
	 * @brief �����߳���Ϣѭ���Ŀ��лص�������������
	 *
	 * @param IdleCallback ���лص�������
	 * @param IdleParam ���в�����
	 */
	DXG_EXPORT void DXG_API SetThreadMessageLoopIdleInformation(MSGLOOPIDLE IdleCallback, IDLEPARAM IdleParam);

	/**
	 * @brief ����������ע�ᵽ��ǰ�̡߳�
	 *
	 * @param hWnd ���ھ����
	 * @return ����ɹ����� TRUE�����򷵻� FALSE��
	 */
	DXG_EXPORT BOOL DXG_API RegisterHWNDAttributesToThread(HWND hWnd);

	/**
	 * @brief �ӵ�ǰ�߳�ע���������ԡ�
	 *
	 * @param hWnd ���ھ����
	 * @return ����ɹ����� TRUE�����򷵻� FALSE��
	 */
	DXG_EXPORT BOOL DXG_API UnregisterHWNDAttributesFromThread(HWND hWnd);

	/**
	 * @brief �����ٱ�󶨵���ǰ�̵߳Ĵ������ԡ�
	 *
	 * @param hWnd ���ھ����
	 * @param hAccel ���ٱ�����
	 * @return ����ɹ����� TRUE�����򷵻� FALSE��
	 */
	DXG_EXPORT BOOL DXG_API BindAcceleratorToThreadHWNDAttributes(HWND hWnd, HACCEL hAccel);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !DXGRAPHICSEXC_H