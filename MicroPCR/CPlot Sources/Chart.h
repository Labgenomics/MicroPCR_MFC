#pragma once

#include "ChartData.h"
#include "Axis.h"
#include "ImagePalette.h"
#include <afxtempl.h>

#define		kMaxDataSets		20
#define		kMaxAxes			4

// Comment this line out (or set to 0) to have black backgrounds as default
#define		DEFAULT_WHITE_BACKGROUND	1

enum
{
	kPopupError = -1,
		kPopupNoAction = -2,
		kPopupUpdate = -3
};

#define		kMinimumPopupValue		3000

class CChart : public CObject
{
public:
	DECLARE_SERIAL( CChart )

	// Variables
	BOOL		m_UseMajorHorizontalGrids;
	BOOL		m_UseMajorVerticalGrids;
	BOOL		m_UseMinorHorizontalGrids;
	BOOL		m_UseMinorVerticalGrids;
	COLORREF	m_MajorGridColor, m_MinorGridColor;
	int			m_MajorGridLineSize, m_MinorGridLineSize;
	int			m_MajorGridLineStyle, m_MinorGridLineStyle;

	CChartData	*m_DataSets[kMaxDataSets];
	CAxis		*m_Axes[kMaxAxes];
	int			m_DataID[kMaxDataSets];
	int			m_CurDataID;

	CString		m_DataTitle[kMaxDataSets];

	short		m_DataSetCount;
	short		m_AxisCount;

	CString		m_Title;
	LOGFONT		m_TitleFont;
	COLORREF	m_TitleColor;

	COLORREF	m_BackgroundColor, m_LineColor;

	BOOL		m_EraseBackground;

	// Colors and connecting lines (or psym=10) is taken
	// care of by the deriving class, not this class.

	// Functions
	CChart();
	virtual ~CChart();
	virtual void	Serialize(CArchive& ar);
	virtual BOOL	WriteTextFile( CString pathName );

	inline void		SetTitle( CString title ) { m_Title = title; }
	inline void		SetBackgroundColor( COLORREF color ) { m_BackgroundColor = color; }
	inline void		SetLineColor( COLORREF color ) { m_LineColor = color; }
	inline int		GetAxisCount( void ) { return m_AxisCount; }
	inline int		GetMajorGridLineSize( void ) { return m_MajorGridLineSize; }
	inline int		GetMinorGridLineSize( void ) { return m_MinorGridLineSize; }
	inline COLORREF	GetMajorGridColor( void ) { return m_MajorGridColor; }
	inline COLORREF	GetMinorGridColor( void ) { return m_MinorGridColor; }
	inline COLORREF	GetTitleColor( void ) { return m_TitleColor; }

	CString	GetDataTitle( int dataID ) { int i = GetIndex(dataID); if( i < 0 ) return L""; else return m_DataTitle[i]; }
	void	SetDataTitle( int dataID, CString title );

	CAxis			*GetAxis( int which ) { if( which >= 0 && which < m_AxisCount ) return m_Axes[which]; else return NULL; }
	CAxis			*GetAxisByLocation( int location );

	inline CString	GetTitle( void ) { return m_Title; }
	inline	COLORREF	GetBackgroundColor( void ) { return m_BackgroundColor; }
	inline	COLORREF	GetLineColor( void ) { return m_LineColor; }
	inline	void	SetEraseBackground( BOOL how ) { m_EraseBackground = how; }
	inline	BOOL	GetEraseBackground( void ) { return m_EraseBackground; }
	void			SetMajorGridLineSize( int lineSize ) { if( lineSize > 0 ) m_MajorGridLineSize = lineSize; }
	void			SetMinorGridLineSize( int lineSize ) { if( lineSize > 0 ) m_MinorGridLineSize = lineSize; }
	void			SetMajorGridColor( COLORREF color ) { m_MajorGridColor = color; }
	void			SetMinorGridColor( COLORREF color ) { m_MinorGridColor = color; }
	void			SetTitleColor( COLORREF color ) { m_TitleColor = color; }


