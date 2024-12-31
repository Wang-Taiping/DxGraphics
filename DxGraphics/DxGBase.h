
#ifndef DXGBASE_H
#define DXGBASE_H

#ifdef DXG_EXPORT
#error Macro 'DXG_EXPORT' already defined.
#endif // DXG_EXPORT

#ifdef DXG_BUILD
#define DXG_EXPORT	__declspec(dllexport)
#else
#define DXG_EXPORT	__declspec(dllimport)
#endif // DXG_BUILD

#ifdef DXG_STATIC
#undef DXG_EXPORT
#define DXG_EXPORT
#endif // DXG_STATIC

#define DXG_API			__stdcall
#define DXG_CALLBACK	__stdcall

#endif // !DXGBASE_H
