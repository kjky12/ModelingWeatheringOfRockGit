#pragma once


// CDlgOpenglView ��ȭ �����Դϴ�.

class CDlgOpenglView : public CDialog
{
	DECLARE_DYNAMIC(CDlgOpenglView)

public:
	CDlgOpenglView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgOpenglView();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_OPENGL_VIEW_DIALOG };


	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	
	int DrawGLScene();
	int DrawGLSceneTEST();
	int DrawVoxelScene();
	int DrawFileGLScene();
	int DrawFileExternScene();
	int DrawFileColor();
	int DrawFilePorosity();
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.


protected:
	CWGL m_wgl;
	

	
	
	bool m_bResizing;
	BOOL m_bLButtonDown;
	CPoint m_ptLBDown;

	BOOL m_bRButtonDown;
	CPoint m_ptRBDown;
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgOpenglView)
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
