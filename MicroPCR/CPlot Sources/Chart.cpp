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
#include "Axis.h"
#include "AxisSettings.h"
#include "CPlotUtility.h"
#include "ColorControl.h"
#include "Chart.h"

IMPLEMENT_SERIAL( CChart, CObject, 2 )

// CChart constructors
CChart::CChart()
{
	m_UseMajorHorizontalGrids = FALSE;
	m_UseMajorVerticalGrids = FALSE;
	m_UseMinorHorizontalGrids = FALSE;
	m_UseMinorVerticalGrids = FALSE;

	m_Title = "";

	// Default is black background
	m_BackgroundColor = RGB( 0, 0, 0 );
	m_LineColor = RGB( 255, 255, 255 );
	m_TitleColor = RGB( 255, 255, 255 );

#ifdef	DEFAULT_WHITE_BACKGROUND
	m_BackgroundColor = RGB( 255, 255, 255 );
	m_LineColor = RGB( 0, 0, 0 );
	m_TitleColor = RGB( 0, 0, 0 );
#endif

	m_MajorGridColor = RGB( 128, 128, 128 );
	m_MinorGridColor = m_MajorGridColor;

	m_MajorGridLineSize = 1;
	m_MinorGridLineSize = 1;

	m_MajorGridLineStyle = PS_SOLID;
	m_MinorGridLineStyle = PS_DOT;

	m_DataSetCount = 0;
	m_AxisCount = 0;

	m_EraseBackground = TRUE;

	int		i;

	for( i = 0; i < kMaxDataSets; i++ )
	{
		m_DataID[i] = -1;
		m_DataSets[i] = NULL;
		m_DataTitle[i] = "";
	}

	for( i = 0; i < kMaxAxes; i++ ) m_Axes[i] = NULL;

	m_CurDataID = 1;

	CFont		font;
	font.CreateFont(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,L"Arial");
	font.GetLogFont( &m_TitleFont );

	m_LastClientRect.SetRectEmpty();

	RegisterControlClass();
}

// When we destroy, have to take care of
// our axes and data sets since they aren't
// kept track of anywhere else
CChart::~CChart()
{
	int				i;

	// Delete data sets and axes, perhaps
	for( i = 0; i < m_AxisCount; i++ )
	{
		delete m_Axes[i];
	}

	for( i = 0; i < m_DataSetCount; i++ )
	{
		delete m_DataSets[i];
	}
}

void CChart::Serialize(CArchive& ar)
{
	int			i;
	if (ar.IsStoring())
	{
		ar << m_UseMajorHorizontalGrids << m_UseMajorVerticalGrids;
		ar << m_UseMinorHorizontalGrids << m_UseMinorVerticalGrids;
		ar << m_MajorGridColor << m_MinorGridColor;
		ar << m_CurDataID;
		ar << m_DataSetCount << m_AxisCount;
		ar << m_Title;
		ar << m_TitleColor << m_BackgroundColor << m_LineColor;
		ar << m_MajorGridLineSize << m_MinorGridLineSize;

		for( i = 0; i < m_DataSetCount; i++ )
		{
			m_DataSets[i]->Serialize( ar );
		}

		for( i = 0; i < m_AxisCount; i++ )
		{
			m_Axes[i]->Serialize( ar );
		}

		ar.Write( m_DataID, kMaxDataSets * sizeof( int ) );
		ar.Write( &m_TitleFont, sizeof( LOGFONT ) );
 
		for( i = 0; i < kMaxDataSets; i++ )
			ar << m_DataTitle[i];
  }
   else
   {
 		ar >> m_UseMajorHorizontalGrids >> m_UseMajorVerticalGrids;
		ar >> m_UseMinorHorizontalGrids >> m_UseMinorVerticalGrids;
		ar >> m_MajorGridColor >> m_MinorGridColor;
		ar >> m_CurDataID;
		ar >> m_DataSetCount >> m_AxisCount;
		ar >> m_Title;
		ar >> m_TitleColor >> m_BackgroundColor >> m_LineColor;
		ar >> m_MajorGridLineSize >> m_MinorGridLineSize;

		for( i = 0; i < m_DataSetCount; i++ )
		{
			m_DataSets[i]->Serialize( ar );
		}

		for( i = 0; i < m_AxisCount; i++ )
		{
			m_Axes[i]->Serialize( ar );
		}

		ar.Read( m_DataID, kMaxDataSets * sizeof( int ) );
		ar.Read( &m_TitleFont, sizeof( LOGFONT ) );

		for( i = 0; i < kMaxDataSets; i++ )
			ar >> m_DataTitle[i];
  }
}

