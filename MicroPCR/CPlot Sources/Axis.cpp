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
#include "Chart.h"
#include "Axis.h"
#include "CPlotUtility.h"
#include <math.h>

// Constants for popup menus
const int		tickSizes[] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
const int		nTickSizes = 9;

IMPLEMENT_SERIAL( CAxis, CObject, 1 )

CAxis::CAxis()
{
	SetDefaults();
}

// Set the default for all the CAxis parameters
void	CAxis::SetDefaults( void )
{
	m_TickCount = -4;
	m_MinorTickCount = -5;
	m_Range[0] = m_Range[1] = 0.0;
	m_Visible = TRUE;
	m_Title = "";
	m_Location = kLocationBottom;

#ifdef DEFAULT_WHITE_BACKGROUND
	m_Color = RGB( 0, 100, 0 );
	m_TitleColor = RGB( 0, 0, 0 );
#else
	m_Color = RGB( 0, 255, 0 );
	m_TitleColor = RGB( 255, 255, 255 );
#endif

	m_TickLength = 20;
	m_MinorTickLength = 10;
	m_RangeSet = FALSE;

	m_LineSize = 1;

	CFont		font1, font2;

	font1.CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,L"Arial");
	font1.GetLogFont( &m_TitleFont );

	font2.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,L"Arial");
	font2.GetLogFont( &m_LabelFont );
}

CAxis::~CAxis()
{
}

void CAxis::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
    {
	   ar << m_TickCount << m_MinorTickCount << m_TickLength;
	   ar << m_MinorTickLength << m_Range[0] << m_Range[1];
	   ar << m_Visible << m_Title << m_Location;
	   ar  << m_Color;
	   ar << m_TitleColor << m_RangeSet;
	   ar << m_LineSize;
	   ar.Write( &m_TitleFont, sizeof( LOGFONT ) );
	   ar.Write( &m_LabelFont, sizeof( LOGFONT ) );
    }
    else
    {
	   ar >> m_TickCount >> m_MinorTickCount >> m_TickLength;
	   ar >> m_MinorTickLength >> m_Range[0] >> m_Range[1];
	   ar >> m_Visible >> m_Title >> m_Location;
	   ar >> m_Color;
	   ar >> m_TitleColor >> m_RangeSet;
	   ar >> m_LineSize;
	   ar.Read( &m_TitleFont, sizeof( LOGFONT ) );
	   ar.Read( &m_LabelFont, sizeof( LOGFONT ) );
    }
}

// Overridden from CChart
void	CAxis::OnDraw( CDC *pDC, CRect clientRect, CRect chartRect )
{
	OnDraw( pDC, clientRect, chartRect, clientRect );
}

