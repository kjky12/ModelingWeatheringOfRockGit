//Define.h

#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <map>
#include <vector>
#include <set>
using namespace std;

//Font-----------------------------------------------------------------------------------------------------------------------
#define dfDEFAULT_FONT																L"����"
#define dfDEFAULT_FONT_SIZE															11
#define dfFONT_NAME																	L"����"
#define dfSMALL_FONT_SIZE															8

//Wnd Message ---------------------------------------------------------------------------------------------------------------
#define WM_FINISH_SIM_ONE_STEP_MSG													WM_USER + 500
#define WM_FINISH_VOXEL_MSG															WM_USER + 501
#define WM_FINISH_SOLID_VOXEL_MSG													WM_USER + 502

#define WM_CHANGE_POS_ROT_MSG														WM_USER + 800

//! �ùķ��̼� �޼���
#define WM_SIMULATION_VOXEL															WM_USER + 999



///////////////////////////////////////// Control Config Define////////////////////////////////////////////////////////////

#define dfRESULT_STRING_LENGTH															100

#define	dfROCK_PARTICLE_SIZE			1.0											// mm ����


//#define	dfTHREAD_MAX_CNT				8
//#define	dfTHREAD_MAX_CNT					4
#define	dfTHREAD_MAX_CNT						4
//#define	dfTHREAD_MAX_CNT					4

#define dfDIVIDE_CNT	20.0

///////////////////////////////////////// Struct Define////////////////////////////////////////////////////////////


typedef	struct	ST_VECTOR	//	���� ��ġ ����ü
{
	float fX;
	float fY;
	float fZ;

}ST_VECTOR;

typedef	struct	 ST_BASE_ROCK_PARTICLE	//	�⺻ �ϼ� ���� ����ü
{
	bool			bUse;						// ��뿩�� 
//	COLORREF		crColor;					// ����
	float			fPorosity;					// ���ط�
//	float			fWaterAbsorptionRatio;		// �����
	short			iExternalSideCnt;			// �ܺ� ���� �ܸ� ����
	vector<short>	vecExternalSide;			// �ܺ� ���� �ܸ� [0:��,1:��,2:��,3:��,4:��,5:��] -> 0:����, 1:�ܺ�

	short			sStoneType;					//StoneType[ 0:����, 1:�弮, 2:����, 3:���弮, 4:����, 5:ƼŸ����Ʈ, 6:������, 7:��ż�, 8:��ϼ�]

	float			fGranularDisintegration;	// �Ի�ر� ���ް� 

	short			sLayerIdx;					//! ���̾� ����(0: ���� ���� -> ���ڰ� Ŭ���� ǥ�� ���ؾ��ҵ�)
	float			fCenterRange;				//! �߽ɺο��� �Ÿ�(�Ÿ��� ���̾ ������.)

//	ST_VECTOR		vPosInfo;					// ��ġ ����
//	short			iLayerInfo;					// ���̾� ����

}ST_BASE_ROCK_PARTICLE;

