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
#include "CPlotUtility.h"
#include "PlotSettingsDialog.h"
#include "DataSettingsDialog.h"
#include "Chart.h"

/////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL( CXYChart, CObject, 3 )
CXYChart::CXYChart()
{
	int			i;

	for( i = 0; i < kMaxDataSets; i++ )
	{
		SetDefaults( i );
	}
}

// Set the defaults for all the variables in an XY Plot
void	CXYChart::SetDefaults( int index )
{
	int			i = index;

	if( i < 0 || i > kMaxDataSets ) return;

	#ifdef DEFAULT_WHITE_BACKGROUND
		m_PlotColor[i] = RGB(0,0,0);
#else
		m_PlotColor[i] = RGB(255,255,255);
#endif
		m_DataRangesSet[i] = FALSE;

		m_MarkerType[i] = kXYMarkerNone;
		m_MarkerSize[i] = 10;
		m_MarkerFill[i] = FALSE;
		m_MarkerFrequency[i] = -10;
		m_ChartType[i] = kXYChartConnect;

		m_DataLineSize[i] = 1;

		m_DataLineStyle[i] = PS_SOLID;

}

CXYChart::~CXYChart()
{
}


// Copy the settings from one chart to another
void	CXYChart::CopyChartData( CXYChart *chart )
{
	int			i;

	m_DataSetCount = chart->m_DataSetCount;

	for( i = 0; i < m_DataSetCount; i++ )
	{
		m_PlotColor[i] = chart->m_PlotColor[i];
		m_DataRangesSet[i] = chart->m_DataRangesSet[i];

		m_MarkerType[i] = chart->m_MarkerType[i];
		m_MarkerSize[i] = chart->m_MarkerSize[i];
		m_MarkerFill[i] = chart->m_MarkerFill[i];
		m_MarkerFrequency[i] = chart->m_MarkerFrequency[i];
		m_ChartType[i] = chart->m_ChartType[i];

		m_DataTitle[i] = chart->m_DataTitle[i];

		m_DataLineSize[i] = chart->m_DataLineSize[i];
		m_DataLineStyle[i] = chart->m_DataLineStyle[i];

	}
}

// Deletes a data set, and moves all the settings
// around in their arrays
BOOL	CXYChart::DeleteData( int dataRef )
{
	int			index = GetIndex( dataRef );
	int			i;

	if( index < 0 ) return FALSE;

	for( i = index; i < m_DataSetCount-1; i++ )
	{
		m_PlotColor[i] = m_PlotColor[i+1];
		m_MarkerSize[i] = m_MarkerSize[i+1];
		m_ChartType[i] = m_ChartType[i+1];
		m_MarkerType[i] = m_MarkerType[i+1];
		m_MarkerFrequency[i] = m_MarkerFrequency[i+1];
		m_DataLineSize[i] = m_DataLineSize[i+1];
		m_DataLineStyle[i] = m_DataLineStyle[i+1];

		m_XDataMin[i] = m_XDataMin[i+1];
		m_XDataMax[i] = m_XDataMax[i+1];
		m_YDataMin[i] = m_YDataMin[i+1];
		m_YDataMax[i] = m_YDataMax[i+1];

		m_DataRangesSet[i] = m_DataRangesSet[i+1];

		m_MarkerFill[i] = m_MarkerFill[i+1];
	}

	return CChart::DeleteData( dataRef );
}

// Deletes every data set
void	CXYChart::DeleteAllData( void )
{
	BOOL		keepGoing = TRUE;

	while( keepGoing == TRUE && m_DataSetCount > 0 )
	{
		keepGoing = DeleteData( m_DataID[0] );
	}
}

void CXYChart::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar.Write( m_PlotColor, kMaxDataSets * sizeof( COLORREF ) );
		ar.Write( m_MarkerSize, kMaxDataSets * sizeof( int ) );
		ar.Write( m_MarkerType, kMaxDataSets * sizeof( int ) );
		ar.Write( m_MarkerFrequency, kMaxDataSets * sizeof( int ) );
		ar.Write( m_ChartType, kMaxDataSets * sizeof( int ) );
		ar.Write( m_XDataMin, kMaxDataSets * sizeof( double ) );
		ar.Write( m_XDataMax, kMaxDataSets * sizeof( double ) );
		ar.Write( m_YDataMin, kMaxDataSets * sizeof( double ) );
		ar.Write( m_YDataMax, kMaxDataSets * sizeof( double ) );
		ar.Write( m_DataRangesSet, kMaxDataSets * sizeof( BOOL ) );
		ar.Write( m_DataLineSize, kMaxDataSets * sizeof( int ) );
		ar.Write( m_MarkerFill, kMaxDataSets * sizeof( BOOL ) );
		ar.Write( m_DataLineStyle, kMaxDataSets * sizeof( int ) );
	}
	else
	{
		ar.Read( m_PlotColor, kMaxDataSets * sizeof( COLORREF ) );
		ar.Read( m_MarkerSize, kMaxDataSets * sizeof( int ) );
		ar.Read( m_MarkerType, kMaxDataSets * sizeof( int ) );
		ar.Read( m_MarkerFrequency, kMaxDataSets * sizeof( int ) );
		ar.Read( m_ChartType, kMaxDataSets * sizeof( int ) );
		ar.Read( m_XDataMin, kMaxDataSets * sizeof( double ) );
		ar.Read( m_XDataMax, kMaxDataSets * sizeof( double ) );
		ar.Read( m_YDataMin, kMaxDataSets * sizeof( double ) );
		ar.Read( m_YDataMax, kMaxDataSets * sizeof( double ) );
		ar.Read( m_DataRangesSet, kMaxDataSets * sizeof( BOOL ) );
		ar.Read( m_DataLineSize, kMaxDataSets * sizeof( int ) );
		ar.Read( m_MarkerFill, kMaxDataSets * sizeof( BOOL ) );
		ar.Read( m_DataLineStyle, kMaxDataSets * sizeof( int ) );
	}
}