// Overridden from CChart
void	CAxis::OnDraw( CDC *pDC, CRect clientRect, CRect chartRect, CRect updateRect )
{
	double			position, range;
	TEXTMETRIC		tm;
	LOGFONT			logFont;
	CRect			textRect;
	CSize			tmSize(0,0);
	unsigned int	textMode;
	int				tickHeight = 0;
	int				maxTickLength;
	BOOL			keepGoing = TRUE;
	CFont			font;

	// Tick marks and hashes
	{
		CPoint		tickPoint;
		CPen		pen( PS_SOLID, m_LineSize, m_Color ), *pOldPen;
		double		scale, interval, tickScale;
		int			i, j, sign;
		CFont		*pOldFont;
		CString		tickLabel;
		CSize		labelExtent(0,0);
		long		startPoint;
		int			tickMarkLength, minorTickMarkLength;
		
		keepGoing = TRUE;
		
		range = m_Range[1] - m_Range[0];
		
		// Setup of ranges, pens
		pOldPen = pDC->SelectObject( &pen );
		pDC->SetBkMode( TRANSPARENT );
		pDC->SetTextColor( m_Color );
		
		memcpy( &logFont, &m_LabelFont, sizeof( LOGFONT ) );
		
		// Set up font and make sure that the font size will fit
		// the width or height of our chart
		while( keepGoing == TRUE )
		{
			pOldFont = MySelectFont( pDC, &font, &logFont );
			pDC->GetTextMetrics( &tm );
			
			switch( GetLocation() )
			{
			case kLocationBottom: case kLocationTop:
				if( 3 * tm.tmAveCharWidth > abs(chartRect.Width()) / GetTickCount() && logFont.lfHeight > 4 )
					logFont.lfHeight--;
				else
					keepGoing = FALSE;
				break;
			case kLocationLeft: case kLocationRight:
				if( tm.tmHeight > abs(chartRect.Height()) / GetTickCount() && logFont.lfHeight > 4  )
					logFont.lfHeight--;
				else
					keepGoing = FALSE;
				break;
			}
			
			pDC->SelectObject( pOldFont );
		}
		
		MySelectFont( pDC, &font, &logFont );
		
		// Get the size of tick marks, and approximate height of font.
		// The orientation determines whether this height is vertical or horizontal
		switch( GetLocation() )
		{
		case kLocationBottom: case kLocationTop:
			textMode = DT_NOCLIP | DT_CENTER | DT_SINGLELINE | (GetLocation()==kLocationBottom?DT_TOP:DT_BOTTOM);
			tickHeight = tm.tmHeight;
			tickMarkLength = m_TickLength;
			minorTickMarkLength = m_MinorTickLength;
			break;
		case kLocationLeft: case kLocationRight:
			textMode = DT_NOCLIP | DT_VCENTER | DT_SINGLELINE | (GetLocation()==kLocationRight?DT_LEFT:DT_RIGHT);
			// Don't calculate tickHeight here because it is calculated later
			tickMarkLength = m_TickLength;
			minorTickMarkLength = m_MinorTickLength;
			break;
		default: break;
		}
		
		// For moving our labels and titles, we need to be away
		// from the longest tick mark
		maxTickLength = max( minorTickMarkLength, tickMarkLength );
		
		// Do we add or subtract distance to move labels?
		if( GetLocation() == kLocationBottom || GetLocation() == kLocationRight )
			sign = -1; else sign = 1;
		
		interval = GetTickPosition(1) - GetTickPosition(0);
		tickScale = interval / this->GetMinorTickCount();
		
		// If we have equal range, we have no axis
		if( FloatEqual( m_Range[1], m_Range[0] ) == FALSE )
		{
			for( i = 0; i <= this->GetTickCount()+1; i++ )
			{
				position = GetTickPosition( i );

				switch( GetLocation() )
				{
				case kLocationBottom: case kLocationTop:
					scale = (double)chartRect.Width() / range;
					
					startPoint = GetLocation() == kLocationBottom?chartRect.bottom:chartRect.top;
					// Major Tick marks
					tickPoint.y = startPoint - tickMarkLength/2;
					tickPoint.x = chartRect.left + (long)((position-m_Range[0]) * scale);
					pDC->MoveTo( tickPoint );
					tickPoint.y += tickMarkLength;
					pDC->LineTo( tickPoint );
					
					// Minor tick marks
					if( i <= this->GetTickCount() )
						for( j = 1; j < this->GetMinorTickCount(); j++ )
						{
							tickPoint.x = chartRect.left + (long)((position-m_Range[0]+j*tickScale) * scale);
							tickPoint.y = startPoint + minorTickMarkLength/2;
							pDC->MoveTo( tickPoint );
							tickPoint.y -= minorTickMarkLength;
							pDC->LineTo( tickPoint );
						}
						
						// Tick labels
						tickPoint.x = chartRect.left + (long)((position-m_Range[0]) * scale);
						tickPoint.y = startPoint + sign*tickMarkLength/2;
						textRect.left = textRect.right = tickPoint.x;
						textRect.top = textRect.bottom = tickPoint.y;
						tickLabel = GetTickLabel( i );
						pDC->DrawText( tickLabel, (LPRECT)&textRect, textMode );
						break;
				case kLocationLeft: case kLocationRight:
					// Major Tick marks
					startPoint = (GetLocation()==kLocationLeft?chartRect.left:chartRect.right);
					
					scale = (double)chartRect.Height() / range;
					tickPoint.y = chartRect.bottom - (long)((position-m_Range[0]) * scale);
					tickPoint.x = startPoint - tickMarkLength/2;
					pDC->MoveTo( tickPoint );
					tickPoint.x += tickMarkLength;
					pDC->LineTo( tickPoint );
					
					// Minor tick marks
					if( i <= this->GetTickCount() )
						for( j = 1; j < this->GetMinorTickCount(); j++ )
						{
							tickPoint.y = chartRect.bottom - (long)((position-m_Range[0]+j*tickScale) * scale);
							tickPoint.x = startPoint - minorTickMarkLength/2;
							pDC->MoveTo( tickPoint );
							tickPoint.x += minorTickMarkLength;
							pDC->LineTo( tickPoint );
						}
						
						// Tick labels
						tickPoint.y = chartRect.bottom - (long)((position-m_Range[0]) * scale);
						tickPoint.x = startPoint - sign*tickMarkLength/2;
						textRect.left = textRect.right = tickPoint.x;
						textRect.top = textRect.bottom = tickPoint.y;
						tickLabel = GetTickLabel( i );
						pDC->DrawText( tickLabel, (LPRECT)&textRect, textMode );
						labelExtent = pDC->GetTextExtent( tickLabel );
						if( labelExtent.cx > tickHeight ) tickHeight = labelExtent.cx;
						break;
				default: break;
				}
			}
			
			pDC->SelectObject( pOldPen );
			pDC->SelectObject( pOldFont );
		} // Draw labels
	} // draw ticks and tick marks

	if( m_Title != "" )
	{ // Title
		long			height;
		CFont			font, *pOldFont, *pOldFont2;

		keepGoing = TRUE;

		memcpy( &logFont, &m_TitleFont, sizeof( LOGFONT ) );

		// Adjust the title size so that it fits in our space
		while( keepGoing == TRUE )
		{
			pOldFont = MySelectFont( pDC, &font, &logFont );
			tmSize = pDC->GetOutputTextExtent( m_Title );
			pDC->SelectObject( pOldFont );

			switch( GetLocation() )
			{
			case kLocationLeft: case kLocationRight:
				if( tmSize.cx > abs(chartRect.Height()) )
					logFont.lfHeight--;
				else
					keepGoing = FALSE;
				break;
			case kLocationTop: case kLocationBottom:
				if( tmSize.cx > abs(chartRect.Width()) )
					logFont.lfHeight--;
				else
					keepGoing = FALSE;
				break;
			}

			if( logFont.lfHeight <= 4 ) keepGoing = FALSE;
		}

		// Find the rectangle that encloses our axis title
		switch( GetLocation() )
		{
		case kLocationLeft:

			logFont.lfEscapement = 2700;
			logFont.lfOrientation = 2700;

			textMode = DT_LEFT | DT_BOTTOM | DT_SINGLELINE;
			pOldFont2 = MySelectFont( pDC, &font, &logFont );

			tmSize = pDC->GetOutputTextExtent( m_Title );
			// This is the good rect
			textRect = chartRect;
			textRect.right = chartRect.left - 10 - tickHeight - maxTickLength/2;
			textRect.left = textRect.right - tmSize.cy;
			height = chartRect.Height();

			// Adjust position for font orientation and size
			textRect.bottom = chartRect.top + height/2 - tmSize.cx/2 - tmSize.cy/2;
			textRect.top = textRect.bottom;

			// Rotation requires an additional adjustment
			textRect.top += tmSize.cx + tmSize.cy/2;
			textRect.bottom -= tmSize.cy/2;

			if( textRect.bottom < clientRect.bottom ) textRect.bottom = clientRect.bottom;
			if( textRect.top > clientRect. top ) textRect.top = clientRect.top;
			break;
		case kLocationRight:
			logFont.lfEscapement = 900;
			logFont.lfOrientation = 900;

			textMode = DT_LEFT | DT_TOP | DT_SINGLELINE;
			pOldFont2 = MySelectFont( pDC, &font, &logFont );
			tmSize = pDC->GetOutputTextExtent( m_Title );
			height = chartRect.Height();

			textRect.left = chartRect.right + tickHeight + tmSize.cy + maxTickLength/2;
			textRect.right = textRect.left - tmSize.cy;
			textRect.top = chartRect.top + height/2 + tmSize.cx/2;
			textRect.bottom = textRect.top - tmSize.cx;

			if( textRect.bottom < clientRect.bottom ) textRect.bottom = clientRect.bottom;
			if( textRect.top > clientRect. top ) textRect.top = clientRect.top;
			break;
		case kLocationTop:
			textRect = chartRect;
			textRect.bottom = chartRect.top + 5 + tickHeight + maxTickLength/2;
			textRect.top = textRect.bottom + tmSize.cy;

			logFont.lfEscapement = 0;
			logFont.lfOrientation = 0;

			textMode = DT_CENTER | DT_BOTTOM | DT_SINGLELINE;
			pOldFont2 = MySelectFont( pDC, &font, &logFont );
			break;
		case kLocationBottom:
			textRect = chartRect;
			textRect.top = chartRect.bottom - 5 - tickHeight - maxTickLength/2;
			textRect.bottom = textRect.top - tmSize.cy;

			logFont.lfEscapement = 0;
			logFont.lfOrientation = 0;

			textMode = DT_CENTER | DT_TOP | DT_SINGLELINE;
			pOldFont2 = MySelectFont( pDC, &font, &logFont );
			break;
		}

		pDC->SetBkMode( TRANSPARENT );
		pDC->SetTextColor( m_TitleColor );
		pDC->DrawText( m_Title, (LPRECT) textRect, textMode );

		pDC->SelectObject( pOldFont2 );
	} // draw title

}

