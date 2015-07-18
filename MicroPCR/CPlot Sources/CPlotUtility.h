#pragma once
#include <math.h>

CFont	*MySelectFont( CDC *pDC, CFont *font, LOGFONT *logFont );
CString	GetFontDescription( LOGFONT *font );
void	MyFrameRect( CDC *pDC, CRect rect );
double	RoundToRange( double value, double range );
UINT	GetPopupCheckedFlag( BOOL how );
void	AddFontSizePopup( CMenu *addMenu, CString titleString, long compare, int startVal );

BOOL	FloatEqual( float a, float b );
BOOL	FloatEqual( double a, double b );

#define		IDS_COLORTABLE_START	45000
#define		kColorTableCount		33
#define		IDS_COLORTABLE_END		(IDS_COLORTABLE_START+kColorTableCount)

void	AddColorTableMenu( CMenu *menu, UINT startVal = IDS_COLORTABLE_START );

const int		fontSizes[] = {8, 10, 12, 18, 24, 36, 48};
const int		nFontSizes = 7;