// Virtual function
void	CChart::OnDraw( CDC *pDC, CRect destRect, CRect updateRect )
{
	// CChart has no native drawing procedure. Instead,
	// when a subclass is made, an OnDraw function should
	// be overloaded and made to draw that plot type.
	m_LastClientRect = destRect;
}

// Virtual function
void	CChart::OnDraw( CDC *pDC, CRect destRect )
{
	// CChart has no native drawing procedure. Instead,
	// when a subclass is made, an OnDraw function should
	// be overloaded and made to draw that plot type.
	m_LastClientRect = destRect;
}

// Returns the data set requested, or NULL if invalid
CChartData	*CChart::GetAbsoluteDataSet( int which )
{
	CChartData		*chartData;

	if( which < 0 || which >= m_DataSetCount ) return NULL;

	chartData = m_DataSets[which];

	if( chartData == NULL ) return NULL;

	return chartData;
}

CChartData	*CChart::GetDataSet( int dataID )
{
	int		i = GetIndex( dataID );

	if( i >= 0 ) return GetAbsoluteDataSet( i ); else return NULL;
}

// Adds an axis to the plot. Returns a pointer to
// this axis which need not be stored by the user
// but is useful for initial setup of the axis
CAxis	*CChart::AddAxis( short location )
{
	CAxis		*axis;

	if( m_AxisCount >= kMaxAxes ) return NULL;

	if( (axis = GetAxisByLocation( location )) != NULL )
	{
		axis->SetDefaults();
		axis->SetLocation(location);
		return axis;
	}
	else
	{
		axis = new CAxis;

		axis->SetLocation( location );

		// Add to our axis data sets
		m_Axes[m_AxisCount] = axis;
		m_AxisCount++;

		return axis;
	}
}

// Returns the axis at a specified location (left, top, etc)
// or NULL if no such axis exists
CAxis	*CChart::GetAxisByLocation( int location )
{
	int			i;
	CAxis		*axis;

	for( i = 0; i < GetAxisCount(); i++ )
	{
		axis = GetAxis(i);
		if( axis != NULL && axis->GetLocation() == location )
			return axis;
	}

	return NULL;
}

// Adjusts the chart size for the presence (or lack thereof)
// of axes.
CRect	CChart::AdjustPlotSizeForAxes( CDC *pDC, CRect plotRect )
{
	int			i;
	CAxis		*axis;
	CSize		axisDims, paddingDims;

	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = m_Axes[i];
		axisDims = axis->GetMinDisplaySize( pDC );
		paddingDims = axis->GetPaddingSize( pDC );
		switch( axis->GetLocation() )
		{
		case kLocationBottom:
			plotRect.bottom += axisDims.cy;
			plotRect.left += paddingDims.cx;
			plotRect.right -= paddingDims.cy;
			break;
		case kLocationTop:
			plotRect.top -= axisDims.cy;
			plotRect.left += paddingDims.cx;
			plotRect.right -= paddingDims.cy;
			break;
		case kLocationLeft:
			plotRect.left += axisDims.cx;
			plotRect.bottom += paddingDims.cx;
			plotRect.top -= paddingDims.cy;
			break;
		case kLocationRight:
			plotRect.right -= axisDims.cx;
			plotRect.bottom += paddingDims.cx;
			plotRect.top -= paddingDims.cy;
			break;
		default: break;
		}
	}

	return plotRect;
}

// If there is a title, shrink the plot size appropriately.
CRect	CChart::AdjustPlotSizeForTitle( CDC *pDC, CRect plotRect )
{
	if( m_Title != "" )
	{
		CSize		tmSize(0,0);
		CFont		*pOldFont, font;

		pOldFont = MySelectFont( pDC, &font, &m_TitleFont );
		tmSize = pDC->GetTextExtent( m_Title );
		plotRect.top -= tmSize.cy;
		pDC->SelectObject( pOldFont );
	}

	return plotRect;
}