// Returns the rectangle, in the current coordinate space,
// that the axis will take up. Used to adjust the size
// of the main chart when an axis is near.
CSize	CAxis::GetMinDisplaySize( CDC *pDC )
{
	CSize		minSize(0,0);
	TEXTMETRIC	tm;
	CFont		font, *pOldFont;

	// We have to calculate the height (or width) of the tick marks,
	// the fonts (titles, if any) and the labels
	if( m_Title != "" )
	{
		pOldFont = MySelectFont( pDC, &font, &m_TitleFont );
		minSize = pDC->GetTextExtent( m_Title );
		pDC->SelectObject( pOldFont );
	}

	if( this->GetTickCount() > 0 )
	{
		CSize		temp(0,0), temp2(0,0);
		int			i;

		pOldFont = MySelectFont( pDC, &font, &m_LabelFont );

		pDC->GetTextMetrics(&tm);
		for( i = 0; i <= this->GetTickCount()+1; i++ )
		{
			temp = pDC->GetTextExtent( GetTickLabel( i ) );
			if( temp.cx > temp2.cx ) temp2.cx = temp.cx;
			if( temp.cy > temp2.cy ) temp2.cy = temp.cy;
		}

		// Some approximate spacings
		if( m_Location == kLocationLeft || m_Location == kLocationRight )
			{ minSize.cy += temp2.cx + 15; minSize.cx += temp2.cy + 15; }
		else
			{ minSize.cx += temp2.cx + 10; minSize.cy += temp2.cy + 10; }

		pDC->SelectObject( pOldFont );
	}

	// Finally the tick marks
	minSize.cy += max(this->m_MinorTickLength, this->m_TickLength)/2;

	// If we're left or right, switch x and y
	if( m_Location == kLocationLeft || m_Location == kLocationRight )
	{
		CSize	temp;
		temp.cx = minSize.cy;
		temp.cy = minSize.cx;

		return temp;
	}
	else
	{
		return minSize;
	}
}

