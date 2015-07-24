
// MicroPCRDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MicroPCR.h"
#include "MicroPCRDlg.h"
#include "ConvertTool.h"
#include "FileManager.h"
#include "PIDManagerDlg.h"

#include <locale.h>
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMicroPCRDlg 대화 상자



CMicroPCRDlg::CMicroPCRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMicroPCRDlg::IDD, pParent)
	, openConstants(true)	// true 로 한 후, 함수를 호출하여 닫도록 한다. 
	, openGraphView(false)
	, isConnected(false)
	, m_cMaxActions(1000)
	, m_cTimeOut(1000)
	, m_cArrivalDelta(0.5)
	, m_cTemperature(0)
	, m_sProtocolName(L"")
	, m_totalActionNumber(0)
	, m_currentActionNumber(-1)
	, actions(NULL)
	, m_nLeftSec(0)
	, m_blinkCounter(0)
	, m_timerCounter(0)
	, recordFlag(false)
	, blinkFlag(false)
	, isRecording(false)
	, isStarted(false)
	, isCompletePCR(false)
	, isTargetArrival(false)
	, isFirstDraw(false)
	, m_startTime(0)
	, m_nLeftTotalSec(0)
	, m_prevTargetTemp(0)
	, m_currentTargetTemp(0)
	, m_timeOut(0)
	, m_leftGotoCount(-1)
	, m_recordingCount(0)
	, m_recStartTime(0)
	, m_cGraphYMin(0)
	, m_cGraphYMax(4096)
	, ledControl(1)
	, currentCmd(CMD_READY)
	, m_kp(0.0)
	, m_ki(0.0)
	, m_kd(0.0)
	, isFanOn(false)
	, m_cIntegralMax(INTGRALMAX)
	, loadedPID(L"")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMicroPCRDlg::~CMicroPCRDlg()
{
	// 생성한 객체를 소멸자에서 제거해준다.
	if( device != NULL )
		delete device;
	if( actions != NULL )
		delete []actions;
}

void CMicroPCRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_CHAMBER_TEMP, m_cProgressBar);
	DDX_Control(pDX, IDC_CUSTOM_PID_TABLE, m_cPidTable);
	DDX_Text(pDX, IDC_EDIT_MAX_ACTIONS, m_cMaxActions);
	DDV_MinMaxInt(pDX, m_cMaxActions, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_TIME_OUT, m_cTimeOut);
	DDV_MinMaxInt(pDX, m_cTimeOut, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_ARRIVAL_DELTA, m_cArrivalDelta);
	DDV_MinMaxFloat(pDX, m_cArrivalDelta, 0, 10.0);
	DDX_Text(pDX, IDC_EDIT_CHAMBER_TEMP, m_cTemperature);
	DDX_Control(pDX, IDC_LIST_PCR_PROTOCOL, m_cProtocolList);
	DDX_Text(pDX, IDC_EDIT_GRAPH_Y_MIN, m_cGraphYMin);
	DDV_MinMaxInt(pDX, m_cGraphYMin, 0, 4096);
	DDX_Text(pDX, IDC_EDIT_Y_MAX, m_cGraphYMax);
	DDV_MinMaxInt(pDX, m_cGraphYMax, 0, 4096);
	DDX_Text(pDX, IDC_EDIT_INTEGRAL_MAX, m_cIntegralMax);
	DDV_MinMaxFloat(pDX, m_cIntegralMax, 0.0, 10000.0);
}

BEGIN_MESSAGE_MAP(CMicroPCRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_SET_SERIAL, SetSerial)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONSTANTS, &CMicroPCRDlg::OnBnClickedButtonConstants)
	ON_BN_CLICKED(IDC_BUTTON_CONSTANTS_APPLY, &CMicroPCRDlg::OnBnClickedButtonConstantsApply)
	ON_BN_CLICKED(IDC_BUTTON_PCR_START, &CMicroPCRDlg::OnBnClickedButtonPcrStart)
	ON_BN_CLICKED(IDC_BUTTON_PCR_OPEN, &CMicroPCRDlg::OnBnClickedButtonPcrOpen)
	ON_BN_CLICKED(IDC_BUTTON_PCR_RECORD, &CMicroPCRDlg::OnBnClickedButtonPcrRecord)
	ON_BN_CLICKED(IDC_BUTTON_GRAPHVIEW, &CMicroPCRDlg::OnBnClickedButtonGraphview)
	ON_BN_CLICKED(IDC_BUTTON_FAN_CONTROL, &CMicroPCRDlg::OnBnClickedButtonFanControl)
	ON_BN_CLICKED(IDC_BUTTON_ENTER_PID_MANAGER, &CMicroPCRDlg::OnBnClickedButtonEnterPidManager)
END_MESSAGE_MAP()


// CMicroPCRDlg 메시지 처리기

