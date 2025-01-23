#ifndef DXGRAPHICS_H // 包含保护以防止多次包含
#define DXGRAPHICS_H

#include "DxGBase.h" // 包含基础头文件

#ifdef __cplusplus // 检查编译器是否为 C++

#include <d2d1_3.h>   // 包含必要的 Direct2D 头文件
#include <dwrite_3.h> // 包含必要的 DirectWrite 头文件

// 管理 WIC 图像工厂的类
class DXG_EXPORT DxImagingFactory
{
public:
    using ImagingFactory = IWICImagingFactory; // IWICImagingFactory 的别名

    DxImagingFactory(); // 构造函数：初始化 DxImagingFactory 对象
    ~DxImagingFactory(); // 析构函数：清理 DxImagingFactory 使用的资源
    DxImagingFactory(const DxImagingFactory&) = delete; // 删除拷贝构造函数以防止复制
    DxImagingFactory& operator=(const DxImagingFactory&) = delete; // 删除拷贝赋值运算符以防止复制

    /**
     * 初始化 WIC 图像工厂。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize();

    /**
     * 取消初始化 WIC 图像工厂，释放其持有的任何资源。
     */
    void Uninitialize();

    /**
     * 获取 WIC 图像工厂的指针。
     * @return IWICImagingFactory 的指针。
     */
    ImagingFactory* Factory();

    /**
     * 转换运算符到 ImagingFactory*。
     * @return IWICImagingFactory 的指针。
     */
    operator ImagingFactory* ();

private:
    ImagingFactory* pImagingFactory; // 指向 WIC 图像工厂的指针
};

// 管理 DirectWrite 工厂的类
class DXG_EXPORT DxWriteFactory
{
public:
    using WriteFactory = IDWriteFactory5; // IDWriteFactory5 的别名

    DxWriteFactory(); // 构造函数：初始化 DxWriteFactory 对象
    ~DxWriteFactory(); // 析构函数：清理 DxWriteFactory 使用的资源
    DxWriteFactory(const DxWriteFactory&) = delete; // 删除拷贝构造函数以防止复制
    DxWriteFactory& operator=(const DxWriteFactory&) = delete; // 删除拷贝赋值运算符以防止复制

    /**
     * 初始化 DirectWrite 工厂。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize();

    /**
     * 取消初始化 DirectWrite 工厂，释放其持有的任何资源。
     */
    void Uninitialize();

    /**
     * 获取 DirectWrite 工厂的指针。
     * @return IDWriteFactory5 的指针。
     */
    WriteFactory* Factory();

    /**
     * 转换运算符到 WriteFactory*。
     * @return IDWriteFactory5 的指针。
     */
    operator WriteFactory* ();

private:
    WriteFactory* pWriteFactory; // 指向 DirectWrite 工厂的指针
};

// 管理字体集合的类
class DXG_EXPORT DxFontCollection
{
public:
    using WriteMemoryLoader = IDWriteInMemoryFontFileLoader; // 字体文件加载器的别名
    using WriteFontCollection = IDWriteFontCollection1; // 字体集合的别名

    DxFontCollection(); // 构造函数：初始化 DxFontCollection 对象
    ~DxFontCollection(); // 析构函数：清理 DxFontCollection 使用的资源
    DxFontCollection(const DxFontCollection&) = delete; // 删除拷贝构造函数以防止复制
    DxFontCollection& operator=(const DxFontCollection&) = delete; // 删除拷贝赋值运算符以防止复制

    /**
     * 使用文件路径初始化字体集合。
     * @param pWriteFactory DirectWrite 工厂的指针。
     * @param szPath 字体文件的路径。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szPath);

    /**
     * 使用资源初始化字体集合。
     * @param pWriteFactory DirectWrite 工厂的指针。
     * @param hModule 包含资源的模块句柄。
     * @param szResourceName 资源的名称。
     * @param szResourceType 资源的类型。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);

    /**
     * 取消初始化字体集合，释放其持有的任何资源。
     */
    void Uninitialize();

    /**
     * 获取字体集合的指针。
     * @return IDWriteFontCollection1 的指针。
     */
    WriteFontCollection* FontCollection();