// These are always returned axis-specific
// so for bottom  = left, right,
// for right = bottom pad, top pad
// for top = left, right
// for left = bottom, top

// This function looks at the first and last tick mark of
// an axis to see how much it extends over the edge of the
// chart. It assumes that the first and last tick marks
// go on the end of the axis. This may change in the future.
CSize	CAxis::GetPaddingSize( CDC *pDC )
{
	CSize		temp(0,0), temp2(0,0);
	TEXTMETRIC	tm;

	if( this->GetTickCount() > 0 )
	{
		CFont		*pOldFont, font;
		int			i;

		pOldFont = MySelectFont( pDC, &font, &m_LabelFont );

		pDC->GetTextMetrics(&tm);

		// Get info for first and last tick marks
		for( i = 0; i <= this->GetTickCount()+1; i+= this->GetTickCount() )
		{
			temp = pDC->GetTextExtent( GetTickLabel( i ) );
			switch( GetLocation() )
			{
			case kLocationBottom: case kLocationTop:
				if( i == 0 ) temp2.cx = temp.cx / 2; else temp2.cy = temp.cx / 2;
				break;
			case kLocationLeft: case kLocationRight:
				temp2.cx = temp2.cy = tm.tmHeight;
				break;
			}
		}
		pDC->SelectObject( pOldFont );
	}

	return temp2;
}

