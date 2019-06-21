//EAGlobal.cpp


#include "stdafx.h"
#include "Global.h"

#include <math.h>
// 전역 변수 설정----------------------------------------------------------------------------------------------------------

map<int,vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>>		g_MapBaseRockData;	// Map <조각 ID / 구성 정보(3차원 X(가로)*Y(세로)*Z(높이)> 
map<CString,ST_PARTICLE_POS>								g_MapOutsideData;	// Map <배열 XYZ 위치  / 입자 위치 정보> 

//방향 - 초기 -> 0:상, 1:하, 2:좌, 3:우, 4:앞, 5:뒤 현재 처리된 복셀 데이터로 -> 0:상, 1:하, 2:좌, 3:우, 4:앞, 5:뒤
map<int, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>>				g_MapShowData;	// map< 방향, <배열 XYZ 위치  / 입자 위치 정보> >
map<int, vector<vector<ST_VOXEL_3D_POS_VIEWER>>>				g_mapShowDataLink;	//! 가시화를위해 연결..




vector<ST_VOXEL_3D_POS>										g_CompletevecVoxelPos;

map<CString,ST_VOXEL_3D_POS>									g_MapCompleteVoxelPos;
vector<ST_DELETE_VOXEL_POS>										g_vecCompleteVoxelPos; //! 외부 복셀 위치
vector<ST_DELETE_VOXEL_POS>										g_vecCompleteVoxelPosSolid; //! 내부 복셀까지 복셀 위치


// 메모리처리 ----------------------------------------------------------------------------------------------------------
//! 복셀의 1, 0
vector<bool>											g_vecVoxelState;
vector<bool>											g_vecVoxelDivState[dfTHREAD_MAX_CNT];

vector<bool>											g_vecVoxelXState;
vector<bool>											g_vecVoxelYState;
vector<bool>											g_vecVoxelZState;
vector<bool>											g_vecVoxelTotal;

vector<bool>											g_vecVoxelXStateCuda;
vector<bool>											g_vecVoxelYStateCuda;
vector<bool>											g_vecVoxelZStateCuda;
vector<bool>											g_vecVoxelTotalCuda;


vector<bool>											g_vecVoxelTotalCudaTemp;


vector<ST_VOXEL_FILE_POS>								g_vecNumData;
vector<ST_VOXEL_FILE_POS>								g_vecSurData;


BOOL														g_bVoxelComplete;
float														g_fRectSize;
int															g_nSlideView;

int														g_n3DShowType;
int														g_n3DShowEye;
int														g_nDivideCnt;
int														g_nSelectAxis;

//! 가시화 선택
int														g_n3DSeeType;
int														g_n3DSeeEye;
bool													g_b3DCheckLine;

CString														g_strFilePathName;

// 값
CArray<double,double>				g_arrValue[2];
// 색상
CArray<COLORREF,COLORREF>			g_arrColor[2];

float								g_fRockParticleSize;

int g_nXVoxCnt;
int g_nYVoxCnt;
int g_nZVoxCnt;


CString g_strModulePath;

// 전역 함수 설정----------------------------------------------------------------------------------------------------------



void gf_Init()
{

	g_nSlideView = 100;
	g_bVoxelComplete = FALSE;
	g_CompletevecVoxelPos.clear();
	g_MapCompleteVoxelPos.clear();
	g_vecCompleteVoxelPos.clear();

	g_arrValue[0].Add(0.0);
	g_arrValue[0].Add(1.0);
	g_arrValue[0].Add(2.0);
	g_arrValue[0].Add(3.0);
	g_arrValue[0].Add(4.0);
	g_arrValue[0].Add(5.0);
	g_arrValue[0].Add(6.0);
	g_arrValue[0].Add(7.0);
	g_arrValue[0].Add(8.0);
	g_arrValue[0].Add(9.0);
	g_arrValue[0].Add(10.0);

	g_arrColor[0].Add( RGB(0,0,255));
	g_arrColor[0].Add( RGB(0,54,255));
//	g_arrColor[0].Add( RGB(0,11,255));
	g_arrColor[0].Add( RGB(0,173,255));
	g_arrColor[0].Add( RGB(0,250,255));
	g_arrColor[0].Add( RGB(0,255,183));
//	g_arrColor[0].Add( RGB(0,255,121));
	g_arrColor[0].Add( RGB(0,255,68));
//	g_arrColor[0].Add( RGB(7,255,0));
	g_arrColor[0].Add( RGB(108,255,0));
	g_arrColor[0].Add( RGB(199,255,0));
	g_arrColor[0].Add( RGB(255,215,0));
//	g_arrColor[0].Add( RGB(255,133,0));
	g_arrColor[0].Add( RGB(255,76,0));
	g_arrColor[0].Add( RGB(255,0,0));


	g_arrValue[1].Add(0.0);
 	g_arrValue[1].Add(20.0);
 	g_arrValue[1].Add(40.0);
 	g_arrValue[1].Add(60.0);
 	g_arrValue[1].Add(80.0);
	g_arrValue[1].Add(100.0);

	g_arrColor[1].Add( RGB(255,0,0));
	g_arrColor[1].Add( RGB(255,129,25));
 	g_arrColor[1].Add( RGB(255,247,77));
 	g_arrColor[1].Add( RGB(200,200,74));
 	g_arrColor[1].Add( RGB(162,195,158));
	g_arrColor[1].Add( RGB(0,0,255));



	g_nXVoxCnt = 0;
	g_nYVoxCnt = 0;
	g_nZVoxCnt = 0;
		

	g_nDivideCnt = 10;

	g_n3DShowType = 0;
	g_n3DShowEye = 0;


	g_strModulePath = L"";

	g_vecVoxelState.clear();
	g_vecVoxelXState.clear();
	g_vecVoxelYState.clear();
	g_vecVoxelZState.clear();
	g_vecVoxelTotal.clear();
}


