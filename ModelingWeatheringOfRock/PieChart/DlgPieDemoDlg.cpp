// DlgPieDemoDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ModelingWeatheringOfRock.h"
#include "DlgPieDemoDlg.h"
#include "afxdialogex.h"


// CDlgPieDemoDlg ��ȭ �����Դϴ�.

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


// CDlgPieDemoDlg �޽��� ó�����Դϴ�.


void CDlgPieDemoDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnOK();
}


void CDlgPieDemoDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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