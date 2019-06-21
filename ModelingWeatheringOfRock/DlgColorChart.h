#pragma once


// CDlgColorChart 대화 상자입니다.

class CDlgColorChart : public CDialog
{
	DECLARE_DYNAMIC(CDlgColorChart)

public:
	CDlgColorChart(CWnd* pParent, int nType);   // 표준 생성자입니다.
	virtual ~CDlgColorChart();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COLOR_CHART_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();


	void FillColor();

	int m_nType;
};
