#pragma once

// Axis locations
enum {
	kLocationLeft = 0,
		kLocationBottom,
		kLocationRight,
		kLocationTop
};

// Constants for popup menus
#define		kMenuAxisRange		100
#define		kMenuAxisMinimum	(1*kMenuAxisRange)
#define		kMenuDataRange		100
#define		kMenuDataMinimum	(1*kMenuDataRange+1000)

class CAxis : public CObject
{
public:
	DECLARE_SERIAL( CAxis )

	int			m_TickCount;	// number of tick marks
	int			m_MinorTickCount;	// number of minor ticks
	int			m_TickLength, m_MinorTickLength;	// length of tick marks (logical units)
	int			m_LineSize;		// thickness of the line
	double		m_Range[2];		// range of the axis
	BOOL		m_Visible;		// is axis visible?
	CString		m_Title;		// axis title
	short		m_Location;		// location of axis (left, bottom, top, etc.)
	LOGFONT		m_TitleFont;	// font to use for title
	LOGFONT		m_LabelFont;	// font to use for axis labels
	COLORREF	m_Color;		// color to use for axis and font
	COLORREF	m_TitleColor;	// color to use for title
	BOOL		m_RangeSet;		// has the range been set?

	CAxis();
	~CAxis();
	void	SetDefaults( void );

	virtual void		Serialize(CArchive& ar);

	void		OnDraw( CDC *pDC, CRect clientRect, CRect chartRect );
	void		OnDraw( CDC *pDC, CRect clientRect, CRect chartRect, CRect updateRect );

	double		GetTickPosition( int whichTick );
	CString		GetTickLabel( int whichTick );

	// some inline functions
	inline int		GetTickCount( void ) { return (abs(m_TickCount)); }
	inline int		GetMinorTickCount( void ) { return (abs(m_MinorTickCount)); }
	inline BOOL		IsVisible( void ) { return m_Visible; }
	inline void		GetTitle( CString &title ) { title = m_Title; }
	inline short	GetLocation( void ) { return m_Location; }
	inline COLORREF	GetColor( void ) { return m_Color; }
	inline COLORREF GetTitleColor( void ) { return m_TitleColor; }
	inline double	GetLowerRange( void ) { return m_Range[0]; }
	inline double	GetUpperRange( void ) { return m_Range[1]; }
	inline int		GetLineSize( void ) { return m_LineSize; }
	inline BOOL		IsRangeSet( void ) { return m_RangeSet; }

	void		GetRange( double *range ) { memcpy( range, m_Range, 2 * sizeof( double ) ); }
	void		SetTickCount( int tickCount ) { if( tickCount < 0 && m_TickCount > 0 ) return; m_TickCount = tickCount; }
	void		SetMinorTickCount( int tickCount ) { if( tickCount < 0 && m_MinorTickCount > 0 ) return; m_MinorTickCount = tickCount; }
	void		SetRange( double low, double high ) { m_Range[0] = low; m_Range[1] = high; m_RangeSet = TRUE; }
	void		SetRange( BOOL set, double low, double high ) { m_Range[0] = low; m_Range[1] = high; m_RangeSet = set; }
	void		SetTitle( CString title ) { m_Title = title; }
	void		SetLocation( short location ) { m_Location = location; }
	void		SetColor( COLORREF color ) { m_Color = color; }
	void		SetTitleColor( COLORREF color ) {m_TitleColor = color; }
	void		SetLineSize( int lineSize ) { if( lineSize > 0 ) m_LineSize = lineSize; }
	CSize		GetPaddingSize( CDC *pDC );
	void		CopyAxis( CAxis *axis );
	void		AddAxisPopupMenus( CMenu *menu, int startVal );
	void		AddNumberOfTicksPopup( CMenu *addMenu, CString titleString, long compare, int startVal );
	int			InterpretAxisPopupMenuItem( int which );

	CSize		GetMinDisplaySize( CDC *pDC );
};
