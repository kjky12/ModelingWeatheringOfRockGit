//CGPUCalcRockAgingInner.cuh ���� ������ �Ʒ��� ���� ���ش�.



#pragma once

#include "Define.h"

#include <cuda_runtime.h>
//#include <helper_cuda.h>


#define dfMAX_PART_CNT		4
#define dfMAX_FACE_CNT		1024
#define dfMAX_VERTEX_CNT	512





#ifdef __cplusplus 
extern "C" {//<-- extern ����
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

	//! ���� �����
	float m_fCalcWaterInnerAbsorption;
	//! ���̾ ���� ���� ������
	float m_fCalcLayerWaterAborption;
	//! ���� ��â
	float m_fCalcWaterChange;

	//ST_PARTICLE_POS	m_st_particle_pos;

	void SetInnderVoxelData(int nRepeatCnt, int nPrarticlePosCnt, ST_PARTICLE_POS_CUDA	*pstPrarticlePos, ST_PARTICLE_POS_CUDA	*pstPrarticlePosMask);

};



#ifdef __cplusplus 

}

#endif


