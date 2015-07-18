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

// PlotSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ColorControl.h"
#include "CPlotUtility.h"
#include "PlotSettingsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotSettingsDialog dialog


CPlotSettingsDialog::CPlotSettingsDialog(CWnd* pParent, CXYChart *chart )
	: CDialog(CPlotSettingsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlotSettingsDialog)
	m_MinorHorizontalGrids = FALSE;
	m_MinorVerticalGrids = FALSE;
	m_AutoXRange = FALSE;
	m_AutoYRange = FALSE;
	m_MajorHorizontalGrids = FALSE;
	m_MajorVerticalGrids = FALSE;
	m_PlotTitle = _T("");
	m_TitleFont = _T("");
	m_XRangeHigh = 0.0f;
	m_XRangeLow = 0.0f;
	m_YRangeHigh = 0.0f;
	m_YRangeLow = 0.0f;
	m_MajorGridSize = 0;
	m_MinorGridSize = 0;
	m_MajorGridLinestyle = 0;
	m_MinorGridLinestyle = 0;
	//}}AFX_DATA_INIT

	m_pChart = chart;
}


void CPlotSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotSettingsDialog)
	DDX_Control(pDX, IDC_PS_YRANGE_LOW, m_YRangeLowControl);
	DDX_Control(pDX, IDC_PS_YRANGE_HIGH, m_YRangeHighControl);
	DDX_Control(pDX, IDC_PS_XRANGE_LOW, m_XRangeLowControl);
	DDX_Control(pDX, IDC_PS_XRANGE_HIGH, m_XRangeHighControl);
	DDX_Control(pDX, IDC_PS_CHOOSE_TITLE_FONT, m_ChooseTitleFont);
	DDX_Check(pDX, IDC_MINOR_HORIZONTAL_GRIDS, m_MinorHorizontalGrids);
	DDX_Check(pDX, IDC_MINOR_VERTICAL_GRIDS, m_MinorVerticalGrids);
	DDX_Check(pDX, IDC_PS_AUTO_XRANGE, m_AutoXRange);
	DDX_Check(pDX, IDC_PS_AUTO_YRANGE, m_AutoYRange);
	DDX_Check(pDX, IDC_PS_MAJOR_HORIZONTAL_GRIDS, m_MajorHorizontalGrids);
	DDX_Check(pDX, IDC_PS_MAJOR_VERTICAL_GRIDS, m_MajorVerticalGrids);
	DDX_Text(pDX, IDC_PS_PLOT_TITLE, m_PlotTitle);
	DDX_Text(pDX, IDC_PS_TITLE_FONT, m_TitleFont);
	DDX_Text(pDX, IDC_PS_XRANGE_HIGH, m_XRangeHigh);
	DDX_Text(pDX, IDC_PS_XRANGE_LOW, m_XRangeLow);
	DDX_Text(pDX, IDC_PS_YRANGE_HIGH, m_YRangeHigh);
	DDX_Text(pDX, IDC_PS_YRANGE_LOW, m_YRangeLow);
	DDX_Text(pDX, IDC_MAJOR_GRID_SIZE, m_MajorGridSize);
	DDV_MinMaxUInt(pDX, m_MajorGridSize, 0, 1000);
	DDX_Text(pDX, IDC_MINOR_GRID_SIZE, m_MinorGridSize);
	DDV_MinMaxUInt(pDX, m_MinorGridSize, 0, 1000);
	DDX_CBIndex(pDX, IDC_PLOT_MAJOR_LINESTYLE, m_MajorGridLinestyle);
	DDX_CBIndex(pDX, IDC_PLOT_MINOR_LINESTYLE, m_MinorGridLinestyle);
	//}}AFX_DATA_MAP

	// Set up the color controls to use DDX
	if( pDX->m_bSaveAndValidate == TRUE )
	{
		m_MajorGridColor = CColorControl::GetColorControlColor( this, IDC_PS_COLOR_MAJOR_GRIDS );
		m_MinorGridColor = CColorControl::GetColorControlColor( this, IDC_PS_COLOR_MINOR_GRIDS );
		m_BackgroundColor = CColorControl::GetColorControlColor( this, IDC_PS_COLOR_BACKGROUND );
		m_LineColor = CColorControl::GetColorControlColor( this, IDC_PS_COLOR_BORDER );
		m_TitleColor = CColorControl::GetColorControlColor( this, IDC_PS_COLOR_TITLE_FONT );
	}
	else // put things into dialog
	{
		CColorControl::SetColorControlColor( this, IDC_PS_COLOR_MAJOR_GRIDS, m_MajorGridColor );
		CColorControl::SetColorControlColor( this, IDC_PS_COLOR_MINOR_GRIDS, m_MinorGridColor );
		CColorControl::SetColorControlColor( this, IDC_PS_COLOR_BACKGROUND, m_BackgroundColor );
		CColorControl::SetColorControlColor( this, IDC_PS_COLOR_BORDER, m_LineColor );
		CColorControl::SetColorControlColor( this, IDC_PS_COLOR_TITLE_FONT, m_TitleColor );
	}

}


