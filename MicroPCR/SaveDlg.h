#pragma once


// CSaveDlg 대화 상자입니다.

class CSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveDlg)

public:
	CSaveDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSaveDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_cLabel;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_cNoticeLabel;
};