BOOL CMicroPCRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	/* UI Settings */
	bmpProgress.LoadBitmapW(IDB_BITMAP_SCALE);
	bmpRecNotWork.LoadBitmapW(IDB_BITMAP_REC_NOT_WORKING); 
	bmpRecWork.LoadBitmapW(IDB_BITMAP_REC_WORKING);

	((CButton*)GetDlgItem(IDC_BUTTON_PCR_RECORD))->SetBitmap((HBITMAP)bmpRecNotWork);

	m_cProgressBar.SetRange32(-10, 110);
	m_cProgressBar.SendMessage(PBM_SETBARCOLOR,0,RGB(200, 0, 50));

	SetDlgItemText(IDC_EDIT_ELAPSED_TIME, L"0m 0s");

	CFont font;
	CRect rect;
	CString labels[3] = { L"No", L"Temp.", L"Time" };

	font.CreatePointFont(100, L"Arial", NULL);

	m_cProtocolList.SetFont(&font);
	m_cProtocolList.GetClientRect(&rect);

	for(int i=0; i<3; ++i)
		m_cProtocolList.InsertColumn(i, labels[i], LVCFMT_CENTER, (rect.Width()/3));

	actions = new Action[m_cMaxActions];

	OnBnClickedButtonConstants();

	// 150725 PID Check
	// 이전에 불러온 PID 값이 있는지 확인한다.
	CString recentPath;
	vector< CString > pidList;
	FileManager::loadRecentPath(FileManager::PID_PATH, recentPath);
	FileManager::enumFiles( L"./PID/", pidList );

	do
	{
		if( recentPath.IsEmpty() ){
			if( pidList.empty() )
				AfxMessageBox(L"저장된 PID Parameter 가 존재하지 않습니다.\n생성하는 창으로 이동됩니다.");
			else
				AfxMessageBox(L"최근 사용한 PID Parameter 가 존재하지 않습니다.\n선택하는 창으로 이동됩니다.");
	
			OnBnClickedButtonEnterPidManager();
			break;
		}
		else{
			// recentPath 를 불러오면서 문제가 생긴 경우, recentPath 값을 
			// 지움으로써 위의 if 문이 동작하도록 한다.
			if( !FileManager::loadPID( recentPath, pids ) ){
				DeleteFile(RECENT_PID_PATH);
				recentPath.Empty();
			}
			else{
				loadedPID = recentPath;
				SetDlgItemText(IDC_EDIT_LOADED_PID, loadedPID);
				break;
			}
		}
	}while(true);

	initPidTable();
	loadPidTable();

	// Chart Settings
	CAxis *axis;
	axis = m_Chart.AddAxis( kLocationBottom );
	axis->SetTitle(L"PCR Cycles");
	axis->SetRange(0, 40);

	axis = m_Chart.AddAxis( kLocationLeft );
	axis->SetTitle(L"Sensor Value");
	axis->SetRange(m_cGraphYMin, m_cGraphYMax);

	sensorValues.push_back( 1.0 );

	device = new CDeviceConnect( GetSafeHwnd() );

	// 연결 시도
	BOOL status = device->CheckDevice();

	if( status )
	{
		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Connected");
		isConnected = true;

		CFile file;
		BOOL status = file.Open(RECENT_PATH, CFile::modeRead);
		// 최근 프로토콜 경로가 저장된 파일이 있다면
		if( status )
		{
			file.Close();
			// 그 경로로 파일을 로드하여 리스트를 만든다.
			loadRecentProtocol();
 
			SetTimer(1, TIMER_DURATION, NULL);
		}
		else
		{
			// 파일이 없다면 새로 만들어두고 에러메시지를 호출한다.
			file.Open(RECENT_PATH, CFile::modeCreate);
			file.Close();
			AfxMessageBox(L"No Recent Protocol File! Please Read Protocol!");
		}
	}
	else
	{
		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Disconnected");
		isConnected = false;
	}

	enableWindows();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMicroPCRDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect graphRect;

		int oldMode = dc.SetMapMode( MM_LOMETRIC );

		graphRect.SetRect( 15, 350, 570, 760 );
		
		dc.DPtoLP( (LPPOINT)&graphRect, 2 );

		CDC *dc2 = CDC::FromHandle(dc.m_hDC);
		m_Chart.OnDraw( dc2, graphRect, graphRect );

		dc.SetMapMode( oldMode );

		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMicroPCRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMicroPCRDlg::SetSerial(WPARAM wParam, LPARAM lParam)
{
	char *Serial = (char *)lParam;
	return TRUE;
}

BOOL CMicroPCRDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	// 연결 시도
	BOOL status = device->CheckDevice();

	if( status )
	{
		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Connected");
		isConnected = true;

		CFile file;
		BOOL status = file.Open(RECENT_PATH, CFile::modeRead);
		// 최근 프로토콜 경로가 저장된 파일이 있다면
		if( status )
		{
			file.Close();
			// 그 경로로 파일을 로드하여 리스트를 만든다.
			loadRecentProtocol();
 
			SetTimer(1, TIMER_DURATION, NULL);
		}
		else
		{
			// 파일이 없다면 새로 만들어두고 에러메시지를 호출한다.
			file.Open(RECENT_PATH, CFile::modeCreate);
			file.Close();
			AfxMessageBox(L"No Recent Protocol File! Please Read Protocol!");
		}
	}
	else
	{
		SetDlgItemText(IDC_EDIT_DEVICE_STATUS, L"Disconnected");
		isConnected = false;

		KillTimer(1);
	}

	enableWindows();

	return TRUE;
}

