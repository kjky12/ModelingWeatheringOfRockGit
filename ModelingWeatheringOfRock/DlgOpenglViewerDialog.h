#pragma once
#include "afxwin.h"

#include "DlgOpenglViewer.h"
#include "DlgColorChart.h"

// CDlgOpenglViewerDialog ��ȭ �����Դϴ�.

class CDlgOpenglViewerDialog : public CDialog
{
	DECLARE_DYNAMIC(CDlgOpenglViewerDialog)

public:
	CDlgOpenglViewerDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgOpenglViewerDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_OPENGL_VIEWER_DIALOG };

	//! ī�޶� ����
	CComboBox m_comboSeePos;

	CStatic m_staticViewSize;

	CDlgOpenglViewer*	m_pDlgOpenglViewer;
	CDlgColorChart*		m_pDlgColorChartViewer;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.


protected:

	
	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgOpenglViewerDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();

	
	afx_msg void OnCbnSelchangeComboSeePos();
	CComboBox m_comboSeePos2;
	afx_msg void OnCbnSelchangeComboSeePos2();
	CString m_streditScale;
	afx_msg void OnBnClickedButton1();
	CButton m_checkVerwLine;
	afx_msg void OnBnClickedCheck1();
	CStatic m_staticViewColorChart;
};
