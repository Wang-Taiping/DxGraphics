#pragma once

#ifndef DXGRAPHICS_H
#define DXGRAPHICS_H

#include "DxGBase.h"

#ifdef __cplusplus

#include <d2d1_3.h>
#include <dwrite_3.h>

class DXG_EXPORT DxImagingFactory
{
public:
	using ImagingFactory = IWICImagingFactory;

	DxImagingFactory();
	~DxImagingFactory();
	DxImagingFactory(const DxImagingFactory&) = delete;
	DxImagingFactory& operator=(const DxImagingFactory&) = delete;
	HRESULT Initialize();
	void Uninitialize();
	ImagingFactory* Factory();
	operator ImagingFactory* ();

private:
	ImagingFactory* pImagingFactory;
};

class DXG_EXPORT DxWriteFactory
{
public:
	using WriteFactory = IDWriteFactory5;

	DxWriteFactory();
	~DxWriteFactory();
	DxWriteFactory(const DxWriteFactory&) = delete;
	DxWriteFactory& operator=(const DxWriteFactory&) = delete;
	HRESULT Initialize();
	void Uninitialize();
	WriteFactory* Factory();
	operator WriteFactory* ();

private:
	WriteFactory* pWriteFactory;
};

class DXG_EXPORT DxFontCollection
{
public:
	using WriteMemoryLoader = IDWriteInMemoryFontFileLoader;
	using WriteFontCollection = IDWriteFontCollection1;

	DxFontCollection();
	~DxFontCollection();
	DxFontCollection(const DxFontCollection&) = delete;
	DxFontCollection& operator=(const DxFontCollection&) = delete;
	HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szPath);
	HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);
	void Uninitialize();
	WriteFontCollection* FontCollection();
	DxWriteFactory::WriteFactory* WriteFactory();
	LPCWSTR FontFamily() const;
	operator WriteFontCollection* ();
	operator DxWriteFactory::WriteFactory* ();

private:
	WriteMemoryLoader* pMemoryLoader;
	WriteFontCollection* pFontCollection;
	DxWriteFactory::WriteFactory* pWriteFactory;
	LPWSTR lpszFontFamily;
};

class DXG_EXPORT DxTextFormat
{
public:
	using WriteTextFormat = IDWriteTextFormat;

	DxTextFormat();
	~DxTextFormat();
	DxTextFormat(const DxTextFormat&) = delete;
	DxTextFormat& operator=(const DxTextFormat&) = delete;
	HRESULT Initialize(DxWriteFactory::WriteFactory* pWriteFactory, LPCWSTR szFontFamilyName, DxFontCollection::WriteFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);
	HRESULT Initialize(DxFontCollection* pFontCollection, float fFontSize, LPCWSTR szLocaleName);
	void Uninitialize();
	WriteTextFormat* TextFormat();
	DxWriteFactory::WriteFactory* WriteFactory();
	float FontSize() const;
	operator WriteTextFormat* ();
	operator DxWriteFactory::WriteFactory* ();

private:
	WriteTextFormat* pTextFormat;
	float fFontSize;
	DxWriteFactory::WriteFactory* pWriteFactory;
};

class DXG_EXPORT DxFactory2D
{
public:
	using D2DFactory = ID2D1Factory5;

	DxFactory2D();
	~DxFactory2D();
	DxFactory2D(const DxFactory2D&) = delete;
	DxFactory2D& operator=(const DxFactory2D&) = delete;
	HRESULT Initialize();
	void Uninitialize();
	D2DFactory* Factory();
	operator D2DFactory* ();

private:
	D2DFactory* pFactory;
};

class DXG_EXPORT DxTarget2D
{
public:
	using D2DTarget = ID2D1HwndRenderTarget;
	enum Interlayer
	{
		GDI_ON_DIRECT2D,
		DIRECT2D_ON_GDI
	};
	const D2D1_COLOR_F Transparent = D2D1::ColorF(D2D1::ColorF::Black, 0.0f);

