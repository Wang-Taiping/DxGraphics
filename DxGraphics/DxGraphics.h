#ifndef DXGRAPHICS_H // ���������Է�ֹ��ΰ���
#define DXGRAPHICS_H

#include "DxGBase.h" // ��������ͷ�ļ�

#ifdef __cplusplus // ���������Ƿ�Ϊ C++

#include <d2d1_3.h>   // ������Ҫ�� Direct2D ͷ�ļ�
#include <dwrite_3.h> // ������Ҫ�� DirectWrite ͷ�ļ�

// ���� WIC ͼ�񹤳�����
class DXG_EXPORT DxImagingFactory
{
public:
    using ImagingFactory = IWICImagingFactory; // IWICImagingFactory �ı���

    DxImagingFactory(); // ���캯������ʼ�� DxImagingFactory ����
    ~DxImagingFactory(); // �������������� DxImagingFactory ʹ�õ���Դ
    DxImagingFactory(const DxImagingFactory&) = delete; // ɾ���������캯���Է�ֹ����
    DxImagingFactory& operator=(const DxImagingFactory&) = delete; // ɾ��������ֵ������Է�ֹ����

    /**
     * ��ʼ�� WIC ͼ�񹤳���
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize();

    /**
     * ȡ����ʼ�� WIC ͼ�񹤳����ͷ�����е��κ���Դ��
     */
    void Uninitialize();

    /**
     * ��ȡ WIC ͼ�񹤳���ָ�롣
     * @return IWICImagingFactory ��ָ�롣
     */
    ImagingFactory* Factory();

    /**
     * ת��������� ImagingFactory*��
     * @return IWICImagingFactory ��ָ�롣
     */
    operator ImagingFactory* ();

private:
    ImagingFactory* pImagingFactory; // ָ�� WIC ͼ�񹤳���ָ��
};

// ���� DirectWrite ��������
class DXG_EXPORT DxWriteFactory
{
public:
    using WriteFactory = IDWriteFactory5; // IDWriteFactory5 �ı���

    DxWriteFactory(); // ���캯������ʼ�� DxWriteFactory ����
    ~DxWriteFactory(); // �������������� DxWriteFactory ʹ�õ���Դ
    DxWriteFactory(const DxWriteFactory&) = delete; // ɾ���������캯���Է�ֹ����
    DxWriteFactory& operator=(const DxWriteFactory&) = delete; // ɾ��������ֵ������Է�ֹ����

    /**
     * ��ʼ�� DirectWrite ������
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize();

    /**
     * ȡ����ʼ�� DirectWrite �������ͷ�����е��κ���Դ��
     */
    void Uninitialize();

    /**
     * ��ȡ DirectWrite ������ָ�롣
     * @return IDWriteFactory5 ��ָ�롣
     */
    WriteFactory* Factory();

    /**
     * ת��������� WriteFactory*��
     * @return IDWriteFactory5 ��ָ�롣
     */
    operator WriteFactory* ();

private:
    WriteFactory* pWriteFactory; // ָ�� DirectWrite ������ָ��
};

// �������弯�ϵ���
class DXG_EXPORT DxFontCollection
{
public:
    using WriteMemoryLoader = IDWriteInMemoryFontFileLoader; // �����ļ��������ı���
    using WriteFontCollection = IDWriteFontCollection1; // ���弯�ϵı���

    DxFontCollection(); // ���캯������ʼ�� DxFontCollection ����
    ~DxFontCollection(); // �������������� DxFontCollection ʹ�õ���Դ
    DxFontCollection(const DxFontCollection&) = delete; // ɾ���������캯���Է�ֹ����
    DxFontCollection& operator=(const DxFontCollection&) = delete; // ɾ��������ֵ������Է�ֹ����

