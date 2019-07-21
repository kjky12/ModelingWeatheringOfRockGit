//Global.h

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Define.h"
#include "Config.h"


//! ���α׷� ������ ó��
typedef enum E_DATA_PROCESS
{
	dfDATA_PROCESS_MEMORY = 0,
	dfDATA_PROCESS_FILE		
}E_DATA_PROCESS;


//! ��� ��� ���
typedef enum E_LOGIC_PROCESS
{
	dfLOGIC_CPU_CPU = 0,
	dfLOGIC_GPU_GPU,
	dfLOGIC_COUNT,
}E_LOGIC_PROCESS;

//! ��� ��� ���
typedef enum E_DATA_MEM_IDX
{
	dfVOXEL_IDX = 0,
	dfX_IDX,
	dfY_IDX,
	dfZ_IDX,
	dfTOTAL_IDX,
	dfTOTAL_COUNT,
}E_DATA_MEM_IDX;


// ���� ���� ����----------------------------------------------------------------------------------------------------------

// Map <���� ID / ���� ����(3���� X(����)*Y(����)*Z(����)> 
extern	map<int,vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>>		g_MapBaseRockData;	
// Map <�迭 XYZ ��ġ  / ���� ��ġ ����> 
extern	map<CString,ST_PARTICLE_POS>							g_MapOutsideData;	

//���� - 0:��, 1:��, 2:��, 3:��, 4:��, 5:��
extern	map<int, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>>				g_MapShowData;	// Map <�迭 XYZ ��ġ  / ���� ��ġ ����> 
extern	map<int, vector<vector<ST_VOXEL_3D_POS_VIEWER>>>				g_mapShowDataLink;	//! ����ȭ������ ����..



extern	vector<ST_VOXEL_3D_POS>									g_CompletevecVoxelPos;

extern	map<CString,ST_VOXEL_3D_POS>								g_MapCompleteVoxelPos;
extern	vector<ST_DELETE_VOXEL_POS>									g_vecCompleteVoxelPos;
extern	vector<ST_DELETE_VOXEL_POS>										g_vecCompleteVoxelPosSolid; //! ���� �������� ���� ��ġ

// �޸�ó�� ----------------------------------------------------------------------------------------------------------
//! ������ 1, 0
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

//! ����ȭ�� ���� ����
extern	int														g_n3DSeeType;
extern	int														g_n3DSeeEye;
extern	bool													g_b3DCheckLine;

extern  CString													g_strFilePathName;

// ��
extern CArray<double,double>				g_arrValue[2];
// ����
extern CArray<COLORREF,COLORREF>			g_arrColor[2];

//! �信�� ��������..
extern int g_nXVoxCnt;
extern int g_nYVoxCnt;
extern int g_nZVoxCnt;

extern float								g_fRockParticleSize;


extern CString g_strModulePath;

// ���� �Լ� ����----------------------------------------------------------------------------------------------------------


extern void gf_Init();
extern void gf_Exit();

extern void gf_GetEfieldColor(double dValue,int &nRed,int &nGreen,int &nBlue, int nType = 0);

// �������� ��ġ ��������
extern CString	gf_GetModulePath();


// �ؽ�Ʈ �Է� ���� ��ġ�� ������ ����
extern BOOL	gf_CreateTextFile(CString strFilePath);
// �Է��� ����� ���Ͽ� �ؽ�Ʈ�� �Է�.
extern BOOL	gf_InsertTextFile(CString strFilePath,CString strInsetText);
// �Է��� ����� ���� ����
extern BOOL	gf_DeleteTextFile(CString strFilePath);


// AsciiToUTF
extern void		gf_AsciiToUTF(CString strText, LPSTR &lpTemp);
extern LPSTR	gf_AsciiToUTF(CString strText);							// AsciiToUTF


// ini ���Ͽ��� CRect ��ǥ �������� �Լ�
extern CRect gf_GetRectCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// ini ���Ͽ��� ���� Edit CRect ��ǥ �������� �Լ�
extern CRect gf_GetSingleTextCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// ini ���Ͽ��� Skin List CRect ��ǥ �������� �Լ�
extern CRect gf_GetSkinListCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// ini ���Ͽ��� CPoint ��ǥ �������� �Լ�(left & Top ��ǥ�� ������)
extern CPoint gf_GetPtCtrlPos(LPCTSTR lpGrpName, LPCTSTR lpCtrlName);

// �޸� �����ϴ� �Լ���-------------------------------------------------------------
extern CRect gf_CnvtStringToRect(CString sRect);
extern CPoint gf_CnvtStringToPt(CString sRect);
// �ؽ�Ʈ ��ü �������� �Լ� �켱 ��� ����!!
extern CString gf_GetCtrlText(LPCTSTR sPath,LPCTSTR lpGrpName, LPCTSTR lpCtrlName);
//-----------------------------------------------------------------------------------

// �ﰢ�� ������� ���� ������ ã�� �˰���
extern int GetRayIntersectsTriangle(ST_VECTOR p, ST_VECTOR d,ST_VECTOR v0, ST_VECTOR v1, ST_VECTOR v2);

#endif