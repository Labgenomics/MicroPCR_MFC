// PIDCreateDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "PIDCreateDlg.h"
#include "FileManager.h"


// CPIDCreateDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPIDCreateDlg, CDialog)

CPIDCreateDlg::CPIDCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIDCreateDlg::IDD, pParent)
	, m_cStartTemp(25)
	, m_cTargetTemp(25)
	, m_cKp(0)
	, m_cKi(0)
	, m_cKd(0)
	, saveLabel(L"")
{
	
}

CPIDCreateDlg::~CPIDCreateDlg()
{
}

void CPIDCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PID_START_TEMP, m_cStartTemp);
	DDV_MinMaxFloat(pDX, m_cStartTemp, 4, 104);
	DDX_Text(pDX, IDC_EDIT_PID_TARGET_TEMP2, m_cTargetTemp);
	DDV_MinMaxFloat(pDX, m_cTargetTemp, 4, 104);
	DDX_Text(pDX, IDC_EDIT_PID_P, m_cKp);
	//DDV_MinMaxFloat(pDX, m_cKp, 0, 200);
	DDX_Text(pDX, IDC_EDIT_PID_D, m_cKd);
	//DDV_MinMaxFloat(pDX, m_cKd, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PID_I, m_cKi);
	//DDV_MinMaxFloat(pDX, m_cKi, 0, 1);
	DDX_Control(pDX, IDC_COMBO_PID_REMOVAL, m_cRemovalList);
	DDX_Control(pDX, IDC_CUSTOM_PID_TABLE3, m_cPidTable);
}


BEGIN_MESSAGE_MAP(CPIDCreateDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PID_CREATE_ADD, &CPIDCreateDlg::OnBnClickedButtonPidCreateAdd)
	ON_BN_CLICKED(IDC_BUTTON_PID_CREATE_COMPLETE, &CPIDCreateDlg::OnBnClickedButtonPidCreateComplete)
	ON_BN_CLICKED(IDC_BUTTON_PID_CREATE_REMOVAL, &CPIDCreateDlg::OnBnClickedButtonPidCreateRemoval)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CUSTOM_PID_TABLE3, OnGridEndEdit)
END_MESSAGE_MAP()


BOOL CPIDCreateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initPidTable();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// Enter, Esc 버튼 막기
BOOL CPIDCreateDlg::PreTranslateMessage(MSG* pMsg)
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

void CPIDCreateDlg::OnBnClickedButtonPidCreateAdd()
{
	if( !UpdateData() )
		return;

	if( m_cStartTemp == m_cTargetTemp )
	{
		AfxMessageBox(L"시작온도와 타겟온도는 같은 값이 될 수 없습니다.");
		return;
	}

	int pidSize = pids.size();
	
	for(int i=0; i<pidSize; ++i){
		PID pid = pids[i];
		
		if( pid.startTemp == m_cStartTemp &&
			pid.targetTemp == m_cTargetTemp ){
				AfxMessageBox(L"이미 존재하는 pid parameter 입니다.");
				return;
		}
	}

	// Row size 를 변경한다
	m_cPidTable.SetRowCount(pidSize+2);

	// 현재 값을 저장한다. 
	// 제거할 수 있는 콤보 박스에도 넣어준다.
	pids.push_back( PID(m_cStartTemp, m_cTargetTemp, m_cKp, m_cKi, m_cKd) );

	CString label;
	label.Format(L"PID #%d", pidSize+1);
	m_cRemovalList.AddString(label);

	initPidTable();
	loadPidTable();

	m_cStartTemp = m_cTargetTemp = 25;
	m_cKp = m_cKd = m_cKi = 0;

	// 초기화된 변수 값을 control 에 넘겨준다.
	UpdateData(false);
}

// Save 할 이름을 물어보는 Dialog
#include "SaveDlg.h"

