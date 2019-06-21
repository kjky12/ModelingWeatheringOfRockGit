// DlgColorChart.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ModelingWeatheringOfRock.h"
#include "DlgColorChart.h"


// CDlgColorChart ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgColorChart, CDialog)

CDlgColorChart::CDlgColorChart(CWnd* pParent, int nType)
	: CDialog(CDlgColorChart::IDD, pParent)
{
	m_nType = nType;
}

CDlgColorChart::~CDlgColorChart()
{
}

void CDlgColorChart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgColorChart, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgColorChart::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgColorChart::OnBnClickedCancel)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgColorChart �޽��� ó�����Դϴ�.

void CDlgColorChart::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	return;
	OnOK();
}

void CDlgColorChart::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	return;
	OnCancel();
}

void CDlgColorChart::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.

	int indexSize = g_arrValue[m_nType].GetCount();

	if ( indexSize <= 0 )
	{
		return;
	}
	CString strTemp = L"";

	GRADIENT_RECT rect;

	CRect colorRectSize;
	GetClientRect(&colorRectSize);

	//	���� �ε����� ǥ���ϴ� ������ Ȯ���Ѵ�. 
	int _height  = colorRectSize.Height();
	int _width = colorRectSize.Width();

	CPoint ptCenter = colorRectSize.CenterPoint();

	CFont *pftOld, FontInfo;

	//����� ��Ʈ����

	FontInfo.CreateFont(12, 0,0,0, FW_HEAVY, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"����");


	CPen GrayPen, *oldpen;
	// ���ֻ����� ���� [6/21/2012 ���ؿ�]
	//GrayPen.CreatePen(PS_DOT, 1, RGB(220,220,220));
	GrayPen.CreatePen(PS_DOT, 1, RGB(230,40,200));
	oldpen=dc.SelectObject(&GrayPen);

	//��Ʈ�� ���� ����
	pftOld = dc.SelectObject(&FontInfo);
	dc.SetBkMode(TRANSPARENT);

	// ���������� ���� [6/21/2012 ���ؿ�]
	//dc.SetTextColor(RGB(255,255,255));
	dc.SetTextColor(RGB(0,0,0));

	CRect rcText;

	UINT nFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	UINT nFormatLeft = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	UINT nFormatRight = DT_RIGHT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;

	double dValue = 0.0;
	double dValue1 = 0.0;


	//	���� ���� ���ڷκ��� ������ ǥ���� ������ ������ Ȯ���Ѵ�. 
	int rectCount	 = indexSize - 1;

	//	������ ���� ǥ�� ������ ���� ����Ѵ�. 
	double rectHeight = (double)_height / (double)rectCount;

	TRIVERTEX vert[2];
	rect.UpperLeft  = 0;
	rect.LowerRight = 1;

	int i;

	for ( i = 0; i < rectCount; i++)
	{
		//	�׶��̼��� ���� ��ǥ
		vert[0].x		= 0;
		vert[0].y		= rectHeight * i;

		//	�׶��̼��� ���� ����
		vert[0].Red		= GetRValue( g_arrColor[m_nType].GetAt(rectCount - i) ) << 8;
		vert[0].Green	= GetGValue( g_arrColor[m_nType].GetAt(rectCount - i) ) << 8;
		vert[0].Blue	= GetBValue( g_arrColor[m_nType].GetAt(rectCount - i) ) << 8;
		vert[0].Alpha	= 0x0000;

		//	�׶��̼��� �� ��ǥ
		vert[1].x		= _width;
		vert[1].y		= rectHeight * (i+1);

		//	�׶��̼��� �� ����
		vert[1].Red		= GetRValue( g_arrColor[m_nType].GetAt(rectCount - i-1) ) << 8;
		vert[1].Green	= GetGValue( g_arrColor[m_nType].GetAt(rectCount - i-1) ) << 8;
		vert[1].Blue	= GetBValue( g_arrColor[m_nType].GetAt(rectCount - i-1) ) << 8;
		vert[1].Alpha	= 0x0000;

		GradientFill(dc.m_hDC, vert, 2, &rect, 1, GRADIENT_FILL_RECT_V);

		dc.MoveTo( vert[0].x, vert[0].y );  
		dc.LineTo( vert[1].x, vert[0].y );


	}

	for ( i = 0; i < rectCount; i++)
	{
		//	�׶��̼��� ���� ��ǥ
		vert[0].x		= 0;
		vert[0].y		= rectHeight * i;

		//	�׶��̼��� �� ��ǥ
		vert[1].x		= _width;
		vert[1].y		= rectHeight * (i+1);

		dValue = g_arrValue[m_nType].GetAt(rectCount - i - 1);
		strTemp.Format( L"%g", dValue);


		if(i == (rectCount - 1))
		{
			rcText.left = ptCenter.x - 50;
			rcText.top = vert[1].y- 14;
			rcText.right = rcText.left + 100;
			rcText.bottom = 14 + rcText.top;
		}
		else
		{
			rcText.left = ptCenter.x - 50;
			rcText.top = vert[1].y- 7;
			rcText.right = rcText.left + 100;
			rcText.bottom = 14 + rcText.top;
		}
		// �ؽ�Ʈ�� ȭ�鿡 ǥ��
		dc.DrawText(strTemp, &rcText,  nFormat);

	}

	dc.MoveTo( vert[0].x-1, vert[0].y );  
	dc.LineTo( vert[1].x-1, vert[0].y );

	dValue = g_arrValue[m_nType].GetAt(rectCount);
	strTemp.Format( L"%g", dValue);

	//		dc.TextOutW( vert[0].x, vert[1].y, strTemp);
	rcText.left = ptCenter.x - 50;
	rcText.top = 0;
	rcText.right = rcText.left + 100;
	rcText.bottom = 15 + rcText.top;
	// �ؽ�Ʈ�� ȭ�鿡 ǥ��
	dc.DrawText(strTemp, &rcText,  nFormat);


	dc.SelectObject(oldpen);
	// ��Ʈ �ʱ�ȭ
	dc.SelectObject(pftOld);

	GrayPen.DeleteObject();
	// ��Ʈ ����
	FontInfo.DeleteObject();

	FillColor();
}




void CDlgColorChart::FillColor()
{

	

}