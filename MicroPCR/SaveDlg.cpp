// SaveDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "SaveDlg.h"


// CSaveDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSaveDlg, CDialog)

CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
	, m_cLabel(_T(""))
	, m_cNoticeLabel(_T(""))
{

}

CSaveDlg::~CSaveDlg()
{
}

void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SAVE_LABEL, m_cLabel);
	DDX_Text(pDX, IDC_STATIC_SAVE_LABEL, m_cNoticeLabel);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSaveDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CSaveDlg 메시지 처리기입니다.

void CSaveDlg::OnBnClickedOk()
{
	UpdateData();

	if( m_cLabel.IsEmpty() ){
		AfxMessageBox(L"저장할 이름을 입력하세요.");
		return;
	}

	OnOK();
}

BOOL CSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 타이틀을 label 값에 따라 변경해준다.
	// 타이틀 변경은 InitDialog 가 수행될 때부터 가능하기에 여기에 처리함.
	CString titleCheck;
	GetDlgItemText(IDC_STATIC_SAVE_LABEL, titleCheck);
	if( titleCheck.Find(L"PID") != -1 )
		SetWindowText(L"Save PID");
	else
		SetWindowText(L"Save Protocol");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CSaveDlg::PreTranslateMessage(MSG* pMsg)
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
