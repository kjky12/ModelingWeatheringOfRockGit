//CGPUSolid.cuh 파일 내에는 아래와 같이 써준다.



#pragma once
#include <cuda_runtime.h>

#ifdef __cplusplus 
extern "C" {//<-- extern 시작
#endif

class CGPUSolid
{

	public:
	CGPUSolid(void);
	virtual ~CGPUSolid(void);

	unsigned long long m_nMaxBlockSize;
	unsigned long long m_nMaxThreadSize;

	int sum_cuda(int a, int b, int *c);


	int Sum_TEST(int *a, int *b, int *c);


	void GpuInfoPrint();

	//! 메인과의 쌍둥이 멤버 변수를 만들어줌 그리고 시작하기 전에 값을 모두 넣어 준다.

	unsigned long long m_nXFileVoxCnt;
	unsigned long long m_nYFileVoxCnt;
	unsigned long long m_nZFileVoxCnt;
	unsigned long long m_nSolidTotalCnt;

	void CUDA_SolidVoxelizationX(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelXResult);
	void CUDA_SolidVoxelizationY(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelYResult);
	void CUDA_SolidVoxelizationZ(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelZResult);
	void CUDA_SolidVoxelizationZTest(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelZResult, unsigned int unBlockStep);

	//! XYZ 복셀 수행
	void CUDA_SolidVoxelizationXYZ(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelResultTotal);
	//! XYZ 복셀 수행 -> 블록으로 분할
	void CUDA_SolidVoxelizationXYZBlock(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelResultTotal);

	//! 전체 토탈 복셀 합치기
	void CUDA_SolidVoxelizationTotal(unsigned int  ulVoxelSize, bool *bVoxelStateX, bool *bVoxelStateY, bool *bVoxelStateZ, bool *bVoxelStateTotal);



};



#ifdef __cplusplus 

}

#endif


