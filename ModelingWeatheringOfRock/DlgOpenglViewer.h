#pragma once
#include "afxwin.h"


// CDlgOpenglViewer 대화 상자입니다.

class CDlgOpenglViewer : public CDialog
{
	DECLARE_DYNAMIC(CDlgOpenglViewer)

public:
	CDlgOpenglViewer(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgOpenglViewer();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OPENGL_VIEWER };





	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	
	//! 가시를 위한
	int DrawSeeViewer(int nSeePos);


	int DrawGLScene();
	int DrawFileExternScene();
	int DrawFileObjScene();

	struct sPartData1
	{
		float fMaxX;
		float fMaxY;
		float fMaxZ;
		float fMinX;
		float fMinY;
		float fMinZ;

		sPartData1() {
			fMaxX = -99999.9;
			fMaxY = -99999.9;
			fMaxZ = -99999.9;
			fMinX = 99999.9;
			fMinY = 99999.9;
			fMinZ = 99999.9;
		}
	};

	sPartData1 PartData;

	BOOL m_bComplete;

	BOOL m_bTranObj;
	BOOL m_bViewAxis;
	float m_f3DScale;
	float m_fScale;
	float m_XRot;
	float m_YRot;
	float m_ZRot;

	float m_XTran;
	float m_YTran;
	float m_ZTran;



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


protected:
	CWGL m_wgl;
	

	
public:
	bool m_bResizing;
protected:
	BOOL m_bLButtonDown;
	CPoint m_ptLBDown;

	BOOL m_bRButtonDown;
	CPoint m_ptRBDown;
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgOpenglViewer)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonUp(UINT nHitTest, CPoint point);

	
};