void gf_Exit()
{

}


void gf_GetEfieldColor(double dValue,int &nRed,int &nGreen,int &nBlue, int nType)
{
	nRed = 0;
	nGreen = 0;
	nBlue = 0;

	double dLevel1 = 0.0;
	double dLevel2 = 0.0;

	COLORREF crTmp;
	COLORREF crTmp2;
	int nCount = g_arrColor[nType].GetCount();

	for(int i = 0 ; i < nCount ; i++)
	{
		if(i == 0)
		{
			dLevel1 = g_arrValue[nType].GetAt(i);

			if(dLevel1 > dValue)
				return;
		}

		else
		{
			dLevel1 = g_arrValue[nType].GetAt(i-1);
			dLevel2 = g_arrValue[nType].GetAt(i);

			if(dLevel1 <= dValue && dLevel2 > dValue)
			{
				crTmp = g_arrColor[nType].GetAt(i-1);
				crTmp2 = g_arrColor[nType].GetAt(i);

				nRed = int((double)( GetRValue(crTmp2) - GetRValue(crTmp) ) / ( dLevel2 - dLevel1 ) * ( dValue - dLevel1 ) + GetRValue(crTmp));
				nGreen = int((double)( GetGValue(crTmp2) - GetGValue(crTmp) ) / ( dLevel2 - dLevel1 ) * ( dValue - dLevel1 ) + GetGValue(crTmp));
				nBlue = int((double)( GetBValue(crTmp2) - GetBValue(crTmp) ) / ( dLevel2 - dLevel1 ) * ( dValue - dLevel1 ) + GetBValue(crTmp));

				break;
			}
			else if ( dLevel2 <= dValue && i == (nCount - 1) )
			{
				crTmp2 = g_arrColor[nType].GetAt(i);

				nRed = GetRValue(crTmp2);
				nGreen = GetGValue(crTmp2);
				nBlue = GetBValue(crTmp2);
				break;
			}
		}

	}




}



/*!
	\brief 실행파일의 위치리턴
	\remarks 실행파일의 폴더 위치를 리턴합니다.
	\param 
	\return	CString
	\warning 
	\date 2008-03-17
*/
CString	gf_GetModulePath()
{
 	if(!g_strModulePath.IsEmpty())
 		return g_strModulePath;


	wchar_t szPath[MAX_PATH];
	memset(szPath, 0x00, MAX_PATH);

	::GetModuleFileName(NULL, szPath, MAX_PATH);

	CString sTempPath = szPath;	

	int iLength = sTempPath.GetLength();
	int iPos = sTempPath.ReverseFind(TCHAR('\\'));

	CString sModulePath = sTempPath.Left(iPos);
	sModulePath += L"\\";

	g_strModulePath = sModulePath;

	return sModulePath;
}