// This algorithm comes from a paper by Dorothy Pugh (KarlG87710@aol.com)
// "A Robust Generalized Axis-Scaling Macro", presumably from the proceedings
// of the Southeast SAS Users Group Conference and was translated from
// SAS macro langauge to C by yours truly.
void	CChart::GetOptimalRange( double *inRange, double *outRange,
								int *nTicks, int *nMinor, BOOL exactRange )
{
	double		wasted = 0.8, margin = 0.01;

	if( exactRange == TRUE ) { wasted = 0.9; margin = 0.00; }

	GetOptimalRangePrivate( inRange, outRange, nTicks, nMinor, margin, wasted );
}

void	CChart::GetOptimalRangePrivate( double *inRange, double *outRange,
								int *nTicks, int *nMinor, double margin, double wasted )
{
	const double	cutPoint1 = 1.0, cutPoint2 = 1.25, cutPoint3 = sqrt( 10. );
	const int		maxTick = 11, minTick = 3;

	double		mantissa, range, temp, interval, min, max;
	double		oldNew, tickTest, oldMax = inRange[1], oldMin = inRange[0];
	int			rexp, minorTick = 0, flag;
	long		test;

	temp = log10( fabs( inRange[1] - inRange[0] ) );

	mantissa = pow( 10., temp - (int)temp );
	rexp = (int) temp;

	// Which cut point regime do we belong in?
	if( mantissa >= cutPoint3 )
	{
		interval = pow( 10., (double)rexp );
		minorTick = 9;
	}
	else if( mantissa >= cutPoint2 )
	{
		interval = 0.5 * pow( 10., (double)rexp );
		minorTick = 4;
	}
	else
	{
		interval = pow( 10., (double)(rexp-1) );
		minorTick = 9;
	}
	max = RoundToRange( inRange[1] + (0.5 + margin) * interval, interval );
	min = RoundToRange( inRange[0] - (0.5 + margin) * interval, interval );

	// check tick ranges and axis spacing
	flag = 0;
	range = max - min;
	oldNew = (inRange[1]-inRange[0])/range;
	tickTest = RoundToRange( 1. + (max-min-interval)/(0.5*interval),1. );

	if( oldNew < wasted && tickTest <= maxTick && margin <= 0.01 &&
		oldMax < (max - 0.5*interval) && oldMin > (min + 0.5 * interval) )
	{
		max = max - 0.5 * interval;
		min = min + 0.5 * interval;
		interval = 0.5 * interval;
		flag = 1;
		if( minorTick == 9 ) minorTick = 4;
	}

	// Moderate number of tick marks
	test = (long)RoundToRange( (max - min) / interval, 1. );

	while( test >= maxTick && flag == 0 )
	{
		if( test % 2 != 0 ) max = max + interval;

		interval = 2 * interval;
		test = (long)RoundToRange((max-min)/interval,1.);
		if( minorTick == 4 ) minorTick = 9;
	}

	while( test <= minTick && flag == 0 )
	{
		if( test <= 3 ) interval = 0.5 * interval;

		test = (long)RoundToRange( (max-min) / interval, 1. );
		if( minorTick == 9 ) minorTick = 4;
	}

	// Done!
	outRange[0] = min;
	outRange[1] = max;
	*nTicks = (int)fabs(RoundToRange( (max-min) / interval, 1. ))-1;
	*nMinor = minorTick+1;
}

// This does a silly, but useful check for an optimal tick interval
// given a known range. It tries to find the interval set with
// the least remainder. The order is from my head, and what
// I think works best. There's no other good reason for the order.
double		CChart::GetTickOptimalTickInterval( double lower, double upper )
{
	
	double		range = fabs(upper - lower);
	double		error = 1e30, minError = 1e30, logi;

	const int			tries[] = { 7, 6, 8, 5, 9, 4, 10, 3, 2 };
	const int			divTries[] = {4, 5, 3, 8, 6, 7, 2, 9, 10 };

	int			minIndex = 1, i;

	for( i = 0; i < 9; i++ )
	{
		// Take log base i of range
		logi = log( range ) / log( (double)(tries[i]) );

		// See how its error compares to others
		error = fabs(logi - (long) logi);

		if( error < minError )
		{
			minError = error;
			minIndex = tries[i];
		}

		error = fabs(range / divTries[i] - RoundToRange(range / divTries[i], 1.));

		if( error < minError )
		{
			minError = error;
			minIndex = divTries[i];
		}
	}

	return range / (double)minIndex;
}

