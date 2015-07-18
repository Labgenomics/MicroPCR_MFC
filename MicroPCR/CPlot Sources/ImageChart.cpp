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
#include "ImagePlotSettingsDialog.h"
#include "CPlotUtility.h"
#include "Chart.h"

IMPLEMENT_SERIAL( CImageChart, CChart, 1 )

CImageChart::CImageChart()
{
	imageData = NULL;
	m_NDimensions = 0;

	for( int i = 0; i < kMaxDims; i++ )
		m_Dimensions[i] = 0;

	colorTable = 0;

	// Create our default palette;
	palette.SetPalette( kPaletteRainbow );

	m_UserByteImage = FALSE;
	m_UserImage = FALSE;

	m_UserImageData = NULL;
	m_UserByteImageData = NULL;

	m_PlotToWindowSize = FALSE;	// FALSE means plot only scales of 2
	m_MaintainAspectRatio = TRUE;	// FALSE means allow distorted image
}

CImageChart::~CImageChart()
{
	if( imageData != NULL )
	{
		VirtualFree( imageData, 0, MEM_RELEASE );
		imageData = NULL;
	}
}

void CImageChart::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << colorTable;
		ar << plotXRange[0] << plotXRange[1];
		ar << plotYRange[0] << plotYRange[1];
		ar << m_MaintainAspectRatio << m_PlotToWindowSize;
	}
	else
	{
		ar >> colorTable;
		ar >> plotXRange[0] >> plotXRange[1];
		ar >> plotYRange[0] >> plotYRange[1];
		ar >> m_MaintainAspectRatio >> m_PlotToWindowSize;
	}
}

void	CImageChart::OnDraw( CDC *pDC, CRect destRect )
{
	OnDraw( pDC, destRect, destRect );
}

// Draw the contents
void	CImageChart::OnDraw( CDC *pDC, CRect destRect, CRect updateRect )
{
	CString			string;
	int				i;
	CAxis			*axis;
	CSize			axisDims;
	CSize			trim(0,0);
	CRect			plotRect, normalPlotRect;
	double			xRange[] = {1e30, -1e30}, yRange[]={1e30,-1e30};

	updateRect.NormalizeRect();

	// Always start with a scale of 1
	m_Scale = 1;

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

		// Draw the grids using the parent function
		DrawMajorMinorGrids( pDC, plotRect );
	}

	// Draw the axes
	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = m_Axes[i];
		axis->OnDraw( pDC, destRect, plotRect );
	}

	CChart::OnDraw( pDC, destRect );

	m_LastClientRect = destRect;
}

// Draws the background and frame
void	CImageChart::DrawPlotBasics( CDC *pDC, CRect destRect, CRect plotRect )
{
	// Erase the background using parent function
	EraseBackground( pDC, destRect );

	// Draw the frame using the parent function
	DrawFrame( pDC, plotRect );
}

// Get the range of the plot, taking into account whether
// auto ranging has been set up
void	CImageChart::GetPlotRange( double *xRange, double *yRange )
{
	BOOL		haveXRange = FALSE, haveYRange = FALSE;
	CAxis		*axis;
	int			i, *dims;
	double		optRange[2];
	int			optNTicks, optMinorTicks;
	CChartData	*dataSet;

	if( m_DataSetCount <= 0 ) return;

	for( i = 0; i < m_AxisCount; i++ )
	{
		axis = m_Axes[i];
		switch( axis->GetLocation() )
		{
		case kLocationBottom:
			if( axis->IsRangeSet() )
			{
				axis->GetRange( xRange );
				haveXRange = TRUE;
			}
			break;
		case kLocationLeft:
			if( axis->IsRangeSet() )
			{
				axis->GetRange( yRange );
				haveYRange = TRUE;
			}
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
				dims = dataSet->GetDimensions();
				xRange[0] = min( xRange[0], 0 );
				xRange[1] = max( xRange[1], dims[0] );
			}
			if( haveYRange == FALSE )
			{
				dims = dataSet->GetDimensions();
				yRange[0] = min( yRange[0], 0 );
				yRange[1] = max( yRange[1], dims[1] );
			}
		}
	}

		// Now that we've autoranged, set up the autoranged axses
		for( i = 0; i < m_AxisCount; i++ )
		{
			axis = m_Axes[i];
				switch( axis->GetLocation() )
				{
				case kLocationBottom:
					GetOptimalRangeExact( xRange, optRange, &optNTicks, &optMinorTicks );
					memcpy( xRange, optRange, 2*sizeof( double ) );
					axis->SetRange( haveXRange, optRange[0], optRange[1] );
					axis->SetTickCount( -optNTicks );
					axis->SetMinorTickCount( -optMinorTicks );
					break;
				case kLocationTop:
					break;
				case kLocationLeft:
					GetOptimalRangeExact( yRange, optRange, &optNTicks, &optMinorTicks );
					memcpy( yRange, optRange, 2*sizeof( double ) );
					axis->SetRange( haveYRange, optRange[0], optRange[1] );
					axis->SetTickCount( -optNTicks );
					axis->SetMinorTickCount( -optMinorTicks );
					break;
				case kLocationRight:
					break;
				default: break;
				}
		}
}

