// DlgPieDemoDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelingWeatheringOfRock.h"
#include "DlgPieDemoDlg.h"
#include "afxdialogex.h"


// CDlgPieDemoDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgPieDemoDlg, CDialogEx)

CDlgPieDemoDlg::CDlgPieDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPieDemoDlg::IDD, pParent)
{

}

CDlgPieDemoDlg::~CDlgPieDemoDlg()
{
}

void CDlgPieDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPieDemoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgPieDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgPieDemoDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgPieDemoDlg 메시지 처리기입니다.


void CDlgPieDemoDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void CDlgPieDemoDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


BOOL CDlgPieDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	m_wndChart.SubclassDlgItem(IDC_PIECHART1, this);

	UpdateData();
	for (int v = 0; v < vecPiStruct.size(); v++)
		m_wndChart.AddPiece(vecPiStruct[v].crBack, vecPiStruct[v].crText, vecPiStruct[v].fAngle, vecPiStruct[v].strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgPieDemoDlg::AddPice(float fAngle, CString strTitle, COLORREF crBack, COLORREF crText)
{
	
	ST_PI_Struct stTemp(crBack, crText, fAngle, strTitle);
	vecPiStruct.push_back(stTemp);
}

void CDlgPieDemoDlg::ResetPice()
{
	m_wndChart.Reset();
	m_wndChart.InvalidateRect(NULL, FALSE);
}