    /**
     * ʹ���ļ�·����ʼ�����弯�ϡ�
     * @param pWriteFactory DirectWrite ������ָ�롣
     * @param szPath �����ļ���·����
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szPath);

    /**
     * ʹ����Դ��ʼ�����弯�ϡ�
     * @param pWriteFactory DirectWrite ������ָ�롣
     * @param hModule ������Դ��ģ������
     * @param szResourceName ��Դ�����ơ�
     * @param szResourceType ��Դ�����͡�
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);

    /**
     * ȡ����ʼ�����弯�ϣ��ͷ�����е��κ���Դ��
     */
    void Uninitialize();

    /**
     * ��ȡ���弯�ϵ�ָ�롣
     * @return IDWriteFontCollection1 ��ָ�롣
     */
    WriteFontCollection* FontCollection();

    /**
     * ��ȡ DirectWrite ������ָ�롣
     * @return IDWriteFactory5 ��ָ�롣
     */
    DxWriteFactory::WriteFactory* WriteFactory();

    /**
     * ��ȡ����������ơ�
     * @return ����������Ƶ�ָ�롣
     */
    LPCWSTR FontFamily() const;

    /**
     * ת��������� WriteFontCollection*��
     * @return IDWriteFontCollection1 ��ָ�롣
     */
    operator WriteFontCollection* ();

    /**
     * ת��������� WriteFactory*��
     * @return IDWriteFactory5 ��ָ�롣
     */
    operator DxWriteFactory::WriteFactory* ();

private:
    WriteMemoryLoader* pMemoryLoader; // ָ���ڴ��������ָ��
    WriteFontCollection* pFontCollection; // ָ�����弯�ϵ�ָ��
    DxWriteFactory::WriteFactory* pWriteFactory; // ָ��д�빤����ָ��
    LPWSTR lpszFontFamily; // �����������
};

// �����ı���ʽ����
class DXG_EXPORT DxTextFormat
{
public:
    using WriteTextFormat = IDWriteTextFormat; // �ı���ʽ�ı���

    DxTextFormat(); // ���캯������ʼ�� DxTextFormat ����
    ~DxTextFormat(); // �������������� DxTextFormat ʹ�õ���Դ
    DxTextFormat(const DxTextFormat&) = delete; // ɾ���������캯���Է�ֹ����
    DxTextFormat& operator=(const DxTextFormat&) = delete; // ɾ��������ֵ������Է�ֹ����

    /**
     * ��ʼ���ı���ʽ��
     * @param pWriteFactory DirectWrite ������ָ�롣
     * @param szFontFamilyName �����������ơ�
     * @param pFontCollection ���弯�ϵ�ָ�롣
     * @param fFontSize �����С��
     * @param szLocaleName �������ơ�
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szFontFamilyName, DxFontCollection::WriteFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);

    /**
     * ʹ�����弯�ϳ�ʼ���ı���ʽ��
     * @param pFontCollection ���弯�ϵ�ָ�롣
     * @param fFontSize �����С��
     * @param szLocaleName �������ơ�
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize(DxFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);

    /**
     * ȡ����ʼ���ı���ʽ���ͷ�����е��κ���Դ��
     */
    void Uninitialize();

    /**
     * ��ȡ�ı���ʽ��ָ�롣
     * @return IDWriteTextFormat ��ָ�롣
     */
    WriteTextFormat* TextFormat();

    /**
     * ��ȡ DirectWrite ������ָ�롣
     * @return IDWriteFactory5 ��ָ�롣
     */
    DxWriteFactory::WriteFactory* WriteFactory();

    /**
     * ��ȡ�����С��
     * @return �����С��
     */
    float FontSize() const;

    /**
     * ת��������� WriteTextFormat*��
     * @return IDWriteTextFormat ��ָ�롣
     */
    operator WriteTextFormat* ();

    /**
     * ת��������� WriteFactory*��
     * @return IDWriteFactory5 ��ָ�롣
     */
    operator DxWriteFactory::WriteFactory* ();

private:
    WriteTextFormat* pTextFormat; // ָ���ı���ʽ��ָ��
    float fFontSize; // �����С
    DxWriteFactory::WriteFactory* pWriteFactory; // ָ��д�빤����ָ��
};

// ���� Direct2D ��������
class DXG_EXPORT DxFactory2D
{
public:
    using D2DFactory = ID2D1Factory5; // Direct2D �����ı���

