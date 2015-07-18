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
// DataSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Chart.h"
#include "ColorControl.h"
#include "DataSettingsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataSettingsDialog dialog


CDataSettingsDialog::CDataSettingsDialog(CWnd* pParent, CXYChart *chart)
	: CDialog(CDataSettingsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataSettingsDialog)
	m_ChartType = -1;
	m_Title = _T("");
	m_MarkerShape = -1;
	m_MarkerFrequencyNumber = 0;
	m_MarkerFrequencyTotal = 0;
	m_MarkerSize = 0;
	m_DataLineSize = 0;
	m_MarkerFill = FALSE;
	m_DataLineStyle = 0;
	//}}AFX_DATA_INIT

	m_pChart = chart;

	tempChart.CopyChartData( chart );
}


void CDataSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataSettingsDialog)
	DDX_Control(pDX, IDC_DATASET_LABEL, m_DataSetLabel);
	DDX_CBIndex(pDX, IDC_CHART_TYPE, m_ChartType);
	DDX_Text(pDX, IDC_DS_DATA_TITLE, m_Title);
	DDX_CBIndex(pDX, IDC_MARKER_SHAPE, m_MarkerShape);
	DDX_Text(pDX, IDC_MARKER_FREQUENCY_NUMBER, m_MarkerFrequencyNumber);
	DDX_Text(pDX, IDC_MARKER_FREQUENCY_TOTAL_DATA, m_MarkerFrequencyTotal);
	DDX_Text(pDX, IDC_MARKER_SIZE, m_MarkerSize);
	DDX_Text(pDX, IDC_DATA_LINE_SIZE, m_DataLineSize);
	DDV_MinMaxUInt(pDX, m_DataLineSize, 0, 1000);
	DDX_Check(pDX, IDC_DATA_MARKER_FILL, m_MarkerFill);
	DDX_CBIndex(pDX, IDC_DATA_LINESTYLE, m_DataLineStyle);
	//}}AFX_DATA_MAP

	// Add these to the data exchange to get the colors from
	// the color control
	if( pDX->m_bSaveAndValidate == TRUE )
	{
		m_Color = CColorControl::GetColorControlColor( this, IDC_DS_COLOR_LINE );
	}
	else // put things into dialog
	{
		CColorControl::SetColorControlColor( this, IDC_DS_COLOR_LINE, m_Color );
	}
}