// Simply gets the number of tick marks, major and minor, given
// the exact specified range
void	CChart::GetOptimalRangeExact( double *inRange, double *outRange,
								int *nTicks, int *nMinor )
{
	double		temp, tempRange;

	temp = GetTickOptimalTickInterval( inRange[0], inRange[1] );
	*nTicks = (int)fabs(RoundToRange((inRange[1] - inRange[0]) / temp, 1. )) - 1;

	tempRange = temp;
	temp = GetTickOptimalTickInterval( inRange[0], inRange[0] + temp );
	*nMinor = (int)fabs(RoundToRange( tempRange / temp, 1. ));

	// Done!
	outRange[0] = inRange[0];
	outRange[1] = inRange[1];
}

// Draws the title for a plot
void	CChart::DrawPlotTitle( CDC *pDC, CRect destRect, CRect plotRect )
{
	if( m_Title != "" )
	{
		CRect		titleRect;
		CFont		*pOldFont, font;
		CAxis		*axis;
		CSize		axisDims;
		BOOL		keepGoing = TRUE;
		CSize		tmSize;
		LOGFONT		logFont;

		titleRect = plotRect;
		titleRect.top = destRect.top;
		titleRect.bottom = plotRect.top;

		// If we have a top axis, make room for the title
		// around it
		if( (axis = GetAxisByLocation( kLocationTop )) != NULL )
		{
			axisDims = axis->GetMinDisplaySize( pDC );
			titleRect.bottom += axisDims.cy;
		}

		// Make sure there is a minimal separation
		if( titleRect.bottom == plotRect.top ) titleRect.bottom += 10;


		memcpy( &logFont, &m_TitleFont, sizeof( LOGFONT ) );

		// Auto size font. Changes the title font size
		// to smaller if there is not enough space on
		// the plot. Perhaps add a user flag for this?
		while( keepGoing == TRUE )
		{
			pOldFont = MySelectFont( pDC, &font, &logFont );
			tmSize = pDC->GetTextExtent( m_Title );
			if( tmSize.cx > abs(plotRect.Width()) )
				logFont.lfHeight--;
			else
				keepGoing = FALSE;

			if( logFont.lfHeight <= 4 ) keepGoing = FALSE;

			pDC->SelectObject( pOldFont );
		}

		// Draw the text. There should be a background transparent mode
		pDC->SetBkMode( TRANSPARENT );
		pDC->SetTextColor( m_TitleColor );
		pOldFont = MySelectFont( pDC, &font, &logFont );
		pDC->DrawText( m_Title, (LPRECT)titleRect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE );

		pDC->SelectObject( pOldFont );
	}
}

// The following three functions (EraseBackground, DrawMajorMinorGrids,
// and DrawFrame) are parent functions, but are not
// directly called by the parent because they may or may not
// be needed by a child function. Thus they are provided, but
// not automatically called. They must be called by the child
// OnDraw function if needed.
void	CChart::EraseBackground( CDC *pDC, CRect destRect )
{
	// Erase the background
	if( m_EraseBackground == TRUE )
	{
		CBrush			brush;
		brush.CreateSolidBrush( m_BackgroundColor );
		pDC->FillRect( (LPCRECT) destRect, &brush );
	}
}