void	CXYChart::OnDraw( CDC *pDC, CRect destRect )
{
	OnDraw( pDC, destRect, destRect );
}

// Draws the chart
void	CXYChart::OnDraw( CDC *pDC, CRect destRect, CRect updateRect )
{
	CString			string;
	int				i;
	CAxis			*axis;
	CSize			axisDims;
	CSize			trim(0,0);
	CRect			plotRect, normalPlotRect, normalDestRect;
	double			xRange[] = {1e30, -1e30}, yRange[]={1e30,-1e30};

	updateRect.NormalizeRect();

	normalDestRect = destRect;

	// Check to see that we are even in the update region
	normalDestRect.NormalizeRect();
	normalDestRect.IntersectRect( normalDestRect, updateRect );
	if( normalDestRect.IsRectEmpty() ) return;

	// Make sure our ranges are set
	// Get ranges for plotting
	GetPlotRange( xRange, yRange );

	// Get the size of our plotting rectangle
	plotRect = GetPlotRect( pDC, destRect );

	// Draw the basic structures for the plot
	DrawPlotBasics( pDC, destRect, plotRect );

	// Draw the title
	DrawPlotTitle( pDC, destRect, plotRect );

	normalPlotRect = plotRect;
	normalPlotRect.NormalizeRect();
	updateRect.IntersectRect( updateRect, normalPlotRect );

	if( updateRect.IsRectEmpty() == FALSE )
	{
		// Draw each data set
		DrawDataSet( pDC, plotRect, xRange, yRange );
	}

	// Draw the axes
	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = m_Axes[i];
		axis->OnDraw( pDC, destRect, plotRect );
	}

	CChart::OnDraw( pDC, destRect );
}

// Gets the range of the data, assuming that the data is sorted
void	CXYChart::GetRangeFromData( CChartData *dataSet, int whichDim, double *outRange )
{
	double	*data;
	int		*dims;

	if( dataSet == NULL || whichDim < 0 || whichDim > 1 ) return;


	data = dataSet->GetData();
	dims = dataSet->GetDimensions();

	outRange[0] = data[whichDim * dims[1]];
	outRange[1] = data[(whichDim+1) * dims[1] - 1];
}

// Draws the actual data set, including markers
void	CXYChart::DrawDataSet( CDC *pDC, CRect plotRect, double *xRange, double *yRange )
{
	double		*data;
	int			*dims;
	int			dimCount;
	int			i, j;
	CPoint		point, lastPoint;
	double		xScale, yScale;
	CRgn		rgn;
	int			saveDC;
	CChartData	*dataSet;
	CRect		dPlotRect = plotRect;

	// Create a region for clipping
	saveDC = pDC->SaveDC();
	if( pDC->IsPrinting() == FALSE )
	{
		dPlotRect.InflateRect( 1, -1, -1, 1 );
		pDC->LPtoDP( (LPPOINT) &dPlotRect, 2); 
		rgn.CreateRectRgn( dPlotRect.left, dPlotRect.top, dPlotRect.right, dPlotRect.bottom );
		pDC->SelectClipRgn( &rgn );
	}

	for( i = 0; i < m_DataSetCount; i++ )
	{
		CPen		pen, *pOldPen;
		CBrush		brush;
		dataSet = GetAbsoluteDataSet( i );
		data = dataSet->GetData();

		dims = dataSet->GetDimensions();
		dimCount = dataSet->GetNDimensions();

		if( data == NULL || dimCount != 2 || dims[0] != 2 ) continue;

		pen.CreatePen( m_DataLineStyle[i], m_DataLineSize[i], m_PlotColor[i] );

		pOldPen = pDC->SelectObject( &pen );

		// We now have a rect and an xRange and a yRange, just some
		// simple transformations should get us our plot
		xScale = ((double)plotRect.Width())/(xRange[1] - xRange[0]);
		yScale = ((double)plotRect.Height())/(yRange[1] - yRange[0]);

		// Get our first point
		point.x = (long)(xScale * (data[0]-xRange[0]) + plotRect.left);
		point.y = (long)(plotRect.bottom - yScale * (data[dims[1]]-yRange[0]));

		// Make sure we are in the bounds of our chart
		if( point.x > plotRect.right ) point.x = plotRect.right;
		if( point.x < plotRect.left ) point.x = plotRect.left;

		if( point.y > plotRect.top ) point.y = plotRect.top;
		if( point.y < plotRect.bottom ) point.y = plotRect.bottom;

		switch( m_ChartType[i] )
		{
		case kXYChartPlain:
			pDC->MoveTo( point );
			DrawMarker( pDC, point, i, 0 );
			for( j = 1; j < dims[1]; j++ )
			{
				point.x = (long)(xScale * (data[j]-xRange[0]) + plotRect.left);
				point.y = (long)(plotRect.bottom - yScale * (data[dims[1]+j]-yRange[0]));
				DrawMarker( pDC, point, i, j );
			}
			break;
		case kXYChartConnect:
			pDC->MoveTo( point );

			DrawMarker( pDC, point, i, 0 );
			
			lastPoint = point;

			for( j = 1; j < dims[1]; j++ )
			{
				point.x = (long)(xScale * (data[j]-xRange[0]) + plotRect.left);
				point.y = (long)(plotRect.bottom - yScale * (data[dims[1]+j]-yRange[0]));
				if( lastPoint != point )
					pDC->LineTo( point );
				DrawMarker( pDC, point, i, j );
				lastPoint = point;
			}
			break;
		case kXYChartHistogram:
			{
				CPoint		nextPoint;

				lastPoint = point;
				lastPoint.x = plotRect.left;
				pDC->MoveTo( lastPoint );
				for( j = 0; j < dims[1]; j++ )
				{
					point.x = (long)(xScale * (data[j]-xRange[0]) + plotRect.left);
					point.y = (long)(plotRect.bottom - yScale * (data[dims[1]+j]-yRange[0]));
					if( j < dims[1] - 1 )
					{
						nextPoint = lastPoint = point;
						nextPoint.x = (long)(xScale * (data[j+1]-xRange[0]) + plotRect.left);
						nextPoint.y = (long)(plotRect.bottom - yScale * (data[dims[1]+j+1]-yRange[0]));
						lastPoint.x = point.x + (nextPoint.x - point.x) / 2;
						pDC->LineTo( lastPoint );
						lastPoint.y = nextPoint.y;
						pDC->LineTo( lastPoint );
					}
					else
					{
						lastPoint = point;
						lastPoint.x = plotRect.right;
						pDC->LineTo( lastPoint );
					}
					DrawMarker( pDC, point, i, j );
				}


				}
			break;
		}
		
		pDC->SelectObject( pOldPen );
	}

	pDC->RestoreDC( saveDC );
}