void CPIDCreateDlg::OnBnClickedButtonPidCreateComplete()
{
	int size = pids.size();

	if( size == 0 ){
		int res = MessageBox(L"저장한 PID 가 없어 저장되지 않습니다.\n종료하겠습니까?", L"알림", MB_OKCANCEL);

		if( res == IDOK )
			OnCancel();
		else
			return;
	}

	static CSaveDlg dlg;
	dlg.m_cNoticeLabel = L"저장할 PID 이름을 입력하세요.";
	
	if( dlg.DoModal() == IDOK ){
		saveLabel = dlg.m_cLabel;
		dlg.m_cLabel = L"";

		CreateDirectory(L"./PID/", NULL);

		if( FileManager::findFile( L"./PID/", saveLabel ) ){
			AfxMessageBox(L"해당 PID 명은 이미 존재합니다.\n다시 시도해주세요.");
			return;
		}

		FileManager::savePID( saveLabel, pids );
		OnOK();
	}
}

void CPIDCreateDlg::OnBnClickedButtonPidCreateRemoval()
{
	int selectIndex = m_cRemovalList.GetCurSel();

	if( selectIndex == -1 ){
		AfxMessageBox(L"제거할 PID Parameter 를 선택하세요.");
		return;
	}

	pids.erase( pids.begin() + selectIndex );
	
	m_cRemovalList.ResetContent();
	for(int i=0; i<pids.size(); ++i){
		CString item;
		item.Format(L"PID #%d", i+1);
		m_cRemovalList.AddString(item);
	}

	initPidTable();
	loadPidTable();
}

static const int PID_TABLE_COLUMN_WIDTHS[6] = { 50, 100, 100, 50, 50, 50 };

void CPIDCreateDlg::initPidTable()
{
	// grid control 을 여러 값들을 설정한다.
	m_cPidTable.SetListMode(true);

	m_cPidTable.DeleteAllItems();

	m_cPidTable.SetSingleRowSelection();
	m_cPidTable.SetSingleColSelection();
	m_cPidTable.SetRowCount(1);
	m_cPidTable.SetColumnCount(PID_CONSTANTS_MAX+1);
    m_cPidTable.SetFixedRowCount(1);
    m_cPidTable.SetFixedColumnCount(1);
	m_cPidTable.SetEditable(true);

	// 초기 gridControl 의 table 값들을 설정해준다.
	DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE;

    for (int col = 0; col < m_cPidTable.GetColumnCount(); col++) { 
		GV_ITEM Item;
        Item.mask = GVIF_TEXT|GVIF_FORMAT;
        Item.row = 0;
        Item.col = col;

        if (col > 0) {
                Item.nFormat = DT_LEFT|DT_WORDBREAK;
                Item.strText = PID_TABLE_COLUMNS[col-1];
        }

        m_cPidTable.SetItem(&Item);
		m_cPidTable.SetColumnWidth(col, PID_TABLE_COLUMN_WIDTHS[col]);
    }
}

void CPIDCreateDlg::loadPidTable()
{
	m_cPidTable.SetRowCount(pids.size()+1);

	for(int i=0; i<pids.size(); ++i){
		float *temp[5] = { &(pids[i].startTemp), &(pids[i].targetTemp), 
			&(pids[i].kp), &(pids[i].kd), &(pids[i].ki)};
		for(int j=0; j<PID_CONSTANTS_MAX+1; ++j){
			GV_ITEM item;
			item.mask = GVIF_TEXT|GVIF_FORMAT;
			item.row = i+1;
			item.col = j;
			item.nFormat = DT_LEFT|DT_WORDBREAK;

			// 첫번째 column 은 PID 1 으로 표시
			if( j == 0 )
				item.strText.Format(L"PID #%d", i+1);
			else
				item.strText.Format(L"%.4f", *temp[j-1]);
			
			m_cPidTable.SetItem(&item);
		}
	}
}

void CPIDCreateDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	pids.clear();

	for (int row = 1; row < m_cPidTable.GetRowCount(); ++row) {
		CString startTemp = m_cPidTable.GetItemText(row, 1);
		CString targetTemp = m_cPidTable.GetItemText(row, 2);
		CString kp = m_cPidTable.GetItemText(row, 3);
		CString kd = m_cPidTable.GetItemText(row, 4);
		CString ki = m_cPidTable.GetItemText(row, 5);

		pids.push_back( PID( _wtof(startTemp), _wtof(targetTemp), _wtof(kp), _wtof(kd), _wtof(ki) ) );
	}

	*pResult = 0;
}