BOOL CMicroPCRDlg::PreTranslateMessage(MSG* pMsg)
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

void CMicroPCRDlg::Serialize(CArchive& ar)
{
	// Constants 값을 저장할 때 사용함.
	if (ar.IsStoring())
	{
		ar << m_cMaxActions << m_cTimeOut << m_cArrivalDelta << m_cGraphYMin << m_cGraphYMax << m_cIntegralMax;

		for(int i=0; i<pids.size(); ++i){
			ar << pids[i].startTemp;
			ar << pids[i].targetTemp;
			ar << pids[i].kp;
			ar << pids[i].kd;
			ar << pids[i].ki;
		}
	}
	else	// Constants 값을 파일로부터 불러올 때 사용한다.
	{
		pids.clear();

		ar >> m_cMaxActions >> m_cTimeOut >> m_cArrivalDelta >> m_cGraphYMin >> m_cGraphYMax >> m_cIntegralMax;

		for(int i=0; i<PID_CONSTANTS_MAX; ++i){
			float startTemp, targetTemp, kp, kd, ki;
			ar >> startTemp;
			ar >> targetTemp;
			ar >> kp;
			ar >> kd;
			ar >> ki;

			// 불러와서 pids vector 에 저장한다. 
			pids.push_back( PID( startTemp, targetTemp, kp, kd, ki ) );
		}

		UpdateData(FALSE);
	}
}

static const int PID_TABLE_COLUMN_WIDTHS[6] = { 88, 120, 120, 75, 75, 75 };

// pid table 을 grid control 을 그리기 위해 설정.
void CMicroPCRDlg::initPidTable()
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

// 파일로부터 pid 값을 불러와서 table 에 그려준다.
void CMicroPCRDlg::loadPidTable()
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

void CMicroPCRDlg::savePidTable()
{
	FileManager::savePID( loadedPID, pids );
}

void CMicroPCRDlg::enableWindows()
{
	GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(isConnected&&!loadedPID.IsEmpty());
	GetDlgItem(IDC_BUTTON_PCR_OPEN)->EnableWindow(isConnected);
	GetDlgItem(IDC_BUTTON_PCR_RECORD)->EnableWindow(isConnected);
}

void CMicroPCRDlg::saveRecentProtocol(CString path)
{
	CFile file;
	char str[256] = "";
	int j =0;
	file.Open(RECENT_PATH, CFile::modeCreate | CFile::modeWrite);
	// 프로토콜 파일을 저장하기 위해 경로 설정, 한글이 깨질 수 있기 때문에 추가함.
	WideCharToMultiByte(CP_ACP, 0, path, -1, str, 256, NULL, NULL);
	strcat(str, "\r\n");
	while(str[j] != '\n')
		j++;
	file.Write(str, j+3);
	file.Close();
}

void CMicroPCRDlg::loadRecentProtocol(void)
{
	CString path;
	CStdioFile file;
	file.Open(RECENT_PATH, CFile::modeRead);
	// 경로에 한글이 포함될 수 있기 때문에 추가함.
	setlocale(LC_ALL, "korean");
	file.ReadString(path);
	file.Close();

	readProtocol(path);
}