    /**
     * 获取 DirectWrite 工厂的指针。
     * @return IDWriteFactory5 的指针。
     */
    DxWriteFactory::WriteFactory* WriteFactory();

    /**
     * 获取字体家族名称。
     * @return 字体家族名称的指针。
     */
    LPCWSTR FontFamily() const;

    /**
     * 转换运算符到 WriteFontCollection*。
     * @return IDWriteFontCollection1 的指针。
     */
    operator WriteFontCollection* ();

    /**
     * 转换运算符到 WriteFactory*。
     * @return IDWriteFactory5 的指针。
     */
    operator DxWriteFactory::WriteFactory* ();

private:
    WriteMemoryLoader* pMemoryLoader; // 指向内存加载器的指针
    WriteFontCollection* pFontCollection; // 指向字体集合的指针
    DxWriteFactory::WriteFactory* pWriteFactory; // 指向写入工厂的指针
    LPWSTR lpszFontFamily; // 字体家族名称
};

// 管理文本格式的类
class DXG_EXPORT DxTextFormat
{
public:
    using WriteTextFormat = IDWriteTextFormat; // 文本格式的别名

    DxTextFormat(); // 构造函数：初始化 DxTextFormat 对象
    ~DxTextFormat(); // 析构函数：清理 DxTextFormat 使用的资源
    DxTextFormat(const DxTextFormat&) = delete; // 删除拷贝构造函数以防止复制
    DxTextFormat& operator=(const DxTextFormat&) = delete; // 删除拷贝赋值运算符以防止复制

    /**
     * 初始化文本格式。
     * @param pWriteFactory DirectWrite 工厂的指针。
     * @param szFontFamilyName 字体家族的名称。
     * @param pFontCollection 字体集合的指针。
     * @param fFontSize 字体大小。
     * @param szLocaleName 区域名称。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szFontFamilyName, DxFontCollection::WriteFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);

    /**
     * 使用字体集合初始化文本格式。
     * @param pFontCollection 字体集合的指针。
     * @param fFontSize 字体大小。
     * @param szLocaleName 区域名称。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize(DxFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);

    /**
     * 取消初始化文本格式，释放其持有的任何资源。
     */
    void Uninitialize();

    /**
     * 获取文本格式的指针。
     * @return IDWriteTextFormat 的指针。
     */
    WriteTextFormat* TextFormat();

    /**
     * 获取 DirectWrite 工厂的指针。
     * @return IDWriteFactory5 的指针。
     */
    DxWriteFactory::WriteFactory* WriteFactory();

    /**
     * 获取字体大小。
     * @return 字体大小。
     */
    float FontSize() const;

    /**
     * 转换运算符到 WriteTextFormat*。
     * @return IDWriteTextFormat 的指针。
     */
    operator WriteTextFormat* ();

    /**
     * 转换运算符到 WriteFactory*。
     * @return IDWriteFactory5 的指针。
     */
    operator DxWriteFactory::WriteFactory* ();

private:
    WriteTextFormat* pTextFormat; // 指向文本格式的指针
    float fFontSize; // 字体大小
    DxWriteFactory::WriteFactory* pWriteFactory; // 指向写入工厂的指针
};

// 管理 Direct2D 工厂的类
class DXG_EXPORT DxFactory2D
{
public:
    using D2DFactory = ID2D1Factory5; // Direct2D 工厂的别名

    DxFactory2D(); // 构造函数：初始化 DxFactory2D 对象
    ~DxFactory2D(); // 析构函数：清理 DxFactory2D 使用的资源
    DxFactory2D(const DxFactory2D&) = delete; // 删除拷贝构造函数以防止复制
    DxFactory2D& operator=(const DxFactory2D&) = delete; // 删除拷贝赋值运算符以防止复制

    /**
     * 初始化 Direct2D 工厂。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize();

    /**
     * 取消初始化 Direct2D 工厂，释放其持有的任何资源。
     */
    void Uninitialize();

    /**
     * 获取 Direct2D 工厂的指针。
     * @return ID2D1Factory5 的指针。
     */
    D2DFactory* Factory();