// From the data, get the minimum size of the image
// at full size, in pixels
CSize	CImageChart::GetMinDataDimensions( void )
{
	int			i, *dims;
	CChartData	*dataSet;
	CSize		minSize(0,0);

	for( i = 0; i < m_DataSetCount; i++ )
	{
		dataSet = GetAbsoluteDataSet( i );

		dims = dataSet->GetDimensions();

		minSize.cx = max( minSize.cx, dims[0] );
		minSize.cy = max( minSize.cy, dims[1] );
	}

	return minSize;

}

// Draw the image
void	CImageChart::DrawDataSet( CDC *pDC, CRect plotRect, double *xRange, double *yRange )
{
	double		*data;
	BYTE		*imageDataPtr;
	int			*dims;
	int			dimCount;
	int			i;
	CPoint		singlePoint, point, lastPoint;
	CRgn		rgn;
	int			saveDC;
	CChartData	*dataSet;
	CRect		dPlotRect = plotRect;
	CSize		sizeConverter;

	if( m_Scale == 0 ) return;

	// Find the logical equivalent of one display point
	singlePoint.x = singlePoint.y = 1;
	pDC->DPtoLP( &singlePoint );

	// Create a region for clipping
	saveDC = pDC->SaveDC();
	if( pDC->IsPrinting() )
	{
		// Adjust plotting rectangle
		plotRect.left += 1;
		plotRect.bottom += 1;
	}

	plotRect.OffsetRect( singlePoint.x, singlePoint.y );
	plotRect.right -= singlePoint.x;
	plotRect.bottom -= singlePoint.y;

	for( i = 0; i < m_DataSetCount; i++ )
	{
		CPen		pen;
		CBrush		brush;
		BITMAPINFO	*bitmapInfo = palette.GetBitmapInfo();
		long		xPos, yPos;

		// Error, couldn't load a bitmap
		if( bitmapInfo == NULL ) return;

		dataSet = GetAbsoluteDataSet( i );
		data = dataSet->GetData();
		dims = dataSet->GetDimensions();
		dimCount = dataSet->GetNDimensions();

		// Use imageData to display our data
		bitmapInfo->bmiHeader.biWidth     = dims[0] / m_Scale;
		bitmapInfo->bmiHeader.biHeight    = dims[1] / m_Scale;
	
		if( pDC->IsPrinting() == TRUE || (m_PlotToWindowSize == TRUE) )
		{
			sizeConverter.cx = plotRect.Width();
			sizeConverter.cy = -plotRect.Height();
		}
		else
		{
			sizeConverter.cx = dims[0]/m_Scale;
			sizeConverter.cy = dims[1]/m_Scale;

			pDC->DPtoLP( (LPSIZE) &sizeConverter );
		}

		xPos = (plotRect.Width()/2+plotRect.left) - sizeConverter.cx / 2;
		yPos = (plotRect.bottom-(plotRect.Height()/2)) - sizeConverter.cy / 2 - singlePoint.y;

		// Here we'll have to make the data if necessary
		if( m_Scale > 1 )
		{
			imageDataPtr = GetScaledImageData( imageDataPtr, dims, TRUE );
		}
		else
		{
			if( m_UserByteImage ) imageDataPtr = m_UserByteImageData; else imageDataPtr = imageData;
		}

		StretchDIBits( pDC->GetSafeHdc(), xPos, yPos, sizeConverter.cx, sizeConverter.cy,
			0, 0, dims[0]/m_Scale, dims[1]/m_Scale, imageDataPtr, palette.GetBitmapInfo(),
			DIB_RGB_COLORS, SRCCOPY );

		if( m_Scale > 1 )
		{
			GetScaledImageData( imageDataPtr, dims, FALSE );
		}
	}

	pDC->RestoreDC( saveDC );
}

