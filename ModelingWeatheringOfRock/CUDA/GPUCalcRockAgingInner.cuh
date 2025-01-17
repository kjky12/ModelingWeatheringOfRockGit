//CGPUCalcRockAgingInner.cuh 파일 내에는 아래와 같이 써준다.



#pragma once

#include "Define.h"

#include <cuda_runtime.h>
//#include <helper_cuda.h>


#define dfMAX_PART_CNT		4
#define dfMAX_FACE_CNT		1024
#define dfMAX_VERTEX_CNT	512





#ifdef __cplusplus 
extern "C" {//<-- extern 시작
#endif

class CGPUCalcRockAgingInner
{

	public:
	CGPUCalcRockAgingInner(void);
	virtual ~CGPUCalcRockAgingInner(void);


	unsigned long long m_nXFileVoxCnt;
	unsigned long long m_nYFileVoxCnt;
	unsigned long long m_nZFileVoxCnt;

	float m_fTopRate;
	float m_fSideRate;
	float m_fBottomRate;
	float m_fCoefficient;

	//! 수분 흡수율
	float m_fCalcWaterInnerAbsorption;
	//! 레이어별 수분 부피 차감율
	float m_fCalcLayerWaterAborption;
	//! 수분 팽창
	float m_fCalcWaterChange;

	//ST_PARTICLE_POS	m_st_particle_pos;

	void SetInnderVoxelData(int nRepeatCnt, int nPrarticlePosCnt, ST_PARTICLE_POS_CUDA	*pstPrarticlePos);

};



#ifdef __cplusplus 

}

#endif


