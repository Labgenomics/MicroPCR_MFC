#if !defined(AFX_COLORCONTROL_H__ED9FB5AA_9F63_4F41_8194_2AE77AD49906__INCLUDED_)
#define AFX_COLORCONTROL_H__ED9FB5AA_9F63_4F41_8194_2AE77AD49906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorControl window

class CColorControl : public CWnd
{
// Construction
public:
	DECLARE_DYNCREATE(CColorControl)

	CColorControl();

	static WNDPROC lpfnSuperWnd;

	// Attributes
public:
	static LRESULT CALLBACK EXPORT
		WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	COLORREF	GetColor( void );
	void		SetColor( COLORREF color );

// Operations
// A shortcut for setting the color on a color control
static void	CColorControl::SetColorControlColor( CDialog *dialog, UINT control, COLORREF color )
{
	CColorControl			*colorControl;

	colorControl = (CColorControl*)dialog->GetDlgItem( control );

	if( colorControl != NULL && colorControl->IsKindOf( RUNTIME_CLASS( CColorControl ) ) )
	{
		colorControl->SetColor( color );
	}
}

// A shortcut for getting the color on a color control
static COLORREF	CColorControl::GetColorControlColor( CDialog *dialog, UINT control )
{
	CColorControl			*colorControl;

	colorControl = (CColorControl*)dialog->GetDlgItem( control );

	if( colorControl != NULL && colorControl->IsKindOf( RUNTIME_CLASS( CColorControl ) ) )
	{
		return colorControl->GetColor();
	}

	return 0;
}
public:
	COLORREF		m_Color;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorControl)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorControl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCONTROL_H__ED9FB5AA_9F63_4F41_8194_2AE77AD49906__INCLUDED_)