// This calculates the size of our plot based on the window
// size and sets up any scaling that is needed
CRect	CImageChart::AdjustPlotSizeForWindowSize( CDC *pDC, CRect destRect, CRect plotRect )
{
	CSize			minSize;
	BOOL			keepGoing = TRUE;
	CRect			originalPlotRect = plotRect;
	BOOL			bigWidth = FALSE, bigHeight = FALSE;

	// Note: When we set up this rectangle, we add one display unit to the right
	// and bottom of the rect. This is subtracted in the display function, but
	// allows the frame to be drawn correctly.

	m_Scale = 1;

	// Get minimum data dimensions (in pixels)
	minSize = GetMinDataDimensions();

	// Convert to pixels
	pDC->LPtoDP( (LPPOINT) &plotRect, 2 );
	originalPlotRect = plotRect;

	// In some cases, we don't modify at all
	if( m_PlotToWindowSize == TRUE )
	{
		if( m_MaintainAspectRatio == TRUE )
		{
			double		ratio;
			
			// ratio of width to height to maintain aspect ratio
			ratio = fabs( (double)minSize.cx ) / fabs( (double)minSize.cy );
			
			if( (float)abs(plotRect.Width()) / (float)abs(plotRect.Height()) > ratio )
			{
				// chop off some width
				plotRect.right = plotRect.left + (int)(abs(plotRect.Height()) * ratio);
			}
			else
			{
				// chop some off height
				plotRect.bottom = plotRect.top + (int)(abs(plotRect.Width()) / ratio);
			}
			
			plotRect.OffsetRect( (originalPlotRect.Width()-plotRect.Width())/2,
				(originalPlotRect.Height()-plotRect.Height())/2 );
			
		}

		if( minSize.cx % 2 == 0 && abs(plotRect.Width()) % 2 != 0 )
			plotRect.right-= abs(plotRect.Width()) % 2;
		
		if( minSize.cy % 2 == 0 && abs(plotRect.Height()) % 2 != 0 )
			plotRect.bottom+= abs(plotRect.Height()) % 2;
		
		pDC->DPtoLP( (LPPOINT) &plotRect, 2 );

		return plotRect;
	}

	// if m_PlotToWindowSize == FALSE, then plot scaled no matter
	// the value of m_MaintainAspectRatio

	// Get minimum data dimensions (in pixels)
	minSize = GetMinDataDimensions();

	// Check that the width isn't larger than our size
	if( plotRect.Width() > (minSize.cx/m_Scale+2) )
	{
		if( pDC->IsPrinting() == FALSE )
		{
			// If it is, make it smaller
			plotRect.left = originalPlotRect.Width()/2 - minSize.cx / (2*m_Scale) + originalPlotRect.left;
			plotRect.right = plotRect.left + minSize.cx/m_Scale+1;
		}
		else
		{
			// But if we're printing, it's OK
			bigWidth = TRUE;
		}
	}

	// Check that our height isn't larger than our size
	if( plotRect.Height() > (minSize.cy/m_Scale+2) )
	{
		if( pDC->IsPrinting() == FALSE )
		{
			// If it is, make it smaller
			plotRect.top = originalPlotRect.Height()/2 - minSize.cy / (2*m_Scale) + originalPlotRect.top;// - 1;
			plotRect.bottom = plotRect.top + minSize.cy/m_Scale + 1;
		}
		else // But if we're printing, it's OK
			bigHeight = TRUE;
	}

	if( pDC->IsPrinting() )
	{
		// Scaling setup for printing
		if( bigHeight && bigWidth )
		{
			int		thisScale = 2, bestScale;

			bestScale = (int)min( originalPlotRect.Width() / minSize.cx, originalPlotRect.Height() / minSize.cy );
			
			plotRect.left = originalPlotRect.Width()/2 - (minSize.cx * bestScale) / 2 + originalPlotRect.left;
			plotRect.right = plotRect.left + minSize.cx * bestScale+1;
			plotRect.top = originalPlotRect.Height()/2 - (minSize.cy * bestScale) / 2 + originalPlotRect.top;
			plotRect.bottom = plotRect.top + minSize.cy * bestScale+1;
		}
	}
	else
	{
		// Scaling setup for drawing to screen
		while( keepGoing == TRUE && ((minSize.cx/m_Scale+2) > originalPlotRect.Width() || (minSize.cy/m_Scale+2) > originalPlotRect.Height() ) )
		{ 
			m_Scale *= 2;
			plotRect.left = originalPlotRect.Width()/2 - minSize.cx / (2*m_Scale) + originalPlotRect.left;// - 1;
			plotRect.right = plotRect.left + minSize.cx/m_Scale + 1;
			plotRect.top = originalPlotRect.Height()/2 - minSize.cy / (2*m_Scale) + originalPlotRect.top;// - 1;
			plotRect.bottom = plotRect.top + minSize.cy/m_Scale + 1;

			if( plotRect.Width() < 10 || plotRect.Height() < 10 )
			{
				keepGoing = FALSE;
				m_Scale = 0; // don't bother displaying data
			}
		}
	}

	pDC->DPtoLP( (LPPOINT) &plotRect, 2 );

	return plotRect;
}