    /**
     * 转换运算符到 D2DFactory*。
     * @return ID2D1Factory5 的指针。
     */
    operator D2DFactory* ();

private:
    D2DFactory* pFactory; // 指向 Direct2D 工厂的指针
};

// 管理 Direct2D 渲染目标的类
class DXG_EXPORT DxTarget2D
{
public:
    using D2DTarget = ID2D1HwndRenderTarget; // 渲染目标的别名
    enum Interlayer // 中间层类型的枚举
    {
        GDI_ON_DIRECT2D, // GDI 在 Direct2D 上
        DIRECT2D_ON_GDI // Direct2D 在 GDI 上
    };
    const D2D1_COLOR_F Transparent = D2D1::ColorF(D2D1::ColorF::Black, 0.0f); // 透明色

    DxTarget2D(); // 构造函数：初始化 DxTarget2D 对象
    ~DxTarget2D(); // 析构函数：清理 DxTarget2D 使用的资源
    DxTarget2D(const DxTarget2D&) = delete; // 删除拷贝构造函数以防止复制
    DxTarget2D& operator=(const DxTarget2D&) = delete; // 删除拷贝赋值运算符以防止复制

    /**
     * 使用 Direct2D 工厂和窗口句柄初始化渲染目标。
     * @param p2DFactory Direct2D 工厂的指针。
     * @param hWnd 窗口句柄。
     * @return 表示初始化成功或失败的 HRESULT。
     */
    HRESULT Initialize(DxFactory2D::D2DFactory* p2DFactory, HWND hWnd);

    /**
     * 取消初始化渲染目标，释放其持有的任何资源。
     */
    void Uninitialize();

    /**
     * 设置中间层类型。
     * @param interlayer 要设置的中间层类型。
     */
    void SetInterlayer(Interlayer interlayer = GDI_ON_DIRECT2D);

    /**
     * 获取中间层类型。
     * @return 当前的中间层类型。
     */
    Interlayer GetInterlayer() const;

    /**
     * 开始在渲染目标上绘制。
     * @return 如果成功开始绘制返回 true，否则返回 false。
     */
    bool BeginDraw();

    /**
     * 结束在渲染目标上的绘制。
     * @return 表示操作成功或失败的 HRESULT。
     */
    HRESULT EndDraw();