// Draws the background, major and minor grids, and the frame
void	CXYChart::DrawPlotBasics( CDC *pDC, CRect destRect, CRect plotRect )
{
	// Erase the background using parent function
	EraseBackground( pDC, destRect );

	// Draw the grids using the parent function
	DrawMajorMinorGrids( pDC, plotRect );

	// Draw the frame using the parent function
	DrawFrame( pDC, plotRect );
}

// Gets the range of the plot from the axes, taking
// into account auto ranging if used
void	CXYChart::GetPlotRange( double *xRange, double *yRange )
{
	BOOL		haveXRange = FALSE, haveYRange = FALSE;
	BOOL		haveBottom = FALSE, haveLeft = FALSE;
	CAxis		*axis;
	int			i;
	double		optRange[2];
	int			optNTicks, optMinorTicks;
	CChartData	*dataSet;

	xRange[0] = 1e30;
	xRange[1] = -1e30;
	yRange[0] = 1e30;
	yRange[1] = -1e30;

	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = m_Axes[i];
		switch( axis->GetLocation() )
		{
		case kLocationTop:
			if( haveBottom == TRUE || axis->IsRangeSet() == FALSE )
				break;
			// fall through
		case kLocationBottom:
			if( axis->IsRangeSet() )
			{
				axis->GetRange( xRange );
				haveXRange = TRUE;
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
				axis->GetRange( yRange );
				haveYRange = TRUE;
			}
			haveLeft = TRUE;
			break;
		default: break;
		}
	}
		// Autoranging assumes the data is sorted
	if( haveXRange == FALSE || haveYRange == FALSE )
	{
		// Get the x range from the data
		for( i = 0; i < m_DataSetCount; i++ )
		{
			dataSet = GetAbsoluteDataSet( i );
			if( haveXRange == FALSE )
			{
				xRange[0] = min( xRange[0], m_XDataMin[i] );
				xRange[1] = max( xRange[1], m_XDataMax[i] );
			}
			if( haveYRange == FALSE )
			{
				yRange[0] = min( yRange[0], m_YDataMin[i] );
				yRange[1] = max( yRange[1], m_YDataMax[i] );
			}
		}
	}

		// Now that we've autoranged, set up the autoranged axses
		haveLeft = haveBottom = FALSE;
		for( i = 0; i < m_AxisCount; i++ )
		{
			axis = m_Axes[i];
			{

				switch( axis->GetLocation() )
				{
				case kLocationTop:
					if( haveBottom == TRUE ) break;
					// fall through
				case kLocationBottom:
					if( haveXRange )
						GetOptimalRangeExact( xRange, optRange, &optNTicks, &optMinorTicks );
					else
					{
						GetOptimalRange( xRange, optRange, &optNTicks, &optMinorTicks, TRUE );
						axis->SetRange( FALSE, optRange[0], optRange[1] );
					}

					memcpy( xRange, optRange, 2*sizeof( double ) );
					axis->SetTickCount( -optNTicks );
					axis->SetMinorTickCount( -optMinorTicks );
					haveBottom = TRUE;
					break;
				case kLocationRight:
					if( haveLeft == TRUE ) break;
					// fall through
				case kLocationLeft:
					if( haveYRange )
						GetOptimalRangeExact( yRange, optRange, &optNTicks, &optMinorTicks );
					else
					{
						GetOptimalRange( yRange, optRange, &optNTicks, &optMinorTicks, FALSE );
						axis->SetRange( FALSE, optRange[0], optRange[1] );
					}
					memcpy( yRange, optRange, 2*sizeof( double ) );
					axis->SetTickCount( -optNTicks );
					axis->SetMinorTickCount( -optMinorTicks );
					haveLeft = TRUE;
					break;
				default: break;
				}
			}
	}
}

