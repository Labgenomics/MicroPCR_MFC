// PIDManagerDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "PIDManagerDlg.h"
#include "FileManager.h"


// CPIDManagerDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPIDManagerDlg, CDialog)

CPIDManagerDlg::CPIDManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIDManagerDlg::IDD, pParent)
	, selectedPID(L"")
	, selectedIndex(-1)
	, hasPidList(false)
{

}

CPIDManagerDlg::~CPIDManagerDlg()
{
}

void CPIDManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PID, m_cPidList);
	DDX_Control(pDX, IDC_CUSTOM_PID_TABLE2, m_cPidTable);
}


BEGIN_MESSAGE_MAP(CPIDManagerDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PID_NEW, &CPIDManagerDlg::OnBnClickedButtonPidNew)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CPIDManagerDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPIDManagerDlg::OnBnClickedButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST_PID, &CPIDManagerDlg::OnLbnSelchangeListPid)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CUSTOM_PID_TABLE2, OnGridEndEdit)
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

BOOL CPIDManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initPidList();
	initPidTable();

	if( pidList.size() == 0 ){
		GetDlgItem(IDC_BUTTON_PID_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PID_SELECT)->EnableWindow(FALSE);
	}

	selectedIndex = -1;

	if( m_cPidList.GetCount() != 0 )
		hasPidList = true;
	else
		OnBnClickedButtonPidNew();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

#include "PIDCreateDlg.h"
// 새로운 PID 를 생성할 수 있는 Dialog 를 생성한다.
void CPIDManagerDlg::OnBnClickedButtonPidNew()
{
	CPIDCreateDlg dlg;

	if( dlg.DoModal() == IDOK )
	{
		initPidList();
		AfxMessageBox(L"새로운 PID 가 생성되었습니다.");
		
		GetDlgItem(IDC_BUTTON_PID_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PID_SELECT)->EnableWindow(TRUE);

		hasPidList = true;
	}
}

void CPIDManagerDlg::OnBnClickedButtonSelect()
{
	int idx = m_cPidList.GetCurSel();

	if( idx == -1 ){
		AfxMessageBox(L"사용할 PID 를 선택해주세요.");
		return;
	}

	selectedPID = pidList[idx];

 	OnOK();
}

void CPIDManagerDlg::OnBnClickedButtonDelete()
{
	int idx = m_cPidList.GetCurSel();

	if( idx == -1 ){
		AfxMessageBox(L"삭제할 PID 를 선택해주세요.");
		return;
	}

	selectedPID = pidList[idx];

	// 해당 파일을 삭제하고, 새로 pid List 를 불러온다.
	DeleteFile( L"./PID/" + selectedPID );
	initPidList();
	initPidTable();

	// 전부 비워졌는지 체크
	if( m_cPidList.GetCount() != 0 )
		hasPidList = true;
	else
		hasPidList = false;

	selectedPID = L"";
}


void CPIDManagerDlg::initPidList()
{
	// 기존에 있던 pid list 를 제거한다. 
	pidList.clear();
	m_cPidList.ResetContent();

	// PID 폴더가 없을 수 있으므로, 생성해준다.
	CreateDirectory(L"./PID/", NULL);
	FileManager::enumFiles( L"./PID/", pidList );

	for(int i=0; i<pidList.size(); ++i)
		m_cPidList.AddString( pidList[i] );

	if( pidList.size() == 0 ){
		GetDlgItem(IDC_BUTTON_PID_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PID_SELECT)->EnableWindow(FALSE);
	}
}

static const int PID_TABLE_COLUMN_WIDTHS[6] = { 41, 100, 100, 50, 50, 50 };

void CPIDManagerDlg::initPidTable()
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

void CPIDManagerDlg::loadPidTable()
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

void CPIDManagerDlg::OnLbnSelchangeListPid()
{
	selectedIndex = m_cPidList.GetCurSel();

	if( selectedIndex != -1 ){
		selectedPID = pidList[selectedIndex];

		// PID 파일이 아닌 경우 제거하고 리스트를 다시 만들어줌.
		if( !FileManager::loadPID( selectedPID, pids ) ){
			AfxMessageBox( selectedPID + L"는 올바른 PID 포맷이 아닙니다. 제거됩니다." );
			DeleteFile( L"./PID/" + selectedPID );
			initPidList();
			return;
		}

		// 얻어진 pids 값을 통해 pid table 로 보여준다.
		initPidTable();
		loadPidTable();
	}
}

void CPIDManagerDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	pids.clear();

	for (int row = 1; row < m_cPidTable.GetRowCount(); ++row) {
		CString startTemp = m_cPidTable.GetItemText(row, 1);
		CString targetTemp = m_cPidTable.GetItemText(row, 2);
		CString kp = m_cPidTable.GetItemText(row, 3);
		CString kd = m_cPidTable.GetItemText(row, 4);
		CString ki = m_cPidTable.GetItemText(row, 5);

		pids.push_back( PID( _wtof(startTemp), _wtof(targetTemp), _wtof(kp), _wtof(ki), _wtof(kd) ) );
	}

	if( !FileManager::savePID( selectedPID, pids ) ){
		AfxMessageBox(L"PID 를 변경하는 중에 문제가 발생하였습니다.\n개발자에게 문의하세요.");
		return;
	}

	m_cPidList.SetCurSel(selectedIndex);
	OnLbnSelchangeListPid();

	*pResult = 0;
}

bool CPIDManagerDlg::isHasPidList()
{
	return hasPidList;
}