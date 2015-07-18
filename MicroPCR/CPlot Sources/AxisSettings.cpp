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
#include "Axis.h"
#include "ColorControl.h"
#include "CPlotUtility.h"
#include "AxisSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAxisSettings dialog


CAxisSettings::CAxisSettings(CWnd* pParent, CChart *chart)
	: CDialog(CAxisSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAxisSettings)
	m_AutoMajorTicks = FALSE;
	m_AutoMinorTicks = FALSE;
	m_AutoRange = FALSE;
	m_MajorTickLength = 0;
	m_MinorTickLength = 0;
	m_NumMinorTicks = 0;
	m_Title = _T("");
	m_RangeHigh = 0.0f;
	m_RangeLow = 0.0f;
	m_NumMajorTicks = 0;
	m_TitleFontDescription = _T("");
	m_LabelFontDescription = _T("");
	//}}AFX_DATA_INIT

	m_pChart = chart;
	m_CurAxis = 0;

	// we have to copy the data from all the CAxes to let the user
	// modify more than one at a time without having to ask
	// each time they change axes "do you want to save these changes?"
	for( int i = 0; i < m_pChart->GetAxisCount(); i++ )
	{
		m_Axes[i].CopyAxis( m_pChart->GetAxis(i) );
	}
}


void CAxisSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAxisSettings)
	DDX_Control(pDX, IDC_AXIS_LABEL, m_AxisLabel);
	DDX_Check(pDX, IDC_AS_AUTO_MAJOR_TICKS, m_AutoMajorTicks);
	DDX_Check(pDX, IDC_AS_AUTO_MINOR_TICKS, m_AutoMinorTicks);
	DDX_Check(pDX, IDC_AS_AUTO_XRANGE, m_AutoRange);
	DDX_Text(pDX, IDC_AS_MAJOR_TICK_LENGTH, m_MajorTickLength);
	DDX_Text(pDX, IDC_AS_MINOR_TICK_LENGTH, m_MinorTickLength);
	DDX_Text(pDX, IDC_AS_NUMBER_MINOR_TICKS, m_NumMinorTicks);
	DDX_Text(pDX, IDC_AS_PLOT_TITLE, m_Title);
	DDX_Text(pDX, IDC_AS_XRANGE_HIGH, m_RangeHigh);
	DDX_Text(pDX, IDC_AS_XRANGE_LOW, m_RangeLow);
	DDX_Text(pDX, IDC_NUMBER_MAJOR_TICKS, m_NumMajorTicks);
	DDX_Text(pDX, IDC_PS_TITLE_FONT, m_TitleFontDescription);
	DDX_Text(pDX, IDC_AS_LABEL_FONT, m_LabelFontDescription);
	//}}AFX_DATA_MAP

	if( pDX->m_bSaveAndValidate == TRUE )
	{
		m_LineColor = CColorControl::GetColorControlColor( this, IDC_AS_COLOR_LINE );
		m_LabelColor = CColorControl::GetColorControlColor( this, IDC_AS_COLOR_TITLE_FONT );
	}
	else // put things into dialog
	{
		CColorControl::SetColorControlColor( this, IDC_AS_COLOR_LINE, m_LineColor );
		CColorControl::SetColorControlColor( this, IDC_AS_COLOR_TITLE_FONT, m_LabelColor );
	}
}


BEGIN_MESSAGE_MAP(CAxisSettings, CDialog)
	//{{AFX_MSG_MAP(CAxisSettings)
	ON_BN_CLICKED(IDC_AS_CHOOSE_TITLE_FONT, OnChooseTitleFont)
	ON_BN_CLICKED(IDC_AXIS_NEXT, OnAxisNext)
	ON_BN_CLICKED(IDC_AXIS_PREVIOUS, OnAxisPrevious)
	ON_BN_CLICKED(IDC_AS_CHOOSE_LABEL_FONT, OnChooseLabelFont)
	ON_EN_CHANGE(IDC_AS_NUMBER_MINOR_TICKS, OnChangeNumberMinorTicks)
	ON_EN_CHANGE(IDC_NUMBER_MAJOR_TICKS, OnChangeNumberMajorTicks)
	ON_EN_CHANGE(IDC_AS_XRANGE_HIGH, OnChangeXrangeHigh)
	ON_EN_CHANGE(IDC_AS_XRANGE_LOW, OnChangeXrangeLow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAxisSettings message handlers

// Give the user a dialog box to choose font characteristics
void CAxisSettings::OnChooseTitleFont() 
{
	OnChooseFont( m_Font, m_TitleFontDescription );
}

// Give the user a dialog box to choose font characteristics
void CAxisSettings::OnChooseLabelFont() 
{
	OnChooseFont( m_LabelFont, m_LabelFontDescription );
}

// Sets up a font dialog box for the user, and then
// gets a font description as well
void	CAxisSettings::OnChooseFont( LOGFONT &font, CString &description )
{
	CClientDC	dc(this);

	int			oldHeight = font.lfHeight;

	// Do some math in order to get the font size in points
	font.lfHeight = -MulDiv(oldHeight, dc.GetDeviceCaps(LOGPIXELSY), 72);

	CFontDialog		dlg( &font );

	if( dlg.DoModal() == IDOK )
	{
		memcpy( &font, dlg.m_cf.lpLogFont, sizeof( LOGFONT ) );
		font.lfHeight = -MulDiv(font.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY));
		description = GetFontDescription( &font );
		UpdateData( FALSE );
	}
}

// Move to the next axis in the dialog box
void CAxisSettings::OnAxisNext() 
{
	SaveDataFromAxis();

	m_CurAxis = (m_CurAxis+1) % m_pChart->GetAxisCount();

	SetupDataFromAxis();
}