typedef	struct	 ST_PARTICLE_POS		//	���� ��ġ ����ü
{
	int x;
	int y;
	int z;
	
	bool			bUse;						// ��뿩�� -> [1/23/2019 kjky12] �̻���̶�� �����
	float			fPorosity;					// ���ط�(���� ������ - ���ϸ�) -> ���� ������(�ܺο� ���� �Ѵ� ���� ����ص� �ɵ��ϴ�)
	short			iExternalSideCnt;			// �ܺ� ���� �ܸ� ����
	//vector<short>	vecExternalSide;			// �ܺ� ���� �ܸ� [0:��,1:��,2:��,3:��,4:��,5:��]
	//  [6/27/2019 kjky12] �迭�� ��ȯ
	//vector<short>	vecExternalSide;			// �ܺ� ���� �ܸ� [0:��,1:��,2:��,3:��,4:��,5:��]
	bool			abExternalSide[6];				// �ܺ� ���� �ܸ� [0:��,1:��,2:��,3:��,4:��,5:��] -> ��� ����
	float			fGranularDisintegration;	// �Ի�ر� ���ް� (���� ������ - ����þ�) -> �ܺ� �Ի�ر�(ü��)
	
	/*
	fHaveWater
	max : 1.0, ���� ��ȭ������ ���� ��â���� ���ؼ� 1�� ������ �α� ���ڵ��� ���ط��� ��������. 
	Ƚ���� �ݺ��ɼ��� ��ȭ���� ��������. �׸��� �α� ���ڰ� �ر��Ǵ� ��� ������ȭ���� ���� ������.
	*/	
	float			fHaveWater;					// ������ȭ�� -> ���ο� ���� �ֺ� ���ڵ��� ó���Ҷ�
	
	short			sStoneType;					//StoneType[0:����, 1:�弮, 2:����, 3:���弮, 4:����, 5:ƼŸ����Ʈ, 6:������, 7:��ż�, 8:��ϼ�]

	bool			bInOut;						//! �������� �ܺ����� Ȯ�� ���� 0:����, 1:�ܺ�

	short			sLayerIdx;					//! ���̾� ����(0: ���� �ܺ� -> ���ڰ� Ŭ���� ǥ�� ���ؾ��ҵ�)
	


}ST_PARTICLE_POS;

typedef	struct	 ST_PARTICLE_POS_CUDA		//	���� ��ġ ����ü CUDA����
{
	float			fPorosity;					// ���ط�(���� ������ - ���ϸ�) -> ���� ������(�ܺο� ���� �Ѵ� ���� ����ص� �ɵ��ϴ�)
	bool			abExternalSide[6];			// �ܺ� ���� �ܸ� [0:��,1:��,2:��,3:��,4:��,5:��] -> ��� ����
	
	float			fGranularDisintegration;	// �Ի�ر� ���ް� (���� ������ - ����þ�) -> �ܺ� �Ի�ر�(ü��)
	/*
	fHaveWater
	max : 1.0, ���� ��ȭ������ ���� ��â���� ���ؼ� 1�� ������ �α� ���ڵ��� ���ط��� ��������. 
	Ƚ���� �ݺ��ɼ��� ��ȭ���� ��������. �׸��� �α� ���ڰ� �ر��Ǵ� ��� ������ȭ���� ���� ������.
	*/	
	float			fHaveWater;					// ������ȭ�� -> ���ο� ���� �ֺ� ���ڵ��� ó���Ҷ�
	
	short			sStoneType;					//StoneType[0:����, 1:�弮, 2:����, 3:���弮, 4:����, 5:ƼŸ����Ʈ, 6:������, 7:��ż�, 8:��ϼ�]
	bool			bInOut;						//! �������� �ܺ����� Ȯ�� ���� 0:����, 1:�ܺ�

	short			sLayerIdx;					//! ���̾� ����(0: ���� �ܺ� -> ���ڰ� Ŭ���� ǥ�� ���ؾ��ҵ�)
	


}ST_PARTICLE_POS_CUDA;


typedef	struct	 ST_PARTICLE_POS_UNIT_PROCESS		// GPUó������ ó���Ǵ� ������ ���� ����ü(ST_PARTICLE_POS ����ü�� �Ϻ� �����͸� ���!) -> ���� �޸� �������� ó���ϰ� ���� ����ŷ�� �Է� ����
{

	float			fPorosity;					// ���ط�(���� ������ - ���ϸ�) -> ���� ������(�ܺο� ���� �Ѵ� ���� ����ص� �ɵ��ϴ�)

	float			fHaveWater;					// ������ȭ�� -> ���ο� ���� �ֺ� ���ڵ��� ó���Ҷ�

}ST_PARTICLE_POS_UNIT_PROCESS;