    /**
     * 使用指定的颜色清除渲染目标。
     * @param Color 要清除目标的颜色。
     */
    void Clear(D2D1_COLOR_F Color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

    /**
     * 填充位图在渲染目标上。
     * @param pBitmap 要填充的位图的指针。
     * @param dstRect 渲染目标上的目标矩形。
     * @param srcRect 位图中的源矩形。
     * @param alpha 透明度值。
     */
    void FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 }, FLOAT alpha = 1.0f);

    /**
     * 在渲染目标上放置位图。
     * @param pBitmap 要放置的位图的指针。
     * @param dstRect 渲染目标上的目标矩形。
     * @param srcRect 位图中的源矩形。
     * @param alpha 透明度值。
     * @return 位图放置的矩形。
     */
    D2D1_RECT_F PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 }, FLOAT alpha = 1.0f);

    /**
     * 在渲染目标上绘制进度条。
     * @param Rect 进度条的矩形。
     * @param Percentage 进度百分比（0.0f 到 100.0f）。
     * @param BackColor 进度条的背景色。
     * @param FrontColor 进度条的前景色。
     */
    void DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor); // 0.0f ~ 100.0f

    /**
     * @brief 使用指定参数绘制按钮。
     *
     * 此函数在指定矩形内创建按钮，显示提供的文本并使用特定格式。
     * 它还处理可选参数，如背景图像、透明度以及文本、背景和边框的颜色。
     * 最初，它会计算文本布局并调整字体大小以确保其适合按钮内。
     * 如果在将字体大小减少到最小后文本仍然溢出，函数将返回false。
     *
     * 注意：当使用背景图像时，背景颜色将被忽略。背景图像将按比例缩放以适应
     * 矩形，同时保持其纵横比。如果矩形的纵横比与图像的纵横比不同，函数将通过
     * 减小矩形的宽度或高度来调整矩形以匹配图像的纵横比。透明度（alpha）参数仅影响
     * 背景图像，如果未使用背景图像，则无关。
     *
     * @param Rect 指向定义按钮绘制区域的D2D1_RECT_F结构的指针。
     * @param szText 指向包含按钮上显示文本的宽字符串的指针。
     * @param TextFormat 指向指定文本格式属性的DxTextFormat对象的指针。
     * @param pBackgroundBitmap 可选，指向按钮背景图像的ID2D1Bitmap对象的指针。默认为nullptr。
     * @param alpha 表示背景图像透明度级别的浮点值。默认为1.0f（完全不透明）。
     * @param FrontColor 指定文本颜色的D2D1_COLOR_F结构。默认为黑色。
     * @param BackColor 指定按钮背景颜色的D2D1_COLOR_F结构。默认为白色；如果使用背景图像则被忽略。
     * @param BorderColor 指定按钮边框颜色的D2D1_COLOR_F结构。默认为黑色。
     * @return bool 如果按钮绘制成功返回true，否则返回false。
     */
    bool DrawButton(D2D1_RECT_F& Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap = nullptr, FLOAT alpha = 1.0f, D2D1_COLOR_F FrontColor = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F BackColor = D2D1::ColorF(D2D1::ColorF::White), D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Black));

    /**
     * @brief 使用指定参数绘制按钮。
     *
     * 此函数在指定矩形内创建按钮，显示提供的文本并使用特定格式。
     * 它还处理可选参数，如背景图像、透明度以及文本、背景和边框的颜色。
     * 最初，它会计算文本布局并调整字体大小以确保其适合按钮内。
     * 如果在将字体大小减少到最小后文本仍然溢出，函数将返回false。
     *
     * 注意：当使用背景图像时，背景颜色将被忽略。背景图像将按比例缩放以适应
     * 矩形，同时保持其纵横比。如果矩形的纵横比与图像的纵横比不同，函数将通过
     * 减小矩形的宽度或高度来调整矩形以匹配图像的纵横比。透明度（alpha）参数仅影响
     * 背景图像，如果未使用背景图像，则无关。
     *
     * @param Rect 指向定义按钮绘制区域的D2D1_RECT_F结构的指针。
     * @param szText 指向包含按钮上显示文本的宽字符串的指针。
     * @param TextFormat 指向指定文本格式属性的DxTextFormat对象的指针。
     * @param pBackgroundBitmap 可选，指向按钮背景图像的ID2D1Bitmap对象的指针。默认为nullptr。
     * @param alpha 表示背景图像透明度级别的浮点值。默认为1.0f（完全不透明）。
     * @param FrontColor 指定文本颜色的D2D1_COLOR_F结构。默认为黑色。
     * @param BackColor 指定按钮背景颜色的D2D1_COLOR_F结构。默认为白色；如果使用背景图像则被忽略。
     * @param BorderColor 指定按钮边框颜色的D2D1_COLOR_F结构。默认为黑色。
     * @return bool 如果按钮绘制成功返回true，否则返回false。
     */
    bool DrawButton(D2D1_RECT_F* Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap = nullptr, FLOAT alpha = 1.0f, D2D1_COLOR_F FrontColor = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F BackColor = D2D1::ColorF(D2D1::ColorF::White), D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Black));

    /**
     * 调整渲染目标的大小。
     * @param Width 渲染目标的新宽度。
     * @param Height 渲染目标的新高度。
     * @return 表示操作成功或失败的HRESULT。
     */
    HRESULT Resize(UINT Width, UINT Height);

    /**
     * 使用大小结构调整渲染目标的大小。
     * @param pixelSize 渲染目标的新大小。
     * @return 表示操作成功或失败的HRESULT。
     */
    HRESULT Resize(const D2D1_SIZE_U& pixelSize);

    /**
     * 使用大小结构指针调整渲染目标的大小。
     * @param pixelSize 渲染目标的新大小。
     * @return 表示操作成功或失败的HRESULT。
     */
    HRESULT Resize(const D2D1_SIZE_U* pixelSize);

    /**
     * 获取渲染目标的指针。
     * @return 指向ID2D1HwndRenderTarget的指针。
     */
    D2DTarget* Target();

    /**
     * 转换操作符到D2DTarget*。
     * @return 指向ID2D1HwndRenderTarget的指针。
     */
    operator D2DTarget* ();

    /**
     * 刷新GDI（图形设备接口）操作。
     */
    void FlushGDI() const;