// Returns the distance along the axis, in DATA UNITS
// where the requested tick mark lies
double		CAxis::GetTickPosition( int whichTick )
{
	double		range;

	// Assume user has checked the tick values first
	if( whichTick < 0 || (this->GetTickCount() > 0 && whichTick > this->GetTickCount()+1) )
		return 0.0;

	range = m_Range[1] - m_Range[0];

	if( GetTickCount() > 0 )
	{
		// User has specified number of ticks
		return m_Range[0] + range * (double)whichTick/((double)this->GetTickCount()+1.);
	}
	else
	{
		// For now, just use four regions
		return m_Range[0] + range * (double)whichTick/((double)5.0);
	}
}

// Gets the label string for the requested tick mark.
// Useful in future if want to add user-specified
// tick labels.
CString		CAxis::GetTickLabel( int whichTick )
{
	double			value = GetTickPosition( whichTick );
	CString			string;

	string.Format( L"%g", value );

	return string;
}

// Copies the data of one axis to another
void	CAxis::CopyAxis( CAxis * axis )
{
	m_TickCount = axis->m_TickCount;
	m_MinorTickCount = axis->m_MinorTickCount;
	m_TickLength = axis->m_TickLength;
	m_MinorTickLength = axis->m_MinorTickLength;
	m_Range[0] = axis->m_Range[0];
	m_Range[1] = axis->m_Range[1];
	m_Visible = axis->m_Visible;
	m_Title = axis->m_Title;
	m_Location = axis->m_Location;
	memcpy( &m_TitleFont, &axis->m_TitleFont, sizeof( LOGFONT ) );
	memcpy( &m_LabelFont, &axis->m_LabelFont, sizeof( LOGFONT ) );
	m_TitleFont = axis->m_TitleFont;
	m_Color = axis->m_Color;
	m_TitleColor = axis->m_TitleColor;
	m_RangeSet = axis->m_RangeSet;
}

// These constants are for the popup menu support. They
// facilitate making and interpretation of the menus
#define		kMenuAxisTitleSize		1
#define		kMenuAxisLabelSize		(kMenuAxisTitleSize+nFontSizes)
#define		kMenuAxisMajorTickCount		(kMenuAxisLabelSize+nFontSizes)
// The +1 below is because there is one additional tick size (which is the "Auto" item)
#define		kMenuAxisMinorTickCount		(kMenuAxisMajorTickCount+nTickSizes+1)