// This function returns the image data, taking into account whether we
// have a scaling factor. For simple scaling, it just removes data points,
// it does not do any averaging.
BYTE		*CImageChart::GetScaledImageData( BYTE *data, int *dims, BOOL allocate )
{
	long		row, column;
	long		count = dims[0] * dims[1], newSize = (dims[0]/m_Scale) * (dims[1] / m_Scale);
	long		newDim1 = dims[1] / m_Scale, newDim0 = dims[0] / m_Scale, scaleDim0, idScaleDim0;
	long		msSquared = m_Scale * m_Scale;
	BYTE		*imageDataPtr;

	// All of the modulus 4 and sizes of 4 in this function are to make sure
	// that our data is word aligned, since that is required for
	// StretchDIBits()

	if( m_UserByteImage )
		imageDataPtr = m_UserByteImageData;
	else
		imageDataPtr = imageData;

	if( allocate == TRUE )
	{
		// If we're supposed to allocate a new array, do it
		if( newDim0 % 4 == 0 ) scaleDim0 = newDim0; else scaleDim0 = newDim0 + (4-newDim0%4);
		newSize = (scaleDim0) * (dims[1] / m_Scale);

		if( dims[0] % 4 == 0 ) idScaleDim0 = dims[0]; else idScaleDim0 = dims[0] + (4-dims[0]%4);
		data = (BYTE*)VirtualAlloc( NULL, newSize,
			MEM_COMMIT, PAGE_READWRITE );

		// Copy every m_Scaleth data point
		for( row = 0; row < newDim1; row++ )
		{
			if( row * m_Scale < dims[1] )
			{
				for( column = 0; column < newDim0; column++ )
				{
					if( column * m_Scale < dims[0] )
						data[column + scaleDim0 * row] = imageDataPtr[(row*m_Scale)*idScaleDim0 + column*m_Scale];
					else
						data[column + scaleDim0 * row] = imageDataPtr[ dims[0] - 1 + idScaleDim0 * (row * m_Scale)];
				}
			}
			else
			{
				for( column = 0; column < newDim0; column++ )
				{
					if( column * m_Scale < dims[0] )
						data[column + scaleDim0 * row] = imageDataPtr[(row*m_Scale)*idScaleDim0 + column*m_Scale];
					else
						data[column + scaleDim0 * row] = imageDataPtr[ dims[0] - 1 + idScaleDim0 * (dims[1]-1)];
				}
			}
		}

		return data;
	}
	else
	{
		// If we didn't free it last time, make sure we do
		if( data != NULL )
		{
			VirtualFree( data, 0, MEM_RELEASE );
			data = NULL;
		}

		return NULL;
	}
}