private:
    D2DTarget* pTarget; // 指向渲染目标的指针
    HWND hWnd; // 窗口句柄
    Interlayer interlayer; // 交互层类型
    bool tagDraw; // 绘制标志
};

// 管理图像的类
class DXG_EXPORT DxImage
{
public:
    using Image = ID2D1Bitmap; // 位图的别名

    DxImage(); // 构造函数：初始化DxImage对象
    ~DxImage(); // 析构函数：清理DxImage使用的资源
    DxImage(const DxImage&) = delete; // 删除拷贝构造函数以防止拷贝
    DxImage& operator=(const DxImage&) = delete; // 删除拷贝赋值运算符以防止拷贝

    /**
     * 从文件路径加载图像。
     * @param pImagingFactory 指向WIC成像工厂的指针。
     * @param pRenderTarget 指向渲染目标的指针。
     * @param szPath 图像文件的路径。
     * @return 表示操作成功或失败的HRESULT。
     */
    HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath);

    /**
     * 从资源加载图像。
     * @param pImagingFactory 指向WIC成像工厂的指针。
     * @param pRenderTarget 指向渲染目标的指针。
     * @param hModule 包含资源的模块句柄。
     * @param szResourceName 资源的名称。
     * @param szResourceType 资源的类型。
     * @return 表示操作成功或失败的HRESULT。
     */
    HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);

    /**
     * 释放图像，释放其持有的任何资源。
     */
    void Release();

    /**
     * 获取位图的指针。
     * @return 指向ID2D1Bitmap的指针。
     */
    Image* Bitmap();

    /**
     * 获取位图指针的地址。
     * @return 指向ID2D1Bitmap指针的地址。
     */
    Image** BitmapPtr();

    /**
     * 转换操作符到Image*。
     * @return 指向ID2D1Bitmap的指针。
     */
    operator Image* ();

    /**
     * 转换操作符到Image**。
     * @return 指向ID2D1Bitmap指针的地址。
     */
    operator Image** ();

private:
    Image* pImage; // 指向图像的指针
};

// Direct2D操作的实用函数

/**
 * 计算填充位图的源矩形。
 * @param bitmapRect 位图的矩形。
 * @param targetRect 渲染目标上的目标矩形。
 * @return 用于填充的源矩形。
 */
DXG_EXPORT D2D1_RECT_F DxGCalcFillBitmapSourceRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect);

/**
 * 计算放置位图的目标矩形。
 * @param bitmapRect 位图的矩形。
 * @param targetRect 渲染目标上的目标矩形。
 * @return 用于放置的目标矩形。
 */
DXG_EXPORT D2D1_RECT_F DxGCalcPutBitmapTargetRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect);

/**
 * 使用整数坐标计算填充位图的源矩形。
 * @param bitmapRect 位图的矩形。
 * @param targetRect 渲染目标上的目标矩形。
 * @return 用于填充的源矩形。
 */
DXG_EXPORT D2D1_RECT_U DxGCalcFillBitmapSourceRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect);

/**
 * 使用整数坐标计算放置位图的目标矩形。
 * @param bitmapRect 位图的矩形。
 * @param targetRect 渲染目标上的目标矩形。
 * @return 用于放置的目标矩形。
 */
DXG_EXPORT D2D1_RECT_U DxGCalcPutBitmapTargetRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect);

/**
 * 获取系统的DPI缩放比例。
 * @return DPI缩放因子。
 */
DXG_EXPORT FLOAT DxGetDpiScaleForSystem();

/**
 * 获取特定窗口的DPI缩放比例。
 * @param hWnd 窗口句柄。
 * @return 窗口的DPI缩放因子。
 */
DXG_EXPORT FLOAT DxGetDpiScaleForWindow(HWND hWnd);

/**
 * 将像素转换为设备独立像素（DIPs）。
     * @param Pixels 像素数量。
     * @param DpiScale DPI缩放因子。
     * @return DIPs的数量。
     */
DXG_EXPORT FLOAT DxGPixelsToDips(UINT Pixels, FLOAT DpiScale);