    DxFactory2D(); // ���캯������ʼ�� DxFactory2D ����
    ~DxFactory2D(); // �������������� DxFactory2D ʹ�õ���Դ
    DxFactory2D(const DxFactory2D&) = delete; // ɾ���������캯���Է�ֹ����
    DxFactory2D& operator=(const DxFactory2D&) = delete; // ɾ��������ֵ������Է�ֹ����

    /**
     * ��ʼ�� Direct2D ������
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize();

    /**
     * ȡ����ʼ�� Direct2D �������ͷ�����е��κ���Դ��
     */
    void Uninitialize();

    /**
     * ��ȡ Direct2D ������ָ�롣
     * @return ID2D1Factory5 ��ָ�롣
     */
    D2DFactory* Factory();

    /**
     * ת��������� D2DFactory*��
     * @return ID2D1Factory5 ��ָ�롣
     */
    operator D2DFactory* ();

private:
    D2DFactory* pFactory; // ָ�� Direct2D ������ָ��
};

// ���� Direct2D ��ȾĿ�����
class DXG_EXPORT DxTarget2D
{
public:
    using D2DTarget = ID2D1HwndRenderTarget; // ��ȾĿ��ı���
    enum Interlayer // �м�����͵�ö��
    {
        GDI_ON_DIRECT2D, // GDI �� Direct2D ��
        DIRECT2D_ON_GDI // Direct2D �� GDI ��
    };
    const D2D1_COLOR_F Transparent = D2D1::ColorF(D2D1::ColorF::Black, 0.0f); // ͸��ɫ

    DxTarget2D(); // ���캯������ʼ�� DxTarget2D ����
    ~DxTarget2D(); // �������������� DxTarget2D ʹ�õ���Դ
    DxTarget2D(const DxTarget2D&) = delete; // ɾ���������캯���Է�ֹ����
    DxTarget2D& operator=(const DxTarget2D&) = delete; // ɾ��������ֵ������Է�ֹ����

    /**
     * ʹ�� Direct2D �����ʹ��ھ����ʼ����ȾĿ�ꡣ
     * @param p2DFactory Direct2D ������ָ�롣
     * @param hWnd ���ھ����
     * @return ��ʾ��ʼ���ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT Initialize(DxFactory2D::D2DFactory* p2DFactory, HWND hWnd);

    /**
     * ȡ����ʼ����ȾĿ�꣬�ͷ�����е��κ���Դ��
     */
    void Uninitialize();

    /**
     * �����м�����͡�
     * @param interlayer Ҫ���õ��м�����͡�
     */
    void SetInterlayer(Interlayer interlayer = GDI_ON_DIRECT2D);

    /**
     * ��ȡ�м�����͡�
     * @return ��ǰ���м�����͡�
     */
    Interlayer GetInterlayer() const;

    /**
     * ��ʼ����ȾĿ���ϻ��ơ�
     * @return ����ɹ���ʼ���Ʒ��� true�����򷵻� false��
     */
    bool BeginDraw();

    /**
     * ��������ȾĿ���ϵĻ��ơ�
     * @return ��ʾ�����ɹ���ʧ�ܵ� HRESULT��
     */
    HRESULT EndDraw();

