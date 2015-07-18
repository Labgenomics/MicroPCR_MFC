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
/*      CColorControl - A simple class for showing a square of color in a     */
/*      dialog box, and allowing the user to select a new color.              */
/*============================================================================*/

/* ############################################################################################################################## */


#include "stdafx.h"
#include "ColorControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorControl

IMPLEMENT_DYNCREATE(CColorControl, CWnd)


CColorControl::CColorControl()
{
	m_Color = RGB( 255, 255, 255 );
}

CColorControl::~CColorControl()
{
}


BEGIN_MESSAGE_MAP(CColorControl, CWnd)
	//{{AFX_MSG_MAP(CColorControl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CColorControl message handlers

LRESULT CALLBACK EXPORT
CColorControl::WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// create new item and attach it
	CColorControl* pView = new CColorControl();
	pView->Attach(hWnd);

	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

	// then call it for this first message
#ifdef STRICT
	return ::CallWindowProc(AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}

void CColorControl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect		clientRect;

	GetClientRect( clientRect );

	CBrush		brush;

	brush.CreateSolidBrush( m_Color );

	dc.FillRect( clientRect, &brush );

	dc.DrawEdge( clientRect, EDGE_SUNKEN, BF_RECT );
}

// Set the color of the control
void	CColorControl::SetColor( COLORREF color )
{
	m_Color = color;
	Invalidate(FALSE);
}

// Get the color of the control
COLORREF	CColorControl::GetColor( void )
{
	return m_Color;
}

// On double click, show color selection window
void CColorControl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Let the user choose a color
	CColorDialog		dlg( m_Color );

	if( dlg.DoModal() == IDOK )
	{
		m_Color = dlg.GetColor();
		Invalidate(FALSE);
	}
}

void CColorControl::PostNcDestroy() 
{
	// needed to clean up the C++ CWnd object
	delete this;
}