// Draws a marker at the current point, if a marker should be drawn there
void	CXYChart::DrawMarker( CDC *pDC, CPoint point, int whichDataSet, int dataPointNumber )
{
	CBrush		brush, *pOldBrush;
	CRect		rect;
	CPen		pen, *oldPen;
	CPoint		startPoint, startLocation;

	if( m_MarkerType[whichDataSet] == kXYMarkerNone ) return;

	// Check to see if we want to draw this marker
	if( m_MarkerFrequency[whichDataSet] > 0 )
	{
		// positive means every nth data point
		if( dataPointNumber % m_MarkerFrequency[whichDataSet] != 0 ) return;
	}
	else if( m_MarkerFrequency[whichDataSet] < 0 )
	{
		// negative means n number of times
		int			n_elements = (m_DataSets[whichDataSet]->GetDimensions())[1];
		int			remainder = n_elements / (int)(abs(m_MarkerFrequency[whichDataSet]));

		if( ((dataPointNumber+remainder/2+1) % (n_elements/abs(m_MarkerFrequency[whichDataSet])) != 0 ) )
			return;
	}
	else
		return; // == 0 means no marker

	startLocation = pDC->GetCurrentPosition();

	pen.CreatePen( PS_SOLID, 1, m_PlotColor[whichDataSet] );

	oldPen = pDC->SelectObject( &pen );

	rect.SetRect( point.x - m_MarkerSize[whichDataSet], point.y - m_MarkerSize[whichDataSet],
		point.x + m_MarkerSize[whichDataSet], point.y + m_MarkerSize[whichDataSet] );

	brush.CreateSolidBrush( m_PlotColor[whichDataSet] );

	pOldBrush = pDC->SelectObject( &brush );

	pDC->BeginPath();

	switch( m_MarkerType[whichDataSet] )
	{
	case kXYMarkerNone: return; break;
	case kXYMarkerCircle:
		// the size is the radius
		pDC->Arc( (LPCRECT)rect, point, point );
		break;
	case kXYMarkerSquare:
		MyFrameRect( pDC, rect );
		break;
	case kXYMarkerTriangle:
		// Need to update this to actually center correctly using
		// the triangle's geometry
		startPoint.x = point.x;
		startPoint.y = point.y - m_MarkerSize[whichDataSet];
		pDC->MoveTo( startPoint );
		startPoint.Offset( m_MarkerSize[whichDataSet], 2 * m_MarkerSize[whichDataSet] );
		pDC->LineTo( startPoint );
		startPoint.Offset( -2*m_MarkerSize[whichDataSet], 0 );
		pDC->LineTo( startPoint );
		startPoint.Offset( m_MarkerSize[whichDataSet], -2 * m_MarkerSize[whichDataSet] );
		pDC->LineTo( startPoint );
		pDC->MoveTo( point );
		break;
	case kXYMarkerX:
		startPoint.x = point.x - m_MarkerSize[whichDataSet];
		startPoint.y = point.y - m_MarkerSize[whichDataSet];
		pDC->MoveTo( startPoint );
		startPoint.Offset( 2*m_MarkerSize[whichDataSet], 2*m_MarkerSize[whichDataSet] );
		pDC->LineTo( startPoint );
		startPoint.Offset( -2*m_MarkerSize[whichDataSet], 0 );
		pDC->MoveTo( startPoint );
		startPoint.Offset( 2*m_MarkerSize[whichDataSet], -2*m_MarkerSize[whichDataSet] );
		pDC->LineTo( startPoint );

		// Move back to beginning
		pDC->MoveTo( point );
		break;

	}

	pDC->EndPath();

	if( m_MarkerFill[whichDataSet] )
		pDC->StrokeAndFillPath();
	else
		pDC->StrokePath();

	pDC->MoveTo( startLocation );
	pDC->SelectObject( oldPen );
	pDC->SelectObject( pOldBrush );
}

// Sets up the data range variable for the
// specified data set
void	CXYChart::GetDataRange( int dataID )
{
	int			i,j;
	double		minX, minY, maxX, maxY;
	CChartData	*dataSet;
	int			*dims;
	double		*data;

	i = GetIndex( dataID );
	if( i < 0 ) return;

	if( m_DataRangesSet[i] == FALSE )
	{
		minX = 1e30; minY = 1e30;
		maxX = -1e30; maxY = -1e30;
		m_DataRangesSet[i] = TRUE;
		
		dataSet = GetAbsoluteDataSet( i );
		dims = dataSet->GetDimensions();
		data = dataSet->GetData();
		
		// Just find min and max of x
		for( j = 0; j < dims[1]; j++)
		{
			if( data[j] < minX ) minX = data[j];
			if( data[j] > maxX ) maxX = data[j];
		}
		
		for( j = dims[1]; j < 2*dims[1]; j++)
		{
			if( data[j] < minY ) minY = data[j];
			if( data[j] > maxY ) maxY = data[j];
		}
		
		m_XDataMin[i] = minX;
		m_XDataMax[i] = maxX;
		m_YDataMin[i] = minY;
		m_YDataMax[i] = maxY;
	}
}