    /**
     * ʹ��ָ������ɫ�����ȾĿ�ꡣ
     * @param Color Ҫ���Ŀ�����ɫ��
     */
    void Clear(D2D1_COLOR_F Color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

    /**
     * ���λͼ����ȾĿ���ϡ�
     * @param pBitmap Ҫ����λͼ��ָ�롣
     * @param dstRect ��ȾĿ���ϵ�Ŀ����Ρ�
     * @param srcRect λͼ�е�Դ���Ρ�
     * @param alpha ͸����ֵ��
     */
    void FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 }, FLOAT alpha = 1.0f);

    /**
     * ����ȾĿ���Ϸ���λͼ��
     * @param pBitmap Ҫ���õ�λͼ��ָ�롣
     * @param dstRect ��ȾĿ���ϵ�Ŀ����Ρ�
     * @param srcRect λͼ�е�Դ���Ρ�
     * @param alpha ͸����ֵ��
     * @return λͼ���õľ��Ρ�
     */
    D2D1_RECT_F PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 }, FLOAT alpha = 1.0f);

    /**
     * ����ȾĿ���ϻ��ƽ�������
     * @param Rect �������ľ��Ρ�
     * @param Percentage ���Ȱٷֱȣ�0.0f �� 100.0f����
     * @param BackColor �������ı���ɫ��
     * @param FrontColor ��������ǰ��ɫ��
     */
    void DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor); // 0.0f ~ 100.0f

    /**
     * @brief ʹ��ָ���������ư�ť��
     *
     * �˺�����ָ�������ڴ�����ť����ʾ�ṩ���ı���ʹ���ض���ʽ��
     * ���������ѡ�������米��ͼ��͸�����Լ��ı��������ͱ߿����ɫ��
     * �������������ı����ֲ����������С��ȷ�����ʺϰ�ť�ڡ�
     * ����ڽ������С���ٵ���С���ı���Ȼ���������������false��
     *
     * ע�⣺��ʹ�ñ���ͼ��ʱ��������ɫ�������ԡ�����ͼ�񽫰�������������Ӧ
     * ���Σ�ͬʱ�������ݺ�ȡ�������ε��ݺ����ͼ����ݺ�Ȳ�ͬ��������ͨ��
     * ��С���εĿ�Ȼ�߶�������������ƥ��ͼ����ݺ�ȡ�͸���ȣ�alpha��������Ӱ��
     * ����ͼ�����δʹ�ñ���ͼ�����޹ء�
     *
     * @param Rect ָ���尴ť���������D2D1_RECT_F�ṹ��ָ�롣
     * @param szText ָ�������ť����ʾ�ı��Ŀ��ַ�����ָ�롣
     * @param TextFormat ָ��ָ���ı���ʽ���Ե�DxTextFormat�����ָ�롣
     * @param pBackgroundBitmap ��ѡ��ָ��ť����ͼ���ID2D1Bitmap�����ָ�롣Ĭ��Ϊnullptr��
     * @param alpha ��ʾ����ͼ��͸���ȼ���ĸ���ֵ��Ĭ��Ϊ1.0f����ȫ��͸������
     * @param FrontColor ָ���ı���ɫ��D2D1_COLOR_F�ṹ��Ĭ��Ϊ��ɫ��
     * @param BackColor ָ����ť������ɫ��D2D1_COLOR_F�ṹ��Ĭ��Ϊ��ɫ�����ʹ�ñ���ͼ���򱻺��ԡ�
     * @param BorderColor ָ����ť�߿���ɫ��D2D1_COLOR_F�ṹ��Ĭ��Ϊ��ɫ��
     * @return bool �����ť���Ƴɹ�����true�����򷵻�false��
     */
    bool DrawButton(D2D1_RECT_F& Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap = nullptr, FLOAT alpha = 1.0f, D2D1_COLOR_F FrontColor = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F BackColor = D2D1::ColorF(D2D1::ColorF::White), D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Black));

    /**
     * @brief ʹ��ָ���������ư�ť��
     *
     * �˺�����ָ�������ڴ�����ť����ʾ�ṩ���ı���ʹ���ض���ʽ��
     * ���������ѡ�������米��ͼ��͸�����Լ��ı��������ͱ߿����ɫ��
     * �������������ı����ֲ����������С��ȷ�����ʺϰ�ť�ڡ�
     * ����ڽ������С���ٵ���С���ı���Ȼ���������������false��
     *
     * ע�⣺��ʹ�ñ���ͼ��ʱ��������ɫ�������ԡ�����ͼ�񽫰�������������Ӧ
     * ���Σ�ͬʱ�������ݺ�ȡ�������ε��ݺ����ͼ����ݺ�Ȳ�ͬ��������ͨ��
     * ��С���εĿ�Ȼ�߶�������������ƥ��ͼ����ݺ�ȡ�͸���ȣ�alpha��������Ӱ��
     * ����ͼ�����δʹ�ñ���ͼ�����޹ء�
     *
     * @param Rect ָ���尴ť���������D2D1_RECT_F�ṹ��ָ�롣
     * @param szText ָ�������ť����ʾ�ı��Ŀ��ַ�����ָ�롣
     * @param TextFormat ָ��ָ���ı���ʽ���Ե�DxTextFormat�����ָ�롣
     * @param pBackgroundBitmap ��ѡ��ָ��ť����ͼ���ID2D1Bitmap�����ָ�롣Ĭ��Ϊnullptr��
     * @param alpha ��ʾ����ͼ��͸���ȼ���ĸ���ֵ��Ĭ��Ϊ1.0f����ȫ��͸������
     * @param FrontColor ָ���ı���ɫ��D2D1_COLOR_F�ṹ��Ĭ��Ϊ��ɫ��
     * @param BackColor ָ����ť������ɫ��D2D1_COLOR_F�ṹ��Ĭ��Ϊ��ɫ�����ʹ�ñ���ͼ���򱻺��ԡ�
     * @param BorderColor ָ����ť�߿���ɫ��D2D1_COLOR_F�ṹ��Ĭ��Ϊ��ɫ��
     * @return bool �����ť���Ƴɹ�����true�����򷵻�false��
     */
    bool DrawButton(D2D1_RECT_F* Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap = nullptr, FLOAT alpha = 1.0f, D2D1_COLOR_F FrontColor = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F BackColor = D2D1::ColorF(D2D1::ColorF::White), D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Black));

    /**
     * ������ȾĿ��Ĵ�С��
     * @param Width ��ȾĿ����¿�ȡ�
     * @param Height ��ȾĿ����¸߶ȡ�
     * @return ��ʾ�����ɹ���ʧ�ܵ�HRESULT��
     */
    HRESULT Resize(UINT Width, UINT Height);

    /**
     * ʹ�ô�С�ṹ������ȾĿ��Ĵ�С��
     * @param pixelSize ��ȾĿ����´�С��
     * @return ��ʾ�����ɹ���ʧ�ܵ�HRESULT��
     */
    HRESULT Resize(const D2D1_SIZE_U& pixelSize);

    /**
     * ʹ�ô�С�ṹָ�������ȾĿ��Ĵ�С��
     * @param pixelSize ��ȾĿ����´�С��
     * @return ��ʾ�����ɹ���ʧ�ܵ�HRESULT��
     */
    HRESULT Resize(const D2D1_SIZE_U* pixelSize);

    /**
     * ��ȡ��ȾĿ���ָ�롣
     * @return ָ��ID2D1HwndRenderTarget��ָ�롣
     */
    D2DTarget* Target();

    /**
     * ת����������D2DTarget*��
     * @return ָ��ID2D1HwndRenderTarget��ָ�롣
     */
    operator D2DTarget* ();

    /**
     * ˢ��GDI��ͼ���豸�ӿڣ�������
     */
    void FlushGDI() const;