// Sets the palette that the current image uses
void	CImageChart::SetPalette( int whichPalette )
{
	// User has to force a redraw
	palette.SetPalette( whichPalette );
}

// Sets the palette from an added menu item
void	CImageChart::SetPaletteMenu( int id )
{
	if( id >= IDS_COLORTABLE_START && id <= IDS_COLORTABLE_END )
	{
		int			which;
		
		which = id - IDS_COLORTABLE_START + IDR_CT_FIRST;
		SetPalette( which );
	}

}

// Gets the rectangle of the current image. It returns
// solely the image rectangle, not the plot rectangle,
// despite its name.
CRect	CImageChart::GetPlotRect( CDC *pDC, CRect clientRect )
{
	CRect		plotRect = clientRect;

	// Ask the axes what size they want
	plotRect = AdjustPlotSizeForAxes( pDC, plotRect );

	// What size do we need for the title
	plotRect = AdjustPlotSizeForTitle( pDC, plotRect );

	// Add spacing
	plotRect.InflateRect( -20, 20 );

	// Check to make sure there is enough room to display all our stuff
	// If the plot rect is larger than the size of our image rect, then
	// just shrink the plot rect to the image rect.
	plotRect = AdjustPlotSizeForWindowSize( pDC, clientRect, plotRect );

	return plotRect;
}

// This is based on the values set for the x and y axes
void	CImageChart::LPToNearest( CDC *pDC, LPPOINT point, double *data )
{
	double				xRange[2], yRange[2];
	int					*dims, nDims;
	CChartData			*dataSet;

	LPToData( pDC, point, data );

	dataSet = GetAbsoluteDataSet( 0 );

	if( dataSet == NULL ) return;

	nDims = dataSet->GetNDimensions();
	dims = dataSet->GetDimensions();

	if( nDims != 2 ) return;

	GetRange( xRange, yRange );

	data[0] = RoundToRange( data[0], (xRange[1] - xRange[0]) / (dims[0]) );
	data[1] = RoundToRange( data[1], (yRange[1] - yRange[0]) / (dims[1]) );

	if( xRange[1] > xRange[0] )
	{
		if( data[0] < xRange[0] ) data[0] = xRange[0];
		if( data[0] > xRange[1] ) data[0] = xRange[1];
	}
	else
	{
		if( data[0] > xRange[0] ) data[0] = xRange[0];
		if( data[0] < xRange[1] ) data[0] = xRange[1];
	}

	if( yRange[1] > yRange[0] )
	{
		if( data[1] < yRange[0] ) data[1] = yRange[0];
		if( data[1] > yRange[1] ) data[1] = yRange[1];
	}
	else
	{
		if( data[1] > yRange[0] ) data[1] = yRange[0];
		if( data[1] < yRange[1] ) data[1] = yRange[1];
	}
}

// Same as DataToLP
void	CImageChart::NearestToLP( CDC *pDC, double *data, LPPOINT point )
{
	DataToLP( pDC, data, point );
}

// Use this function to set up a temporary pointer to a
// word aligned byte image. This allows the user to keep
// track of the data, and reduces the amount of data
// copying that has to be done to display it. Call this
// function before a draw and it will use the pointer
// instead of the stored image.
void	CImageChart::SetUserByteImagePtr( BYTE *imagePtr )
{
	m_UserByteImageData = imagePtr;

	if( imagePtr == NULL ) m_UserByteImage = FALSE; else m_UserByteImage = TRUE;
}

// Use this function to set up a temporary pointer to an
// image. This allows the user to keep
// track of the data, and reduces the amount of data
// copying that has to be done to display it. Call this
// function before a draw and it will use the pointer
// instead of the stored image to create the word-aligned
// image buffer.
void	CImageChart::SetUserImagePtr( double *imagePtr )
{
	m_UserImageData = imagePtr;

	if( imagePtr == NULL ) m_UserImage = FALSE; else m_UserImage = TRUE;
}