void CMicroPCRDlg::readProtocol(CString path)
{
	CFile file;
	if( path.IsEmpty() || !file.Open(path, CFile::modeRead) )
	{
		AfxMessageBox(L"No Recent Protocol File! Please Read Protocol!");
		return;
	}

	int fileSize = (int)file.GetLength() + 1;
	char *inTemp = new char[fileSize * sizeof(char)];
	file.Read(inTemp, fileSize);

	CString inString = AfxCharToString(inTemp);
	
	m_sProtocolName = getProtocolName(path);

	if( m_sProtocolName.GetLength() > MAX_PROTOCOL_LENGTH )
	{
		AfxMessageBox(L"Protocol Name is Too Long !");
		return;
	}

	if( inTemp ) delete[] inTemp;

	int markPos = inString.Find(L"%PCR%");
	if( markPos < 0 )
	{
		AfxMessageBox(L"This is not PCR File !!");
		return;
	}
	markPos = inString.Find(L"%END");
	if( markPos < 0 )
	{
		AfxMessageBox(L"This is not PCR File !!");
		return;
	}

	int line = 0;
	for(int i=0; i<markPos; i++)
	{
		if( inString.GetAt(i) == '\n' )
			line++;
	}

	m_totalActionNumber = 0;
	for(int i=0; i<line; i++)
	{
		int linePos = inString.FindOneOf(L"\n\r");
		CString oneLine = (CString)inString.Left(linePos);
		inString.Delete(0, linePos + 2);
		if( i > 0 )
		{
			int spPos = oneLine.FindOneOf(L" \t");
			// Label Extraction
			actions[m_totalActionNumber].Label = oneLine.Left(spPos);
			oneLine.Delete(0, spPos);
			oneLine.TrimLeft();
			// Temperature Extraction
			spPos = oneLine.FindOneOf(L" \t");
			CString tmpStr = oneLine.Left(spPos);
			oneLine.Delete(0, spPos);
			oneLine.TrimLeft();

			actions[m_totalActionNumber].Temp = (double)_wtof(tmpStr);

			bool timeflag = false;
			wchar_t tempChar = NULL;

			for(int j=0; j<oneLine.GetLength(); j++)
			{
				tempChar = oneLine.GetAt(j);
				if( tempChar == (wchar_t)'m' )
					timeflag = true;
				else if( tempChar == (wchar_t)'M' )
					timeflag = true;
				else
					timeflag = false;
			}

			// Duration Extraction
			double time = (double)_wtof(oneLine);

			if(timeflag)
				actions[m_totalActionNumber].Time = time * 60;
			else
				actions[m_totalActionNumber].Time = time;

			timeflag = false;
			
			if( actions[m_totalActionNumber].Label != "GOTO" )
			{
				m_nLeftTotalSec += (int)(actions[m_totalActionNumber].Time);
			}

			int label = 0;
			CString temp;
			if( actions[m_totalActionNumber].Label == "GOTO" )
			{
				while(true && actions[m_totalActionNumber].Temp != 0 && actions[m_totalActionNumber].Temp < 101 )
				{
					temp.Format(L"%.0f", actions[m_totalActionNumber].Temp);
					if( actions[label++].Label == temp) break;
				}

				for(int j=0; j<actions[m_totalActionNumber].Time; j++)
				{
					for(int k=label-1; k<m_totalActionNumber; k++)
						m_nLeftTotalSec += (int)(actions[k].Time);
				}
			}
			m_totalActionNumber++;
		}
	}

	int labelNo = 1;

	for(int i=0; i<m_totalActionNumber; i++)
	{
		if( actions[i].Label != "GOTO" )
		{
			if( _ttoi(actions[i].Label) != labelNo )
			{
				AfxMessageBox(L"Label numbering error");
				return;
			}
			else
				labelNo++;

			if( (actions[i].Temp > 100) || (actions[i].Temp < 0) )
			{
				AfxMessageBox(L"Target Temperature error!!");
				return;
			}

			if( (actions[i].Time > 3600) || (actions[i].Time < 0) )
			{
				AfxMessageBox(L"Target Duration error!!");
				return;
			}
		}
		else
		{
			if( (actions[i].Temp > (double)_wtof(actions[i-1].Label) ) ||
				(actions[i].Temp < 1) )
			{
				AfxMessageBox(L"GOTO Label error !!");
				return;
			}

			if( (actions[i].Time > 100) || (actions[i].Time < 1) )
			{
				AfxMessageBox(L"GOTO repeat count error !!");
				return;
			}
		}
	}

	m_nLeftTotalSecBackup = m_nLeftTotalSec;

	// 리스트에 표시해준다.
	displayList();
}

void CMicroPCRDlg::displayList()
{
	int j=0;

	m_cProtocolList.DeleteAllItems();

	for(int i=0; i<m_totalActionNumber; i++)
	{
		m_cProtocolList.InsertItem(i, actions[i].Label);		// 라벨 값을 리스트에 표시한다.
		CString tempString;

		if( actions[i].Label == "GOTO" )		// GOTO 일 경우
		{
			tempString.Format(L"%d", (int)actions[i].Temp);
			m_cProtocolList.SetItemText(i, 1, tempString);

			tempString.Format(L"%d",(int)actions[i].Time);
			m_cProtocolList.SetItemText(i, 2, tempString);
		}
		else					// 숫자 일 경우
		{
			tempString.Format(L"%d", (int)actions[i].Temp);
			m_cProtocolList.SetItemText(i, 1, tempString);			// 온도를 리스트에 표시한다.

			// 시간 받기(분단위)
			int durs = (int)actions[i].Time;
			// 소수점 값을 초로 환산
			int durm = durs/60;
			durs = durs%60;

			if( durs == 0 )
			{
				if( durm == 0 ) tempString.Format(L"∞");
				else tempString.Format(L"%dm", durm);
			}
			else
			{
				if( durm == 0 ) tempString.Format(L"%ds", durs);
				else tempString.Format(L"%dm %ds", durm, durs);
			}

			m_cProtocolList.SetItemText(i, 2, tempString);		// 시간을 리스트에 표현한다.
		}
	}

	// 프로토콜 이름
	// InvalidateRect(&CRect(10, 5, 10 * m_sProtocolName.GetLength() + MAX_PROTOCOL_LENGTH, 34));		// Protocol Name 갱신

	// 전체 시간을 시, 분, 초 단위로 계산
	int second = m_nLeftTotalSec % 60;
	int minute = m_nLeftTotalSec / 60;
	int hour   = minute / 60;
	minute = minute - hour * 60;

	// 전체 시간 표시
	CString leftTime;
	leftTime.Format(L"%02d:%02d:%02d", hour, minute, second);
	SetDlgItemText(IDC_EDIT_LEFT_PROTOCOL_TIME, leftTime);
}

