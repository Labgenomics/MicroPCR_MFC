
// MicroPCRDlg.h : 헤더 파일
//

#pragma once
#include "DeviceConnect.h"
#include "afxcmn.h"
#include ".\gridctrl_src\gridctrl.h"
#include "Chart.h"
#include "UserDefs.h"
#include "mmtimers.h"
#include "TempGraphDlg.h"

// CMicroPCRDlg 대화 상자
class CMicroPCRDlg : public CDialog
{
private:
	CDeviceConnect *device;
	bool isConnected;
	
	CBitmap bmpProgress, bmpRecNotWork, bmpRecWork;
	bool openConstants, openGraphView;

	double m_cTemperature;

	CProgressCtrl m_cProgressBar;
	CGridCtrl m_cPidTable;
	CListCtrl m_cProtocolList;

	CMMTimers* m_Timer;

	CXYChart m_Chart;
	vector< double > sensorValues;

	int m_cGraphYMin;
	int m_cGraphYMax;

	bool isFirstDraw;

	void addSensorValue(double val);
	void clearSensorValue();

private:
	vector< PID > pids;
	float m_kp, m_ki, m_kd;

	void findPID();

	int m_cMaxActions;
	int m_cTimeOut;
	float m_cArrivalDelta;

	void initPidTable();
	void loadPidTable();
	void loadConstants();
	void saveConstants();
	void enableWindows();

	// for initialize
	void initValues();

	CString m_sProtocolName;
	int m_totalActionNumber;
	int m_currentActionNumber;
	
	Action *actions;
	
	void readProtocol(CString path);
	void displayList();
	CString getProtocolName(CString path);

	// for temporary
	void blinkTask();
	void timeTask();
	void PCREndTask();
	
	int m_blinkCounter, m_timerCounter;

	bool isRecording, recordFlag;
	bool blinkFlag;
	bool isStarted;
	bool isCompletePCR;
	bool isTargetArrival;

	double m_startTime;
	DWORD m_recStartTime;
	double m_prevTargetTemp, m_currentTargetTemp;

	int m_nLeftTotalSecBackup;
	int m_nLeftSec, m_nLeftTotalSec;
	int m_timeOut;

	int m_leftGotoCount;
	int ledControl_wg, ledControl_r, ledControl_g, ledControl_b;

	CStdioFile m_recFile, m_recPDFile, m_recPDFile2;
	int m_recordingCount, m_cycleCount, m_cycleCount2;

	// 버튼에 따라 현재 보낼 command 값을 설정한다.
	BYTE currentCmd;
	bool isFanOn, isLedOn;

	// Photodiode 값을 저장
	BYTE photodiode_h, photodiode_l;

	float m_cIntegralMax;

	CString loadedPID;

	// For temp graph
	CTempGraphDlg tempGraphDlg;
	bool isTempGraphOn;

	// For target temp comparison
	bool targetTempFlag;
	bool freeRunning;
	int freeRunningCounter;

	bool emergencyStop;

// 생성입니다.
public:
	CMicroPCRDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	virtual ~CMicroPCRDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MICROPCR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT SetSerial(WPARAM wParam, LPARAM lParam);

	virtual LRESULT OnmmTimer(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	afx_msg void OnBnClickedButtonConstants();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void Serialize(CArchive& ar);
	afx_msg void OnBnClickedButtonConstantsApply();
	afx_msg void OnBnClickedButtonPcrStart();
	afx_msg void OnBnClickedButtonPcrOpen();
	afx_msg void OnBnClickedButtonPcrRecord();
	afx_msg void OnBnClickedButtonGraphview();
	afx_msg void OnBnClickedButtonFanControl();
	afx_msg void OnBnClickedButtonEnterPidManager();
	afx_msg void OnBnClickedButtonLedControl();
	afx_msg void OnBnClickedCheckTempGraph();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BYTE m_cCompensation;
	BYTE m_cWG_PWM;
	BYTE m_cRed_PWM;
	BYTE m_cGreen_PWM;
	BYTE m_cBlue_PWM;
	afx_msg void OnBnClickedButtonPwmApply();
	afx_msg void OnBnClickedCheckPwmUiHold();
	bool uiHoldFlag;
	CSliderCtrl m_SliderBlue;
	afx_msg void OnNMReleasedcaptureSliderBlue(NMHDR *pNMHDR, LRESULT *pResult);
};
