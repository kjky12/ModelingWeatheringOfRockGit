//Define.h

#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <map>
#include <vector>
#include <set>
using namespace std;

//Font-----------------------------------------------------------------------------------------------------------------------
#define dfDEFAULT_FONT																L"돋움"
#define dfDEFAULT_FONT_SIZE															11
#define dfFONT_NAME																	L"돋움"
#define dfSMALL_FONT_SIZE															8

//Wnd Message ---------------------------------------------------------------------------------------------------------------
#define WM_FINISH_SIM_ONE_STEP_MSG													WM_USER + 500
#define WM_FINISH_VOXEL_MSG															WM_USER + 501
#define WM_FINISH_SOLID_VOXEL_MSG													WM_USER + 502

#define WM_CHANGE_POS_ROT_MSG														WM_USER + 800

//! 시뮬레이션 메세지
#define WM_SIMULATION_VOXEL															WM_USER + 999



///////////////////////////////////////// Control Config Define////////////////////////////////////////////////////////////

#define dfRESULT_STRING_LENGTH															100

#define	dfROCK_PARTICLE_SIZE			1.0											// mm 단위


//#define	dfTHREAD_MAX_CNT				8
//#define	dfTHREAD_MAX_CNT					4
#define	dfTHREAD_MAX_CNT						4
//#define	dfTHREAD_MAX_CNT					4

#define dfDIVIDE_CNT	20.0

///////////////////////////////////////// Struct Define////////////////////////////////////////////////////////////


typedef	struct	ST_VECTOR	//	입자 위치 구조체
{
	float fX;
	float fY;
	float fZ;

}ST_VECTOR;

typedef	struct	 ST_BASE_ROCK_PARTICLE	//	기본 암석 입자 구조체
{
	bool			bUse;						// 사용여부 
//	COLORREF		crColor;					// 색상
	float			fPorosity;					// 공극률
//	float			fWaterAbsorptionRatio;		// 흡수율
	short			iExternalSideCnt;			// 외부 노출 단면 개수
	vector<short>	vecExternalSide;			// 외부 노출 단면 [0:상,1:하,2:좌,3:우,4:앞,5:뒤] -> 0:내부, 1:외부

	short			sStoneType;					//StoneType[ 0:공극, 1:장석, 2:석영, 3:사장석, 4:흑운모, 5:티타나이트, 6:각섬석, 7:녹렴석, 8:녹니석]

	float			fGranularDisintegration;	// 입상붕괴 도달값 

	short			sLayerIdx;					//! 레이어 정보(0: 가장 내부 -> 숫자가 클수록 표면 정해야할듯)
	float			fCenterRange;				//! 중심부와의 거리(거리로 레이어를 나눈다.)

//	ST_VECTOR		vPosInfo;					// 위치 정보
//	short			iLayerInfo;					// 레이어 정보

}ST_BASE_ROCK_PARTICLE;

typedef	struct	 ST_PARTICLE_POS		//	입자 위치 구조체
{
	int x;
	int y;
	int z;
	
	bool			bUse;						// 사용여부 -> [1/23/2019 kjky12] 미사용이라고 보면됨
	float			fPorosity;					// 공극률(랜덤 분포도 - 라일리) -> 입은 데미지(외부와 내부 둘다 같이 사용해되 될듯하다)
	short			iExternalSideCnt;			// 외부 노출 단면 개수
	//vector<short>	vecExternalSide;			// 외부 노출 단면 [0:상,1:하,2:좌,3:우,4:앞,5:뒤]
	//  [6/27/2019 kjky12] 배열로 변환
	//vector<short>	vecExternalSide;			// 외부 노출 단면 [0:상,1:하,2:좌,3:우,4:앞,5:뒤]
	bool			abExternalSide[6];				// 외부 노출 단면 [0:상,1:하,2:좌,3:우,4:앞,5:뒤] -> 사용 유무
	float			fGranularDisintegration;	// 입상붕괴 도달값 (랜덤 분포도 - 가우시안) -> 외부 입상붕괴(체력)
	
	/*
	fHaveWater
	max : 1.0, 수분 포화율에서 부피 팽창율을 곱해서 1이 넘으면 인근 입자들의 공극률이 높아진다. 
	횟수가 반복될수록 포화도가 높아진다. 그리고 인근 입자가 붕괴되는 경우 수분포화도를 나눠 가진다.
	*/	
	float			fHaveWater;					// 수분포화도 -> 내부에 공극 주변 입자들을 처리할때
	
	short			sStoneType;					//StoneType[0:공극, 1:장석, 2:석영, 3:사장석, 4:흑운모, 5:티타나이트, 6:각섬석, 7:녹렴석, 8:녹니석]

	bool			bInOut;						//! 내부인지 외부인지 확인 변수 0:내부, 1:외부

	short			sLayerIdx;					//! 레이어 정보(0: 가장 외부 -> 숫자가 클수록 표면 정해야할듯)
	


}ST_PARTICLE_POS;

