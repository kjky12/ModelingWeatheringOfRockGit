#pragma once


// CDlgColorChart ��ȭ �����Դϴ�.

class CDlgColorChart : public CDialog
{
	DECLARE_DYNAMIC(CDlgColorChart)

public:
	CDlgColorChart(CWnd* pParent, int nType);   // ǥ�� �������Դϴ�.
	virtual ~CDlgColorChart();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_COLOR_CHART_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();


	void FillColor();

	int m_nType;
};