private:
    D2DTarget* pTarget; // ָ����ȾĿ���ָ��
    HWND hWnd; // ���ھ��
    Interlayer interlayer; // ����������
    bool tagDraw; // ���Ʊ�־
};

// ����ͼ�����
class DXG_EXPORT DxImage
{
public:
    using Image = ID2D1Bitmap; // λͼ�ı���

    DxImage(); // ���캯������ʼ��DxImage����
    ~DxImage(); // ��������������DxImageʹ�õ���Դ
    DxImage(const DxImage&) = delete; // ɾ���������캯���Է�ֹ����
    DxImage& operator=(const DxImage&) = delete; // ɾ��������ֵ������Է�ֹ����

    /**
     * ���ļ�·������ͼ��
     * @param pImagingFactory ָ��WIC���񹤳���ָ�롣
     * @param pRenderTarget ָ����ȾĿ���ָ�롣
     * @param szPath ͼ���ļ���·����
     * @return ��ʾ�����ɹ���ʧ�ܵ�HRESULT��
     */
    HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath);

    /**
     * ����Դ����ͼ��
     * @param pImagingFactory ָ��WIC���񹤳���ָ�롣
     * @param pRenderTarget ָ����ȾĿ���ָ�롣
     * @param hModule ������Դ��ģ������
     * @param szResourceName ��Դ�����ơ�
     * @param szResourceType ��Դ�����͡�
     * @return ��ʾ�����ɹ���ʧ�ܵ�HRESULT��
     */
    HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);

    /**
     * �ͷ�ͼ���ͷ�����е��κ���Դ��
     */
    void Release();

    /**
     * ��ȡλͼ��ָ�롣
     * @return ָ��ID2D1Bitmap��ָ�롣
     */
    Image* Bitmap();

    /**
     * ��ȡλͼָ��ĵ�ַ��
     * @return ָ��ID2D1Bitmapָ��ĵ�ַ��
     */
    Image** BitmapPtr();

    /**
     * ת����������Image*��
     * @return ָ��ID2D1Bitmap��ָ�롣
     */
    operator Image* ();

    /**
     * ת����������Image**��
     * @return ָ��ID2D1Bitmapָ��ĵ�ַ��
     */
    operator Image** ();