void	CChart::DrawMajorMinorGrids( CDC *pDC, CRect plotRect )
{
	// Draw the major and minor grids
	{
		// Get the grid information from the axes
		double		scale, range, theRange[2];
		CAxis		*axis;
		CPoint		point, tickPoint;
		int			i, j, k;
		double		position, interval, tickScale;
		CPen		pen( m_MajorGridLineStyle, m_MajorGridLineSize, RGB(0,0,0) );
		CPen		*pOldPen, minorPen( m_MinorGridLineStyle, m_MinorGridLineSize, RGB(0,0,0) );

		pDC->SetBkColor( m_BackgroundColor );

		pDC->SetBkMode( TRANSPARENT );

		// We base our grids on the left or bottom axis, if they exist.
		for( i = 0; i < m_AxisCount; i++ )
		{
			axis = m_Axes[i];
			if( axis->GetLocation() == kLocationBottom || axis->GetLocation() == kLocationLeft )
			{
				axis->GetRange( theRange );
				range = theRange[1] - theRange[0];
	
				pen.DeleteObject();
				pen.CreatePen( m_MajorGridLineStyle, m_MajorGridLineSize, m_MajorGridColor );
				pOldPen = pDC->SelectObject( &pen );
	
				if( axis->GetLocation() == kLocationLeft &&
					(m_UseMajorHorizontalGrids == TRUE || m_UseMinorHorizontalGrids == TRUE ) )
				{
					scale = (double)plotRect.Height() / range;
					for( j = 0; j <= axis->GetTickCount(); j++ )
					{
						position = axis->GetTickPosition( j );
						if( m_UseMajorHorizontalGrids == TRUE && j > 0 )
						{
							point.y = (long)(plotRect.bottom - scale * (position - theRange[0]));
							point.x = plotRect.left;
							pDC->MoveTo( point );
							point.x = plotRect.right;
							pDC->LineTo( point );
						}

						if( m_UseMinorHorizontalGrids == TRUE )
						{
							CPen		*pOldPen2;

							interval = axis->GetTickPosition(1) - axis->GetTickPosition(0);
							tickScale = interval / axis->GetMinorTickCount();
							tickPoint = point;

							minorPen.DeleteObject();
							minorPen.CreatePen( m_MinorGridLineStyle, m_MinorGridLineSize, m_MinorGridColor );
							pOldPen2 = pDC->SelectObject( &minorPen );
							for( k = 1; k < axis->GetMinorTickCount(); k++ )
							{
								tickPoint.y = plotRect.bottom - (long)((position-theRange[0]+(k*tickScale)) * scale);
								tickPoint.x = plotRect.left;
								pDC->MoveTo( tickPoint );
								tickPoint.x = plotRect.right;
								pDC->LineTo( tickPoint );
							}
							pDC->SelectObject( pOldPen2 );
						}
					}
				}

				if( axis->GetLocation() == kLocationBottom &&
					(m_UseMajorVerticalGrids == TRUE || m_UseMinorVerticalGrids == TRUE ) )
				{
					scale = (double)plotRect.Width() / range;
					for( j = 0; j <= axis->GetTickCount(); j++ )
					{
						position = axis->GetTickPosition( j );
						if( m_UseMajorVerticalGrids == TRUE && j > 0 )
						{
							point.x = (long)(scale * (position - theRange[0]) + plotRect.left);
							point.y = plotRect.bottom;
							pDC->MoveTo( point );
							point.y = plotRect.top;
							pDC->LineTo( point );
						}
						 
						if( m_UseMinorVerticalGrids == TRUE )
						{
							CPen		*pOldPen2;

							interval = axis->GetTickPosition(1) - axis->GetTickPosition(0);
							tickScale = interval / axis->GetMinorTickCount();
							tickPoint = point;

							minorPen.DeleteObject();
							minorPen.CreatePen( m_MinorGridLineStyle, m_MinorGridLineSize, m_MinorGridColor );
							pOldPen2 = pDC->SelectObject( &minorPen );
							for( k = 1; k < axis->GetMinorTickCount(); k++ )
							{
								tickPoint.x = plotRect.left + (long)((position-theRange[0]+(k*tickScale)) * scale);
								tickPoint.y = plotRect.bottom;
								pDC->MoveTo( tickPoint );
								tickPoint.y = plotRect.top;
								pDC->LineTo( tickPoint );
							}
							pDC->SelectObject( pOldPen2 );
						}
					}
				}

				pDC->SelectObject( pOldPen );
			}
		}
	}
}

void	CChart::DrawFrame( CDC *pDC, CRect plotRect )
{
	CPen			pen( PS_SOLID, 1, m_LineColor );
	CPen			*pOldPen;

	pOldPen = pDC->SelectObject( &pen );

	// I was having problems with CDC::FrameRect, so I
	// wrote my own. It uses the current pen instead
	// of a brush, and doesn't fill. It also saves
	// the pen position.
	MyFrameRect( pDC, plotRect  );
	pDC->SelectObject( pOldPen );
}

// Default, overridable function
CRect	CChart::GetPlotRect( CDC *pDC, CRect clientRect )
{
	return clientRect;
}

