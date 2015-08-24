#pragma once

#include ".\GraphControl\GraphCtrl.h"
// CTempGraphDlg 대화 상자입니다.

class CTempGraphDlg : public CDialog
{
	DECLARE_DYNAMIC(CTempGraphDlg)

private:
	GraphCtrl m_Graph;

public:
	CTempGraphDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTempGraphDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TEMP_GRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void addData(float data);
	virtual BOOL DestroyWindow();
};