	virtual void	OnDraw( CDC *pDC, CRect destRect );
	virtual	void	OnDraw( CDC *pDC, CRect destRect, CRect updateRect );
	virtual CRect	GetPlotRect( CDC *pDC, CRect clientRect );

	// These functions do coordinate conversion between LP (LO_METRIC) and Data
	// or DP and data
	void	DataToLP( CDC *pDC, double *data, LPPOINT point );
	void	LPToData( CDC *pDC, LPPOINT point, double *data );
	virtual void	LPToNearest( CDC *pDC, LPPOINT point, double *data );
	virtual void	NearestToLP( CDC *pDC, double *data, LPPOINT point);

	virtual BOOL	DeleteData( int dataRef );
	virtual void	DeleteAllData( void );

	virtual BOOL	AxisSettings( void );
	virtual BOOL	AxisSettings( int whichAxis );
	virtual BOOL	DataSettings( int whichDataSet );
	virtual BOOL	DataSettings( void );
	virtual BOOL	PlotSettings( void );

	virtual int		ShowPopupMenu( CWnd *window, UINT nFlags, CPoint point );
	virtual int		ShowPopupMenu( CMenu *passedMenu, CWnd *window, UINT nFlags, CPoint point );

	virtual BOOL	SetDataModified( int dataID = -1 );

	CChartData		*GetDataSet( int dataID );
	inline int		GetDataSetCount( void ) { return m_DataSetCount; }
	CAxis			*AddAxis( short location );
	void	GetOptimalRange( double *inRange, double *outRange,
								int *nTicks, int *nMinor, BOOL exactRange );
	void	GetOptimalRangePrivate( double *inRange, double *outRange,
								int *nTicks, int *nMinor, double margin, double wasted );
	void	GetOptimalRangeExact( double *inRange, double *outRange,
								int *nTicks, int *nMinor );
	double		GetTickOptimalTickInterval( double lower, double upper );

	CRect	AdjustPlotSizeForAxes( CDC *pDC, CRect plotRect );
	CRect	AdjustPlotSizeForTitle( CDC *pDC, CRect plotRect );
	void	DrawPlotTitle( CDC *pDC, CRect destRect, CRect plotRect );
	void	EraseBackground( CDC *pDC, CRect destRect );
	void	DrawMajorMinorGrids( CDC *pDC, CRect plotRect );
	void	DrawFrame( CDC *pDC, CRect plotRect );

	void	GetRange( double *xRange, double *yRange );
	void	GetXRange( double *range );
	void	GetYRange( double *range );

	void	SetRange( BOOL setX, double low, double high );
	void	SetXRange( double low, double high );
	void	SetYRange( double low, double high );

	void	IsAutoRange( BOOL &haveXRange, BOOL &haveYRange );
	BOOL	IsXAutoRange( void );
	BOOL	IsYAutoRange( void );
	void	SetAutoRange( BOOL setX );
	void	SetXAutoRange( void );
	void	SetYAutoRange( void );

	// Note that any modifications to this function requires a FULL REBUILD due to (my guess)
	// bugs in Visual C++... a partial build does NOT compile changes to templated functions
template <class T>
int		AddData( T *data, int nDims, int *dimArray )
{
	CChartData *dataSet = new CChartData;
	BOOL returnVal;

	// Make sure there isn't too much data here
	if( m_DataSetCount >= kMaxDataSets ) return -1;

	returnVal = dataSet->SetData( data, nDims, dimArray );

	if( returnVal == FALSE ) return -1;

	// Now add this to our typed pointer array
	m_DataSets[m_DataSetCount] = dataSet;

	m_DataID[m_DataSetCount] = m_CurDataID;

	m_CurDataID++;
	m_DataSetCount++;

	return (m_CurDataID -1);
} // End AddData declaration

// Set data retains the features of the data including
// labeling, etc.
template <class T>
int		SetData( int dataID, T *data, int nDims, int *dimArray )
{
	int			 index = GetIndex( dataID );

	if( index < 0 ) return -1;

	if( m_DataSets[index]->SetData( data, nDims, dimArray ) )
		return dataID;
	else
		return -1;
} // End SetData declaration

protected:
	CRect			m_LastClientRect;