void gf_AsciiToUTF(CString strText, LPSTR &lpTemp)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, (CStringA)strText, -1, 0, 0);
	LPWSTR pWTitle = new WCHAR[nLen+1];
	pWTitle[nLen] = '\0';
	MultiByteToWideChar(CP_ACP, 0, (CStringA)strText, -1, pWTitle, nLen+sizeof(WCHAR));

	nLen = WideCharToMultiByte(CP_UTF8, 0, pWTitle, nLen, 0, 0, NULL, NULL);
	//LPSTR pBTitle = new CHAR[nLen+1];
	lpTemp = new CHAR[nLen+1];
	lpTemp[nLen] = '\0';
	WideCharToMultiByte(CP_UTF8, 0, pWTitle, -1, lpTemp, nLen+sizeof(CHAR), NULL, NULL);

	M_A_DELETE(pWTitle);
}

LPSTR gf_AsciiToUTF(CString strText)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, (CStringA)strText, -1, 0, 0);
	LPWSTR pWTitle = new WCHAR[nLen+1];
	pWTitle[nLen] = '\0';
	MultiByteToWideChar(CP_ACP, 0, (CStringA)strText, -1, pWTitle, nLen+sizeof(WCHAR));

	nLen = WideCharToMultiByte(CP_UTF8, 0, pWTitle, nLen, 0, 0, NULL, NULL);
	LPSTR pBTitle = new CHAR[nLen+1];
	pBTitle[nLen] = '\0';
	WideCharToMultiByte(CP_UTF8, 0, pWTitle, -1, pBTitle, nLen+sizeof(CHAR), NULL, NULL);

	M_A_DELETE(pWTitle);
	return pBTitle;
}



/*!
	\brief 컨트롤위 위치 리턴
	\remarks 컨트롤의 위치정보를 리턴합니다.
	\param lpGrpName  그룹명
	\param lpCtrlName 컨트롤명
	\param RectCtrl  위치
	\return BOOL
	\warning 
	\date 2008-03-17
*/

CRect gf_GetRectCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName)
{
	CString sPath = gf_GetModulePath() + dfINI_POSITION_FILE_NAME;

	wchar_t szValue[MAX_PATH];
	memset(szValue, 0, MAX_PATH);

	BOOL bResult = ::GetPrivateProfileString(lpGrpName,
		lpCtrlName,
		_T(""),
		szValue,
		MAX_PATH,
		LPCTSTR(sPath));

	CRect RectCtrl = gf_CnvtStringToRect(szValue);

	return RectCtrl;
}

CRect gf_GetSingleTextCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName)
{
	CString sPath = gf_GetModulePath() + dfINI_POSITION_FILE_NAME;

	wchar_t szValue[MAX_PATH];
	memset(szValue, 0, MAX_PATH);

	BOOL bResult = ::GetPrivateProfileString(lpGrpName,
		lpCtrlName,
		_T(""),
		szValue,
		MAX_PATH,
		LPCTSTR(sPath));

	CRect RectCtrl = gf_CnvtStringToRect(szValue);

	int nHeight = RectCtrl.Height();

	nHeight = nHeight - dfDEFAULT_FONT_SIZE;

	RectCtrl.top += (nHeight / 2);
	RectCtrl.bottom -= (nHeight / 2);

	return RectCtrl;
}

// ini 파일에서 Skin List CRect 좌표 가져오는 함수
CRect gf_GetSkinListCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName)
{
	CString sPath = gf_GetModulePath() + dfINI_POSITION_FILE_NAME;

	wchar_t szValue[MAX_PATH];
	memset(szValue, 0, MAX_PATH);

	BOOL bResult = ::GetPrivateProfileString(lpGrpName,
		lpCtrlName,
		_T(""),
		szValue,
		MAX_PATH,
		LPCTSTR(sPath));

	CRect RectCtrl = gf_CnvtStringToRect(szValue);

 	RectCtrl.right += 8;
 	RectCtrl.bottom += 5;

	return RectCtrl;
}

CPoint gf_GetPtCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName)
{
	CString sPath = gf_GetModulePath() + dfINI_POSITION_FILE_NAME;

	wchar_t szValue[MAX_PATH];
	memset(szValue, 0, MAX_PATH);

	BOOL bResult = ::GetPrivateProfileString(lpGrpName,
		lpCtrlName,
		_T(""),
		szValue,
		MAX_PATH,
		LPCTSTR(sPath));

	CPoint ptCtrl = gf_CnvtStringToPt(szValue);

	return ptCtrl;
}

/*!
\brief String 위치정보를 Rect 로 변경
\remarks String 위치정보를 Rect 로 변경합니다.
\param sRect  
\param  
\return CRect
\warning 
\date 2008-03-17
\author	IM Tech R&D Dept
*/