// Delete the current data set
void	CImageChart::DeleteImage( void )
{
	if( m_DataSetCount >= 1 )
	{
		// Should only be one data set
		if( m_DataSets[0] != NULL )
		{
			m_DataSets[0]->DeleteData();
			m_DataSetCount = 0;
			delete m_DataSets[0]; // delete the object
			m_DataSets[0] = NULL;
		}
	}
}

// Call this function when you have modified the image
// array in place. It assumes you haven't modified the
// size of the array. Use AddImage() for that purpose.
BOOL	CImageChart::SetDataModified( int dataID, BOOL doScale, BOOL doOffset )
{
	CChartData		*dataSet;
	double			min, max;
	double			*data;
	long			i, j, size = 1, allocSize, allocDim0;
	double			scale = 1.;
	int				*dimArray;
	int				newDim1, nDims;

	dataSet = GetDataSet( dataID );

	if( dataSet == NULL || (nDims = dataSet->GetNDimensions()) != 2 ) return FALSE;

	data = dataSet->GetData();

	dimArray = dataSet->GetDimensions();

	min = max = data[0];

	for( i = 0; i < nDims; i++ )
		size *= dimArray[i];

	if( size <= 0 ) return FALSE;

	if( dimArray[0] % 4 == 0 ) newDim1 = dimArray[0]; else newDim1 = dimArray[0] + (4-dimArray[0]%4);
	allocSize = (newDim1) * dimArray[1];

	if( imageData == NULL )
	{
		imageData = (BYTE *) VirtualAlloc( NULL, allocSize, 
			MEM_COMMIT, PAGE_READWRITE );

		if( imageData == NULL ) return FALSE;
	}

	// Decide whether we are scaling and offseting. Find min and max if
	// we are doing either
	if( doScale == TRUE || doOffset == TRUE )
	{
		for( i = 0; i < size; i++ )
		{
			if( data[i] < min ) min = data[i];
			if( data[i] > max ) max = data[i];
		}

		if( doOffset == TRUE ) max -= min;

		scale =  ((double)palette.GetNumEntries()-1) / (double) (max);
	}

	allocDim0 = newDim1;

	for( i = 0, j = 0; i < allocSize; i++ )
	{
		// If we are in padding, don't worry about data
		if( i % allocDim0 >= dimArray[0] ){ continue;}

		// We shouldn't be modifying data[i], but a new data set
		if( doOffset == TRUE && doScale == FALSE ) imageData[i] = (BYTE) ((double)data[j] - min);
		else if( doOffset == TRUE && doScale == TRUE ) imageData[i] = (BYTE) (((double)data[j]-min) * scale);
		else if( doOffset == FALSE && doScale == TRUE ) imageData[i] = (BYTE)((double)data[j] * scale);
		else imageData[i] = (BYTE) data[j];

		j++;
	}

	return TRUE;
}

// Show the plot settings dialog
BOOL	CImageChart::PlotSettings( void )
{
	CImagePlotSettingsDialog	dialog( NULL, this );
	
	if( dialog.DoModal() == IDOK ) return TRUE; else return FALSE;
}

// There is no data settings dialog for image charts
BOOL	CImageChart::DataSettings( void )
{
	return FALSE;
}

// Some constants for the popup menus
enum
{
	kMenuScaleToWindow = 1,
		kMenuMaintainAspectRatio, //2
		kMenuMajorHorizGrids, //3
		kMenuMajorVertGrids, //4
		kMenuMinorHorizGrids, //5
		kMenuMinorVertGrids, //6

		kMenuTitleFont = 10,

		kMenuColorTable = kMenuTitleFont + nFontSizes + 1,

		kMenuPlotSettings = 999
};


