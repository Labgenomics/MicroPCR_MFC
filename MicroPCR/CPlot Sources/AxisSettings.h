#if !defined(AFX_AXISSETTINGS_H__F2C6589E_94BD_4A95_9F8A_4DA329C89016__INCLUDED_)
#define AFX_AXISSETTINGS_H__F2C6589E_94BD_4A95_9F8A_4DA329C89016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AxisSettings.h : header file
//
#include "Chart.h"

/////////////////////////////////////////////////////////////////////////////
// CAxisSettings dialog

class CAxisSettings : public CDialog
{
// Construction
public:
	CAxisSettings(CWnd* pParent, CChart *chart);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAxisSettings)
	enum { IDD = IDD_AXIS_SETTINGS };
	CButton	m_AxisLabel;
	BOOL	m_AutoMajorTicks;
	BOOL	m_AutoMinorTicks;
	BOOL	m_AutoRange;
	UINT	m_MajorTickLength;
	UINT	m_MinorTickLength;
	UINT	m_NumMinorTicks;
	CString	m_Title;
	float	m_RangeHigh;
	float	m_RangeLow;
	UINT	m_NumMajorTicks;
	CString	m_TitleFontDescription;
	CString	m_LabelFontDescription;
	//}}AFX_DATA

	// Needs to be public to allow for opening the dialog
	// box to other than the 0th axis
	int				m_CurAxis;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAxisSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CChart			*m_pChart;
	LOGFONT			m_Font, m_LabelFont;
	COLORREF		m_LabelColor, m_LineColor;
	CAxis			m_Axes[kMaxAxes];

	CString	GetMainLabel( void );
	void	SetupDataFromAxis( void );
	void	SaveDataFromAxis( void );
	void	OnChooseFont( LOGFONT &font, CString &description );

	// Generated message map functions
	//{{AFX_MSG(CAxisSettings)
	afx_msg void OnChooseTitleFont();
	afx_msg void OnAxisNext();
	afx_msg void OnAxisPrevious();
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseLabelFont();
	virtual void OnOK();
	afx_msg void OnChangeNumberMinorTicks();
	afx_msg void OnChangeNumberMajorTicks();
	afx_msg void OnChangeXrangeHigh();
	afx_msg void OnChangeXrangeLow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AXISSETTINGS_H__F2C6589E_94BD_4A95_9F8A_4DA329C89016__INCLUDED_)