// Move to the previous axis in the dialog box
void CAxisSettings::OnAxisPrevious() 
{
	SaveDataFromAxis();

	if( m_CurAxis == 0 ) m_CurAxis = m_pChart->GetAxisCount()-1;
	else m_CurAxis = (m_CurAxis-1) % m_pChart->GetAxisCount();

	SetupDataFromAxis();
}

BOOL CAxisSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetupDataFromAxis();

	return TRUE;  // return TRUE unless you set the focus to a control
}

// This copies the data from the axis passed to the constructor
void	CAxisSettings::SetupDataFromAxis( void )
{
	CAxis	*axis = &m_Axes[m_CurAxis];

	if( axis != NULL )
	{
		m_AutoMajorTicks = axis->m_TickCount < 0;
		m_AutoMinorTicks = axis->m_MinorTickCount < 0;
		m_AutoRange = !axis->IsRangeSet();
		m_MajorTickLength = axis->m_TickLength/10;
		m_MinorTickLength = axis->m_MinorTickLength/10;
		m_NumMinorTicks = axis->GetMinorTickCount();
		axis->GetTitle(m_Title);
		m_RangeHigh = (float)axis->GetUpperRange();
		m_RangeLow = (float)axis->GetLowerRange();
		m_NumMajorTicks = axis->GetTickCount()+1;
		m_TitleFontDescription = GetFontDescription( &axis->m_TitleFont );
		memcpy( &m_Font, &axis->m_TitleFont, sizeof( LOGFONT ) );

		m_LabelFontDescription = GetFontDescription( &axis->m_LabelFont );
		memcpy( &m_LabelFont, &axis->m_LabelFont, sizeof( LOGFONT ) );

		m_LineColor = axis->GetColor();
		m_LabelColor = axis->m_TitleColor;

		m_AxisLabel.SetWindowText( GetMainLabel() );
		UpdateData( FALSE );
	}
		
}

// This copies the dialog data back to the temporary axis
void	CAxisSettings::SaveDataFromAxis( void )
{
	CAxis	*axis = &m_Axes[m_CurAxis];

	if( UpdateData( TRUE ) == FALSE ) return;

	if( axis != NULL )
	{
		if( m_AutoRange == TRUE )
			axis->m_RangeSet = FALSE;
		else
			axis->SetRange( TRUE, m_RangeLow, m_RangeHigh );

		axis->m_TickLength = 10 * m_MajorTickLength;
		axis->m_MinorTickLength = 10 * m_MinorTickLength;

		axis->m_MinorTickCount = abs((int)m_NumMinorTicks);
		axis->m_TickCount = abs((int)m_NumMajorTicks)-1;

		if( m_AutoMajorTicks ) axis->m_TickCount = -(axis->m_TickCount);
		if( m_AutoMinorTicks ) axis->m_MinorTickCount = -axis->m_MinorTickCount;
		axis->SetTitle( m_Title );

		memcpy( &axis->m_TitleFont, &m_Font, sizeof( LOGFONT ) );

		memcpy( &axis->m_LabelFont, &m_LabelFont, sizeof( LOGFONT ) );

		axis->SetColor( m_LineColor );
		axis->m_TitleColor = m_LabelColor;
	}
}

// Gets a label for which axis this is
CString	CAxisSettings::GetMainLabel( void )
{
	CString		string, location;
	CAxis		*axis = &m_Axes[m_CurAxis];

	if( axis != NULL )
	{
		switch( axis->GetLocation() )
		{
		case kLocationBottom: location = L"bottom"; break;
		case kLocationLeft: location = L"left"; break;
		case kLocationRight: location = L"right"; break;
		case kLocationTop: location = L"top"; break;
		default: break;
		}
	}
	else location = L"";

	string.Format( L"Axis %d of %d, (%s)", m_CurAxis+1, m_pChart->m_AxisCount, location );

	return string;
}

// User pressed OK, save our data, copy it back to the chart
// axes, and go home.
void CAxisSettings::OnOK() 
{
	// Record the data
	SaveDataFromAxis();

	// Copy the data back to the plot
	for( int i = 0; i < m_pChart->GetAxisCount(); i++ )
	{
		m_pChart->GetAxis(i)->CopyAxis( &m_Axes[i] );
	}

	CDialog::OnOK();
}

// If user typed into minor ticks edit, it is no longer "auto"
// Change the check box to reflect this
void CAxisSettings::OnChangeNumberMinorTicks() 
{
	CButton		*button = (CButton*)GetDlgItem( IDC_AS_AUTO_MINOR_TICKS );

	if( button != NULL )
	{
		button->SetCheck( FALSE );
	}
}

// If user typed into major ticks edit, it is no longer "auto"
// Change the check box to reflect this
void CAxisSettings::OnChangeNumberMajorTicks() 
{	
	CButton		*button = (CButton*)GetDlgItem( IDC_AS_AUTO_MAJOR_TICKS );

	if( button != NULL )
	{
		button->SetCheck( FALSE );
	}
}

// If user typed into the range edit box, it is no longer "auto"
// Change the check box to reflect this
void CAxisSettings::OnChangeXrangeHigh() 
{
	CButton		*button = (CButton*)GetDlgItem( IDC_AS_AUTO_XRANGE );

	if( button != NULL )
	{
		button->SetCheck( FALSE );
	}
}

// If user typed into the range edit box, it is no longer "auto"
// Change the check box to reflect this
void CAxisSettings::OnChangeXrangeLow() 
{
	CButton		*button = (CButton*)GetDlgItem( IDC_AS_AUTO_XRANGE );

	if( button != NULL )
	{
		button->SetCheck( FALSE );
	}
}

