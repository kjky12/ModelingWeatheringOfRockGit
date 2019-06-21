#pragma once

#include "ColourPicker.h"
#include "PieChartCtrl.h"
// CDlgPieDemoDlg ��ȭ �����Դϴ�.

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
	CDlgPieDemoDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgPieDemoDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PIEDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