// Shows the contextual menu (shortcut menu) for an image chart.
// If passedMenu is not NULL, the new menu is appended to
// passedMenu, otherwise, a new menu is created.
int		CImageChart::ShowPopupMenu( CMenu *passedMenu, CWnd *window, UINT nFlags, CPoint point )
{
	CMenu			menu, *menuPtr;
	CMenu			axisMenus[kMaxAxes];
	int				result;
	UINT			flag;

	if( window == NULL ) return FALSE;
	
	if( passedMenu == NULL || (!::IsMenu(passedMenu->m_hMenu)) )
	{
		menuPtr = &menu;
		menuPtr->CreatePopupMenu();
	}
	else
		menuPtr = passedMenu;

	// Add the default items:
	flag = GetPopupCheckedFlag(this->m_PlotToWindowSize);
	menuPtr->AppendMenu( MF_STRING | flag, kMenuScaleToWindow, L"Scale to window" );
	flag = GetPopupCheckedFlag(this->m_MaintainAspectRatio);
	menuPtr->AppendMenu( MF_STRING | flag, kMenuMaintainAspectRatio, L"Maintain aspect ratio" );

	// Separator
	menuPtr->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

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

	{
		CMenu  ctSubMenu;
		ctSubMenu.CreatePopupMenu();
		// Color table
		AddColorTableMenu( &ctSubMenu, kMenuColorTable );
		menuPtr->AppendMenu( MF_POPUP | MF_STRING, (UINT) ctSubMenu.Detach(), L"Color Table" );
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
	for( int i = 0; i < GetAxisCount(); i++ )
	{
		GetAxis(i)->AddAxisPopupMenus( menuPtr, (kMenuAxisRange*i)+kMenuAxisMinimum );
	}

	// Separator
	if( GetAxisCount() > 0 )
		menuPtr->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR)0 );

	// Settings...
	menuPtr->AppendMenu( MF_STRING, kMenuPlotSettings, L"Settings..." );

	result = menuPtr->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x,
		point.y, window );

	return InterpretPopupMenuItem( result );
}

// Overloaded function for showing popup menu
int		CImageChart::ShowPopupMenu( CWnd *window, UINT nFlags, CPoint point )
{
	return ShowPopupMenu( NULL, window, nFlags, point );
}

// Looks at the result of a popup menu selection and determines
// what the user selected, and takes the appropriate action.
int		CImageChart::InterpretPopupMenuItem( int result )
{
	int			whichAxis;
	int			returnVal;

	if( result >= kMinimumPopupValue ) return result;

	if( result <= 0 ) return kPopupError;

	if( result < kMenuAxisRange )
	{
		switch( result )
		{
		case kMenuScaleToWindow: m_PlotToWindowSize = !m_PlotToWindowSize; break;
		case kMenuMaintainAspectRatio: m_MaintainAspectRatio = !m_MaintainAspectRatio; break;
		case kMenuMajorHorizGrids: m_UseMajorHorizontalGrids = !m_UseMajorHorizontalGrids; break;
		case kMenuMajorVertGrids: m_UseMajorVerticalGrids = !m_UseMajorVerticalGrids; break;
		case kMenuMinorHorizGrids: m_UseMinorHorizontalGrids = !m_UseMinorHorizontalGrids; break;
		case kMenuMinorVertGrids: m_UseMinorVerticalGrids = !m_UseMinorVerticalGrids; break;
		default: break;
		}

		if( result >= kMenuTitleFont && result < kMenuTitleFont + nFontSizes )
		{
			m_TitleFont.lfHeight = fontSizes[result-kMenuTitleFont];
		}

		if( result >= kMenuColorTable && result < kMenuColorTable + kColorTableCount ) // hard wire
		{
			// In color table menu
			SetPaletteMenu( result - kMenuColorTable + IDS_COLORTABLE_START );
		}

		return kPopupUpdate;
	}

	if( result == kMenuPlotSettings )
	{
		if( PlotSettings() ) return kPopupUpdate; else return kPopupNoAction;
	}

	// These must be axis sets
	whichAxis = (result-kMenuAxisMinimum) / kMenuAxisRange;

	if( whichAxis < 0 || whichAxis > GetAxisCount() ) return kPopupError;

	if( (result-kMenuAxisMinimum) % kMenuAxisRange == kMenuAxisRange-1 ) // have to do this one manually
		if( AxisSettings(whichAxis) ) return kPopupUpdate; else return kPopupNoAction;

	if( (returnVal = GetAxis(whichAxis)->InterpretAxisPopupMenuItem( (result-kMenuAxisMinimum) % kMenuAxisRange )) == kPopupError )
		return result;
	else
		return returnVal;
}