CString CMicroPCRDlg::getProtocolName(CString path)
{
	int pos = 0;
	CString protocol = path;

	for(int i=0; i<protocol.GetLength(); i++)
	{
		if( protocol.GetAt(i) == '\\' )
			pos = i;
	}

	return protocol.Mid(pos + 1, protocol.GetLength());
}

void CMicroPCRDlg::OnBnClickedButtonConstants()
{
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	if( openConstants )
	{
		SetWindowPos(NULL, w/3, h/3, 585, 375, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_CONSTANTS, L"PID 열기");
	}
	else
	{
		if( openGraphView )
			OnBnClickedButtonGraphview();

		SetWindowPos(NULL, w/5, h/3, 1175, 375, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_CONSTANTS, L"PID 닫기");
	}

	m_cPidTable.SetEditable(!openConstants);
	openConstants = !openConstants;
}

void CMicroPCRDlg::OnBnClickedButtonGraphview()
{
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	if( openGraphView )
	{
		SetWindowPos(NULL, w/3, h/3, 585, 375, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_GRAPHVIEW, L"그래프 열기");
	}
	else
	{
		if( openConstants )
			OnBnClickedButtonConstants();
		SetWindowPos(NULL, w/3, h/7, 585, 800, SWP_NOZORDER);
		SetDlgItemText(IDC_BUTTON_GRAPHVIEW, L"그래프 닫기");
	}

	openGraphView = !openGraphView;
}

void CMicroPCRDlg::OnBnClickedButtonConstantsApply()
{
	if( !UpdateData() )
		return;

	if( !pids.empty() ){
		pids.clear();
	
		for (int row = 1; row < m_cPidTable.GetRowCount(); row++) {
			CString startTemp = m_cPidTable.GetItemText(row, 1);
			CString targetTemp = m_cPidTable.GetItemText(row, 2);
			CString kp = m_cPidTable.GetItemText(row, 3);
			CString kd = m_cPidTable.GetItemText(row, 4);
			CString ki = m_cPidTable.GetItemText(row, 5);
	
			pids.push_back( PID( _wtof(startTemp), _wtof(targetTemp), _wtof(kp), _wtof(kd), _wtof(ki) ) );
		}

		// 변경된 값에 따라 저장해준다.
		FileManager::savePID( loadedPID, pids );
	}

	CAxis *axis = m_Chart.GetAxisByLocation( kLocationLeft );
	axis->SetRange(m_cGraphYMin, m_cGraphYMax);

	// 동작 중일 경우, 새로 변경된 값으로 pid 를 설정한다.
	if( isStarted )
		findPID();
}

void CMicroPCRDlg::OnBnClickedButtonPcrStart()
{
	if( m_totalActionNumber < 1 )
	{
		AfxMessageBox(L"There is no action list. Please load task file!!");
		return;
	}

	if( !isStarted )
	{
		isStarted = true;

		KillTimer(1);

		GetDlgItem(IDC_BUTTON_FAN_CONTROL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PCR_OPEN)->EnableWindow(FALSE);
		SetDlgItemText(IDC_BUTTON_PCR_START, L"PCR Stop");

		currentCmd = CMD_PCR_RUN;

		if( !isRecording )
			OnBnClickedButtonPcrRecord();

		m_startTime = clock();

		isFirstDraw = false;
		clearSensorValue();

		SetTimer(1, TIMER_DURATION, NULL);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_PCR_OPEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FAN_CONTROL)->EnableWindow(TRUE);

		PCREndTask();
	}
}

void CMicroPCRDlg::OnBnClickedButtonPcrOpen()
{
	if( !isConnected )
		return;

	m_nLeftTotalSec = 0;

	CFileDialog fdlg(TRUE, NULL, NULL, NULL, L"*.txt |*.txt|");
	if( fdlg.DoModal() == IDOK )
	{
		saveRecentProtocol(fdlg.GetPathName());

		m_sProtocolName = getProtocolName(fdlg.GetPathName());

		readProtocol(fdlg.GetPathName());
	}
}

void CMicroPCRDlg::OnBnClickedButtonPcrRecord()
{
	if( !isRecording )
	{
		CreateDirectory(L"./Record/", NULL);

		CString fileName, fileName2;
		CTime time = CTime::GetCurrentTime();
		fileName = time.Format(L"./Record/%Y%m%d-%H%M-%S.txt");
		fileName2 = time.Format(L"./Record/pd%Y%m%d-%H%M-%S.txt");
		
		m_recFile.Open(fileName, CStdioFile::modeCreate|CStdioFile::modeWrite);
		m_recFile.WriteString(L"Number	Time	Temperature\n");

		m_recPDFile.Open(fileName2, CStdioFile::modeCreate|CStdioFile::modeWrite);
		m_recPDFile.WriteString(L"Cycle	Time	Value\n");
		m_recordingCount = 0;
		m_cycleCount = 0;
		m_recStartTime = timeGetTime();
	}
	else
	{
		m_recFile.Close();
		m_recPDFile.Close();
	}

	isRecording = !isRecording;
}

