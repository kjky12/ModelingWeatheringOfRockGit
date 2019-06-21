#pragma once

#include "ColourPicker.h"
#include "PieChartCtrl.h"
// CDlgPieDemoDlg 대화 상자입니다.

typedef struct ST_PI_Struct
{
	COLORREF crBack;
	COLORREF crText;
	float fAngle;
	CString strTitle;

	ST_PI_Struct(COLORREF crBackT, COLORREF crTextT, float fAngleT, CString strTitleT)
	{
		crBack	= crBackT ;
		crText	= crTextT;
		fAngle	= fAngleT;
		strTitle= strTitleT;
	}
}ST_PI_Struct;


class CDlgPieDemoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPieDemoDlg)

public:
	CDlgPieDemoDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPieDemoDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PIEDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	vector<ST_PI_Struct>	vecPiStruct;

	CPieChartCtrl m_wndChart;
	virtual BOOL OnInitDialog();

	

	void AddPice(float fAngle, CString strTitle, COLORREF crBack, COLORREF crText = RGB(0,0,0));
	void ResetPice();
};
