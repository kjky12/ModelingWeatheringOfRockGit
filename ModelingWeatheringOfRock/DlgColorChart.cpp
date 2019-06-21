// DlgColorChart.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelingWeatheringOfRock.h"
#include "DlgColorChart.h"


// CDlgColorChart 대화 상자입니다.

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


// CDlgColorChart 메시지 처리기입니다.

void CDlgColorChart::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	return;
	OnOK();
}

void CDlgColorChart::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	return;
	OnCancel();
}

void CDlgColorChart::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	int indexSize = g_arrValue[m_nType].GetCount();

	if ( indexSize <= 0 )
	{
		return;
	}
	CString strTemp = L"";

	GRADIENT_RECT rect;

	CRect colorRectSize;
	GetClientRect(&colorRectSize);

	//	색상 인덱스를 표시하는 영역을 확인한다. 
	int _height  = colorRectSize.Height();
	int _width = colorRectSize.Width();

	CPoint ptCenter = colorRectSize.CenterPoint();

	CFont *pftOld, FontInfo;

	//사용할 폰트생성

	FontInfo.CreateFont(12, 0,0,0, FW_HEAVY, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"돋움");


	CPen GrayPen, *oldpen;
	// 자주색으로 변경 [6/21/2012 박준영]
	//GrayPen.CreatePen(PS_DOT, 1, RGB(220,220,220));
	GrayPen.CreatePen(PS_DOT, 1, RGB(230,40,200));
	oldpen=dc.SelectObject(&GrayPen);

	//폰트와 색상 지정
	pftOld = dc.SelectObject(&FontInfo);
	dc.SetBkMode(TRANSPARENT);

	// 검정색으로 변경 [6/21/2012 박준영]
	//dc.SetTextColor(RGB(255,255,255));
	dc.SetTextColor(RGB(0,0,0));

	CRect rcText;

	UINT nFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	UINT nFormatLeft = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
	UINT nFormatRight = DT_RIGHT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;

	double dValue = 0.0;
	double dValue1 = 0.0;


	//	색상 범례 숫자로부터 색상을 표시할 영역의 갯수를 확인한다. 
	int rectCount	 = indexSize - 1;

	//	각각의 색상 표시 영역의 폭을 계산한다. 
	double rectHeight = (double)_height / (double)rectCount;

	TRIVERTEX vert[2];
	rect.UpperLeft  = 0;
	rect.LowerRight = 1;

	int i;

	for ( i = 0; i < rectCount; i++)
	{
		//	그라데이션의 시작 좌표
		vert[0].x		= 0;
		vert[0].y		= rectHeight * i;

		//	그라데이션의 시작 색상
		vert[0].Red		= GetRValue( g_arrColor[m_nType].GetAt(rectCount - i) ) << 8;
		vert[0].Green	= GetGValue( g_arrColor[m_nType].GetAt(rectCount - i) ) << 8;
		vert[0].Blue	= GetBValue( g_arrColor[m_nType].GetAt(rectCount - i) ) << 8;
		vert[0].Alpha	= 0x0000;

		//	그라데이션의 끝 좌표
		vert[1].x		= _width;
		vert[1].y		= rectHeight * (i+1);

		//	그라데이션의 끝 색상
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
		//	그라데이션의 시작 좌표
		vert[0].x		= 0;
		vert[0].y		= rectHeight * i;

		//	그라데이션의 끝 좌표
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
		// 텍스트를 화면에 표시
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
	// 텍스트를 화면에 표시
	dc.DrawText(strTemp, &rcText,  nFormat);


	dc.SelectObject(oldpen);
	// 폰트 초기화
	dc.SelectObject(pftOld);

	GrayPen.DeleteObject();
	// 폰트 삭제
	FontInfo.DeleteObject();

	FillColor();
}




void CDlgColorChart::FillColor()
{

	

}