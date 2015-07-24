#pragma once
#include "afxwin.h"
#include ".\gridctrl_src\gridctrl.h"
#include "UserDefs.h"

// CPIDCreateDlg 대화 상자입니다.

class CPIDCreateDlg : public CDialog
{
private:
	float m_cStartTemp;
	float m_cTargetTemp;
	float m_cKp;
	float m_cKi;
	float m_cKd;
	CComboBox m_cRemovalList;
	CGridCtrl m_cPidTable;

	vector< PID > pids;
	CString saveLabel;

	void initPidTable();
	void loadPidTable();

	DECLARE_DYNAMIC(CPIDCreateDlg)

public:
	CPIDCreateDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPIDCreateDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PID_CREATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPidCreateAdd();
	afx_msg void OnBnClickedButtonPidCreateComplete();
	afx_msg void OnBnClickedButtonPidCreateRemoval();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
};