// Gets all data ranges, and sets up the data range
// variable for them
void	CXYChart::GetDataRanges( void )
{
	int			i;

	for( i = 0; i < m_DataSetCount; i++ )
	{
		GetDataRange( m_DataID[i] );
	}
}

// Gets the rect that encloses the actual plot data,
// not the entire chart including axes
CRect	CXYChart::GetPlotRect( CDC *pDC, CRect clientRect )
{
	CRect	plotRect = clientRect;

	// Ask the axes what size they want
	plotRect = AdjustPlotSizeForAxes( pDC, plotRect );

	// What size do we need for the title
	plotRect = AdjustPlotSizeForTitle( pDC, plotRect );

	// Add spacing
	plotRect.InflateRect( -20, 20 );

	return plotRect;
}


void	CXYChart::LPToNearest( CDC *pDC, LPPOINT point, double *data, int dataID )
{
	CChartData			*dataSet;
	double				*dataVals, minDist = 1e30;
	int					nDims, *dims, i, minIndex = -1;

	LPToData( pDC, point, data );

	if( dataID >= 0 )
	{
		dataSet = GetDataSet( dataID );
		if( dataSet == NULL ) return;
	}
	else
	{
		dataSet = GetAbsoluteDataSet( 0 );
		if( dataSet == NULL ) return;
	}

	dims = dataSet->GetDimensions();
	nDims = dataSet->GetNDimensions();
	dataVals = dataSet->GetData();

	if( nDims != 2 || dims[0] != 2 ) return;

	for( i = 0; i < dims[1]; i++ )
	{
		if( fabs( dataVals[i] - data[0] ) < minDist )
		{
			minDist = fabs(dataVals[i] - data[0]);
			minIndex = i;
		}
	}
	
	if( minIndex >= 0 )
	{
		data[0] = dataVals[minIndex];
		data[1] = dataVals[minIndex + dims[1]];
	}
}

void	CXYChart::NearestToLP( CDC *pDC, double *data, LPPOINT point, int dataID )
{
	DataToLP( pDC, data, point );
}


// Set the color the data set
void	CXYChart::SetDataColor( int dataID, COLORREF color )
{
	int			index = GetIndex( dataID );

	if( index < 0 ) return;

	m_PlotColor[index] = color;
}

// Set the size of the marker in a data set
void	CXYChart::SetMarkerSize( int dataID, int size )
{
	int			index = GetIndex( dataID );

	if( index < 0 ) return;

	m_MarkerSize[index] = size;
}

// Set the type of marker in a data set
void	CXYChart::SetMarkerType( int dataID, int type )
{
	int			index = GetIndex( dataID );

	if( index < 0 ) return;

	m_MarkerType[index] = type;
}

void	CXYChart::SetMarkerFillState( int dataID, BOOL how )
{
	int			index = GetIndex( dataID );

	if( index < 0 ) return;

	m_MarkerFill[index] = how;
}

// Set the type of chart for a data set
void	CXYChart::SetChartType( int dataID, int type )
{
	int			index = GetIndex( dataID );

	if( index < 0 ) return;

	m_ChartType[index] = type;
}

// Set the frequency of markers for a data set
void	CXYChart::SetMarkerFrequency( int dataID, int freq )
{
	int			index = GetIndex( dataID );

	if( index < 0 ) return;

	m_MarkerFrequency[index] = freq;
}


BOOL	CXYChart::SetDataModified( int dataID )
{
	if( dataID < 0 )
	{
		GetDataRanges();
		// all have been modified
	}
	else
	{
		GetDataRange( dataID );
		// one has been modified
	}

	return TRUE;
}

// Show the plot settings dialog box
BOOL	CXYChart::PlotSettings( void )
{
	CPlotSettingsDialog		dialog( NULL, this );

	if( dialog.DoModal() == IDOK ) return TRUE; else return FALSE;
}

// Show the data settings dialog box
BOOL	CXYChart::DataSettings( void )
{
	return DataSettings( 0 );
}

// Show a specific data settings dialog box
BOOL	CXYChart::DataSettings( int whichDataSet )
{
	CDataSettingsDialog		dialog( NULL, this );

	if( whichDataSet < 0 || whichDataSet >= GetDataSetCount() ) return FALSE;

	dialog.m_CurDataSet = whichDataSet;

	if( dialog.DoModal() == IDOK ) return TRUE; else return FALSE;
}

// Some constants for the popup menus
enum
{
		kMenuMajorHorizGrids=1,
		kMenuMajorVertGrids,
		kMenuMinorHorizGrids,
		kMenuMinorVertGrids,

		kMenuTitleFont = 10,

		kMenuPlotSettings = 999
};

#define			nLineSizes			10
#define			kMenuChartType		0
#define			kMenuLineSize		(1+kChartTypeCount)
#define			kMenuLineStyle		(1+kMenuLineSize+nLineSizes)
#define			kMenuMarker			(1+kMenuLineStyle+5)
#define			kMenuMarkerFill		(kMarkerTypeCount)
#define			kMenuMarkerSize		(kMenuMarkerFill+1)
#define			nMarkerSizes		8