private:
    Image* pImage; // ָ��ͼ���ָ��
};

// Direct2D������ʵ�ú���

/**
 * �������λͼ��Դ���Ρ�
 * @param bitmapRect λͼ�ľ��Ρ�
 * @param targetRect ��ȾĿ���ϵ�Ŀ����Ρ�
 * @return ��������Դ���Ρ�
 */
DXG_EXPORT D2D1_RECT_F DxGCalcFillBitmapSourceRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect);

/**
 * �������λͼ��Ŀ����Ρ�
 * @param bitmapRect λͼ�ľ��Ρ�
 * @param targetRect ��ȾĿ���ϵ�Ŀ����Ρ�
 * @return ���ڷ��õ�Ŀ����Ρ�
 */
DXG_EXPORT D2D1_RECT_F DxGCalcPutBitmapTargetRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect);

/**
 * ʹ����������������λͼ��Դ���Ρ�
 * @param bitmapRect λͼ�ľ��Ρ�
 * @param targetRect ��ȾĿ���ϵ�Ŀ����Ρ�
 * @return ��������Դ���Ρ�
 */
DXG_EXPORT D2D1_RECT_U DxGCalcFillBitmapSourceRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect);

/**
 * ʹ����������������λͼ��Ŀ����Ρ�
 * @param bitmapRect λͼ�ľ��Ρ�
 * @param targetRect ��ȾĿ���ϵ�Ŀ����Ρ�
 * @return ���ڷ��õ�Ŀ����Ρ�
 */
DXG_EXPORT D2D1_RECT_U DxGCalcPutBitmapTargetRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect);

/**
 * ��ȡϵͳ��DPI���ű�����
 * @return DPI�������ӡ�
 */
DXG_EXPORT FLOAT DxGetDpiScaleForSystem();

/**
 * ��ȡ�ض����ڵ�DPI���ű�����
 * @param hWnd ���ھ����
 * @return ���ڵ�DPI�������ӡ�
 */
DXG_EXPORT FLOAT DxGetDpiScaleForWindow(HWND hWnd);

/**
 * ������ת��Ϊ�豸�������أ�DIPs����
     * @param Pixels ����������
     * @param DpiScale DPI�������ӡ�
     * @return DIPs��������
     */
DXG_EXPORT FLOAT DxGPixelsToDips(UINT Pixels, FLOAT DpiScale);

/**
 * ���豸�������أ�DIPs��ת��Ϊ���ء�
 * @param Dips DIPs��������
 * @param DpiScale DPI�������ӡ�
 * @return ���ص�������
 */
DXG_EXPORT UINT DxGDipsToPixels(FLOAT Dips, FLOAT DpiScale);

/**
 * �Ӿ��μ����С��
 * @param Rect ���Ρ�
 * @return ��ΪD2D1_SIZE_F�ṹ�Ĵ�С��
 */
