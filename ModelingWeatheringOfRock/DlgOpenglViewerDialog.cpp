// DlgOpenglViewerDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelingWeatheringOfRock.h"
#include "DlgOpenglViewerDialog.h"

// CDlgOpenglViewerDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgOpenglViewerDialog, CDialog)

CDlgOpenglViewerDialog::CDlgOpenglViewerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOpenglViewerDialog::IDD, pParent)
	, m_streditScale(_T("1"))
{
	m_pDlgOpenglViewer = NULL;

	m_pDlgColorChartViewer = NULL;
}

CDlgOpenglViewerDialog::~CDlgOpenglViewerDialog()
{
}

void CDlgOpenglViewerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEE_POS, m_comboSeePos);
	DDX_Control(pDX, IDC_STATIC_VIEW_SIZE, m_staticViewSize);
	DDX_Control(pDX, IDC_COMBO_SEE_POS2, m_comboSeePos2);

	DDX_Text(pDX,IDC_EDIT1	  ,m_streditScale);

	DDX_Control(pDX, IDC_CHECK1, m_checkVerwLine);
	DDX_Control(pDX, IDC_STATIC_COLOR_CHART, m_staticViewColorChart);
}


BEGIN_MESSAGE_MAP(CDlgOpenglViewerDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgOpenglViewerDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgOpenglViewerDialog::OnBnClickedCancel)
	ON_WM_DESTROY()

	ON_CBN_SELCHANGE(IDC_COMBO_SEE_POS, &CDlgOpenglViewerDialog::OnCbnSelchangeComboSeePos)
	ON_CBN_SELCHANGE(IDC_COMBO_SEE_POS2, &CDlgOpenglViewerDialog::OnCbnSelchangeComboSeePos2)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgOpenglViewerDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgOpenglViewerDialog::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CDlgOpenglViewerDialog 메시지 처리기입니다.

void CDlgOpenglViewerDialog::OnBnClickedOk()
{
	
	ShowWindow(SW_HIDE);

// 	return;
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	OnOK();
	
}

void CDlgOpenglViewerDialog::OnBnClickedCancel()
{
	ShowWindow(SW_HIDE);
// 	return;
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	OnCancel();
}




void CDlgOpenglViewerDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

}

void CDlgOpenglViewerDialog::OnDestroy() 
{
	CDialog::OnDestroy();


	M_W_DELETE(m_pDlgOpenglViewer);

	M_W_DELETE(m_pDlgColorChartViewer);



}
BOOL CDlgOpenglViewerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//CClientDC dc(this);

	m_comboSeePos.AddString(L"상");
	m_comboSeePos.AddString(L"하");
	m_comboSeePos.AddString(L"좌");
	m_comboSeePos.AddString(L"우");
	m_comboSeePos.AddString(L"앞");
	m_comboSeePos.AddString(L"뒤");
	m_comboSeePos.SetCurSel(0);
	
	m_comboSeePos2.AddString(L"상");
	m_comboSeePos2.AddString(L"하");
	m_comboSeePos2.AddString(L"좌");
	m_comboSeePos2.AddString(L"우");
	m_comboSeePos2.AddString(L"앞");
	m_comboSeePos2.AddString(L"뒤");
	m_comboSeePos2.SetCurSel(0);


// 	UpdateData(TRUE);
// 	m_streditScale = L"1";
// 	UpdateData(FALSE);


	g_n3DSeeType = m_comboSeePos.GetCurSel();
	g_n3DSeeEye = m_comboSeePos2.GetCurSel();
	g_b3DCheckLine = m_checkVerwLine.GetCheck();
	
	CRect rcTmp;
	m_staticViewSize.GetWindowRect(&rcTmp);
	ScreenToClient(&rcTmp);


	

	if(!m_pDlgOpenglViewer)
	{
		m_pDlgOpenglViewer = new CDlgOpenglViewer();
		m_pDlgOpenglViewer->Create(IDD_OPENGL_VIEWER,this);

		m_pDlgOpenglViewer->MoveWindow(rcTmp);
		m_pDlgOpenglViewer->ShowWindow(SW_SHOW);
	}



	m_staticViewColorChart.GetWindowRect(&rcTmp);
	ScreenToClient(&rcTmp);

	if(!m_pDlgColorChartViewer)
	{
		m_pDlgColorChartViewer = new CDlgColorChart(this, 1);
		m_pDlgColorChartViewer->Create(IDD_COLOR_CHART_DIALOG,this);



		m_pDlgColorChartViewer->MoveWindow(rcTmp);
		m_pDlgColorChartViewer->ShowWindow(SW_SHOW);

	}



	return 0;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CDlgOpenglViewerDialog::OnCbnSelchangeComboSeePos()
{
	g_n3DSeeType = m_comboSeePos.GetCurSel();
	g_n3DSeeEye = m_comboSeePos.GetCurSel() + 1;


	if(m_pDlgOpenglViewer)
	{
		m_pDlgOpenglViewer->m_bComplete = FALSE;
		m_pDlgOpenglViewer->OnPaint();
	}

}


void CDlgOpenglViewerDialog::OnCbnSelchangeComboSeePos2()
{
// 	g_n3DSeeEye = m_comboSeePos2.GetCurSel() + 1;
// 
// 
// 	if(m_pDlgOpenglViewer)
// 	{
// 		m_pDlgOpenglViewer->m_bComplete = FALSE;
// 		m_pDlgOpenglViewer->OnPaint();
// 	}

}


void CDlgOpenglViewerDialog::OnBnClickedButton1()
{
	UpdateData(TRUE);
	float fScale = _wtof(m_streditScale);
	UpdateData(FALSE);

	if(m_pDlgOpenglViewer)
	{
		m_pDlgOpenglViewer->m_f3DScale = fScale;
		m_pDlgOpenglViewer->m_bComplete = FALSE;
		m_pDlgOpenglViewer->m_bResizing = true;
		m_pDlgOpenglViewer->OnPaint();
	}


}


void CDlgOpenglViewerDialog::OnBnClickedCheck1()
{
	g_b3DCheckLine = m_checkVerwLine.GetCheck();

	if(m_pDlgOpenglViewer)
	{
		m_pDlgOpenglViewer->m_bComplete = FALSE;
		m_pDlgOpenglViewer->OnPaint();
	}

}
