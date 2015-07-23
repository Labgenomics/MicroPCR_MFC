// PIDManagerDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "PIDManagerDlg.h"


// CPIDManagerDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPIDManagerDlg, CDialog)

CPIDManagerDlg::CPIDManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIDManagerDlg::IDD, pParent)
{

}

CPIDManagerDlg::~CPIDManagerDlg()
{
}

void CPIDManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PID, m_cPidList);
	DDX_Control(pDX, IDC_CUSTOM_PID_TABLE2, m_cPidGridList);
}


BEGIN_MESSAGE_MAP(CPIDManagerDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PID_NEW, &CPIDManagerDlg::OnBnClickedButtonPidNew)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CPIDManagerDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPIDManagerDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CPIDManagerDlg 메시지 처리기입니다.

// Enter, Esc 버튼 막기
BOOL CPIDManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg -> message == WM_KEYDOWN )
	{
		if( pMsg -> wParam == VK_RETURN )
			return TRUE;

		if( pMsg -> wParam == VK_ESCAPE )
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


#include "PIDCreateDlg.h"
// 새로운 PID 를 생성할 수 있는 Dialog 를 생성한다.
void CPIDManagerDlg::OnBnClickedButtonPidNew()
{
	static CPIDCreateDlg dlg;

	if( dlg.DoModal() == IDOK )
	{
		
	}
}

void CPIDManagerDlg::OnBnClickedButtonSelect()
{
// 	OnOK();
}

void CPIDManagerDlg::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}