BEGIN_MESSAGE_MAP(CPlotSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CPlotSettingsDialog)
	ON_EN_CHANGE(IDC_PS_XRANGE_LOW, OnChangePSXrangeLow)
	ON_EN_CHANGE(IDC_PS_XRANGE_HIGH, OnChangePSXrangeHigh)
	ON_EN_CHANGE(IDC_PS_YRANGE_HIGH, OnChangePSYrangeHigh)
	ON_EN_CHANGE(IDC_PS_YRANGE_LOW, OnChangePSYrangeLow)
	ON_BN_CLICKED(IDC_PS_CHOOSE_TITLE_FONT, OnPSChooseTitleFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotSettingsDialog message handlers

// Set up the dialog box
BOOL CPlotSettingsDialog::OnInitDialog() 
{
	double		xRange[2], yRange[2];

	CDialog::OnInitDialog();

	m_MajorGridColor = m_pChart->m_MajorGridColor;
	m_MinorGridColor = m_pChart->m_MinorGridColor;
	m_BackgroundColor = m_pChart->m_BackgroundColor;
	m_LineColor = m_pChart->m_LineColor;
	m_TitleColor = m_pChart->m_TitleColor;

	m_MinorHorizontalGrids = m_pChart->m_UseMinorHorizontalGrids;
	m_MinorVerticalGrids = m_pChart->m_UseMinorVerticalGrids;
	m_MajorGridSize = m_pChart->m_MajorGridLineSize;
	m_MinorGridSize = m_pChart->m_MinorGridLineSize;

	m_MajorGridLinestyle = m_pChart->m_MajorGridLineStyle;
	m_MinorGridLinestyle = m_pChart->m_MinorGridLineStyle;

	m_AutoXRange = m_pChart->IsXAutoRange();
	m_AutoYRange = m_pChart->IsYAutoRange();
	m_MajorHorizontalGrids = m_pChart->m_UseMajorHorizontalGrids;
	m_MajorVerticalGrids = m_pChart->m_UseMajorVerticalGrids;
	m_PlotTitle = m_pChart->m_Title;
	m_TitleFont = GetFontDescription( &(m_pChart->m_TitleFont) );
	memcpy( &m_Font, &m_pChart->m_TitleFont, sizeof( LOGFONT ) );

	m_pChart->GetPlotRange( xRange, yRange );
	m_XRangeHigh = (float)xRange[1];
	m_XRangeLow = (float)xRange[0];
	m_YRangeHigh = (float)yRange[1];
	m_YRangeLow = (float)yRange[0];

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// If user types in range boxes, we are no longer
// on auto range, so set the check box
void CPlotSettingsDialog::OnChangePSXrangeLow() 
{
	// uncheck the auto for the x
	CButton		*button;

	button = (CButton*) GetDlgItem( IDC_PS_AUTO_XRANGE );
	if( button != NULL ) button->SetCheck( 0 );
}

// If user types in range boxes, we are no longer
// on auto range, so set the check box
void CPlotSettingsDialog::OnChangePSXrangeHigh() 
{
	// uncheck the auto for the x
	CButton		*button;

	button = (CButton*) GetDlgItem( IDC_PS_AUTO_XRANGE );
	if( button != NULL ) button->SetCheck( 0 );
}

// If user types in range boxes, we are no longer
// on auto range, so set the check box
void CPlotSettingsDialog::OnChangePSYrangeHigh() 
{
	// uncheck the auto for the x
	CButton		*button;

	button = (CButton*) GetDlgItem( IDC_PS_AUTO_YRANGE );
	if( button != NULL ) button->SetCheck( 0 );
}

// If user types in range boxes, we are no longer
// on auto range, so set the check box
void CPlotSettingsDialog::OnChangePSYrangeLow() 
{
	// uncheck the auto for the x
	CButton		*button;

	button = (CButton*) GetDlgItem( IDC_PS_AUTO_YRANGE );
	if( button != NULL ) button->SetCheck( 0 );
}


// Show a font selection dialog box for the user
void CPlotSettingsDialog::OnPSChooseTitleFont() 
{
	CClientDC	dc(this);

	int			oldHeight = m_Font.lfHeight;

	m_Font.lfHeight = -MulDiv(oldHeight, dc.GetDeviceCaps(LOGPIXELSY), 72);

	CFontDialog		dlg( &m_Font );

	if( dlg.DoModal() == IDOK )
	{
		memcpy( &m_Font, dlg.m_cf.lpLogFont, sizeof( LOGFONT ) );
		m_Font.lfHeight = -MulDiv(m_Font.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY));
		m_TitleFont = GetFontDescription( &m_Font );
		UpdateData( FALSE );
	}
}

// Save the dialog settings to permanent storage
void CPlotSettingsDialog::OnOK() 
{
	UpdateData( TRUE );

	CPlotSettingsDialog		*dlg = this;
	
	// TODO: Add extra validation here
	m_pChart->m_MajorGridColor = dlg->m_MajorGridColor;
	m_pChart->m_MinorGridColor = dlg->m_MinorGridColor;
	m_pChart->m_BackgroundColor = dlg->m_BackgroundColor;
	m_pChart->m_LineColor = dlg->m_LineColor;
	m_pChart->m_TitleColor = dlg->m_TitleColor;
	
	m_pChart->m_UseMinorHorizontalGrids = dlg->m_MinorHorizontalGrids;
	m_pChart->m_UseMinorVerticalGrids = dlg->m_MinorVerticalGrids;
	m_pChart->m_UseMajorHorizontalGrids = dlg->m_MajorHorizontalGrids;
	m_pChart->m_UseMajorVerticalGrids = dlg->m_MajorVerticalGrids;

	m_pChart->m_MinorGridLineSize = dlg->m_MinorGridSize;
	m_pChart->m_MajorGridLineSize = dlg->m_MajorGridSize;
	
	m_pChart->m_MinorGridLineStyle = dlg->m_MinorGridLinestyle;
	m_pChart->m_MajorGridLineStyle = dlg->m_MajorGridLinestyle;
	
	m_pChart->m_Title = dlg->m_PlotTitle;
	
	// Copy the font
	memcpy( &m_pChart->m_TitleFont, &dlg->m_Font, sizeof( LOGFONT ) );
	
	if( dlg->m_AutoXRange == FALSE )
		m_pChart->SetXRange( dlg->m_XRangeLow, dlg->m_XRangeHigh );
	else
		m_pChart->SetXAutoRange();
	
	if( dlg->m_AutoYRange == FALSE )
		m_pChart->SetYRange( dlg->m_YRangeLow, dlg->m_YRangeHigh );
	else
		m_pChart->SetYAutoRange();
	
	CDialog::OnOK();
}