	int		GetIndex( int dataID );
	CChartData		*GetAbsoluteDataSet( int which );

private:
	BOOL RegisterControlClass();

};

// Now we can derive certain types of charts from this base class
enum {
	kXYChartPlain = 0,
		kXYChartConnect = 1,
		kXYChartHistogram = 2,
		kChartTypeCount
};

enum {
	kXYMarkerNone = 0,
		kXYMarkerCircle = 1,
		kXYMarkerSquare = 2,
		kXYMarkerTriangle = 3,
		kXYMarkerX = 4,
		kMarkerTypeCount
};


class CXYChart : public CChart
{
public:
	DECLARE_SERIAL( CXYChart )

	// Variables
	int			m_ChartType[kMaxDataSets]; // see above enum
	COLORREF	m_PlotColor[kMaxDataSets]; // An RGB value

	int			m_MarkerType[kMaxDataSets];	// how to mark data values (see enum)
	int			m_MarkerSize[kMaxDataSets];
	int			m_MarkerFrequency[kMaxDataSets];
	BOOL		m_MarkerFill[kMaxDataSets];
	double		m_XDataMin[kMaxDataSets];
	double		m_XDataMax[kMaxDataSets];
	double		m_YDataMin[kMaxDataSets];
	double		m_YDataMax[kMaxDataSets];
	BOOL		m_DataRangesSet[kMaxDataSets];
	int			m_DataLineSize[kMaxDataSets];
	int			m_DataLineStyle[kMaxDataSets];

	CXYChart();
	~CXYChart();
	virtual void	Serialize(CArchive& ar);
	BOOL			WriteTextFile( CString pathName );

	// Override the OnDraw function
	void	OnDraw( CDC *pDC, CRect destRect );
	void	OnDraw( CDC *pDC, CRect destRect, CRect updateRect );
	BOOL	DeleteData( int dataRef );
	void	DeleteAllData( void );
	void	GetRangeFromData( CChartData *dataSet, int whichDim, double *outRange );
	void	DrawDataSet( CDC *pDC, CRect plotRect, double *xRange, double *yRange );
	void	DrawPlotBasics( CDC *pDC, CRect destRect, CRect plotRect );
	void	GetPlotRange( double *xRange, double *yRange );
	void	DrawMarker( CDC *pDC, CPoint point, int whichDataSet, int dataPointNumber );
	CRect	GetPlotRect( CDC *pDC, CRect clientRect );

	void	LPToNearest( CDC *pDC, LPPOINT point, double *data, int dataID = -1 );
	void	NearestToLP( CDC *pDC, double *data, LPPOINT point, int dataID = -1 );

	BOOL	DataSettings( int whichDataSet );
	BOOL	DataSettings( void );
	BOOL	PlotSettings( void );
	int		InterpretPopupMenuItem( int result );
	int		ShowPopupMenu( CWnd *window, UINT nFlags, CPoint point );
	int		ShowPopupMenu( CMenu *passedMenu, CWnd *window, UINT nFlags, CPoint point );
	void	AddDataPopupMenus( int whichDataSet, CMenu *menu, int startVal );
	void	AddChartTypePopup( CMenu *addMenu, CString titleString, long compare, int startVal );
	void	AddLineSizePopup( CMenu *addMenu, CString titleString, int dataID, int startVal );
	void	AddLineStylePopup( CMenu *addMenu, CString titleString, int dataID, int startVal );
	void	AddMarkerPopup( CMenu *addMenu, CString titleString, int dataID, int startVal );
	int		InterpretDataSetPopupMenuItem( int whichDataSet, int result );


