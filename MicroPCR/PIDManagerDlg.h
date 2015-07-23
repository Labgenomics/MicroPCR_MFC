#pragma once
#include "afxwin.h"
#include ".\gridctrl_src\gridctrl.h"
#include "UserDefs.h"


// CPIDManagerDlg 대화 상자입니다.

class CPIDManagerDlg : public CDialog
{
private:

	void initPidTable();

	DECLARE_DYNAMIC(CPIDManagerDlg)

public:
	CPIDManagerDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPIDManagerDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PID_MANAGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cPidList;
	CGridCtrl m_cPidGridList;
	afx_msg void OnBnClickedButtonPidNew();
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonDelete();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
