// PieChartCtrl.cpp : implementation file
//
// Written by Yuheng Zhao (yuheng@ministars.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.

#include "stdafx.h"
#include "PieChartCtrl.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double pi = 3.1415926535;

/////////////////////////////////////////////////////////////////////////////
// CPieChartCtrl

CPieChartCtrl::CPieChartCtrl()
{
	CPieChartCtrl::RegisterWndClass(AfxGetInstanceHandle());

	m_fStartAngle = 0;
	m_colorLine = RGB(0,0,0);
	m_colorDefault = RGB(0,0,255);
	
	m_rectChart.SetRect(0,0,0,0);
	m_bCaptured = FALSE;

	m_strTitle = _T("");

	m_fontTitle.CreateFont(15, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
	
	m_fontInfo.CreateFont(13, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
}

CPieChartCtrl::~CPieChartCtrl()
{
	Reset();
}


BEGIN_MESSAGE_MAP(CPieChartCtrl, CWnd)
	//{{AFX_MSG_MAP(CPieChartCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPieChartCtrl message handlers

BOOL CPieChartCtrl::RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.lpszClassName = L"PIE_CHART_CTRL"; // matches class name in client
	wc.hInstance = hInstance;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hCursor = ::LoadCursor(NULL, IDC_CROSS);
	wc.hIcon = 0;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH) ::GetStockObject(LTGRAY_BRUSH);
	wc.style = CS_GLOBALCLASS; // To be modified
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	return (::RegisterClass(&wc) != 0);
}

void CPieChartCtrl::OnPaint() 
{
	if (m_rectChart.IsRectEmpty()) // First Time
		RecalcRect();

	CPaintDC dc(this); // device context for painting
	CRect clientRect;
	GetClientRect(&clientRect);
	
	CBitmap *pOldMemDCBitmap = NULL;	
	CDC memDC;
	CBitmap bitmap;
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap( &dc, clientRect.Width(), clientRect.Height() );
	CBitmap *pOldmemDCBitmap = (CBitmap*)memDC.SelectObject(&bitmap);
	
	memDC.FillSolidRect(clientRect, GetSysColor(COLOR_3DFACE));

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_colorLine);
	CPen* pOldPen = (CPen*)memDC.SelectStockObject(NULL_PEN);

	CBrush brush;
	brush.CreateSolidBrush(m_colorDefault);
	CBrush* pOldBrush = memDC.SelectObject(&brush);
	memDC.Ellipse(m_rectChart);

	int nCount = m_chartPieces.GetSize();
	CPieChartPiece* pItem;

	CPoint pt1, pt2;
	float fCurrectAngle = m_fStartAngle;
	int nInfo=0;
	CountPoint(fCurrectAngle, pt1);
	for (int i=0; i<nCount; i++)
	{
		pItem = m_chartPieces.GetAt(i);
		fCurrectAngle += pItem->m_fAngle;
		CountPoint(fCurrectAngle, pt2);

		memDC.SelectStockObject(NULL_PEN);
		memDC.SelectObject(&pItem->m_brushBack);
		
		if (pt2!=pt1)
			memDC.Pie(m_rectChart, pt2, pt1);
		
		//Draw line
		memDC.SelectObject(&pen);
		memDC.MoveTo(pt1);
		memDC.LineTo(m_rectChart.CenterPoint());
		memDC.LineTo(pt2);

		//Draw info
		CFont* pOldFont = memDC.SelectObject(&m_fontInfo);
		memDC.SetBkMode(TRANSPARENT);

		if (!(pItem->m_strInfo).IsEmpty())
		{
			CSize sz = memDC.GetTextExtent(pItem->m_strInfo);

			CRect rcColor(clientRect);
			rcColor.left = rcColor.right - sz.cy;
			rcColor.bottom = rcColor.top + sz.cy;
			rcColor.OffsetRect(0, nInfo*rcColor.Height());//+m_nTitleHeight);
			
			memDC.FillSolidRect(rcColor, pItem->m_colorBack); // i must be less than MAX_COLOR
			memDC.DrawEdge(rcColor, EDGE_SUNKEN, BF_RECT);
		
			memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			rcColor.right = rcColor.left - 3;
			rcColor.left = clientRect.left;
			memDC.DrawText(pItem->m_strInfo, rcColor, DT_RIGHT|DT_VCENTER);

			nInfo++;
		}

		// Draw percent
		if (pItem->m_fAngle>25.0)
		{
			float n = fCurrectAngle - (pItem->m_fAngle)/2.0;
			CPoint p;
			CountPoint(n, p, TRUE);
			CString str;
			str.Format(L"%.2f%%", (double)(pItem->m_fAngle)*100.0/360.0);
			CSize sz = memDC.GetTextExtent(str);
			memDC.SetTextColor(pItem->m_colorText);
			memDC.TextOut(p.x-sz.cx/2, p.y-sz.cy/2, str);
		}

		memDC.SelectObject(pOldFont);
		
		pt1 = pt2;
	}
	
	// Draw Line for the out circle
	memDC.SelectObject(&pen);
	memDC.SelectStockObject(NULL_BRUSH);
	memDC.Ellipse(m_rectChart);

	//Draw Title
	if (!m_strTitle.IsEmpty())
	{
		CFont* pOldFont = memDC.SelectObject(&m_fontTitle);
		memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		memDC.SetBkMode(TRANSPARENT);
		memDC.TextOut(1, 1, m_strTitle);
		memDC.SelectObject(pOldFont);
	}

	dc.BitBlt( 0, 0, clientRect.Width(), clientRect.Height(), 
                                       &memDC, 0, 0, SRCCOPY );
	memDC.SelectObject(pOldPen);
	memDC.SelectObject(&pOldBrush);
	memDC.SelectObject(pOldmemDCBitmap);
}