// Adds the menus that pertain to the axis, including auto ranging,
// title size, label size, tick marks, etc. This is only called
// by the popup menu constructors for the chart types.
void	CAxis::AddAxisPopupMenus( CMenu *menu, int startVal )
{
	UINT			flag;
	CString			string;
	CMenu			axisMenu;
	CMenu			subMenu;

	axisMenu.CreatePopupMenu();

	flag = GetPopupCheckedFlag( this->m_RangeSet == FALSE );
	axisMenu.AppendMenu( MF_STRING | flag, startVal, L"Auto range" );

	// Font sizes
	AddFontSizePopup( &axisMenu, L"Title size", this->m_TitleFont.lfHeight, startVal+kMenuAxisTitleSize );
	AddFontSizePopup( &axisMenu, L"Label size", this->m_LabelFont.lfHeight, startVal+kMenuAxisLabelSize );

	axisMenu.AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	// Number of major and minor ticks
	AddNumberOfTicksPopup( &axisMenu, L"Major ticks", m_TickCount+1, startVal + kMenuAxisMajorTickCount );
	AddNumberOfTicksPopup( &axisMenu, L"Minor ticks", m_MinorTickCount, startVal + kMenuAxisMinorTickCount );

	// Add it to main menu
	switch( GetLocation() )
	{
	case kLocationLeft: string = L"Left Axis"; break;
	case kLocationRight: string = L"Right Axis"; break;
	case kLocationTop: string = L"Top Axis"; break;
	case kLocationBottom: string = L"Bottom Axis"; break;
	default: string = L"Error"; break;
	}

	axisMenu.AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	// A settings item is always useful
	axisMenu.AppendMenu( MF_STRING, startVal+kMenuAxisRange-1, L"Settings..." );

	menu->AppendMenu( MF_POPUP, (UINT) axisMenu.GetSafeHmenu(), string );
}

// Adds a popup menu for the number of ticks
void	CAxis::AddNumberOfTicksPopup( CMenu *addMenu, CString titleString, long compare, int startVal )
{
	CMenu			subMenu;
	int				i;
	CString			string;
	UINT			flag;

	subMenu.CreatePopupMenu();

	flag = GetPopupCheckedFlag( compare <= 0 );
	subMenu.AppendMenu( MF_STRING | flag, startVal, L"Auto" );

	for( i = 0; i < nTickSizes; i++ )
	{
		flag = GetPopupCheckedFlag( compare == tickSizes[i] );
		string.Format( L"%d", tickSizes[i] );
		subMenu.AppendMenu( MF_STRING | flag, startVal+i+1, string );
	}

	addMenu->AppendMenu( MF_POPUP, (UINT) subMenu.GetSafeHmenu(), titleString );
}

// Takes a return value from the popup menu and figures
// out what menu function it was. The value of "which"
// should be 0 for the first axis menu item.
int		CAxis::InterpretAxisPopupMenuItem( int which )
{
	if( which < 0 ) return kPopupError;

	// Did user say "auto range"?
	if( which == 0 ) { m_RangeSet = !m_RangeSet; return kPopupUpdate; }

	// font sizes
	if( which < kMenuAxisLabelSize + nFontSizes )
	{
		if( which < kMenuAxisTitleSize + nFontSizes ) // title font
		{
			m_TitleFont.lfHeight = fontSizes[which - kMenuAxisTitleSize];
		}
		else // label font
		{
			m_LabelFont.lfHeight = fontSizes[which - kMenuAxisLabelSize];
		}


		return kPopupUpdate;
	}

	// Number of ticks
	if( which >= kMenuAxisMajorTickCount && which < kMenuAxisMajorTickCount + nTickSizes + 1 ) // major ticks
	{
		if( which - kMenuAxisMajorTickCount == 0 ) m_TickCount = -abs(m_TickCount);
		else m_TickCount = tickSizes[which-kMenuAxisMajorTickCount-1] - 1;
		return kPopupUpdate;
	}
	else if( which >= kMenuAxisMinorTickCount && which < kMenuAxisMinorTickCount + nTickSizes + 1 ) // minor ticks
	{
		if( (which-kMenuAxisMinorTickCount) == 0 ) m_MinorTickCount = -abs(m_TickCount);
		else m_MinorTickCount = tickSizes[(which-kMenuAxisMinorTickCount-1)];
		return kPopupUpdate;
	}

	// Settings (unfortunately, have to do this one in parent)

	return kPopupError;
}