#define			kMenuMarkerFrequency	(kMenuMarkerSize+nMarkerSizes)


// Show the contextual menu (shortcut menu) for an XY Chart. If passedMenu
// is not NULL, the menu is appended to passedMenu, otherwise a new
// menu is created
int		CXYChart::ShowPopupMenu( CMenu *passedMenu, CWnd *window, UINT nFlags, CPoint point )
{
	CMenu			menu, *menuPtr;
	CMenu			axisMenus[kMaxAxes];
	int				result, i;
	UINT			flag;

	if( window == NULL ) return FALSE;
	
	if( passedMenu == NULL || (!::IsMenu(passedMenu->m_hMenu)) )
	{
		menuPtr = &menu;
		menuPtr->CreatePopupMenu();
	}
	else
		menuPtr = passedMenu;

	// Grids sub-menu
	{
	CMenu		gridSubMenu;
	gridSubMenu.CreatePopupMenu();
	flag = GetPopupCheckedFlag(this->m_UseMajorHorizontalGrids);
	gridSubMenu.AppendMenu( MF_STRING | flag, kMenuMajorHorizGrids, L"Major Horizontal Grids" );
	flag = GetPopupCheckedFlag(this->m_UseMajorVerticalGrids);
	gridSubMenu.AppendMenu( MF_STRING | flag, kMenuMajorVertGrids, L"Major Vertical Grids" );
	flag = GetPopupCheckedFlag(this->m_UseMinorHorizontalGrids);
	gridSubMenu.AppendMenu( MF_STRING | flag, kMenuMinorHorizGrids, L"Minor Horizontal Grids" );
	flag = GetPopupCheckedFlag(this->m_UseMinorVerticalGrids);
	gridSubMenu.AppendMenu( MF_STRING | flag, kMenuMinorVertGrids, L"Minor Vertical Grids" );
	menuPtr->AppendMenu( MF_POPUP | MF_STRING, (UINT) gridSubMenu.GetSafeHmenu(), L"Grids" );
	}

	// Separator
	menuPtr->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	// Check for a title, and put in title size if needed
	if( this->m_Title != L"" )
	{
		AddFontSizePopup( menuPtr, L"Title size", this->m_TitleFont.lfHeight, kMenuTitleFont );

		// Separator
		menuPtr->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );
	}

	// Axes
	for( i = 0; i < GetAxisCount(); i++ )
	{
		GetAxis(i)->AddAxisPopupMenus( menuPtr, (kMenuAxisRange*i)+kMenuAxisMinimum );
	}

	// Separator
	if( GetAxisCount() > 0 )
		menuPtr->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	// Data sets
	for( i = 0; i < GetDataSetCount(); i++ )
	{
		AddDataPopupMenus( i, menuPtr, (kMenuDataRange*i) + kMenuDataMinimum );
	}

	// Separator
	if( GetDataSetCount() > 0 )
		menuPtr->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	// Settings...
	menuPtr->AppendMenu( MF_STRING, kMenuPlotSettings, L"Settings..." );

	result = menuPtr->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x,
		point.y, window );

	return InterpretPopupMenuItem( result );
}

// Show the contextual menu (shortcut menu) for an XY Chart
int		CXYChart::ShowPopupMenu( CWnd *window, UINT nFlags, CPoint point )
{
	return ShowPopupMenu( NULL, window, nFlags, point );
}

// Take the result of the popup menu selection, figure out
// what the user wanted, and take the appropriate action
int		CXYChart::InterpretPopupMenuItem( int result )
{
	int			whichAxis, returnVal;

	if( result >= kMinimumPopupValue ) return result;

	if( result <= 0 ) return kPopupError;

	if( result <= kMenuMinorVertGrids )
	{
		switch( result )
		{
		case kMenuMajorHorizGrids: m_UseMajorHorizontalGrids = !m_UseMajorHorizontalGrids; break;
		case kMenuMajorVertGrids: m_UseMajorVerticalGrids = !m_UseMajorVerticalGrids; break;
		case kMenuMinorHorizGrids: m_UseMinorHorizontalGrids = !m_UseMinorHorizontalGrids; break;
		case kMenuMinorVertGrids: m_UseMinorVerticalGrids = !m_UseMinorVerticalGrids; break;
		default: break;
		}

		return kPopupUpdate;
	}

	if( result >= kMenuTitleFont && result < kMenuTitleFont + nFontSizes )
	{
		m_TitleFont.lfHeight = fontSizes[result-kMenuTitleFont];
		return kPopupUpdate;
	}

	if( result == kMenuPlotSettings )
	{
		if( PlotSettings() ) return kPopupUpdate; else return kPopupNoAction;
	}

	if( result >= kMenuAxisMinimum && result < kMenuAxisMinimum + (kMaxAxes*kMenuAxisRange) )
	{
		// These must be axis sets
		whichAxis = (result-kMenuAxisMinimum) / kMenuAxisRange;

		if( whichAxis < 0 || whichAxis > GetAxisCount() ) return kPopupError;

		if( result % kMenuAxisRange == kMenuAxisRange-1 ) // have to do this one manually
			if( AxisSettings(whichAxis) ) return kPopupUpdate; else return kPopupNoAction;

		if( (returnVal = GetAxis(whichAxis)->InterpretAxisPopupMenuItem( (result-kMenuAxisMinimum) % kMenuAxisRange ) ) == kPopupError )
			return result;
		else
			return returnVal;
	}

	if( result >= kMenuDataMinimum && result < kMenuDataMinimum + kMaxDataSets*kMenuDataRange )
	{
		int			whichDataSet;

		whichDataSet = (result-kMenuDataMinimum) / kMenuDataRange;

		if( whichDataSet < 0 || whichDataSet > GetDataSetCount() ) return kPopupError;

		if( (result-kMenuDataMinimum) % kMenuDataRange == kMenuDataRange-1 ) // have to do this one manually
			if( DataSettings(whichDataSet) ) return kPopupUpdate; else return kPopupNoAction;

		if( (returnVal = InterpretDataSetPopupMenuItem( whichDataSet, (result-kMenuDataMinimum) % kMenuDataRange ) ) == kPopupError )
			return result;
		else
			return returnVal;
	}

	return kPopupError;
}