DXG_EXPORT D2D1_SIZE_F DxGCalcSizeFormRect(D2D1_RECT_F Rect);

/**
 * ʹ����������Ӿ��μ����С��
 * @param Rect ���Ρ�
 * @return ��ΪD2D1_SIZE_U�ṹ�Ĵ�С��
 */
DXG_EXPORT D2D1_SIZE_U DxGCalcSizeFormRect(D2D1_RECT_U Rect);

/**
 * ����С������ת��Ϊ�豸�������أ�DIPs����
 * @param Pixels ���ش�С��
 * @param DpiScale DPI�������ӡ�
 * @return DIPs�Ĵ�С��
 */
DXG_EXPORT D2D1_SIZE_F DxGSizePixelsToDips(D2D1_SIZE_U Pixels, FLOAT DpiScale);

/**
 * �����δ�����ת��Ϊ�豸�������أ�DIPs����
 * @param Pixels �����еľ��Ρ�
 * @param DpiScale DPI�������ӡ�
 * @return DIPs�еľ��Ρ�
 */
DXG_EXPORT D2D1_RECT_F DxGRectPixelsToDips(D2D1_RECT_U Pixels, FLOAT DpiScale);

/**
 * ����С���豸�������أ�DIPs��ת��Ϊ���ء�
 * @param Dips DIPs�Ĵ�С��
 * @param DpiScale DPI�������ӡ�
 * @return ���صĴ�С��
 */
DXG_EXPORT D2D1_SIZE_U DxGSizeDipsToPixels(D2D1_SIZE_F Dips, FLOAT DpiScale);

/**
 * �����δ��豸�������أ�DIPs��ת��Ϊ���ء�
 * @param Dips DIPs�еľ��Ρ�
 * @param DpiScale DPI�������ӡ�
 * @return �����еľ��Ρ�
 */
DXG_EXPORT D2D1_RECT_U DxGRectDipsToPixels(D2D1_RECT_F Dips, FLOAT DpiScale);

/**
 * ���㻭������Ŀ�����ľ��Ρ�
 * @param Item ��Ŀ�Ĵ�С��
 * @param Canvas �����ľ��Ρ�
 * @return ���еľ��Ρ�
 */
DXG_EXPORT D2D1_RECT_F DxGCalcCenterRect(D2D1_SIZE_F Item, D2D1_RECT_F Canvas);

/**
 * ʹ������������㻭������Ŀ�����ľ��Ρ�
 * @param Item ��Ŀ�Ĵ�С��
 * @param Canvas �����ľ��Ρ�
 * @return ���еľ��Ρ�
 */
DXG_EXPORT D2D1_RECT_U DxGCalcCenterRect(D2D1_SIZE_U Item, D2D1_RECT_U Canvas);

/**
 * ��LPARAMֵ��ȡ��С��
 * @param lParam LPARAMֵ��
 * @return ��ΪD2D1_SIZE_U�ṹ�Ĵ�С��
 */
DXG_EXPORT D2D1_SIZE_U DxGetSizeFromlParam(LPARAM lParam);

/**
 * ��LPARAMֵ��ȡ�㡣
 * @param lParam LPARAMֵ��
 * @return ��ΪD2D1_POINT_2U�ṹ�ĵ㡣
 */
DXG_EXPORT D2D1_POINT_2U DxGetPointFromlParam(LPARAM lParam);

/**
 * �����Ƿ��ھ����ڡ�
 * @param Point �㡣
 * @param Rect ���Ρ�
 * @return ������ھ�������Ϊtrue������Ϊfalse��
 */
DXG_EXPORT bool DxGIsPointInRect(D2D1_SIZE_F Point, D2D1_RECT_F Rect);

/**
 * ʹ��������������Ƿ��ھ����ڡ�
 * @param Point �㡣
 * @param Rect ���Ρ�
 * @return ������ھ�������Ϊtrue������Ϊfalse��
 */
DXG_EXPORT bool DxGIsPointInRect(D2D1_SIZE_U Point, D2D1_RECT_U Rect);

#endif // __cplusplus

#endif // !DXGRAPHICS_H