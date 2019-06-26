//CGPUCalcRockAgingInner.cuh 파일 내에는 아래와 같이 써준다.



#pragma once

#include <cuda_runtime.h>


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

};



#ifdef __cplusplus 

}

#endif