typedef	struct	 ST_SHOW_DATA		//	����ȭ�� ���� ������ ����ü
{
	int		nDepthOrign;		//! �ʱ� ���� -> ���� ��ġ�� ++�� ī��Ʈ����(�ʱ�ȭ �ÿ���)
	int		nStartIdxCntOrign;	//! �ʱ� ���� ����

	int		nDepth;				//! ���� ���� -> ���� �Ҷ��� �޶���
	int		nPorosityCnt;		//! ���� ����(�ʱ� ���̺��� ������ ����.)
	int		nStartIdxCnt;		//! �ٶ󺸴� ���⿡�� ���� ����� ���� ����(���� ���� ���� ������!!)

	double		dRate;				//! ���ذ� ������ ����[ (nDepth - nPorosityCnt) / nDepthOrign * 100.0]

}ST_SHOW_DATA;




typedef	struct	ST_VOXEL_3D_POS		//	���� ��ġ
{
	float x;
	float y;
	float z;
	
	bool operator < (const ST_VOXEL_3D_POS &rhs) const
	{
		//! map�� Key�� ����ü�� �̿��ҋ� ��� - Test X
		//! stl���� �ε� ���� �ִ� �����͵��̶� �ߺ� �񱳸� ���Ѱ��ε�...
		if( x < rhs.x ) return true;
		if( x > rhs.x ) return false;

		if( y < rhs.y ) return true;
		if( y > rhs.y ) return false;

		if( z < rhs.z ) return true;

		return false;
	}

}ST_VOXEL_3D_POS;

typedef	struct	ST_VOXEL_3D_POS_VIEWER		//	���� ��ġ
{
	//			  ����, ����
	// 0:��, 1:�� -> x, y
	// 2:��, 3:�� -> y, z
	// 4:��, 5:�� -> x, z
	float fHori; //! ������
	float fVert; //! ������

	bool operator < (const ST_VOXEL_3D_POS_VIEWER &rhs) const
	{
		//! map�� Key�� ����ü�� �̿��ҋ� ��� - Test X
		//! stl���� �ε� ���� �ִ� �����͵��̶� �ߺ� �񱳸� ���Ѱ��ε�...
		if( fHori < rhs.fHori ) return true;
		if( fHori > rhs.fHori ) return false;

		if( fVert < rhs.fVert ) return true;

		return false;
	}

	//! ����ִ� ���� 0: ����, 1: ����, 2: ��, 3:�Ʒ�
	vector<int>	vecEmpty;

}ST_VOXEL_3D_POS_VIEWER;

typedef	struct	ST_VOXEL_FILE_POS	//	���� ��ġ
{
	int nX;
	int nY;
	int nZ;
	float			fPorosity;					// ���ط�
	float			fGranularDisintegration;	// �Ի�ر� ���ް� 
}ST_VOXEL_FILE_POS;


typedef	struct	ST_DELETE_VOXEL_POS	//	���� ��ġ
{
	int nX;
	int nY;
	int nZ;
}ST_DELETE_VOXEL_POS;


// typedef	struct	ST_BASE_LAYER_ROCK_PARTICLE		//	�ϼ� �⺻ ���̾� ����ü
// {
// 	bool		bUse;						// ��뿩�� 
// 	
// 	COLORREF	crAvrColor;					// ��� ����
// 	float		fAvrPorosity;				// ��� ���ط�
// 	float		fAvrWaterAbsorptionRatio;	// ��� �����
// // ��뿩�� ���� �ȵ� [5/21/2013 joseph]
// //	short		iExternalSideCnt;			// �ܺ� ���� �ܸ� ����
// 	float		fAvrGranularDisintegration;	// ��� �Ի�ر� ���ް� 
// 	
// 	ST_VECTOR	vPosInfo;					// ��ġ ����
// 	short		iLayerInfo;					// ���̾� ����
// 
// 	vector<vector<vector<ST_BASE_ROCK_PARTICLE>>> vectorBaseRockParticle;
// 
// }ST_BASE_LAYER_ROCK_PARTICLE;




#endif