typedef	struct	 ST_PARTICLE_POS_CUDA		//	입자 위치 구조체 CUDA전용
{
	float			fPorosity;					// 공극률(랜덤 분포도 - 라일리) -> 입은 데미지(외부와 내부 둘다 같이 사용해되 될듯하다)
	bool			abExternalSide[6];			// 외부 노출 단면 [0:상,1:하,2:좌,3:우,4:앞,5:뒤] -> 사용 유무
	
	float			fGranularDisintegration;	// 입상붕괴 도달값 (랜덤 분포도 - 가우시안) -> 외부 입상붕괴(체력)
	/*
	fHaveWater
	max : 1.0, 수분 포화율에서 부피 팽창율을 곱해서 1이 넘으면 인근 입자들의 공극률이 높아진다. 
	횟수가 반복될수록 포화도가 높아진다. 그리고 인근 입자가 붕괴되는 경우 수분포화도를 나눠 가진다.
	*/	
	float			fHaveWater;					// 수분포화도 -> 내부에 공극 주변 입자들을 처리할때
	
	short			sStoneType;					//StoneType[0:공극, 1:장석, 2:석영, 3:사장석, 4:흑운모, 5:티타나이트, 6:각섬석, 7:녹렴석, 8:녹니석]
	bool			bInOut;						//! 내부인지 외부인지 확인 변수 0:내부, 1:외부

	short			sLayerIdx;					//! 레이어 정보(0: 가장 외부 -> 숫자가 클수록 표면 정해야할듯)
	


}ST_PARTICLE_POS_CUDA;


typedef	struct	 ST_PARTICLE_POS_UNIT_PROCESS		// GPU처리에서 처리되는 데이터 전용 구조체(ST_PARTICLE_POS 구조체의 일부 데이터만 사용!) -> 공유 메모리 전용으로 처리하고 메인 마스킹에 입력 예정
{

	float			fPorosity;					// 공극률(랜덤 분포도 - 라일리) -> 입은 데미지(외부와 내부 둘다 같이 사용해되 될듯하다)

	float			fHaveWater;					// 수분포화도 -> 내부에 공극 주변 입자들을 처리할때

}ST_PARTICLE_POS_UNIT_PROCESS;


typedef	struct	 ST_SHOW_DATA		//	가시화를 위한 데이터 구조체
{
	int		nDepthOrign;		//! 초기 깊이 -> 같은 위치를 ++로 카운트해줌(초기화 시에만)
	int		nStartIdxCntOrign;	//! 초기 시작 방향

	int		nDepth;				//! 현재 깊이 -> 재계산 할때는 달라짐
	int		nPorosityCnt;		//! 공극 개수(초기 깊이보다 많을수 없다.)
	int		nStartIdxCnt;		//! 바라보는 방향에서 가장 가까운 복셀 길이(작을 수록 가장 가까운거!!)

	double		dRate;				//! 공극과 복셀의 비율[ (nDepth - nPorosityCnt) / nDepthOrign * 100.0]

}ST_SHOW_DATA;




typedef	struct	ST_VOXEL_3D_POS		//	복셀 위치
{
	float x;
	float y;
	float z;
	
	bool operator < (const ST_VOXEL_3D_POS &rhs) const
	{
		//! map의 Key를 구조체로 이용할떄 사용 - Test X
		//! stl정의 인데 전에 있던 데이터들이랑 중복 비교를 위한것인듯...
		if( x < rhs.x ) return true;
		if( x > rhs.x ) return false;

		if( y < rhs.y ) return true;
		if( y > rhs.y ) return false;

		if( z < rhs.z ) return true;

		return false;
	}

}ST_VOXEL_3D_POS;

typedef	struct	ST_VOXEL_3D_POS_VIEWER		//	복셀 위치
{
	//			  가로, 세로
	// 0:상, 1:하 -> x, y
	// 2:좌, 3:우 -> y, z
	// 4:앞, 5:뒤 -> x, z
	float fHori; //! 가로축
	float fVert; //! 세로축

	bool operator < (const ST_VOXEL_3D_POS_VIEWER &rhs) const
	{
		//! map의 Key를 구조체로 이용할떄 사용 - Test X
		//! stl정의 인데 전에 있던 데이터들이랑 중복 비교를 위한것인듯...
		if( fHori < rhs.fHori ) return true;
		if( fHori > rhs.fHori ) return false;

		if( fVert < rhs.fVert ) return true;

		return false;
	}

	//! 비어있는 방향 0: 좌측, 1: 우측, 2: 위, 3:아래
	vector<int>	vecEmpty;

}ST_VOXEL_3D_POS_VIEWER;

typedef	struct	ST_VOXEL_FILE_POS	//	복셀 위치
{
	int nX;
	int nY;
	int nZ;
	float			fPorosity;					// 공극률
	float			fGranularDisintegration;	// 입상붕괴 도달값 
}ST_VOXEL_FILE_POS;


typedef	struct	ST_DELETE_VOXEL_POS	//	복셀 위치
{
	int nX;
	int nY;
	int nZ;
}ST_DELETE_VOXEL_POS;


// typedef	struct	ST_BASE_LAYER_ROCK_PARTICLE		//	암석 기본 레이어 구조체
// {
// 	bool		bUse;						// 사용여부 
// 	
// 	COLORREF	crAvrColor;					// 평균 색상
// 	float		fAvrPorosity;				// 평균 공극률
// 	float		fAvrWaterAbsorptionRatio;	// 평균 흡수율
// // 사용여부 결정 안됨 [5/21/2013 joseph]
// //	short		iExternalSideCnt;			// 외부 노출 단면 개수
// 	float		fAvrGranularDisintegration;	// 평균 입상붕괴 도달값 
// 	
// 	ST_VECTOR	vPosInfo;					// 위치 정보
// 	short		iLayerInfo;					// 레이어 정보
// 
// 	vector<vector<vector<ST_BASE_ROCK_PARTICLE>>> vectorBaseRockParticle;
// 
// }ST_BASE_LAYER_ROCK_PARTICLE;




#endif