void CMicroPCRDlg::OnBnClickedButtonFanControl()
{
	isFanOn = !isFanOn;

	if( isFanOn )
	{
		currentCmd = CMD_FAN_ON;
		SetDlgItemText(IDC_BUTTON_FAN_CONTROL, L"FAN OFF");
		GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(FALSE);
	}
	else
	{
		currentCmd = CMD_FAN_OFF;
		SetDlgItemText(IDC_BUTTON_FAN_CONTROL, L"FAN ON");
		GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(TRUE);
	}
}

// PID 를 관리할 수 있는 Manager Dialog 를 생성한다.
void CMicroPCRDlg::OnBnClickedButtonEnterPidManager()
{
	static CPIDManagerDlg dlg;

	if( dlg.DoModal() == IDOK )
	{
		loadedPID = dlg.selectedPID;
		SetDlgItemText(IDC_EDIT_LOADED_PID, loadedPID);
		FileManager::saveRecentPath(FileManager::PID_PATH, loadedPID);
		if( !FileManager::loadPID(loadedPID, pids) ){
			AfxMessageBox(L"PID 를 생성하는데 문제가 발생하였습니다.\n개발자에게 문의하세요.");
		}
	}

	if( loadedPID.IsEmpty() )
		AfxMessageBox(L"PID 가 선택되지 않으면 PCR 을 시작할 수 없습니다.");

	if( !dlg.isHasPidList() ){
		loadedPID.Empty();
		SetDlgItemText(IDC_EDIT_LOADED_PID, loadedPID);
		pids.clear();
	}

	// 프로토콜이 비어있으면 Start 버튼 비활성화
	GetDlgItem(IDC_BUTTON_PCR_START)->EnableWindow(!loadedPID.IsEmpty());

	initPidTable();
	loadPidTable();
}


void CMicroPCRDlg::blinkTask()
{
	m_blinkCounter++;
	if( m_blinkCounter > 5 )
	{
		m_blinkCounter = 0;
		if( isRecording )
		{
			recordFlag = !recordFlag;
			if( recordFlag )
				SET_BUTTON_IMAGE(IDC_BUTTON_PCR_RECORD, bmpRecWork);
			else
				SET_BUTTON_IMAGE(IDC_BUTTON_PCR_RECORD, bmpRecNotWork);
		}
		else
			SET_BUTTON_IMAGE(IDC_BUTTON_PCR_RECORD, bmpRecNotWork);

		if( isStarted )
		{
			blinkFlag = !blinkFlag;
			if( blinkFlag )
				m_cProtocolList.SetTextBkColor(RGB(240,200,250));
			else
				m_cProtocolList.SetTextBkColor(RGB(255,255,255));

			m_cProtocolList.RedrawItems(m_currentActionNumber, m_currentActionNumber);
		}
		else
		{
			m_cProtocolList.SetTextBkColor(RGB(255,255,255));
			m_cProtocolList.RedrawItems(0, m_totalActionNumber);
		}
	}
}

CString dslr_title;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char title[128];
	GetWindowTextA(hwnd,title,sizeof(title));

	if( strstr(title, "DSLR Remote Pro for Windows - Connected") != NULL )
		dslr_title = title;

	return TRUE;
}

void CMicroPCRDlg::findPID()
{
	if ( fabs(m_prevTargetTemp - m_currentTargetTemp) < .5 ) 
		return; // if target temp is not change then do nothing 1->.5 correct

	// enable the constant changing.
	GetDlgItem(IDC_BUTTON_CONSTANTS_APPLY)->EnableWindow(FALSE);
	
	double dist	 = 10000;
	int paramIdx = 0;
	
	for ( int i = 0; i < 5; i++ )
	{
		double tmp = fabs(m_prevTargetTemp - pids[i].startTemp) + fabs(m_currentTargetTemp - pids[i].targetTemp);

		if ( tmp < dist )
		{
			dist = tmp;
			paramIdx = i;
		}
	}

	m_kp = pids[paramIdx].kp;
	m_ki = pids[paramIdx].ki;
	m_kd = pids[paramIdx].kd;

	GetDlgItem(IDC_BUTTON_CONSTANTS_APPLY)->EnableWindow(TRUE);
}