// The default functions do nothing to the passed things
void	CChart::DataToLP( CDC *pDC, double *data, LPPOINT point )
{
	CRect		plotRect, tempRect;
	double		xRange[2], yRange[2];
	double		scaleX, scaleY;

	if( point == NULL || pDC == NULL ) return;

	tempRect = m_LastClientRect;
	tempRect.NormalizeRect();
	if( tempRect.IsRectEmpty() ) return;

	plotRect = GetPlotRect( pDC, m_LastClientRect );

	GetRange( xRange, yRange );

	scaleX = (xRange[1] - xRange[0] ) / abs(plotRect.Width());
	scaleY = (yRange[1] - yRange[0] ) / abs(plotRect.Height());

	if( scaleX == 0. || scaleY == 0. ) return;

	point->x = (long)((data[0]-xRange[0]) / scaleX) + plotRect.left;
	point->y = (long)((data[1]-yRange[0]) / scaleY) + plotRect.bottom;
}

void	CChart::LPToData( CDC *pDC, LPPOINT point, double *data )
{
	CRect		plotRect, tempRect;
	double		xRange[2], yRange[2];
	double		scaleX, scaleY;
	
	if( point == NULL || pDC == NULL ) return;

	tempRect = m_LastClientRect;
	tempRect.NormalizeRect();
	if( tempRect.IsRectEmpty() ) return;

	plotRect = GetPlotRect( pDC, m_LastClientRect );

	GetRange( xRange, yRange );

	scaleX = (xRange[1] - xRange[0] ) / abs(plotRect.Width());
	scaleY = (yRange[1] - yRange[0] ) / abs(plotRect.Height());

	if( scaleX == 0. || scaleY == 0. ) return;

	data[0] = (point->x-plotRect.left) * scaleX + xRange[0];
	data[1] = (point->y-plotRect.bottom) * scaleY + yRange[0];
}

void	CChart::LPToNearest( CDC *pDC, LPPOINT point, double *data )
{
	LPToData( pDC, point, data );
}

void	CChart::NearestToLP( CDC *pDC, double *data, LPPOINT point)
{
	DataToLP( pDC, data, point );
}

// Delete a data set given by dataRef. This must
// set up the data arrays correctly without the
// deleted data set.
BOOL	CChart::DeleteData( int dataRef )
{
	int			i, j;

	// Find data ref and delete, shuffling as need be.
	for( i = 0; i < m_DataSetCount; i++ )
	{
		if( m_DataID[i] == dataRef )
		{
			// Delete this data set
			(m_DataSets[i])->DeleteData();

			// Free the object
			delete m_DataSets[i];

			if( i < m_DataSetCount - 1 )
			{
				// rearrange
				for( j = i; j < m_DataSetCount-1; j++ )
				{
					m_DataSets[j] = m_DataSets[j+1];
					m_DataID[j] = m_DataID[j+1];
					m_DataTitle[j] = m_DataTitle[j+1];
				}

				m_DataSets[j] = NULL;
				m_DataID[j] = -1;
				m_DataTitle[j] = "";

			}

			m_DataSetCount--;
			return TRUE;
		}
	}

	return FALSE;
}

// Delete every data set
void	CChart::DeleteAllData( void )
{
	BOOL		keepGoing = TRUE;

	while( keepGoing == TRUE && m_DataSetCount > 0 )
	{
		keepGoing = DeleteData( m_DataID[0] );
	}
}

// Based on a dataID, get an index to the arrays
int		CChart::GetIndex( int dataID )
{
	int			i;

	for( i = 0; i < m_DataSetCount; i++ )
	{
		if( m_DataID[i] == dataID )
		{
			return i;
		}
	}

	return -1;
}

// Call this function when you have modified the data and
// perhaps internal structures need to be updated
BOOL	CChart::SetDataModified( int dataID )
{
	// Nothing to do in default
	return TRUE;
}

// Set the title of a data set
void	CChart::SetDataTitle( int dataID, CString title )
{
	int			index = GetIndex( dataID );

	if( index < 0 ) return;

	m_DataTitle[index] = title;
}

// Shortcut function to set the x range (setX = TRUE) or
// yRange (setX = FALSE) of a chart. Sets the left or
// bottom axis by default, but will use top or right
// if default axes do not exist.
void	CChart::SetRange( BOOL setX, double low, double high )
{
	int				i;
	CAxis			*axis;
	int				bottom = -1, left = -1;

	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = GetAxis( i );
		if( axis == NULL ) continue;

		switch( axis->GetLocation() )
		{
		case kLocationTop:
			if( bottom < 0 ) bottom = i;
			break;
		case kLocationBottom: bottom = i; break;
		case kLocationLeft: left = i; break;
		case kLocationRight:
			if( left < 0 ) left = i;
			break;
		default:
			break;
		}
	}

	// Now check these axes
	if( setX )
	{
		if( bottom >= 0 )
		{
			axis = GetAxis( bottom );
			axis->SetRange( TRUE, low, high );
		}
	}
	else
	{
		if( left >= 0 )
		{
			axis = GetAxis( left );
			axis->SetRange( TRUE, low, high );
		}
	}
}

