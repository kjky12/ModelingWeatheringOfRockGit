//CGPUCalcRockAgingInner.cuh ���� ������ �Ʒ��� ���� ���ش�.



#pragma once

#include "Define.h"

#include <cuda_runtime.h>


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


	//ST_PARTICLE_POS	m_st_particle_pos;

};



#ifdef __cplusplus 

}

#endif