void CMicroPCRDlg::timeTask()
{
	m_timerCounter++;

	// 1s 마다 실행되도록 설정
	if( m_timerCounter == 10 )
	{
		m_timerCounter = 0;

		if( m_nLeftSec == 0 )
		{
			m_currentActionNumber++;

			// clear previous blink
			m_cProtocolList.SetTextBkColor(RGB(255,255,255));
			m_cProtocolList.RedrawItems(0, m_totalActionNumber);

			if( (m_currentActionNumber) >= m_totalActionNumber )
			{
				isCompletePCR = true;
				PCREndTask();
				return;
			}

			if( actions[m_currentActionNumber].Label.Compare(L"GOTO") != 0 )
			{
				m_prevTargetTemp = m_currentTargetTemp;
				m_currentTargetTemp = (int)actions[m_currentActionNumber].Temp;

				isTargetArrival = false;
				m_nLeftSec = (int)(actions[m_currentActionNumber].Time);
				m_timeOut = m_cTimeOut*10;

				int min = m_nLeftSec/60;
				int sec = m_nLeftSec%60;

				// current left protocol time
				CString leftTime;
				if( min == 0 )
					leftTime.Format(L"%ds", sec);
				else
					leftTime.Format(L"%dm %ds", min, sec);
				m_cProtocolList.SetItemText(m_currentActionNumber, 2, leftTime);

				// find the proper pid values.
				findPID();
			}
			else	// is GOTO
			{
				if( m_leftGotoCount < 0 )
					m_leftGotoCount = (int)actions[m_currentActionNumber].Time;

				if( m_leftGotoCount == 0 )
					m_leftGotoCount = -1;
				else
				{
					m_leftGotoCount--;
					CString leftGoto;
					leftGoto.Format(L"%d", m_leftGotoCount);

					m_cProtocolList.SetItemText(m_currentActionNumber, 2, leftGoto);
					
					// GOTO 문의 target label 값을 넣어줌
					CString tmpStr;
					tmpStr.Format(L"%d",(int) actions[m_currentActionNumber].Temp);

					int pos = 0;
					for (pos = 0; pos < m_totalActionNumber; ++pos)
						if( tmpStr.Compare(actions[pos].Label) == 0 )
							break;

					m_currentActionNumber = pos-1;
				}
			}
		}
		else	// action 이 진행중인 경우
		{
			if( !isTargetArrival )
			{
				m_timeOut--;

				if( m_timeOut == 0 )
				{
					AfxMessageBox(L"The target temperature cannot be reached!!");
					PCREndTask();
				}
			}
			else
			{
				m_nLeftSec--;
				m_nLeftTotalSec--;

				int min = m_nLeftSec/60;
				int sec = m_nLeftSec%60;

				// current left protocol time
				CString leftTime;
				if( min == 0 )
					leftTime.Format(L"%ds", sec);
				else
					leftTime.Format(L"%dm %ds", min, sec);
				m_cProtocolList.SetItemText(m_currentActionNumber, 2, leftTime);
				
				// total left protocol time
				min = m_nLeftTotalSec/60;
				sec = m_nLeftTotalSec%60;

				if( min == 0 )
					leftTime.Format(L"%ds", sec);
				else
					leftTime.Format(L"%dm %ds", min, sec);
				SetDlgItemText(IDC_EDIT_LEFT_PROTOCOL_TIME, leftTime);
			}
		}

		// 150108 YJ for camera shoot
		if( m_currentActionNumber != 0 && 
			( m_nLeftSec == 10 &&( m_currentActionNumber == 3 || m_currentActionNumber == 6)) ||
			  m_nLeftSec == 1 &&(m_currentActionNumber == 3))
		{
			dslr_title = "";
			EnumWindows(EnumWindowsProc, NULL);

			if( dslr_title != "" )
			{
				HWND checker = ::FindWindow(NULL, dslr_title);

				if( checker )
					::PostMessage(checker, WM_COMMAND, MAKELONG(1003, BN_CLICKED), (LPARAM)GetSafeHwnd());
			}
		}

		if( m_nLeftSec == 11 && 
			( ((int)(actions[m_currentActionNumber].Temp) == 72) || ((int)(actions[m_currentActionNumber].Temp) == 50) ))
		{
			ledControl = 0;
		}
		else if( m_nLeftSec == 0 && 
			( ((int)(actions[m_currentActionNumber].Temp) == 72) || ((int)(actions[m_currentActionNumber].Temp) == 50) ))
		{
			ledControl = 1;
		}

		// for graph drawing
		if( ((int)(actions[m_currentActionNumber].Temp) == 72) && 
			m_nLeftSec == 1 )
		{
			double lights = (double)(photodiode_h & 0x0f)*255. + (double)(photodiode_l);
			addSensorValue( lights );

			if( isRecording )
			{
				m_cycleCount++;
				CString out;
				out.Format(L"%6d	%8.0f	%3.1f\n", m_cycleCount, (double)(timeGetTime()-m_recStartTime), lights);
				m_recPDFile.WriteString(out);
			}
		}
	}

	int elapsed_time = (int)((((double)clock())-m_startTime)/1000.);
	int min = elapsed_time/60;
	int sec = elapsed_time%60;
	CString temp;
	temp.Format(L"%dm %ds", min, sec);
	SetDlgItemText(IDC_EDIT_ELAPSED_TIME, temp);
}

