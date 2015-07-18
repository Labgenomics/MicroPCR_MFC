/*============================================================================*/
/*                                                                            */
/*                            C O P Y R I G H T                               */
/*                                                                            */
/*                          (C) Copyright 2003 by                             */
/*                              Todd Clements                                 */
/*                           All Rights Reserved                              */
/*                                                                            */
/*      The author assumes no responsibility for the use or reliability of    */
/*      his software.                                                         */
/*                                                                            */
/*============================================================================*/

/* ############################################################################################################################## */

#include "stdafx.h"

#include "ImagePalette.h"

IMPLEMENT_SERIAL( CImagePalette, CObject, 1 )

CImagePalette::CImagePalette()
{
	curPaletteNum = -1;
	this->m_pBitmapInfo = NULL;
}

CImagePalette::~CImagePalette()
{
	if( m_pBitmapInfo != NULL )
		delete m_pBitmapInfo;

	m_pBitmapInfo = NULL;
}

void CImagePalette::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << curPaletteNum;
	}
	else
	{
		ar >> curPaletteNum;

		// Don't know if this is legal!!!!!!!
		LoadPalette( curPaletteNum );
	}
}

// Set the current palette number
void	CImagePalette::SetPalette( int whichPalette )
{
	int		loadWhich = 0;

	if( LoadPalette( whichPalette ) == TRUE )
		curPaletteNum = whichPalette;
}

// Get the number of entries in the palette
// This is currently a constant representing
// the number of entries that Windows doesn't
// hog for itself
int		CImagePalette::GetNumEntries( void )
{
	return kMaxPaletteEntries;
}

// Loads a palette from a palette resource
BOOL	CImagePalette::LoadPalette( int which )
{
	return LoadPalette( MAKEINTRESOURCE( which ) );
}

BOOL	CImagePalette::LoadPalette( LPTSTR string )
{
	HGLOBAL		resource;
	HRSRC		hResInfo;
	BYTE		*data;
	DWORD		size;
	LOGPALETTE	*logPalette;
	PALETTEENTRY	*pPalEntry;
	int			i, entries, whichEntry;

	// Need to get a handle to the resource
	hResInfo = FindResource( NULL, string, _T("ColorTable") );
	if( hResInfo == NULL ) goto loadDefault;
 
	resource = LoadResource( NULL, hResInfo );

	data = (BYTE *) LockResource( resource );

	if( data == NULL ) goto loadDefault;
	
	size = SizeofResource( NULL, hResInfo );
	// Now we can access the elements of our data

	logPalette = (LOGPALETTE *) VirtualAlloc( NULL,
		sizeof( WORD ) + sizeof( WORD ) + kMaxPaletteEntries * sizeof( PALETTEENTRY ),
				MEM_COMMIT, PAGE_READWRITE );

	if( logPalette == NULL ) return FALSE;

	logPalette->palNumEntries = kMaxPaletteEntries;
	logPalette->palVersion = 0;
	pPalEntry = (PALETTEENTRY *)&logPalette->palPalEntry;

	// How many entries in our palette
	entries = size / 3;
	// Now go through and copy entries to palette
	for( i = 0; i < logPalette->palNumEntries; i++ )
	{
		whichEntry = 3*((i*entries)/(logPalette->palNumEntries-1));
		pPalEntry[i].peRed = data[whichEntry];
		pPalEntry[i].peGreen = data[whichEntry+1];
		pPalEntry[i].peBlue = data[whichEntry+2];
		pPalEntry[i].peFlags = 0;
	}

	// So they should all be there now create the palette
	palette.CreatePalette( logPalette );

	UnlockResource( resource );
	FreeResource( resource );
	SetupBitmapInfo( pPalEntry, logPalette->palNumEntries );

	if( logPalette != NULL )
	{
		VirtualFree( logPalette, 0, MEM_RELEASE );
		logPalette = NULL;
	}

	return TRUE;

loadDefault:
	logPalette = (LOGPALETTE *) VirtualAlloc( NULL,
		sizeof( WORD ) + sizeof( WORD ) + 16 * sizeof( PALETTEENTRY ),
				MEM_COMMIT, PAGE_READWRITE );

	if( logPalette == NULL ) return FALSE;

	logPalette->palNumEntries = 16;
	logPalette->palVersion = 0;
	pPalEntry = (PALETTEENTRY *)&logPalette->palPalEntry;

	// Now go through and copy entries to palette
	for( i = 0; i < logPalette->palNumEntries; i++ )
	{
		pPalEntry[i].peRed = i*16;
		pPalEntry[i].peGreen = i*16;
		pPalEntry[i].peBlue = i*16;
		pPalEntry[i].peFlags = 0;
	}

	// So they should all be there now create the palette
	palette.CreatePalette( logPalette );

	FreeResource( resource );
	SetupBitmapInfo( pPalEntry, logPalette->palNumEntries );

	if( logPalette != NULL )
	{
		VirtualFree( logPalette, 0, MEM_RELEASE );
		logPalette = NULL;
	}

	return FALSE;
}

// Sets up a bitmap info structure for the current palette
// This is used by functions like StretchDIBits, so it is
// important to have!
void	CImagePalette::SetupBitmapInfo( PALETTEENTRY *entries, int numEntries )
{
	// we now have a palette, let's set up our bitmap info header
	// Initialize Bitmap Info Header
	DWORD  dwBitmapInfoSize;
	WORD  dwLoopI;
	DWORD *pun16PalColorIndex;
	RGBQUAD	quad;

	if( m_pBitmapInfo != NULL ) delete m_pBitmapInfo;
	m_pBitmapInfo = NULL;

	// Create bitmap info structure for a full frame image
	dwBitmapInfoSize = sizeof(BITMAPINFOHEADER) + (GetNumEntries()) * 4 * sizeof(BYTE);
	m_pBitmapInfo    = (BITMAPINFO *)new BYTE[dwBitmapInfoSize];
	
	m_pBitmapInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes        = 1;
	m_pBitmapInfo->bmiHeader.biBitCount      = 8;
	m_pBitmapInfo->bmiHeader.biCompression   = BI_RGB;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed       = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant  = 0;
	m_pBitmapInfo->bmiHeader.biSizeImage     = 0;
	
	// Initialize DIB Palette
	pun16PalColorIndex = (DWORD*)&m_pBitmapInfo->bmiColors[0];  //pointer to colour palette
	for (dwLoopI = 0 ; dwLoopI < (GetNumEntries()) ; dwLoopI++)
	{
		if( dwLoopI < numEntries )
		{
			quad.rgbBlue = entries[dwLoopI].peBlue;
			quad.rgbGreen = entries[dwLoopI].peGreen;
			quad.rgbRed = entries[dwLoopI].peRed;
			quad.rgbReserved = 0;
		}
		else
			memset( &quad, 0, 4 );

		memcpy( &(pun16PalColorIndex[dwLoopI]), &quad, 4 );
	}
	
	// Define the bitmap info 
	// Need to set up these parameters when loading an image
	m_pBitmapInfo->bmiHeader.biWidth     = 0;
	m_pBitmapInfo->bmiHeader.biHeight    = 0;
}