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

// ImagePlotSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ColorControl.h"
#include "CPlotUtility.h"
#include "ImagePlotSettingsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImagePlotSettingsDialog dialog


CImagePlotSettingsDialog::CImagePlotSettingsDialog(CWnd* pParent, CImageChart *chart)
	: CDialog(CImagePlotSettingsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImagePlotSettingsDialog)
	m_MaintainAspectRatio = FALSE;
	m_MinorHorizontalGrids = FALSE;
	m_MinorVerticalGrids = FALSE;
	m_MajorHorizontalGrids = FALSE;
	m_MajorVerticalGrids = FALSE;
	m_Title = _T("");
	m_TitleFontDescription = _T("");
	m_PlotToWindowSize = FALSE;
	m_MajorGridSize = 0;
	m_MinorGridSize = 0;
	m_MajorGridLinestyle = 0;
	m_MinorGridLinestyle = 0;
	//}}AFX_DATA_INIT

	m_pChart = chart;
}


void CImagePlotSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImagePlotSettingsDialog)
	DDX_Control(pDX, IDC_MAINTAIN_ASPECT_RATIO, m_MaintainAspectRatioControl);
	DDX_Check(pDX, IDC_MAINTAIN_ASPECT_RATIO, m_MaintainAspectRatio);
	DDX_Check(pDX, IDC_MINOR_HORIZONTAL_GRIDS, m_MinorHorizontalGrids);
	DDX_Check(pDX, IDC_MINOR_VERTICAL_GRIDS, m_MinorVerticalGrids);
	DDX_Check(pDX, IDC_PS_MAJOR_HORIZONTAL_GRIDS, m_MajorHorizontalGrids);
	DDX_Check(pDX, IDC_PS_MAJOR_VERTICAL_GRIDS, m_MajorVerticalGrids);
	DDX_Text(pDX, IDC_PS_PLOT_TITLE, m_Title);
	DDX_Text(pDX, IDC_PS_TITLE_FONT, m_TitleFontDescription);
	DDX_Check(pDX, IDC_PLOT_TO_SIZE, m_PlotToWindowSize);
	DDX_Text(pDX, IDC_MAJOR_GRID_SIZE, m_MajorGridSize);
	DDV_MinMaxUInt(pDX, m_MajorGridSize, 0, 1000);
	DDX_Text(pDX, IDC_MINOR_GRID_SIZE, m_MinorGridSize);
	DDV_MinMaxUInt(pDX, m_MinorGridSize, 0, 1000);
	DDX_CBIndex(pDX, IDC_PLOT_MAJOR_LINESTYLE, m_MajorGridLinestyle);
	DDX_CBIndex(pDX, IDC_PLOT_MINOR_LINESTYLE, m_MinorGridLinestyle);
	//}}AFX_DATA_MAP

	// Set up our color controls to use DDX
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


BEGIN_MESSAGE_MAP(CImagePlotSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CImagePlotSettingsDialog)
	ON_BN_CLICKED(IDC_PLOT_TO_SIZE, OnPlotToSize)
	ON_BN_CLICKED(IDC_PS_CHOOSE_TITLE_FONT, OnPsChooseTitleFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagePlotSettingsDialog message handlers

// Set up the dialog box
BOOL CImagePlotSettingsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_MajorGridColor = m_pChart->m_MajorGridColor;
	m_MinorGridColor = m_pChart->m_MinorGridColor;
	m_BackgroundColor = m_pChart->m_BackgroundColor;
	m_LineColor = m_pChart->m_LineColor;
	m_TitleColor = m_pChart->m_TitleColor;

	m_MinorHorizontalGrids = m_pChart->m_UseMinorHorizontalGrids;
	m_MinorVerticalGrids = m_pChart->m_UseMinorVerticalGrids;
	m_MajorHorizontalGrids = m_pChart->m_UseMajorHorizontalGrids;
	m_MajorVerticalGrids = m_pChart->m_UseMajorVerticalGrids;

	m_MajorGridSize = m_pChart->m_MajorGridLineSize;
	m_MinorGridSize = m_pChart->m_MinorGridLineSize;

	m_MajorGridLinestyle = m_pChart->m_MajorGridLineStyle;
	m_MinorGridLinestyle = m_pChart->m_MinorGridLineStyle;

	m_Title = m_pChart->m_Title;
	m_TitleFontDescription = GetFontDescription( &(m_pChart->m_TitleFont) );
	memcpy( &m_Font, &m_pChart->m_TitleFont, sizeof( LOGFONT ) );

	m_MaintainAspectRatio = m_pChart->m_MaintainAspectRatio;
	m_PlotToWindowSize = m_pChart->m_PlotToWindowSize;

	m_MaintainAspectRatioControl.EnableWindow( (m_PlotToWindowSize) );

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Save the dialog box information to permanent storage
void CImagePlotSettingsDialog::OnOK() 
{
	if( UpdateData( TRUE ) == FALSE ) return;

	CImagePlotSettingsDialog		*dlg = this;
	
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

	m_pChart->m_Title = dlg->m_Title;
	
	// Copy the font
	memcpy( &m_pChart->m_TitleFont, &dlg->m_Font, sizeof( LOGFONT ) );
	
	m_pChart->m_MaintainAspectRatio = m_MaintainAspectRatio;
	m_pChart->m_PlotToWindowSize = m_PlotToWindowSize;

	CDialog::OnOK();
}

// If user clicks plot to window size, enable or disable
// aspect ratio check box
void CImagePlotSettingsDialog::OnPlotToSize() 
{
	CButton			*button = (CButton*)GetDlgItem( IDC_PLOT_TO_SIZE );

	if( button != NULL )
	{
		m_MaintainAspectRatioControl.EnableWindow( button->GetCheck() );
	}
}

// Show the user a font selection dialog box
void CImagePlotSettingsDialog::OnPsChooseTitleFont() 
{
	CClientDC	dc(this);

	int			oldHeight = m_Font.lfHeight;

	m_Font.lfHeight = -MulDiv(oldHeight, dc.GetDeviceCaps(LOGPIXELSY), 72);

	CFontDialog		dlg( &m_Font );

	if( dlg.DoModal() == IDOK )
	{
		memcpy( &m_Font, dlg.m_cf.lpLogFont, sizeof( LOGFONT ) );
		m_Font.lfHeight = -MulDiv(m_Font.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY));
		m_TitleFontDescription = GetFontDescription( &m_Font );
		UpdateData( FALSE );
	}
}
