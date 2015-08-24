// TempGraphDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "TempGraphDlg.h"


// CTempGraphDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTempGraphDlg, CDialog)

CTempGraphDlg::CTempGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTempGraphDlg::IDD, pParent)
{

}

CTempGraphDlg::~CTempGraphDlg()
{
}

void CTempGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTempGraphDlg, CDialog)
END_MESSAGE_MAP()


// CTempGraphDlg 메시지 처리기입니다.

BOOL CTempGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	GetClientRect(&rect);

	//  그래프 초기화
	m_Graph.Create(WS_VISIBLE | WS_CHILD, rect, this);
	m_Graph.InitGraph();
	m_Graph.BackgroundColor(RGB(0,0,0));
	m_Graph.GridColor(RGB(200,200,200));
	
	m_Graph.InitData(1, L"온도", 120, 0, RGB(255,0,0), true);
	m_Graph.m_SettingFlg = true;
	m_Graph.DrawRect();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTempGraphDlg::addData(float data)
{
	m_Graph.LineAdd(1, data);
	m_Graph.DrawRect();
}
BOOL CTempGraphDlg::DestroyWindow()
{
	m_Graph.AllClear();
	m_Graph.DestroyWindow();

	return CDialog::DestroyWindow();
}