// Add the popup menu for a specific data set
void	CXYChart::AddDataPopupMenus( int whichDataSet, CMenu *menu, int startVal )
{
	CString			string;
	CMenu			dataMenu;
	CMenu			subMenu;
	int				dataID;

	if( whichDataSet < 0 || whichDataSet >= GetDataSetCount() ) return;

	dataID = m_DataID[whichDataSet];

	dataMenu.CreatePopupMenu();

	AddChartTypePopup( &dataMenu, L"Chart type", this->GetChartType( dataID ), startVal+kMenuChartType );

	AddLineSizePopup( &dataMenu, L"Line size", dataID, startVal + kMenuLineSize );

	AddLineStylePopup( &dataMenu, L"Line style", dataID, startVal + kMenuLineStyle );

	dataMenu.AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	AddMarkerPopup( &dataMenu, L"Marker", dataID, startVal + kMenuMarker );

	string = GetDataTitle( dataID );

	if( string == L"" )
	{
		string.Format( L"Data set %d of %d", whichDataSet + 1, GetDataSetCount() );
	}
	else
	{
		if( string.GetLength() > 15 )
		{
			string = string.Left( 15 );
			string = string + L"...";
		}
	}

	// Add it to main menu
	dataMenu.AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );
	dataMenu.AppendMenu( MF_STRING, startVal+kMenuDataRange-1, L"Settings..." );

	menu->AppendMenu( MF_POPUP, (UINT) dataMenu.GetSafeHmenu(), string );
}

// Add a popup menu for the different chart types
void	CXYChart::AddChartTypePopup( CMenu *addMenu, CString titleString, long compare, int startVal )
{
	UINT			flag;

	flag = GetPopupCheckedFlag( compare == kXYChartPlain );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYChartPlain, L"XY Scatter" );
	flag = GetPopupCheckedFlag( compare == kXYChartConnect );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYChartConnect, L"XY Line" );
	flag = GetPopupCheckedFlag( compare == kXYChartHistogram );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYChartHistogram, L"Histogram" );
}

// Add a popup menu for the different marker types and frequencies
void	CXYChart::AddMarkerPopup( CMenu *addMenu, CString titleString, int dataID, int startVal )
{
	CMenu			subMenu;
	int				i, markerShape, freq;
	CString			string;
	UINT			flag;

	subMenu.CreatePopupMenu();

	// Marker shape
	markerShape = GetMarkerType( dataID );
	flag = GetPopupCheckedFlag( markerShape == kXYMarkerNone );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYMarkerNone, L"No marker" );
	flag = GetPopupCheckedFlag( markerShape == kXYMarkerCircle );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYMarkerCircle, L"Circle marker" );
	flag = GetPopupCheckedFlag( markerShape == kXYMarkerSquare );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYMarkerSquare, L"Square marker" );
	flag = GetPopupCheckedFlag( markerShape == kXYMarkerTriangle );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYMarkerTriangle, L"Triangle marker" );
	flag = GetPopupCheckedFlag( markerShape == kXYMarkerX );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kXYMarkerX, L"X marker" );

	addMenu->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	flag = GetPopupCheckedFlag( GetMarkerFillState( dataID ) );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kMenuMarkerFill, L"Filled marker" );

	addMenu->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );
	
	// Marker size
	for( i = 0; i < nMarkerSizes; i++ )
	{
		flag = GetPopupCheckedFlag( GetMarkerSize( dataID ) == 10*(i+1) );
		string.Format( L"%d", i+1 );
		subMenu.AppendMenu( MF_STRING | flag, startVal+i+kMenuMarkerSize, string );
	}
	addMenu->AppendMenu( MF_POPUP, (UINT) subMenu.GetSafeHmenu(), L"Marker size" );

	// Marker frequency
	freq = abs(GetMarkerFrequency( dataID ));
	if( freq == 1 ) freq = 10;
	flag = GetPopupCheckedFlag( GetMarkerFrequency( dataID ) == 1 );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kMenuMarkerFrequency, L"Mark every point" );
	flag = GetPopupCheckedFlag( (GetMarkerFrequency( dataID )) > 1 );
	string.Format( L"Mark every %d points", freq );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kMenuMarkerFrequency+1, string );
	flag = GetPopupCheckedFlag( (GetMarkerFrequency( dataID )) < 0 );
	string.Format( L"Mark %d points", freq );
	addMenu->AppendMenu( MF_STRING | flag, startVal+kMenuMarkerFrequency+2, string );
}

