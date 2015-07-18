#if !defined(AFX_PLOTSETTINGSDIALOG_H__0A566F0A_2807_424B_BBA0_A7B94A325990__INCLUDED_)
#define AFX_PLOTSETTINGSDIALOG_H__0A566F0A_2807_424B_BBA0_A7B94A325990__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlotSettingsDialog.h : header file
//
#include "Chart.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotSettingsDialog dialog

class CPlotSettingsDialog : public CDialog
{
// Construction
public:
	CPlotSettingsDialog(CWnd* pParent, CXYChart *chart );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlotSettingsDialog)
	enum { IDD = IDD_XY_SETTINGS };
	CEdit	m_YRangeLowControl;
	CEdit	m_YRangeHighControl;
	CEdit	m_XRangeLowControl;
	CEdit	m_XRangeHighControl;
	CButton	m_ChooseTitleFont;
	BOOL	m_MinorHorizontalGrids;
	BOOL	m_MinorVerticalGrids;
	BOOL	m_AutoXRange;
	BOOL	m_AutoYRange;
	BOOL	m_MajorHorizontalGrids;
	BOOL	m_MajorVerticalGrids;
	CString	m_PlotTitle;
	CString	m_TitleFont;
	float	m_XRangeHigh;
	float	m_XRangeLow;
	float	m_YRangeHigh;
	float	m_YRangeLow;
	UINT	m_MajorGridSize;
	UINT	m_MinorGridSize;
	int		m_MajorGridLinestyle;
	int		m_MinorGridLinestyle;
	//}}AFX_DATA

	COLORREF	m_MajorGridColor, m_MinorGridColor, m_BackgroundColor;
	COLORREF	m_LineColor, m_TitleColor;

	LOGFONT		m_Font;

	CXYChart		*m_pChart;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CView		*m_ParentView;

	// Generated message map functions
	//{{AFX_MSG(CPlotSettingsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePSXrangeLow();
	afx_msg void OnChangePSXrangeHigh();
	afx_msg void OnChangePSYrangeHigh();
	afx_msg void OnChangePSYrangeLow();
	afx_msg void OnPSChooseTitleFont();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTSETTINGSDIALOG_H__0A566F0A_2807_424B_BBA0_A7B94A325990__INCLUDED_)
