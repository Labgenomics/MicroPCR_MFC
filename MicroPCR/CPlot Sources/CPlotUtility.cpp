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
#include <math.h>
#include "CPlotUtility.h"
#include "CPlotResource.h"

CFont	*MySelectFont( CDC *pDC, CFont *font, LOGFONT *logFont )
{
	CPoint			point;
	int				nHeight;
	CFont			*pOldFont;

	if( font == NULL ) return NULL;

	// The units of the font height are in points,
	// 1/72 of an inch, or 0.3527777777778 mm
	// Since we're using a lometric mode,
	// we need to convert to tenths of a mm.
	point.x = 0;
	point.y = (long)(logFont->lfHeight / .352777777777778);

	nHeight = logFont->lfHeight;
	logFont->lfHeight = point.y;

	font->DeleteObject(); // will this crash?

	font->CreateFontIndirect( logFont );
	pOldFont = pDC->SelectObject(font);

	logFont->lfHeight = nHeight;
	
	return pOldFont;
}

// Returns a string containing the name and font size of
// the passed font
CString	GetFontDescription( LOGFONT *font )
{
	CString		string;

	if( font == NULL ) return L"";

	string.Format( L"%d point %s", font->lfHeight, font->lfFaceName );
	return string;
}


// The function assumes the use of the current pen
// I was having trouble with CDC::FrameRect,
// so I wrote this. It restores the position of the pen.
void	MyFrameRect( CDC *pDC, CRect rect )
{
	CPoint		lastPoint;

	if( pDC == NULL ) return;

	lastPoint = pDC->GetCurrentPosition();
	pDC->MoveTo( rect.left, rect.top );
	pDC->LineTo( rect.right, rect.top );
	pDC->LineTo( rect.right, rect.bottom );
	pDC->LineTo( rect.left, rect.bottom );
	pDC->LineTo( rect.left, rect.top );
	pDC->MoveTo( lastPoint );
}

// Rounds any number to any number. i.e. round 1.9
// to the nearest 0.4 yields 2.0
double	RoundToRange( double value, double range )
{
	double		half = range / 2;
	double		scale = value / range;
	double		mantissa;
	long		exponent;
	int			sign = value>0?1:-1;

	mantissa = scale - (long) scale;
	exponent = (long) scale;

	if( sign > 0 )
	{
		if( fabs( mantissa ) >= 0.5 )
			return range * (exponent + sign);
		else
			return range * exponent;
	}
	else
	{
		if( fabs( mantissa ) > 0.5 )
			return range * (exponent + sign);
		else
			return range * exponent;
	}
}

// Simply returns a flag value for popup menus
// based on a boolean test
UINT	GetPopupCheckedFlag( BOOL how )
{
	if( how ) return MF_CHECKED;
	else return MF_UNCHECKED;
}

// Adds a popup menu for font sizes.
void	AddFontSizePopup( CMenu *addMenu, CString titleString, long compare, int startVal )
{
	CMenu			subMenu;
	int				i;
	CString			string;
	UINT			flag;

	subMenu.CreatePopupMenu();

	for( i = 0; i < nFontSizes; i++ )
	{
		flag = GetPopupCheckedFlag( compare == fontSizes[i] );
		string.Format( L"%d", fontSizes[i] );
		subMenu.AppendMenu( MF_STRING | flag, startVal+i, string );
	}

	addMenu->AppendMenu( MF_POPUP, (UINT) subMenu.GetSafeHmenu(), titleString );
}

BOOL	FloatEqual( float a, float b )
{ 
	if( a != (float)0.0 )
	{
		if( fabs( a-b )/fabs(a) < 1e-4 )
			return TRUE;
		else
			return FALSE;
	}
	else if( fabs(b) > (float) 1e-4 )
		return FALSE;
	else
		return TRUE;
}

BOOL	FloatEqual( double a, double b )
{ 
	if( a != 0.0 )
	{
		// Test as a fractional value
		if( fabs( a-b )/fabs(a) < 1e-4 )
			return TRUE;
		else
			return FALSE;
	}
	else if( fabs(b) > 1e-4 )
		return FALSE;
	else
		return TRUE;
}

// Adds a color table menu to the menu pointed by *menu
void	AddColorTableMenu( CMenu *menu, UINT startVal )
{
	CString			string, menuString;
	int				curPos, length, newPos, curMenuItem = 0;

	if( menu == NULL ) return;

	if( string.LoadString( IDS_COLOR_TABLES ) == FALSE ) return;

	length = string.GetLength();

	curPos = 0;

	while( curPos < length )
	{
		// Each line is a menu item. A blank line means a separator
		newPos = string.Find( L"\n", curPos );
		if( newPos < 0 ) newPos = length;

		// Extract the sub string
		if( newPos == curPos )
		{
			menu->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR) 0 );
		}
		else
		{
			menuString = string.Mid( curPos, newPos - curPos );
			menu->AppendMenu( MF_STRING, startVal + curMenuItem, menuString );

			curMenuItem++;
		}

		curPos = newPos + 1;
	}
}