// Add a popup for the line size of a data set
void	CXYChart::AddLineSizePopup( CMenu *addMenu, CString titleString, int dataID, int startVal )
{
	CMenu		subMenu;
	int			i;
	UINT		flag;
	CString		string;

	subMenu.CreatePopupMenu();

	// Line size
	for( i = 0; i < nLineSizes; i++ )
	{
		flag = GetPopupCheckedFlag( GetDataLineSize( dataID ) == i+1 );
		string.Format( L"%d", i+1 );
		subMenu.AppendMenu( MF_STRING | flag, startVal+i, string );
	}
	addMenu->AppendMenu( MF_POPUP, (UINT) subMenu.GetSafeHmenu(), titleString );
}

// Add a popup for the line size of a data set
void	CXYChart::AddLineStylePopup( CMenu *addMenu, CString titleString, int dataID, int startVal )
{
	CMenu		subMenu;
	UINT		flag;
	CString		string;

	subMenu.CreatePopupMenu();

	// We'll just hard code these for now
	flag = GetPopupCheckedFlag( GetDataLineStyle( dataID ) == 0 );
	subMenu.AppendMenu( MF_STRING | flag, startVal,   L"Solid ____________" );
	flag = GetPopupCheckedFlag( GetDataLineStyle( dataID ) == 1 );
	subMenu.AppendMenu( MF_STRING | flag, startVal+1, L"Dashed _ _ _ _ _ _" );
	flag = GetPopupCheckedFlag( GetDataLineStyle( dataID ) == 2 );
	subMenu.AppendMenu( MF_STRING | flag, startVal+2, L"Dotted . . . . . ." );
	flag = GetPopupCheckedFlag( GetDataLineStyle( dataID ) == 3 );
	subMenu.AppendMenu( MF_STRING | flag, startVal+3, L"Dash-Dot _ . _ . _" );
	flag = GetPopupCheckedFlag( GetDataLineStyle( dataID ) == 4 );
	subMenu.AppendMenu( MF_STRING | flag, startVal+4, L"Dash-Dot-Dot _ . ." );

	addMenu->AppendMenu( MF_POPUP, (UINT) subMenu.GetSafeHmenu(), titleString );
}

// Result here is rounded down to zero for the data set items
int		CXYChart::InterpretDataSetPopupMenuItem( int whichDataSet, int result )
{
	int			dataID = m_DataID[whichDataSet];

	// result is chart type
	if( result >= kMenuChartType && result < kMenuLineSize )
	{
		// These correlate perfectly
		SetChartType( dataID, result - kMenuChartType );

		return kPopupUpdate;
	}

	// result is line size
	if( result - kMenuLineSize >= 0 && result < kMenuLineSize + nLineSizes )
	{
		SetDataLineSize( dataID, result - kMenuLineSize + 1 );

		return kPopupUpdate;
	}

	// result is line style
	if( result - kMenuLineStyle >= 0 && result < kMenuLineStyle + 5 )
	{
		SetDataLineStyle( dataID, result - kMenuLineStyle );

		return kPopupUpdate;
	}

	// result is marker type
	if( result - kMenuMarker >= 0 && result - kMenuMarker < kMarkerTypeCount )
	{
		SetMarkerType( dataID, result - kMenuMarker );
		return kPopupUpdate;
	}

	// marker fill item
	if( result - kMenuMarker == kMenuMarkerFill )
	{
		SetMarkerFillState( dataID, !GetMarkerFillState(dataID) );
		return kPopupUpdate;
	}

	// marker size popup
	if( result - kMenuMarker >= kMenuMarkerSize && result - kMenuMarker < kMenuMarkerSize+nMarkerSizes )
	{
		SetMarkerSize( dataID, 10 * (result - (kMenuMarkerFill + kMenuMarker)) );
		return kPopupUpdate;
	}


	// marker frequency
	if( result - kMenuMarker >= kMenuMarkerFrequency && result - kMenuMarker < kMenuMarkerFrequency + 3 )
	{
		int			tempResult = result - kMenuMarkerFrequency - kMenuMarker;
		int			freq = abs(GetMarkerFrequency(dataID));

		if( freq == 1 ) freq = 10;

		switch( tempResult )
		{
		case 0: SetMarkerFrequency( dataID, 1 ); break;
		case 1: SetMarkerFrequency( dataID, freq ); break;
		case 2: SetMarkerFrequency( dataID, -freq ); break;
		default: break;
		}

		return kPopupUpdate;
	}


	return kPopupNoAction;
}

// Write the current chart data to a text file
BOOL	CXYChart::WriteTextFile( CString pathName )
{

	CFile				file;
	unsigned long		i;
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
		if( dataSet->GetNDimensions() == 2 && dims[0] == 2 )
		{
			for( i = 0; i < (unsigned int)dims[1]; i++ )
			{
				sprintf( line, "%g %g\r\n", data[i], data[i+dims[1]] );
				file.Write( line, strlen( line ) );
			}
		}

		strcpy( line, "\r\n" );
		if( whichDataSet != GetDataSetCount()-1 )
			file.Write( line, strlen( line ) );
	}

	file.Close();

	return TRUE;
}
