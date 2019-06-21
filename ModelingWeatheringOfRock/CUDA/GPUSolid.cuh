//CGPUSolid.cuh ���� ������ �Ʒ��� ���� ���ش�.



#pragma once
#include <cuda_runtime.h>

#ifdef __cplusplus 
extern "C" {//<-- extern ����
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

	//! ���ΰ��� �ֵ��� ��� ������ ������� �׸��� �����ϱ� ���� ���� ��� �־� �ش�.

	unsigned long long m_nXFileVoxCnt;
	unsigned long long m_nYFileVoxCnt;
	unsigned long long m_nZFileVoxCnt;
	unsigned long long m_nSolidTotalCnt;

	void CUDA_SolidVoxelizationX(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelXResult);
	void CUDA_SolidVoxelizationY(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelYResult);
	void CUDA_SolidVoxelizationZ(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelZResult);
	void CUDA_SolidVoxelizationZTest(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelZResult, unsigned int unBlockStep);

	//! XYZ ���� ����
	void CUDA_SolidVoxelizationXYZ(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelResultTotal);
	//! XYZ ���� ���� -> ������� ����
	void CUDA_SolidVoxelizationXYZBlock(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelResultTotal);

	//! ��ü ��Ż ���� ��ġ��
	void CUDA_SolidVoxelizationTotal(unsigned int  ulVoxelSize, bool *bVoxelStateX, bool *bVoxelStateY, bool *bVoxelStateZ, bool *bVoxelStateTotal);



};



#ifdef __cplusplus 

}

#endif