/**
 * 将设备独立像素（DIPs）转换为像素。
 * @param Dips DIPs的数量。
 * @param DpiScale DPI缩放因子。
 * @return 像素的数量。
 */
DXG_EXPORT UINT DxGDipsToPixels(FLOAT Dips, FLOAT DpiScale);

/**
 * 从矩形计算大小。
 * @param Rect 矩形。
 * @return 作为D2D1_SIZE_F结构的大小。
 */
DXG_EXPORT D2D1_SIZE_F DxGCalcSizeFormRect(D2D1_RECT_F Rect);

/**
 * 使用整数坐标从矩形计算大小。
 * @param Rect 矩形。
 * @return 作为D2D1_SIZE_U结构的大小。
 */
DXG_EXPORT D2D1_SIZE_U DxGCalcSizeFormRect(D2D1_RECT_U Rect);

/**
 * 将大小从像素转换为设备独立像素（DIPs）。
 * @param Pixels 像素大小。
 * @param DpiScale DPI缩放因子。
 * @return DIPs的大小。
 */
DXG_EXPORT D2D1_SIZE_F DxGSizePixelsToDips(D2D1_SIZE_U Pixels, FLOAT DpiScale);

/**
 * 将矩形从像素转换为设备独立像素（DIPs）。
 * @param Pixels 像素中的矩形。
 * @param DpiScale DPI缩放因子。
 * @return DIPs中的矩形。
 */
DXG_EXPORT D2D1_RECT_F DxGRectPixelsToDips(D2D1_RECT_U Pixels, FLOAT DpiScale);

/**
 * 将大小从设备独立像素（DIPs）转换为像素。
 * @param Dips DIPs的大小。
 * @param DpiScale DPI缩放因子。
 * @return 像素的大小。
 */
DXG_EXPORT D2D1_SIZE_U DxGSizeDipsToPixels(D2D1_SIZE_F Dips, FLOAT DpiScale);

/**
 * 将矩形从设备独立像素（DIPs）转换为像素。
 * @param Dips DIPs中的矩形。
 * @param DpiScale DPI缩放因子。
 * @return 像素中的矩形。
 */
DXG_EXPORT D2D1_RECT_U DxGRectDipsToPixels(D2D1_RECT_F Dips, FLOAT DpiScale);

/**
 * 计算画布内项目的中心矩形。
 * @param Item 项目的大小。
 * @param Canvas 画布的矩形。
 * @return 居中的矩形。
 */
DXG_EXPORT D2D1_RECT_F DxGCalcCenterRect(D2D1_SIZE_F Item, D2D1_RECT_F Canvas);

/**
 * 使用整数坐标计算画布内项目的中心矩形。
 * @param Item 项目的大小。
 * @param Canvas 画布的矩形。
 * @return 居中的矩形。
 */
DXG_EXPORT D2D1_RECT_U DxGCalcCenterRect(D2D1_SIZE_U Item, D2D1_RECT_U Canvas);

/**
 * 从LPARAM值获取大小。
 * @param lParam LPARAM值。
 * @return 作为D2D1_SIZE_U结构的大小。
 */
DXG_EXPORT D2D1_SIZE_U DxGetSizeFromlParam(LPARAM lParam);

/**
 * 从LPARAM值获取点。
 * @param lParam LPARAM值。
 * @return 作为D2D1_POINT_2U结构的点。
 */
DXG_EXPORT D2D1_POINT_2U DxGetPointFromlParam(LPARAM lParam);

/**
 * 检查点是否在矩形内。
 * @param Point 点。
 * @param Rect 矩形。
 * @return 如果点在矩形内则为true，否则为false。
 */
DXG_EXPORT bool DxGIsPointInRect(D2D1_SIZE_F Point, D2D1_RECT_F Rect);

/**
 * 使用整数坐标检查点是否在矩形内。
 * @param Point 点。
 * @param Rect 矩形。
 * @return 如果点在矩形内则为true，否则为false。
 */
DXG_EXPORT bool DxGIsPointInRect(D2D1_SIZE_U Point, D2D1_RECT_U Rect);

#endif // __cplusplus

#endif // !DXGRAPHICS_H