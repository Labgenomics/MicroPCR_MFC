#if !defined(AFX_DATASETTINGSDIALOG_H__E4B2FFF9_0BE9_48AC_AD67_930A62C34332__INCLUDED_)
#define AFX_DATASETTINGSDIALOG_H__E4B2FFF9_0BE9_48AC_AD67_930A62C34332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataSettingsDialog.h : header file
//

#include "Chart.h"

/////////////////////////////////////////////////////////////////////////////
// CDataSettingsDialog dialog

class CDataSettingsDialog : public CDialog
{
// Construction
public:
	CDataSettingsDialog(CWnd* pParent, CXYChart *chart);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataSettingsDialog)
	enum { IDD = IDD_DATA_SETTINGS };
	CButton	m_DataSetLabel;
	int		m_ChartType;
	CString	m_Title;
	int		m_MarkerShape;
	UINT	m_MarkerFrequencyNumber;
	UINT	m_MarkerFrequencyTotal;
	UINT	m_MarkerSize;
	UINT	m_DataLineSize;
	BOOL	m_MarkerFill;
	int		m_DataLineStyle;
	//}}AFX_DATA

	// Needs to be public to allow for opening
	// the dialog box to other than the 0th
	// data set
	int				m_CurDataSet;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void	SetRadioButton( int buttonID, BOOL how );
	BOOL	GetRadioButton( int buttonID );
	void	SetupDataSettings( void );
	void	SaveDataSettings( void );

	CXYChart		*m_pChart, tempChart;
	COLORREF		m_Color;

	// Generated message map functions
	//{{AFX_MSG(CDataSettingsDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeMarkerFrequencyNumber();
	afx_msg void OnChangeMarkerFrequencyTotalData();
	afx_msg void OnDataNext();
	afx_msg void OnDataPrevious();
	afx_msg void OnMarkerFrequencyEvery();
	afx_msg void OnMarkerFrequencyNth();
	afx_msg void OnMarkerFrequencyTotal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATASETTINGSDIALOG_H__E4B2FFF9_0BE9_48AC_AD67_930A62C34332__INCLUDED_)