BEGIN_MESSAGE_MAP(CDataSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CDataSettingsDialog)
	ON_EN_CHANGE(IDC_MARKER_FREQUENCY_NUMBER, OnChangeMarkerFrequencyNumber)
	ON_EN_CHANGE(IDC_MARKER_FREQUENCY_TOTAL_DATA, OnChangeMarkerFrequencyTotalData)
	ON_BN_CLICKED(IDC_DATA_NEXT, OnDataNext)
	ON_BN_CLICKED(IDC_DATA_PREVIOUS, OnDataPrevious)
	ON_BN_CLICKED(IDC_MARKER_FREQUENCY_EVERY, OnMarkerFrequencyEvery)
	ON_BN_CLICKED(IDC_MARKER_FREQUENCY_NTH, OnMarkerFrequencyNth)
	ON_BN_CLICKED(IDC_MARKER_FREQUENCY_TOTAL, OnMarkerFrequencyTotal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataSettingsDialog message handlers

BOOL CDataSettingsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetupDataSettings();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataSettingsDialog::OnOK() 
{
	SaveDataSettings();

	m_pChart->CopyChartData( &tempChart );

	CDialog::OnOK();
}

// Sets up the radio buttons for marker frequency
void CDataSettingsDialog::OnChangeMarkerFrequencyNumber() 
{
	// Set the radio buttons
	SetRadioButton( IDC_MARKER_FREQUENCY_EVERY, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_NTH, TRUE );
	SetRadioButton( IDC_MARKER_FREQUENCY_TOTAL, FALSE );
}

// Sets up the radio buttons for marker frequency
void CDataSettingsDialog::OnChangeMarkerFrequencyTotalData() 
{
	// Set the radio buttons
	SetRadioButton( IDC_MARKER_FREQUENCY_EVERY, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_NTH, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_TOTAL, TRUE );
}

// Move to the next data set
void CDataSettingsDialog::OnDataNext() 
{
	SaveDataSettings();

	m_CurDataSet = (m_CurDataSet+1) % m_pChart->GetDataSetCount();

	SetupDataSettings();
}

// Move to the previous data set
void CDataSettingsDialog::OnDataPrevious() 
{
	SaveDataSettings();

	if( m_CurDataSet == 0 ) m_CurDataSet = m_pChart->GetDataSetCount()-1;
	else m_CurDataSet = (m_CurDataSet-1) % m_pChart->GetDataSetCount();

	SetupDataSettings();
}

// Set a radio button
void	CDataSettingsDialog::SetRadioButton( int buttonID, BOOL how )
{
	CButton			*button = (CButton*)GetDlgItem( buttonID );

	if( button != NULL )
		button->SetCheck( how );
}

// Get that status of a radio button
BOOL	CDataSettingsDialog::GetRadioButton( int buttonID )
{
	CButton			*button = (CButton*)GetDlgItem( buttonID );

	if( button != NULL )
		return button->GetCheck();
	else return FALSE;
}

// Set up the dialog box with the currently selected settings
void	CDataSettingsDialog::SetupDataSettings( void )
{
	int		setVal = 0;
	CString		string;

	if( m_CurDataSet < 0 ) m_CurDataSet = 0;
	if( m_CurDataSet > m_pChart->GetDataSetCount() - 1 ) m_CurDataSet = m_pChart->GetDataSetCount() - 1;

	string.Format( L"Data set %d of %d", m_CurDataSet+1, m_pChart->GetDataSetCount() );
	m_DataSetLabel.SetWindowText( string );

	switch( tempChart.m_MarkerType[m_CurDataSet] )
	{
	case kXYMarkerNone: setVal = 0; break;
	case kXYMarkerSquare: setVal = 1; break;
	case kXYMarkerTriangle: setVal = 2; break;
	case kXYMarkerCircle: setVal = 3; break;
	case kXYMarkerX: setVal = 4; break;
	}

	m_MarkerShape = setVal;

	switch( tempChart.m_ChartType[m_CurDataSet] )
	{
	case kXYChartPlain: setVal = 0; break;
	case kXYChartConnect: setVal = 1; break; 
	case kXYChartHistogram: setVal = 2; break; 
	default: setVal = 0; break;
	}

	m_ChartType = setVal;

	m_Color = tempChart.m_PlotColor[m_CurDataSet];

	m_Title = tempChart.m_DataTitle[m_CurDataSet];

	m_MarkerSize = tempChart.m_MarkerSize[m_CurDataSet] / 10;

	m_MarkerFill = tempChart.m_MarkerFill[m_CurDataSet];

	m_DataLineSize = tempChart.m_DataLineSize[m_CurDataSet];

	m_DataLineStyle = tempChart.m_DataLineStyle[m_CurDataSet];

	SetRadioButton( IDC_MARKER_FREQUENCY_EVERY, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_NTH, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_TOTAL, FALSE );

	setVal = tempChart.m_MarkerFrequency[m_CurDataSet];

	if( setVal == 1 || setVal == 0 )
	{
		SetRadioButton( IDC_MARKER_FREQUENCY_EVERY, TRUE );
		m_MarkerFrequencyNumber = 10;
		m_MarkerFrequencyTotal = 10;
	} else if( setVal > 0 )
	{
		SetRadioButton( IDC_MARKER_FREQUENCY_NTH, TRUE );
		m_MarkerFrequencyNumber = setVal;
		m_MarkerFrequencyTotal = 10;
	} else if( setVal < 0 )
	{
		SetRadioButton( IDC_MARKER_FREQUENCY_TOTAL, TRUE );
		m_MarkerFrequencyNumber = 10;
		m_MarkerFrequencyTotal = abs(setVal);
	}

	UpdateData( FALSE );
}

// Save the current dialog box information to permanent storage
void	CDataSettingsDialog::SaveDataSettings( void )
{
	if( UpdateData( TRUE ) == FALSE) return;

	if( m_CurDataSet < 0 ) m_CurDataSet = 0;
	if( m_CurDataSet > m_pChart->GetDataSetCount() - 1 ) m_CurDataSet = m_pChart->GetDataSetCount() - 1;

	switch( m_MarkerShape )
	{
	case 0: tempChart.m_MarkerType[m_CurDataSet] = kXYMarkerNone; break;
	case 1: tempChart.m_MarkerType[m_CurDataSet] = kXYMarkerSquare; break;
	case 2: tempChart.m_MarkerType[m_CurDataSet] = kXYMarkerTriangle; break;
	case 3: tempChart.m_MarkerType[m_CurDataSet] = kXYMarkerCircle; break;
	case 4: tempChart.m_MarkerType[m_CurDataSet] = kXYMarkerX; break;
	}

	switch( m_ChartType )
	{
	case 0: tempChart.m_ChartType[m_CurDataSet] = kXYChartPlain; break;
	case 1: tempChart.m_ChartType[m_CurDataSet] = kXYChartConnect; break;
	case 2: tempChart.m_ChartType[m_CurDataSet] = kXYChartHistogram; break;
	}

	tempChart.m_PlotColor[m_CurDataSet] = m_Color;

	tempChart.m_DataTitle[m_CurDataSet] = m_Title;

	tempChart.m_MarkerSize[m_CurDataSet] = 10 * m_MarkerSize;

	tempChart.m_MarkerFill[m_CurDataSet] = m_MarkerFill;

	tempChart.m_DataLineSize[m_CurDataSet] = m_DataLineSize;

	tempChart.m_DataLineStyle[m_CurDataSet] = m_DataLineStyle;

	if( GetRadioButton( IDC_MARKER_FREQUENCY_EVERY ) == TRUE )
	{
		tempChart.m_MarkerFrequency[m_CurDataSet] = 1;
	}
	else if( GetRadioButton( IDC_MARKER_FREQUENCY_NTH ) == TRUE )
	{
		tempChart.m_MarkerFrequency[m_CurDataSet] = m_MarkerFrequencyNumber;
	} else
	{
		tempChart.m_MarkerFrequency[m_CurDataSet] = -(int)(m_MarkerFrequencyTotal);
	}
}

// Set up radio buttons for marker frequency
void CDataSettingsDialog::OnMarkerFrequencyEvery() 
{
	SetRadioButton( IDC_MARKER_FREQUENCY_EVERY, TRUE );
	SetRadioButton( IDC_MARKER_FREQUENCY_NTH, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_TOTAL, FALSE );
}

// Set up radio buttons for marker frequency
void CDataSettingsDialog::OnMarkerFrequencyNth() 
{
	SetRadioButton( IDC_MARKER_FREQUENCY_EVERY, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_NTH, TRUE );
	SetRadioButton( IDC_MARKER_FREQUENCY_TOTAL, FALSE );
}

// Set up radio buttons for marker frequency
void CDataSettingsDialog::OnMarkerFrequencyTotal() 
{
	SetRadioButton( IDC_MARKER_FREQUENCY_EVERY, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_NTH, FALSE );
	SetRadioButton( IDC_MARKER_FREQUENCY_TOTAL, TRUE );
}