	template <class T>
	int		AddData( T *data, int nDims, int *dimArray )
	{ int returnVal = CChart::AddData( data, nDims, dimArray );
		GetDataRanges();
		SetDefaults( GetIndex( returnVal ) );
	return returnVal;
	}

	template <class T>
		int	SetData( int dataID, T *data, int nDims, int *dimArray )
	{ 		int index = GetIndex( dataID );
	if( index >= 0 ){ int returnVal = CChart::SetData( dataID, data, nDims, dimArray );
	m_DataRangesSet[index] = FALSE;
	if( returnVal >= 0 ) GetDataRange(dataID);
	return returnVal;} else return -1;
	}

	void	SetDataColor( int dataID, COLORREF color );
	COLORREF	GetDataColor( int dataID ) { int i = GetIndex(dataID); if( i < 0 ) return -1; else return m_PlotColor[i]; }
	void	SetMarkerType( int dataID, int type );
	int		GetMarkerType( int dataID ) { int i = GetIndex(dataID); if( i < 0 ) return -1; else return m_MarkerType[i]; }
	void	SetMarkerSize( int dataID, int size );
	int		GetMarkerSize( int dataID )  { int i = GetIndex(dataID); if( i < 0 ) return -1; else return m_MarkerSize[i]; }
	void	SetMarkerFillState( int dataID, BOOL how );
	BOOL	GetMarkerFillState( int dataID ) { int i = GetIndex(dataID); if( i < 0 ) return FALSE; else return m_MarkerFill[i]; }

	void	SetChartType( int dataID, int type );
	int		GetChartType( int dataID )   { int i = GetIndex(dataID); if( i < 0 ) return -1; else return m_ChartType[i]; }
	void	SetMarkerFrequency( int dataID, int freq );
	int		GetMarkerFrequency( int dataID ) { int i = GetIndex(dataID); if( i < 0 ) return 0; else return m_MarkerFrequency[i]; }
	int		GetDataLineSize( int dataID ) { int i = GetIndex( dataID ); if( i < 0 ) return 1; else return m_DataLineSize[i]; }
	void	SetDataLineSize( int dataID, int lineSize ) { int i = GetIndex( dataID ); if( i < 0 ) return; else m_DataLineSize[i] = lineSize; }
	int		GetDataLineStyle( int dataID ) { int i = GetIndex( dataID ); if( i < 0 ) return 1; else return m_DataLineStyle[i]; }
	void	SetDataLineStyle( int dataID, int lineStyle ) { int i = GetIndex( dataID ); if( i < 0 ) return; else m_DataLineStyle[i] = lineStyle; }
	void	CopyChartData( CXYChart *chart );

	BOOL	SetDataModified( int dataID = -1 );

private:
	void	GetDataRanges( void );
	void	GetDataRange( int dataID );
	void	SetDefaults( int index );
};

class CImageChart : public CChart
{
private:
	BOOL		m_UserByteImage, m_UserImage;
	BYTE		*m_UserByteImageData;
	double		*m_UserImageData;

public:
	DECLARE_SERIAL( CImageChart )

	int			colorTable;
	double		plotXRange[2];	// These are in units of pixles
	double		plotYRange[2];	// not in units of axis label
	
	CImagePalette		palette;

	// Store our own plotting data
	BYTE		*imageData;
	int			m_NDimensions;	// number of dimensions in data
	int			m_Dimensions[kMaxDims];	// size of each dimension
	int			m_Scale;
	BOOL		m_PlotToWindowSize;	// FALSE means plot only scales of 2
	BOOL		m_MaintainAspectRatio;	// FALSE means allow distorted image

	CImageChart();
	~CImageChart();
	virtual void	Serialize(CArchive& ar);