void CPieChartCtrl::OnLButtonUp(UINT, CPoint) 
{
	if (m_bCaptured)
	{
		::ReleaseCapture();
		m_bCaptured = FALSE;
	}	
}

void CPieChartCtrl::OnLButtonDown(UINT, CPoint point) 
{
	SetCapture();
	m_bCaptured = TRUE;
	m_ptOldPt = point;
}

void CPieChartCtrl::OnMouseMove(UINT, CPoint point) 
{
	if (m_bCaptured)
	{
		int nOffX = point.x - m_ptOldPt.x;

		m_fStartAngle += nOffX;

		InvalidateRect(m_rectChart, FALSE);

		m_ptOldPt = point;
	}
}

void CPieChartCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	RecalcRect();
}


void CPieChartCtrl::RecalcRect()
{
	CRect rect;
	GetClientRect(&rect);
	if (!m_strTitle.IsEmpty())
	{
		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(&m_fontTitle);
		CSize sz = dc.GetTextExtent(m_strTitle);
		m_nTitleHeight = sz.cy;
		rect.top += sz.cy*2;// Leave  lines.
		dc.SelectObject(pOldFont);
	}

	int nSize = (rect.Width()>rect.Height())?rect.Height():rect.Width();

	m_rectChart = CRect(CPoint(rect.left + (rect.Width()-nSize)/2, 
		rect.top + (rect.Height()-nSize)), 
		CSize(nSize, nSize));
}


BOOL CPieChartCtrl::AddPiece(COLORREF colorBack, COLORREF colorText,
							 float fAngle, const CString& str)
{
	CPieChartPiece* pItem = new CPieChartPiece;
	pItem -> m_colorBack = colorBack;
	pItem -> m_colorText = colorText;
	pItem -> m_brushBack.CreateSolidBrush(colorBack);
	pItem -> m_fAngle = fAngle;
	pItem -> m_strInfo = str;

	try 
	{
		m_chartPieces.Add(pItem);

		InvalidateRect(NULL, FALSE);
		return TRUE;
	}
	catch (CMemoryException* e)
	{
		if (pItem !=NULL) 
			delete pItem;
		e->Delete();
		return FALSE;
	}
}


// bPercent is TRUE when counting the position for the percent info
void CPieChartCtrl::CountPoint(float fAngle, CPoint & pt, BOOL bPercent)
{
	while (fAngle <0)
		fAngle += 360;

	while (fAngle>359)
		fAngle -= 360;

	double dAngle = ((double)fAngle)*pi/(double)180.0;

	double r;
	r = ((double)m_rectChart.Height())/2.0;
	if (bPercent)
		r = r*3.0/5.0;

	double dOffX = (r*sin(dAngle));
	double dOffY = 0.0 - (r*cos(dAngle));

	double dX = ((double)(m_rectChart.right+m_rectChart.left))/2.0;
	double dY = ((double)(m_rectChart.top+m_rectChart.bottom))/2.0;
	
	pt.x = (int)(dX + dOffX);
	pt.y = (int)(dY + dOffY);
}


void CPieChartCtrl::Reset()
{
	int nCount = m_chartPieces.GetSize();
	
	for (int i = 0; i < nCount; i++)
		delete m_chartPieces.GetAt(i);
	m_chartPieces.RemoveAll();
}

void CPieChartCtrl::GetItemColor(int i, COLORREF& color)
{
	int nCount = m_chartPieces.GetSize();
	if (i>=nCount)
		i=nCount-1;

	color = (m_chartPieces.GetAt(i))->m_colorBack;
}

void CPieChartCtrl::SetTitle(const CString & str)
{
	m_strTitle = str;
	RecalcRect();
	InvalidateRect(NULL, FALSE);
}