CRect gf_CnvtStringToRect(CString sRect)
{
	int		iField = 0;
	CString sPos[4];
	int i;
	for ( i = 0; i < 4; i++)
		sPos[i] = _T("");	

	for (i = 0; i < sRect.GetLength(); i++)
	{
		if (sRect.GetAt(i) == 0x2C)
		{
			iField++;
			continue;
		}

		sPos[iField] += sRect.GetAt(i);
	}

	return CRect(atoi((CStringA)sPos[0]), atoi((CStringA)sPos[1]), atoi((CStringA)sPos[2]), atoi((CStringA)sPos[3]));
}

CPoint gf_CnvtStringToPt(CString sRect)
{
	int		iField = 0;
	CString sPos[4];
	int i;
	for ( i = 0; i < 4; i++)
		sPos[i] = _T("");	

	for (i = 0; i < sRect.GetLength(); i++)
	{
		if (sRect.GetAt(i) == 0x2C)
		{
			iField++;
			continue;
		}

		sPos[iField] += sRect.GetAt(i);
	}

	return CPoint(atoi((CStringA)sPos[0]), atoi((CStringA)sPos[1]));
}

/*!
	\brief 컨트롤 Text 리턴
	\remarks 컨트롤의 Text 를 리턴합니다.
	\param lpGrpName  그룹명
	\param lpCtrlName 컨트롤명
	\return CString
	\warning 
	\date 2008-03-17
*/

CString gf_GetCtrlText(LPCTSTR sPath,LPCTSTR lpGrpName, LPCTSTR lpCtrlName)
{
	CString strText;

	wchar_t szValue[32768];
	memset(szValue, 0, 32768);

	BOOL bResult = ::GetPrivateProfileString(lpGrpName,
		lpCtrlName,
		_T(""),
		szValue,
		32768,
		sPath);

	strText = szValue;

	return strText;
}



// 텍스트 입력 파일 위치에 파일을 생성
BOOL gf_CreateTextFile(CString strFilePath)
{
	CStdioFile File;
	
	if(!File.Open(strFilePath, CFile::modeCreate|CFile::modeReadWrite ))
		return FALSE;
	
	File.Close();

	return TRUE;


}
// 입력한 경로의 파일에 텍스트를 입력.
BOOL gf_InsertTextFile(CString strFilePath,CString strInsetText)
{
	CStdioFile File;
	if(!File.Open(strFilePath,CFile::modeReadWrite | CFile::modeNoTruncate))
	{
		return FALSE;
	}

	File.SeekToEnd();
	File.WriteString(strInsetText);
	File.Close();

	return TRUE;

}
// 입력한 경로의 파일 삭제
BOOL gf_DeleteTextFile(CString strFilePath)
{
	CStdioFile File;
	File.Remove(strFilePath);
	return TRUE;
	
}

void CaclVector(ST_VECTOR& a,ST_VECTOR& b,ST_VECTOR& c) 
{
	a.fX = b.fX - c.fX;	
	a.fY = b.fY - c.fY;	
	a.fZ = b.fZ - c.fZ;
}

float innerProduct(ST_VECTOR v,ST_VECTOR q)
{
	return v.fX * q.fX + v.fY * q.fY +v.fZ * q.fZ;
}



void crossProduct(ST_VECTOR& a,ST_VECTOR& b,ST_VECTOR& c)
{
	a.fX = b.fY * c.fZ - c.fY * b.fZ;
	a.fY = b.fZ * c.fX - c.fZ * b.fX;
	a.fZ = b.fX * c.fY - c.fX * b.fY;
}



// 삼각형 폴리곤과 편선의 교차점 찾기 알고리즘
int GetRayIntersectsTriangle(ST_VECTOR p, ST_VECTOR d,ST_VECTOR v0, ST_VECTOR v1, ST_VECTOR v2)
{
	ST_VECTOR e1,e2,h,s,q;
	float a,f,u,v;
	CaclVector(e1,v1,v0);
	CaclVector(e2,v2,v0);

	crossProduct(h,d,e2);
	a = innerProduct(e1,h);

 	if (a > -0.00001 && a < 0.00001)
 		return(false);

	f = 1/a;
	CaclVector(s,p,v0);
	u = f * (innerProduct(s,h));

	if (u < 0.0 || u > 1.0)
		return(false);

	crossProduct(q,s,e1);
	v = f * innerProduct(d,q);

	if (v < 0.0 || u + v > 1.0)
		return(false);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	float t = f * innerProduct(e2,q);

	if (t > 0.00001) // ray intersection
		return(true);

	else // this means that there is a line intersection
		// but not a ray intersection
		return (false);

}