	// Override of parent functions function
	void	OnDraw( CDC *pDC, CRect destRect );
	void	OnDraw( CDC *pDC, CRect destRect, CRect updateRect );
	CRect	GetPlotRect( CDC *pDC, CRect clientRect );

	void	LPToNearest( CDC *pDC, LPPOINT point, double *data );
	void	NearestToLP( CDC *pDC, double *data, LPPOINT point);

	// Normal functions
	void	DrawPlotBasics( CDC *pDC, CRect destRect, CRect plotRect );
	void	GetPlotRange( double *xRange, double *yRange );
	CSize	GetMinDataDimensions( void );
	void	DrawDataSet( CDC *pDC, CRect plotRect, double *xRange, double *yRange );
	CRect	AdjustPlotSizeForWindowSize( CDC *pDC, CRect destRect, CRect plotRect );
	BYTE	*GetScaledImageData( BYTE *data, int *dims, BOOL allocate );
	void	SetPalette( int whichPalette );
	void	SetPaletteMenu( int id );
	int		GetPalette( void ) { return palette.GetCurPaletteNumber(); }
	int		GetPlotScale( void ) { return m_Scale; }
	void	DeleteImage( void );
	BOOL	DeleteData( void ) { DeleteImage(); return TRUE; }
	BOOL	SetDataModified( int dataID, BOOL doScale = TRUE, BOOL doOffset = FALSE );

	BOOL	DataSettings( void );
	BOOL	PlotSettings( void );
	int		ShowPopupMenu( CMenu *passedMenu, CWnd *window, UINT nFlags, CPoint point );
	int		ShowPopupMenu( CWnd *window, UINT nFlags, CPoint point );
	int		InterpretPopupMenuItem( int result );

// Here you can use these functions to set user functions
	// for the byte image or the data image. Note that setting
	// a data image is ONLY done for a double array. Note also
	// that setting the image pointer requires a recalculation
	// of the byte image for each redraw, and can be SLOW.
	void	SetUserByteImagePtr( BYTE *imagePtr );
	void	SetUserImagePtr( double *imagePtr );
	BYTE	*GetUserByteImagePtr( void ) { return m_UserByteImageData; }
	double	*GetUserImagePtr( void ) { return m_UserImageData; }

	// Note that any modifications to this function requires a FULL REBUILD due to (my guess)
	// bugs in Visual C++... a partial build does NOT compile changes to templated functions
	template <class T>
		BOOL	AddImage( T *data, int nDims, int *dimArray, BOOL doScale, BOOL doOffset )
	{
		double		scale = 1.;
		BOOL		returnVal;

		if( data == NULL || nDims != 2) return FALSE;

		// Make sure there isn't too much data here
		// We only allow one data set in image data
		if( m_DataSetCount > 1 ) return FALSE;

		if( imageData != NULL )
		{
			VirtualFree( imageData, 0, MEM_RELEASE );
			imageData = NULL;
		}

		CChartData *dataSet = new CChartData;

		returnVal = dataSet->SetData( data, nDims, dimArray );

		if( returnVal == FALSE )
		{ 
			return FALSE;
		}

		// Now add this to our typed pointer array
		m_DataSets[m_DataSetCount] = dataSet;
		m_DataID[m_DataSetCount] = m_CurDataID;

		m_DataSetCount++;
		m_CurDataID++;

		returnVal = SetDataModified( m_CurDataID-1, doScale, doOffset );

		return returnVal;
	}

		template <class T>
	int		AddData( T *data, int nDims, int *dimArray )
	{ BOOL returnVal = AddImage( data, nDims, dimArray, TRUE, FALSE );
		if( returnVal == TRUE ) return 1; else return -1;
	}

		template <class T>
			int SetData( T *data, int nDims, int *dimArray )
		{
			DeleteImage();
			BOOL returnVal = AddImage( data, nDims, dimArray, TRUE, FALSE );
			if( returnVal == TRUE ) return 1; else return -1;
		}
};
