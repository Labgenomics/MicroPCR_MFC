#if !defined(AFX_IMAGEPLOTSETTINGSDIALOG_H__F4DE25A0_F030_4BDF_9153_28B11BD0A6A3__INCLUDED_)
#define AFX_IMAGEPLOTSETTINGSDIALOG_H__F4DE25A0_F030_4BDF_9153_28B11BD0A6A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImagePlotSettingsDialog.h : header file
//

#include "Chart.h"

/////////////////////////////////////////////////////////////////////////////
// CImagePlotSettingsDialog dialog

class CImagePlotSettingsDialog : public CDialog
{
// Construction
public:
	CImagePlotSettingsDialog(CWnd* pParent, CImageChart *chart );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImagePlotSettingsDialog)
	enum { IDD = IDD_IMAGE_SETTINGS };
	CButton	m_MaintainAspectRatioControl;
	BOOL	m_MaintainAspectRatio;
	BOOL	m_MinorHorizontalGrids;
	BOOL	m_MinorVerticalGrids;
	BOOL	m_MajorHorizontalGrids;
	BOOL	m_MajorVerticalGrids;
	CString	m_Title;
	CString	m_TitleFontDescription;
	BOOL	m_PlotToWindowSize;
	UINT	m_MajorGridSize;
	UINT	m_MinorGridSize;
	int		m_MajorGridLinestyle;
	int		m_MinorGridLinestyle;
	//}}AFX_DATA

	COLORREF	m_MajorGridColor, m_MinorGridColor, m_BackgroundColor;
	COLORREF	m_LineColor, m_TitleColor;

	LOGFONT		m_Font;

	CImageChart	*m_pChart;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagePlotSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImagePlotSettingsDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPlotToSize();
	afx_msg void OnPsChooseTitleFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEPLOTSETTINGSDIALOG_H__F4DE25A0_F030_4BDF_9153_28B11BD0A6A3__INCLUDED_)