void CMicroPCRDlg::PCREndTask()
{
	if( isRecording )
		OnBnClickedButtonPcrRecord();

	isStarted = false;
	currentCmd = CMD_PCR_STOP;

	m_currentActionNumber = -1;
	m_nLeftSec = 0;
	m_nLeftTotalSec = 0;
	m_timerCounter = 0;
	m_startTime = 0;
	recordFlag = false;

	m_nLeftTotalSec = m_nLeftTotalSecBackup;
	m_leftGotoCount = -1;
	m_recordingCount = 0;
	m_recStartTime = 0;
	blinkFlag = false;
	m_timeOut = 0;
	m_blinkCounter = 0;
	ledControl = 1;

	m_kp = 0;
	m_ki = 0;
	m_kd = 0;

	isTargetArrival = false;

	m_prevTargetTemp = m_currentTargetTemp = 25;

	SetDlgItemText(IDC_BUTTON_PCR_START, L"PCR Start");

	if( isCompletePCR )
		AfxMessageBox(L"PCR ended!!");
	else
		AfxMessageBox(L"PCR incomplete!!");

	isCompletePCR = false;
}

void CMicroPCRDlg::OnTimer(UINT_PTR nIDEvent)
{
	blinkTask();

	if( isStarted )
	{
		int errorPrevent = 0;
		timeTask();
	}

	RxBuffer rx;
	TxBuffer tx;
	float currentTemp = 0.0;
	
	memset(&rx, 0, sizeof(RxBuffer));
	memset(&tx, 0, sizeof(TxBuffer));

	tx.cmd = currentCmd;
	tx.currentTargetTemp = (BYTE)m_currentTargetTemp;

	// pid 값을 buffer 에 복사한다.
	memcpy(&(tx.pid_p1), &(m_kp), sizeof(float));
	memcpy(&(tx.pid_i1), &(m_ki), sizeof(float));
	memcpy(&(tx.pid_d1), &(m_kd), sizeof(float));

	// integral max 값을 buffer 에 복사한다.
	memcpy(&(tx.integralMax_1), &(m_cIntegralMax), sizeof(float));

	device->Write((void*)&tx);

	if( device->Read(&rx) == 0 )
		return;

	// Change the currentCmd to Ready after sending once except READY, RUN.
	if( currentCmd == CMD_FAN_OFF )
		currentCmd = CMD_READY;
	else if( currentCmd == CMD_PCR_STOP )
		currentCmd = CMD_READY;

	// 기기로부터 받은 온도 값을 받아와서 저장함.
	// convert BYTE pointer to float type for reading temperature value.
	memcpy(&currentTemp, &(rx.chamber_temp_1), sizeof(float));

	// 기기로부터 받은 Photodiode 값을 받아와서 저장함.
	photodiode_h = rx.photodiode_h;
	photodiode_l = rx.photodiode_l;

	if( currentTemp < 0.0 )
		return;

	if( fabs(currentTemp-m_currentTargetTemp) < m_cArrivalDelta )
		isTargetArrival = true;

	CString tempStr;
	tempStr.Format(L"%3.1f", currentTemp);
	SetDlgItemText(IDC_EDIT_CHAMBER_TEMP, tempStr);
	m_cProgressBar.SetPos((int)currentTemp);

	// 현재 사용중인 PID 값을 보여준다.
	CString pidstr;
	pidstr.Format(L"%.1f", m_kp);
	SetDlgItemText(IDC_EDIT_CURRENT_P, pidstr);
	pidstr.Format(L"%.4f", m_ki);
	SetDlgItemText(IDC_EDIT_CURRENT_I, pidstr);
	pidstr.Format(L"%.1f", m_kd);
	SetDlgItemText(IDC_EDIT_CURRENT_D, pidstr);

	// Check the error from device
	static bool onceShow = true;
	if( rx.currentError == ERROR_ASSERT && onceShow ){
		onceShow = false;
		AfxMessageBox(L"Software error occured!\nPlease contact to developer");
	}

	// Save the recording data.
	if( isRecording )
	{
		m_recordingCount++;
		CString out;
		out.Format(L"%6d	%8.0f	%3.1f\n", m_recordingCount, (double)(timeGetTime()-m_recStartTime), currentTemp);
		m_recFile.WriteString(out);
	}
	
	CDialog::OnTimer(nIDEvent);
}


void CMicroPCRDlg::addSensorValue(double val)
{
	// 기존에 저장된 차트를 지운 후, 
	// 새로 저장한 double 값 vector 에 저장하여
	// 이 값을 기반으로 다시 그림.
	sensorValues.push_back(val);
	m_Chart.DeleteAllData();

	int size = sensorValues.size();

	double *data = new double[size*2];
	int	nDims = 2, dims[2] = { 2, size };

	for(int i=0; i<size; ++i)
	{
		data[i] = i;
		data[i+size] = sensorValues[i];
	}

	m_Chart.AddData( data, nDims, dims );

	InvalidateRect(&CRect(15, 350, 1155, 760));
}

void CMicroPCRDlg::clearSensorValue()
{
	sensorValues.clear();
	sensorValues.push_back( 1.0 );

	m_Chart.DeleteAllData();
	InvalidateRect(&CRect(15, 350, 1155, 760));
}