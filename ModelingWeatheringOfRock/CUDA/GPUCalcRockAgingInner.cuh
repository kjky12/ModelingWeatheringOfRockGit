//CGPUCalcRockAgingInner.cuh ���� ������ �Ʒ��� ���� ���ش�.



#pragma once

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

};



#ifdef __cplusplus 

}

#endif