// Shortcut to set xRange of plot
void	CChart::SetXRange( double low, double high )
{
	SetRange( TRUE, low, high );
}

// Shortcut to set yRange of plot
void	CChart::SetYRange( double low, double high )
{
	SetRange( FALSE, low, high );
}

void	CChart::GetRange( double *xRange, double *yRange )
{
	int				i;
	CAxis			*axis;
	int				bottom = -1, left = -1;

	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = GetAxis( i );
		if( axis == NULL ) continue;

		switch( axis->GetLocation() )
		{
		case kLocationTop:
			if( bottom < 0 ) bottom = i;
			break;
		case kLocationBottom: bottom = i; break;
		case kLocationLeft: left = i; break;
		case kLocationRight:
			if( left < 0 ) left = i;
			break;
		default:
			break;
		}
	}

	// Now check these axes
	if( bottom >= 0 )
	{
		axis = GetAxis( bottom );
		axis->GetRange( xRange );
	}
	else
	{
		// no axis
		xRange[0] = 0; xRange[1] = 10;
	}

	if( left >= 0 )
	{
		axis = GetAxis( left );
		axis->GetRange( yRange );
	}
	else
	{
		// no axis
		yRange[0] = 0; yRange[1] = 10;
	}
}

// Shortcut to get xRange of plot
void	CChart::GetXRange( double *range )
{
	double			yRange[2];
	GetRange( range, yRange );
}

// Shortcut to get yRange of plot
void	CChart::GetYRange( double *range )
{
	double			xRange[2];
	GetRange( xRange, range );
}

// haveXRange and haveYRange are set to whether the
// x and y axes have auto-ranging. This goes based
// on the left and bottom axes by default, but
// will use top/right if defaults do not exist.
void	CChart::IsAutoRange( BOOL &haveXRange, BOOL &haveYRange )
{
	int			i;
	CAxis		*axis;
	BOOL		haveBottom = FALSE, haveLeft = FALSE;

	haveXRange = TRUE, haveYRange = TRUE;

	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = GetAxis(i);
		if( axis == NULL ) continue;

		switch( axis->GetLocation() )
		{
		case kLocationTop:
			if( haveBottom == TRUE || axis->IsRangeSet() == FALSE )
				break;
			// fall through
		case kLocationBottom:
			if( axis->IsRangeSet() )
			{
				haveXRange = FALSE;
			}
			haveBottom = TRUE;
			break;
		case kLocationRight:
			if( haveLeft == TRUE || axis->IsRangeSet() == FALSE )
				break;
			// fall through
		case kLocationLeft:
			if( axis->IsRangeSet() )
			{
				haveYRange = FALSE;
			}
			haveLeft = TRUE;
			break;
		default: break;
		}
	}
}

// Returns whether the x axis is auto ranged
BOOL	CChart::IsXAutoRange( void )
{
	BOOL		xAuto, yAuto;

	IsAutoRange( xAuto, yAuto );

	return xAuto;
}

// Returns whethre the y axis is auto ranged 
BOOL	CChart::IsYAutoRange( void )
{
	BOOL		xAuto, yAuto;

	IsAutoRange( xAuto, yAuto );

	return yAuto;
}

// Sets an axis to be auto ranging. X Axis if
// setX is true, y Axis otherwise. Sets the
// left/bottom axis by default but will use
// right/top if the defaults do not exist.
void	CChart::SetAutoRange( BOOL setX )
{
	int				i;
	CAxis			*axis;
	int				bottom = -1, left = -1;

	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = GetAxis( i );
		if( axis == NULL ) continue;

		switch( axis->GetLocation() )
		{
		case kLocationTop:
			if( bottom < 0 ) bottom = i;
			break;
		case kLocationBottom: bottom = i; break;
		case kLocationLeft: left = i; break;
		case kLocationRight:
			if( left < 0 ) left = i;
			break;
		default:
			break;
		}
	}

	// Now check these axes
	if( setX )
	{
		if( bottom >= 0 )
		{
			axis = GetAxis( bottom );
			axis->m_RangeSet = FALSE;
		}
	}
	else
	{
		if( left >= 0 )
		{
			axis = GetAxis( left );
			axis->m_RangeSet = FALSE;
		}
	}

}

