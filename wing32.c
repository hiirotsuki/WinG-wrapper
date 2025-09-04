/* SPDX-License-Identifier: GPL-2.0-or-later */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "pluginloader.h"

enum WING_DITHER_TYPE
{
	WING_DISPERSED_4x4,
	WING_DISPERSED_8x8,
	WING_CLUSTERED_4x4
};

BOOL WINAPI WinGBitBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, int nYOriginSrc)
{
	return BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
}

HBITMAP WINAPI WinGCreateBitmap(HDC hWinGDC, BITMAPINFO far *pHeader, void far *far *ppBits)
{
	return CreateDIBSection(hWinGDC, pHeader, DIB_RGB_COLORS, ppBits, NULL, 0);
}

HDC WINAPI WinGCreateDC(void)
{
	return CreateCompatibleDC(NULL);
}

HBRUSH WINAPI WinGCreateHalftoneBrush(HDC hdc, COLORREF Color, enum WING_DITHER_TYPE DitherType)
{
	SetStretchBltMode(hdc, HALFTONE);
	
	HPALETTE halftonePalette = CreateHalftonePalette(hdc);
	if(halftonePalette)
	{
		SelectPalette(hdc, halftonePalette, FALSE);
		RealizePalette(hdc);
	}
	
	return CreateSolidBrush(Color);
}

HPALETTE WINAPI WinGCreateHalftonePalette(void)
{
	HDC hdc = GetDC(NULL);
	if(!hdc)
		return NULL;
		
	HPALETTE hPal = CreateHalftonePalette(hdc);
	ReleaseDC(NULL, hdc);
	
	return hPal;
}

UINT WINAPI WinGGetDIBColorTable(HDC hWinGDC, UINT StartIndex, UINT NumberOfEntries, RGBQUAD far *pColors)
{
	return GetDIBColorTable(hWinGDC, StartIndex, NumberOfEntries, pColors);
}

void WINAPI far *WinGGetDIBPointer(HBITMAP hWinGBitmap, BITMAPINFO far *pHeader)
{
	DIBSECTION ds;

	if(GetObject(hWinGBitmap, sizeof(ds), &ds) == sizeof(ds))
	{
		if(pHeader != NULL)
		{
			pHeader->bmiHeader = ds.dsBmih;
			
			if(ds.dsBmih.biCompression == BI_BITFIELDS)
			{
				DWORD *masks = (DWORD *)pHeader->bmiColors;
				masks[0] = ds.dsBitfields[0];
				masks[1] = ds.dsBitfields[1];
				masks[2] = ds.dsBitfields[2];
			}
		}

		return ds.dsBm.bmBits;
	}
	
	return NULL;
}

BOOL WINAPI WinGRecommendDIBFormat(BITMAPINFO far *pHeader)
{
	if(!pHeader)
		return FALSE;

	memset(pHeader, 0, sizeof(BITMAPINFOHEADER));
	pHeader->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	pHeader->bmiHeader.biWidth    = 1;
	pHeader->bmiHeader.biHeight   = 1;
	pHeader->bmiHeader.biPlanes   = 1;
	pHeader->bmiHeader.biBitCount = 8;

	return TRUE;
}

UINT WINAPI WinGSetDIBColorTable(HDC hWinGDC, UINT StartIndex, UINT NumberOfEntries, RGBQUAD far *pColors)
{
	return SetDIBColorTable(hWinGDC, StartIndex, NumberOfEntries, pColors);
}

BOOL WINAPI WinGStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc)
{
	BOOL result;
	int prevMode;
	
	prevMode = SetStretchBltMode(hdcDest, HALFTONE);
	result = StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
	SetStretchBltMode(hdcDest, prevMode);
	
	return result;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			(void)LoadPluginsFromDirectory(L"plugins");
			break;
		}

		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}