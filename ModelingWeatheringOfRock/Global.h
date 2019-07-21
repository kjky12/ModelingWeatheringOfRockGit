//Global.h

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Define.h"
#include "Config.h"


//! 프로그램 데이터 처리
typedef enum E_DATA_PROCESS
{
	dfDATA_PROCESS_MEMORY = 0,
	dfDATA_PROCESS_FILE		
}E_DATA_PROCESS;


//! 쿠다 사용 방법
typedef enum E_LOGIC_PROCESS
{
	dfLOGIC_CPU_CPU = 0,
	dfLOGIC_GPU_GPU,
	dfLOGIC_COUNT,
}E_LOGIC_PROCESS;

//! 쿠다 사용 방법
typedef enum E_DATA_MEM_IDX
{
	dfVOXEL_IDX = 0,
	dfX_IDX,
	dfY_IDX,
	dfZ_IDX,
	dfTOTAL_IDX,
	dfTOTAL_COUNT,
}E_DATA_MEM_IDX;


// 전역 변수 설정----------------------------------------------------------------------------------------------------------

// Map <조각 ID / 구성 정보(3차원 X(가로)*Y(세로)*Z(높이)> 
extern	map<int,vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>>		g_MapBaseRockData;	
// Map <배열 XYZ 위치  / 입자 위치 정보> 
extern	map<CString,ST_PARTICLE_POS>							g_MapOutsideData;	

//방향 - 0:상, 1:하, 2:좌, 3:우, 4:앞, 5:뒤
extern	map<int, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>>				g_MapShowData;	// Map <배열 XYZ 위치  / 입자 위치 정보> 
extern	map<int, vector<vector<ST_VOXEL_3D_POS_VIEWER>>>				g_mapShowDataLink;	//! 가시화를위해 연결..



extern	vector<ST_VOXEL_3D_POS>									g_CompletevecVoxelPos;

extern	map<CString,ST_VOXEL_3D_POS>								g_MapCompleteVoxelPos;
extern	vector<ST_DELETE_VOXEL_POS>									g_vecCompleteVoxelPos;
extern	vector<ST_DELETE_VOXEL_POS>										g_vecCompleteVoxelPosSolid; //! 내부 복셀까지 복셀 위치

// 메모리처리 ----------------------------------------------------------------------------------------------------------
//! 복셀의 1, 0
extern	vector<bool>											g_vecVoxelState;
extern	vector<bool>											g_vecVoxelDivState[dfTHREAD_MAX_CNT];

extern	vector<bool>											g_vecVoxelXState;
extern	vector<bool>											g_vecVoxelYState;
extern	vector<bool>											g_vecVoxelZState;
extern	vector<bool>											g_vecVoxelTotal;

extern	vector<bool>											g_vecVoxelTotalCuda;



extern	vector<ST_VOXEL_FILE_POS>								g_vecNumData;
extern	vector<ST_VOXEL_FILE_POS>								g_vecSurData;


extern	BOOL													g_bVoxelComplete;
extern	float													g_fRectSize;
extern	int														g_nSlideView;

extern	int														g_n3DShowType;
extern	int														g_n3DShowEye;

extern	int														g_nDivideCnt;
extern	int														g_nSelectAxis;

//! 가시화를 위한 변수
extern	int														g_n3DSeeType;
extern	int														g_n3DSeeEye;
extern	bool													g_b3DCheckLine;

extern  CString													g_strFilePathName;

// 값
extern CArray<double,double>				g_arrValue[2];
// 색상
extern CArray<COLORREF,COLORREF>			g_arrColor[2];

//! 뷰에서 쓰기위해..
extern int g_nXVoxCnt;
extern int g_nYVoxCnt;
extern int g_nZVoxCnt;

extern float								g_fRockParticleSize;


extern CString g_strModulePath;

// 전역 함수 설정----------------------------------------------------------------------------------------------------------


extern void gf_Init();
extern void gf_Exit();

extern void gf_GetEfieldColor(double dValue,int &nRed,int &nGreen,int &nBlue, int nType = 0);

// 실행파일 위치 가져오기
extern CString	gf_GetModulePath();


// 텍스트 입력 파일 위치에 파일을 생성
extern BOOL	gf_CreateTextFile(CString strFilePath);
// 입력한 경로의 파일에 텍스트를 입력.
extern BOOL	gf_InsertTextFile(CString strFilePath,CString strInsetText);
// 입력한 경로의 파일 삭제
extern BOOL	gf_DeleteTextFile(CString strFilePath);


// AsciiToUTF
extern void		gf_AsciiToUTF(CString strText, LPSTR &lpTemp);
extern LPSTR	gf_AsciiToUTF(CString strText);							// AsciiToUTF


// ini 파일에서 CRect 좌표 가져오는 함수
extern CRect gf_GetRectCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// ini 파일에서 한줄 Edit CRect 좌표 가져오는 함수
extern CRect gf_GetSingleTextCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// ini 파일에서 Skin List CRect 좌표 가져오는 함수
extern CRect gf_GetSkinListCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// ini 파일에서 CPoint 좌표 가져오는 함수(left & Top 좌표만 가져옴)
extern CPoint gf_GetPtCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// 콤마 분해하는 함수들-------------------------------------------------------------
extern CRect gf_CnvtStringToRect(CString sRect);
extern CPoint gf_CnvtStringToPt(CString sRect);
// 텍스트 전체 가져오는 함수 우선 사용 안함!!
extern CString gf_GetCtrlText(LPCTSTR sPath,LPCTSTR lpGrpName, LPCTSTR lpCtrlName);
//-----------------------------------------------------------------------------------

// 삼각형 폴리곤과 편선의 교차점 찾기 알고리즘
extern int GetRayIntersectsTriangle(ST_VECTOR p, ST_VECTOR d,ST_VECTOR v0, ST_VECTOR v1, ST_VECTOR v2);

#endif