// Shortcut to set the x axis to auto range
void	CChart::SetXAutoRange( void )
{
	SetAutoRange( TRUE );
}

// Shortcut to set the y axis to auto range
void	CChart::SetYAutoRange( void )
{
	SetAutoRange( FALSE );
}

// Shows the axis settings dialog box
BOOL	CChart::AxisSettings( int whichAxis )
{
	CAxisSettings		dialog( NULL, this );

	dialog.m_CurAxis = whichAxis;

	if( dialog.DoModal() == IDOK ) return TRUE; else return FALSE;
}

// Shows the axis settings dialog box for the first axis
BOOL	CChart::AxisSettings( void )
{
	return AxisSettings(0);
}

// Parent function, meant to be overridden.
BOOL	CChart::PlotSettings( void )
{
	return FALSE;
}

// Parent function, meant to be overridden.
BOOL	CChart::DataSettings( int whichDataSet )
{
	return FALSE;
}

// Parent function, meant to be overridden.
BOOL	CChart::DataSettings( void )
{
	return FALSE;
}

// This is a control class for the color control
// that is used in the settings dialog boxes.
WNDPROC CColorControl::lpfnSuperWnd = NULL;

BOOL CChart::RegisterControlClass()
{
	WNDCLASS wcls;

	// check to see if class already registered
	static const TCHAR szClass[] = _T("colorcontrol");
	if (::GetClassInfo(AfxGetInstanceHandle(), szClass, &wcls))
	{
		// name already registered - ok if it was us
		return (wcls.lpfnWndProc == (WNDPROC)CColorControl::WndProcHook);
	}

	// Use standard "button" control as a template.
	VERIFY(::GetClassInfo(NULL, _T("button"), &wcls));
	CColorControl::lpfnSuperWnd = wcls.lpfnWndProc;

	// set new values
	wcls.lpfnWndProc = CColorControl::WndProcHook;
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.lpszClassName = szClass;

	return (RegisterClass(&wcls) != 0);
}

// Parent function, meant to be overridden.
int		CChart::ShowPopupMenu( CWnd *window, UINT nFlags, CPoint point )
{
	return kPopupNoAction;
}

// Parent function, meant to be overridden.
int		CChart::ShowPopupMenu( CMenu *passedMenu, CWnd *window, UINT nFlags, CPoint point )
{
	return kPopupNoAction;
}

// Returns true if successful
// Writes all the current data sets to a text file.
// Only works for one and two dimensional data
BOOL	CChart::WriteTextFile( CString pathName )
{

	CFile				file;
	unsigned long		i,j;
	char				line[50];
	int					whichDataSet, *dims;
	double				*data;
	CChartData			*dataSet;

	if( GetDataSetCount() <= 0 ) return FALSE;

	if( !file.Open( pathName, CFile::modeWrite | CFile::modeCreate ) )
	{
		return FALSE;
	}

	for( whichDataSet = 0; whichDataSet < GetDataSetCount(); whichDataSet++ )
	{
		dataSet = GetAbsoluteDataSet(whichDataSet);
		dims = dataSet->GetDimensions();
		data = dataSet->GetData();

		if( dataSet == NULL ) continue;

		// Write the data!
		if( dataSet->GetNDimensions() == 1 )
		{
			for( i = 0; i < (unsigned int)dataSet->GetDimensions()[0]; i++ )
			{
				sprintf( line, "%g\r\n", data[i] );
				file.Write( line, strlen( line ) );
			}
		} 
		else if( dataSet->GetNDimensions() == 2 )
		{
			for( i = 0; i < (unsigned int)dims[0]; i++ )
			{
				for( j = 0; j < (unsigned int)dims[1]; j++ )
				{
					sprintf( line, "%g\r\n", data[i*dims[0]+j] );
					file.Write( line, strlen( line ) );
				}
			}
		}
		else
		{
			CString		string;

			string.Format( L"Cannot save files of %d dimensions.", dataSet->GetNDimensions() );
			AfxMessageBox( string );
		}

		if( whichDataSet != GetDataSetCount() - 1 )
		{
			strcpy( line, "\r\n" );
			file.Write( line, strlen( line ) );
		}
	}

	file.Close();

	return TRUE;
}