	DxTarget2D();
	~DxTarget2D();
	DxTarget2D(const DxTarget2D&) = delete;
	DxTarget2D& operator=(const DxTarget2D&) = delete;
	HRESULT Initialize(DxFactory2D::D2DFactory* p2DFactory, HWND hWnd);
	void Uninitialize();
	void SetInterlayer(Interlayer interlayer = GDI_ON_DIRECT2D);
	Interlayer GetInterlayer() const;
	bool BeginDraw();
	HRESULT EndDraw();
	void Clear(D2D1_COLOR_F Color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
	void FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 }, FLOAT alpha = 1.0f);
	D2D1_RECT_F PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 }, FLOAT alpha = 1.0f);
	void DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor); // 0.0f ~ 100.0f
	bool DrawButton(D2D1_RECT_F& Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap = nullptr, FLOAT alpha = 1.0f, D2D1_COLOR_F FrontColor = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F BackColor = D2D1::ColorF(D2D1::ColorF::White), D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Black));
	bool DrawButton(D2D1_RECT_F* Rect, LPCWSTR szText, DxTextFormat* TextFormat, ID2D1Bitmap* pBackgroundBitmap = nullptr, FLOAT alpha = 1.0f, D2D1_COLOR_F FrontColor = D2D1::ColorF(D2D1::ColorF::Black), D2D1_COLOR_F BackColor = D2D1::ColorF(D2D1::ColorF::White), D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Black));
	HRESULT Resize(UINT Width, UINT Height);
	HRESULT Resize(const D2D1_SIZE_U& pixelSize);
	HRESULT Resize(const D2D1_SIZE_U* pixelSize);
	D2DTarget* Target();
	operator D2DTarget* ();
	void FlushGDI() const;

private:
	D2DTarget* pTarget;
	HWND hWnd;
	Interlayer interlayer;
	bool tagDraw;
};

class DXG_EXPORT DxImage
{
public:
	using Image = ID2D1Bitmap;

	DxImage();
	~DxImage();
	DxImage(const DxImage&) = delete;
	DxImage& operator=(const DxImage&) = delete;
	HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath);
	HRESULT Load(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType);
	void Release();
	Image* Bitmap();
	Image** BitmapPtr();
	operator Image* ();
	operator Image** ();

private:
	Image* pImage;
};

DXG_EXPORT D2D1_RECT_F CalcFillBitmapSourceRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect);
DXG_EXPORT D2D1_RECT_F CalcPutBitmapTargetRect(D2D1_RECT_F bitmapRect, D2D1_RECT_F targetRect);
DXG_EXPORT D2D1_RECT_U CalcFillBitmapSourceRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect);
DXG_EXPORT D2D1_RECT_U CalcPutBitmapTargetRect(D2D1_RECT_U bitmapRect, D2D1_RECT_U targetRect);

DXG_EXPORT FLOAT GetDpiScaleForSystem();
DXG_EXPORT FLOAT GetDpiScaleForWindow(HWND hWnd);
DXG_EXPORT FLOAT PixelsToDips(UINT Pixels, FLOAT DpiScale);
DXG_EXPORT UINT DipsToPixels(FLOAT Dips, FLOAT DpiScale);

DXG_EXPORT D2D1_SIZE_F CalcSizeFormRect(D2D1_RECT_F Rect);
DXG_EXPORT D2D1_SIZE_U CalcSizeFormRect(D2D1_RECT_U Rect);

DXG_EXPORT D2D1_SIZE_F SizePixelsToDips(D2D1_SIZE_U Pixels, FLOAT DpiScale);
DXG_EXPORT D2D1_RECT_F RectPixelsToDips(D2D1_RECT_U Pixels, FLOAT DpiScale);
DXG_EXPORT D2D1_SIZE_U SizeDipsToPixels(D2D1_SIZE_F Dips, FLOAT DpiScale);
DXG_EXPORT D2D1_RECT_U RectDipsToPixels(D2D1_RECT_F Dips, FLOAT DpiScale);

DXG_EXPORT D2D1_RECT_F CalcCenterRect(D2D1_SIZE_F Item, D2D1_RECT_F Canvas);
DXG_EXPORT D2D1_RECT_U CalcCenterRect(D2D1_SIZE_U Item, D2D1_RECT_U Canvas);

DXG_EXPORT D2D1_SIZE_U GetSizeFromlParam(LPARAM lParam);
DXG_EXPORT D2D1_POINT_2U GetPointFromlParam(LPARAM lParam);

DXG_EXPORT bool IsPointInRect(D2D1_SIZE_F Point, D2D1_RECT_F Rect);
DXG_EXPORT bool IsPointInRect(D2D1_SIZE_U Point, D2D1_RECT_U Rect);

#endif // __cplusplus

#endif // !DXGRAPHICS_H
