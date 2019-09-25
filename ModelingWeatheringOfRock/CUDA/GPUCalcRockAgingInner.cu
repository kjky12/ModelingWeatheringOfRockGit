#include "GPUCalcRockAgingInner.cuh"
#include "cuda.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Windows.h"

#define dfSHARED_MEMORY_SIZE	32*7

#define dfCALC_BLOCK_CNT	8


__device__ __host__ int gcd(int a, int b)
{
	int c;
	while (b != 0)
	{
		c = a % b;
		a = b;
		b = c;
	}
	return a;
}

__device__ __host__ int lcm(int a, int b)
{
    return a * b / gcd(a, b);
}




CGPUCalcRockAgingInner::CGPUCalcRockAgingInner(void)
{

}

CGPUCalcRockAgingInner::~CGPUCalcRockAgingInner(void)
{

}



__device__ void ViewMaskData(ST_PARTICLE_POS_CUDA	*pstPrarticlePosCudaMask, int tid, int nPrarticlePosCntCuda, int nXFileVoxCnt, int nYFileVoxCnt)
{

	float f[7] = {pstPrarticlePosCudaMask[tid].fPorosity,0,0,0,0,0,0};
	float fW[7] = {pstPrarticlePosCudaMask[tid].fHaveWater,0,0,0,0,0,0};

	int nValue = tid + (nXFileVoxCnt * nYFileVoxCnt);
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		f[1] = pstPrarticlePosCudaMask[nValue].fPorosity;
		fW[1] = pstPrarticlePosCudaMask[nValue].fHaveWater;
	}

	nValue = tid - (nXFileVoxCnt * nYFileVoxCnt);
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		f[2] = pstPrarticlePosCudaMask[nValue].fPorosity;
		fW[2] = pstPrarticlePosCudaMask[nValue].fHaveWater;
	}

	nValue = tid + nXFileVoxCnt;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		f[3] = pstPrarticlePosCudaMask[nValue].fPorosity;
		fW[3] = pstPrarticlePosCudaMask[nValue].fHaveWater;
	}

	nValue = tid - nXFileVoxCnt;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		f[4] = pstPrarticlePosCudaMask[nValue].fPorosity;
		fW[4] = pstPrarticlePosCudaMask[nValue].fHaveWater;
	}

	nValue = tid - 1;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		f[5] = pstPrarticlePosCudaMask[nValue].fPorosity;
		fW[5] = pstPrarticlePosCudaMask[nValue].fHaveWater;
	}

	nValue = tid + 1;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		f[6] = pstPrarticlePosCudaMask[nValue].fPorosity;
		fW[6] = pstPrarticlePosCudaMask[nValue].fHaveWater;
	}


	printf( "New Tid : %d -> Porosity:%f,%f,%f,%f,%f,%f,%f\nWater:%f,%f,%f,%f,%f,%f,%f\n", tid, 
		f[0],
		f[1],
		f[2],
		f[3],
		f[4],
		f[5],
		f[6],
		fW[0],
		fW[1],
		fW[2],
		fW[3],
		fW[4],
		fW[5],
		fW[6]
		);
    
}



__device__ void InputMaskDataToSharedMemValue(ST_PARTICLE_POS_CUDA	*pstPrarticlePosCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCudaMask, 
											  ST_PARTICLE_POS_UNIT_PROCESS	*astParticle_pos_unitProcess, 
											  int nIdx,
											  int threadIdxT, int nPrarticlePosCntCuda)
{
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCudaMask[nIdx].fPorosity += astParticle_pos_unitProcess[threadIdxT].fPorosity;
			pstPrarticlePosCudaMask[nIdx].fHaveWater += astParticle_pos_unitProcess[threadIdxT].fHaveWater;
		}
		
	}
}



__global__ void kernelCalcRocking(int nStep, 
								  int nPrarticlePosCntCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCudaMask, 
								  int nXFileVoxCnt, int nYFileVoxCnt, int nZFileVoxCnt,
								  float fCoefficient, float fTopRate, float fSideRate, float fBottomRate,
								  float fCalcWaterInnerAbsorption, float fCalcLayerWaterAborption, float fCalcWaterChange
								  )
{ 
	//return;

	 // ������ �����尡 ���ÿ� ó���Ѵ�. // ���� threadIdx(������ �ε���)�� ���ؼ� ��������� �����Ѵ�. 
	//int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int tid = blockIdx.x * blockDim.x + threadIdx.x + nStep;

	//if(tid > nPrarticlePosCntCuda)
	//	return;

	
	//return;
	//int nExternalSideIdx = threadIdx.x % 6;
    //printf( "x:%d\n", nPrarticlePosCntCuda);

	if(tid >= nPrarticlePosCntCuda)
		return;

	

	pstPrarticlePosCuda[tid].bCheck = true;


	if(pstPrarticlePosCuda[tid].sStoneType == -1)
		return;
	
	//printf("BlockIdx: %d\t BlcokDim: %d\t ThreadIdx: %d\t Step: %d\tMax(%d)\ttid : %d\n", blockIdx.x, blockDim.x, threadIdx.x, nStep, nPrarticlePosCntCuda, tid);
	
	int nThreaIdxSharedMem = threadIdx.x * 7;

	//extern __shared__ ST_PARTICLE_POS_UNIT_PROCESS			astParticle_pos_unitProcess[]; //->0:����, 1:��,2:��,3:��,4:��,5:��,6��
	//__shared__ ST_PARTICLE_POS_UNIT_PROCESS			astParticle_pos_unitProcess[32 * 7]; //->0:����, 1:��,2:��,3:��,4:��,5:��,6��
	__shared__ ST_PARTICLE_POS_UNIT_PROCESS			astParticle_pos_unitProcess[dfSHARED_MEMORY_SIZE]; //->0:����, 1:��,2:��,3:��,4:��,5:��,6��
	memset(astParticle_pos_unitProcess, NULL, sizeof(ST_PARTICLE_POS_UNIT_PROCESS) * dfSHARED_MEMORY_SIZE);
	//memset(astParticle_pos_unitProcess, NULL, sizeof(ST_PARTICLE_POS_UNIT_PROCESS) * lcm(nThreadCnt, 6));
	__syncthreads();

	

	//printf( "IN_OUT : %d\n", pstPrarticlePosCuda[tid].bInOut);
	//if(pstPrarticlePosCuda[tid].bInOut == true) //! �ܺδ� ���� �Ի�ر�
	if(pstPrarticlePosCuda[tid].abExternalSide[0] ||
		pstPrarticlePosCuda[tid].abExternalSide[1] ||
		pstPrarticlePosCuda[tid].abExternalSide[2] ||
		pstPrarticlePosCuda[tid].abExternalSide[3] ||
		pstPrarticlePosCuda[tid].abExternalSide[4] ||
		pstPrarticlePosCuda[tid].abExternalSide[5] ) //! �ܺδ� ���� �Ի�ر�
	{
		//! �ܺ� �Ի�ر��� �������� ó���Ѵ�.(SHARED ����ŷ ���� �޸𸮿� ó��!)
		 astParticle_pos_unitProcess[nThreaIdxSharedMem].fPorosity = (pstPrarticlePosCuda[tid].abExternalSide[0] * fCoefficient * fTopRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[1] * fCoefficient * fBottomRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[2] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[3] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[4] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[5] * fCoefficient * fSideRate);
		
		 pstPrarticlePosCuda[tid].abEffectPorosity[0] = pstPrarticlePosCuda[tid].abExternalSide[0] || pstPrarticlePosCuda[tid].abExternalSide[1] || pstPrarticlePosCuda[tid].abExternalSide[2] || pstPrarticlePosCuda[tid].abExternalSide[3] || pstPrarticlePosCuda[tid].abExternalSide[4] || pstPrarticlePosCuda[tid].abExternalSide[5];

		 
	//printf("Side: %d\t%d\t%d\t%d\t%d\t%d\tTid:%d\n",
	//	pstPrarticlePosCuda[tid].abExternalSide[0],
	//	pstPrarticlePosCuda[tid].abExternalSide[1],
	//	pstPrarticlePosCuda[tid].abExternalSide[2],
	//	pstPrarticlePosCuda[tid].abExternalSide[3],
	//	pstPrarticlePosCuda[tid].abExternalSide[4],
	//	pstPrarticlePosCuda[tid].abExternalSide[5],
	//	tid
	//	);

		
		 //printf("BlockIdx: %d\t BlcokDim: %d\t ThreadIdx: %d\t Step: %d\tMax(%d)\ttid : %d\n", blockIdx.x, blockDim.x, nThreaIdxSharedMem, nStep, nPrarticlePosCntCuda, tid);

		//printf( "OuterMask : %d -> Porosity:%f\n", tid, astParticle_pos_unitProcess[nThreaIdxSharedMem].fPorosity);
	}
	else //! ���� ó��
	{
		//return;

		if(pstPrarticlePosCuda[tid].sStoneType == 0 ) //! ���ظ� ó���ؾ��Ѵ�. //! 1.0�� ������ ������ ���� ������!
		{
			pstPrarticlePosCuda[tid].fPorosity = 0;

			//printf( "InnerMask Check : %d(%f)\n", tid, astParticle_pos_unitProcess[nThreaIdxSharedMem].fHaveWater);


			//__shared__ unsigned int sdata[6];

			//1. ���� ����� = ��������� - {(�ִ� ���̾� Idx - ���� ���̾� Idx) * ���̾ ���� ������ * ��������� }
			float fHaveWaterTemp = fCalcWaterInnerAbsorption - ( (/*sMaxLayerIdx -*/ pstPrarticlePosCuda[tid].sLayerIdx) * fCalcLayerWaterAborption * fCalcWaterInnerAbsorption );
		
			//printf("Water Temp : %f\tLayer : %d\n", fHaveWaterTemp, pstPrarticlePosCuda[tid].sLayerIdx);

			//! 0���� ������ ������ ���� ������
			if(fHaveWaterTemp < 0.0)
				fHaveWaterTemp = 0.0;

			//! ����ŷ�� ����
			astParticle_pos_unitProcess[nThreaIdxSharedMem].fHaveWater += fHaveWaterTemp;

			


			//fPorosity = 0.0;
			//__syncthreads();


			//printf( "Inner : %d -> Water:%f\n", tid, astParticle_pos_unitProcess[nThreaIdxSharedMem].fHaveWater);

			//(pstPrarticlePosCuda[tid].abExternalSide[0] * fCoefficient * fTopRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[1] * fCoefficient * fBottomRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[2] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[3] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[4] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[5] * fCoefficient * fSideRate);


			//3. if(���� ��ȭ�� * ���� ��â�� > 1.0)
			if(((pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) * fCalcWaterChange) > 1.0)
			{

				//! ������ ó���Ǹ� ���� �Լ����� �������� �������ֱ�����!
				//astParticle_pos_unitProcess[nThreaIdxSharedMem].fHaveWater = -0.5;

				//[20190719] kjky12 �ϴ� ���� ó���Ǵ°ɷ� ó���Ǹ� �����Լ����� ������ �����Ѵ�. -> �׷��� �ٽ� -1�� ����������ҵ�	
				//! ��
				//astParticle_pos_unitProcess[nThreaIdxSharedMem + 1].fHaveWater = 1.0;
				astParticle_pos_unitProcess[nThreaIdxSharedMem + 1].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				//astParticle_pos_unitProcess[nThreaIdxSharedMem + 2].fHaveWater = 1.0;
				astParticle_pos_unitProcess[nThreaIdxSharedMem + 2].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				//astParticle_pos_unitProcess[nThreaIdxSharedMem + 3].fHaveWater = 1.0;
				astParticle_pos_unitProcess[nThreaIdxSharedMem + 3].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				//astParticle_pos_unitProcess[nThreaIdxSharedMem + 4].fHaveWater = 1.0;
				astParticle_pos_unitProcess[nThreaIdxSharedMem + 4].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				//astParticle_pos_unitProcess[nThreaIdxSharedMem + 5].fHaveWater = 1.0;
				astParticle_pos_unitProcess[nThreaIdxSharedMem + 5].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				//astParticle_pos_unitProcess[nThreaIdxSharedMem + 6].fHaveWater = 1.0;
				astParticle_pos_unitProcess[nThreaIdxSharedMem + 6].fPorosity += (fCalcWaterChange / 5.0);

				pstPrarticlePosCuda[tid].abEffectPorosity[1] = true;
			}
			
			//int nChangeCnt = 1;
			//int nIdx = tid + (nXFileVoxCnt * nYFileVoxCnt);
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 1].fPorosity >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			nChangeCnt += 1;
			//	}
			//}
			//

			//nIdx = tid - (nXFileVoxCnt * nYFileVoxCnt);
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 2].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			nChangeCnt += 1;
			//	}
			//}
			//

			//nIdx = tid - nXFileVoxCnt;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 3].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			nChangeCnt += 1;
			//	}

			//}


			//nIdx = tid + nXFileVoxCnt;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 4].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			nChangeCnt += 1;
			//	}
			//}
			//

			//nIdx = tid - 1;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 5].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			nChangeCnt += 1;
			//	}
			//}
			//
			//nIdx = tid + 1;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 6].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			nChangeCnt += 1;
			//	}
			//}

			////////////////////////////
			////! ī���� �ȰŸ� ���� �Լ����� �Ի�ر��� �Ͼ ���������ؼ� ������ �־���
			//nIdx = tid;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		astParticle_pos_unitProcess[nThreaIdxSharedMem].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater / nChangeCnt;
			//	}
			//}

			//nIdx = tid + (nXFileVoxCnt * nYFileVoxCnt);
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 1].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			astParticle_pos_unitProcess[nThreaIdxSharedMem + 1].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater / nChangeCnt;
			//	}
			//}
			//

			//nIdx = tid - (nXFileVoxCnt * nYFileVoxCnt);
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 2].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			astParticle_pos_unitProcess[nThreaIdxSharedMem + 2].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater / nChangeCnt;
			//	}
			//}
			//

			//nIdx = tid - nXFileVoxCnt;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 3].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			astParticle_pos_unitProcess[nThreaIdxSharedMem + 3].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater / nChangeCnt;
			//	}

			//}


			//nIdx = tid + nXFileVoxCnt;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 4].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			astParticle_pos_unitProcess[nThreaIdxSharedMem + 4].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater / nChangeCnt;
			//	}
			//}
			//

			//nIdx = tid - 1;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 5].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			astParticle_pos_unitProcess[nThreaIdxSharedMem + 5].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater / nChangeCnt;
			//	}
			//}
			//
			//nIdx = tid + 1;
			//if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
			//{
			//	if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
			//	{
			//		if(pstPrarticlePosCuda[nIdx].fPorosity + astParticle_pos_unitProcess[nThreaIdxSharedMem + 6].fPorosity  >= pstPrarticlePosCuda[nIdx].fGranularDisintegration)
			//			astParticle_pos_unitProcess[nThreaIdxSharedMem + 6].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater / nChangeCnt;
			//	}
			//}

			//printf("tid:%d(%d)->%f\n", tid, nChangeCnt, pstPrarticlePosCuda[tid].fHaveWater / (nChangeCnt + 1));
		}

	}

/*	ViewMaskData(pstPrarticlePosCudaMask, tid, nPrarticlePosCntCuda, nXFileVoxCnt, nYFileVoxCnt);
	return;*/
	

	//printf("HaveWater: %f\t%f\t%f\t%f\t%f\t%f\t%f\tTid:%d(%d)\n",
	//	astParticle_pos_unitProcess[nThreaIdxSharedMem].fHaveWater,
	//	astParticle_pos_unitProcess[nThreaIdxSharedMem+1].fHaveWater,
	//	astParticle_pos_unitProcess[nThreaIdxSharedMem+2].fHaveWater,
	//	astParticle_pos_unitProcess[nThreaIdxSharedMem+3].fHaveWater,
	//	astParticle_pos_unitProcess[nThreaIdxSharedMem+4].fHaveWater,
	//	astParticle_pos_unitProcess[nThreaIdxSharedMem+5].fHaveWater,
	//	astParticle_pos_unitProcess[nThreaIdxSharedMem+6].fHaveWater,
	//	tid, pstPrarticlePosCuda[tid].sStoneType
	//	);

	

	int nIdx = tid;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCuda[nIdx].fPorosity += astParticle_pos_unitProcess[nThreaIdxSharedMem].fPorosity;
			//! ���� �����Լ����� ���� �־������
			pstPrarticlePosCuda[nIdx].fHaveWater += astParticle_pos_unitProcess[nThreaIdxSharedMem].fHaveWater;

			//printf( "InnerMask Check : %d(%f)\n", tid, pstPrarticlePosCuda[nIdx].fHaveWater);
		}
	}

	nIdx = tid + (nXFileVoxCnt * nYFileVoxCnt);
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCuda[nIdx].fPorosity += astParticle_pos_unitProcess[nThreaIdxSharedMem + 1].fPorosity;
			pstPrarticlePosCuda[nIdx].fHaveWater += astParticle_pos_unitProcess[nThreaIdxSharedMem + 1].fHaveWater;
		}
	}
	

	nIdx = tid - (nXFileVoxCnt * nYFileVoxCnt);
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCuda[nIdx].fPorosity += astParticle_pos_unitProcess[nThreaIdxSharedMem + 2].fPorosity;
			pstPrarticlePosCuda[nIdx].fHaveWater += astParticle_pos_unitProcess[nThreaIdxSharedMem + 2].fHaveWater;
		}
	}
	

	nIdx = tid - nXFileVoxCnt;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCuda[nIdx].fPorosity += astParticle_pos_unitProcess[nThreaIdxSharedMem + 3].fPorosity;
			pstPrarticlePosCuda[nIdx].fHaveWater += astParticle_pos_unitProcess[nThreaIdxSharedMem + 3].fHaveWater;
		}

	}


	nIdx = tid + nXFileVoxCnt;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCuda[nIdx].fPorosity += astParticle_pos_unitProcess[nThreaIdxSharedMem + 4].fPorosity;
			pstPrarticlePosCuda[nIdx].fHaveWater += astParticle_pos_unitProcess[nThreaIdxSharedMem + 4].fHaveWater;
		}
	}
	

	nIdx = tid - 1;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCuda[nIdx].fPorosity += astParticle_pos_unitProcess[nThreaIdxSharedMem + 5].fPorosity;
			pstPrarticlePosCuda[nIdx].fHaveWater += astParticle_pos_unitProcess[nThreaIdxSharedMem + 5].fHaveWater;
		}
	}
	
	nIdx = tid + 1;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType != -1)
		{
			pstPrarticlePosCuda[nIdx].fPorosity += astParticle_pos_unitProcess[nThreaIdxSharedMem + 6].fPorosity;
			pstPrarticlePosCuda[nIdx].fHaveWater += astParticle_pos_unitProcess[nThreaIdxSharedMem + 6].fHaveWater;
		}
	}


	//ViewMaskData(pstPrarticlePosCudaMask, tid, nPrarticlePosCntCuda, nXFileVoxCnt, nYFileVoxCnt);

} 


__global__ void kernelCalcRockingShared(int nStep, 
								  int nPrarticlePosCntCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCudaMask, 
								  int nXFileVoxCnt, int nYFileVoxCnt, int nZFileVoxCnt,
								  float fCoefficient, float fTopRate, float fSideRate, float fBottomRate,
								  float fCalcWaterInnerAbsorption, float fCalcLayerWaterAborption, float fCalcWaterChange
								  )
{ 
	

} 


__global__ void kernelCalcRockingMasking(int nStep, 
										 int nPrarticlePosCntCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCudaMask)
{ 
	// ������ �����尡 ���ÿ� ó���Ѵ�. // ���� threadIdx(������ �ε���)�� ���ؼ� ��������� �����Ѵ�. 
	int tid = blockIdx.x * blockDim.x + threadIdx.x + nStep;

	if(tid >= nPrarticlePosCntCuda)
		return;

	//pstPrarticlePosCuda[tid].bCheck = true;


	if(pstPrarticlePosCuda[tid].sStoneType == -1)
		return;


	//printf("Prev:%03d->%f\n",tid, pstPrarticlePosCuda[tid].fPorosity);

	//pstPrarticlePosCuda[tid].fPorosity = pstPrarticlePosCuda[tid].fPorosity + pstPrarticlePosCudaMask[tid].fPorosity;
	//pstPrarticlePosCuda[tid].fHaveWater = pstPrarticlePosCuda[tid].fHaveWater + pstPrarticlePosCudaMask[tid].fHaveWater;
	
	//if( pstPrarticlePosCuda[tid].sStoneType == 0
	//	||(pstPrarticlePosCuda[tid].abExternalSide[0] || pstPrarticlePosCuda[tid].abExternalSide[1] || pstPrarticlePosCuda[tid].abExternalSide[2] || pstPrarticlePosCuda[tid].abExternalSide[3] || pstPrarticlePosCuda[tid].abExternalSide[4] || pstPrarticlePosCuda[tid].abExternalSide[5])
	//) //! �����ε� �ܺο� ��� ��� �ܺη� �Ǵ�
	//{
	//	pstPrarticlePosCuda[tid].sStoneType = -1;
	//}


	//if(pstPrarticlePosCuda[tid].abExternalSide[0] && pstPrarticlePosCuda[tid].abExternalSide[1] && pstPrarticlePosCuda[tid].abExternalSide[2]
	//&& pstPrarticlePosCuda[tid].abExternalSide[3] && pstPrarticlePosCuda[tid].abExternalSide[4] && pstPrarticlePosCuda[tid].abExternalSide[5]) //! 6���� �ܺ��ΰ�� �׳� �ܺη� �Ǵ�
	//{
	//	pstPrarticlePosCuda[tid].sStoneType = -1;
	//}

	if(pstPrarticlePosCuda[tid].sStoneType != 0 )
	{
		//pstPrarticlePosCuda[tid].fPorosity += pstPrarticlePosCudaMask[tid].fPorosity;

		if(pstPrarticlePosCuda[tid].fPorosity >= pstPrarticlePosCuda[tid].fGranularDisintegration ) // �Ի�ر� ���ް��� �����Ͽ� ����
		{
			printf("%d/%d StonType:%d\tPrositiy : %f\tWater : %f[tid]\n", pstPrarticlePosCuda[tid].abEffectPorosity[0], pstPrarticlePosCuda[tid].abEffectPorosity[1],
			pstPrarticlePosCuda[tid].sStoneType
			, pstPrarticlePosCuda[tid].fPorosity, pstPrarticlePosCuda[tid].fHaveWater, tid);

			
			//if(pstPrarticlePosCudaMask[tid].fHaveWater > 0.0)
			//{
			//	//pstPrarticlePosCuda[tid].sStoneType = -2;
			//	pstPrarticlePosCuda[tid].sStoneType = 0; //! �Ѱ��� ������ ����
			//	//! �Ի�ر��� �Ͼ ������ �� ������ �����Լ����� 0.0���� �ʱ�ȭ
			//	pstPrarticlePosCuda[tid].fHaveWater = 0.0;

			//	pstPrarticlePosCuda[tid].fHaveWater = pstPrarticlePosCudaMask[tid].fHaveWater;
			//}
			//else
			//{
			//	pstPrarticlePosCuda[tid].sStoneType = -1;
			//}


			//printf("%3d\t%3d\t%3d\t%d%d%d%d%d%d\t%d\tTid-%d(%f)\n", pstPrarticlePosCuda[tid].x, pstPrarticlePosCuda[tid].y, pstPrarticlePosCuda[tid].z,
			//	pstPrarticlePosCuda[tid].abExternalSide[0],
			//	pstPrarticlePosCuda[tid].abExternalSide[1],
			//	pstPrarticlePosCuda[tid].abExternalSide[2],
			//	pstPrarticlePosCuda[tid].abExternalSide[3],
			//	pstPrarticlePosCuda[tid].abExternalSide[4],
			//	pstPrarticlePosCuda[tid].abExternalSide[5],
			//	pstPrarticlePosCuda[tid].sStoneType,
			//	tid,
			//	pstPrarticlePosCuda[tid].fHaveWater
			//	);

			if(pstPrarticlePosCuda[tid].abExternalSide[0] ||
				pstPrarticlePosCuda[tid].abExternalSide[1] ||
				pstPrarticlePosCuda[tid].abExternalSide[2] ||
				pstPrarticlePosCuda[tid].abExternalSide[3] ||
				pstPrarticlePosCuda[tid].abExternalSide[4] ||
				pstPrarticlePosCuda[tid].abExternalSide[5] ) //! ���� �ܺθ� �ܺ� �Ի�ر�
			{
				pstPrarticlePosCuda[tid].sStoneType = -1;
			}
			else
			{
				//printf("TID\t%d\t->%f\n", tid, pstPrarticlePosCudaMask[tid].fHaveWater);


				pstPrarticlePosCuda[tid].sStoneType = 0; //! �Ѱ��� ������ ����
				//! �Ի�ر��� �Ͼ ������ �� ������ �����Լ����� 0.0���� �ʱ�ȭ
				pstPrarticlePosCuda[tid].fHaveWater = 0.0;

				//pstPrarticlePosCuda[tid].fHaveWater = pstPrarticlePosCudaMask[tid].fHaveWater;
				
				
			}
				

				//if(pstPrarticlePosCuda[tid].sStoneType == -2) //-2 �Ի�ر��� �Ͼ �ű� ������ -> ���� �Ǵ� �ܺ� ���� ���ڷ� ����������Ѵ�
				//{
				//	if(pstPrarticlePosCuda[tid].abExternalSide[0] ||
				//		pstPrarticlePosCuda[tid].abExternalSide[1] || 
				//		pstPrarticlePosCuda[tid].abExternalSide[2] || 
				//		pstPrarticlePosCuda[tid].abExternalSide[3] || 
				//		pstPrarticlePosCuda[tid].abExternalSide[4] || 
				//		pstPrarticlePosCuda[tid].abExternalSide[5])
				//	{
				//		 //! �Ѱ��� ������ �ܺ� ����!
				//		pstPrarticlePosCuda[tid].sStoneType = -1;
				//	}
				//	else
				//	{
				//		
				//		pstPrarticlePosCuda[tid].sStoneType = 0; //! �Ѱ��� ������ ����
				//		//! �Ի�ر��� �Ͼ ������ �� ������ �����Լ����� 0.0���� �ʱ�ȭ
				//		pstPrarticlePosCuda[tid].fHaveWater = 0.0;
				//	}

				//	printf("%3d\t%3d\t%3d\t%d%d%d%d%d%d\t%d\tTid-%d\n", pstPrarticlePosCuda[tid].x, pstPrarticlePosCuda[tid].y, pstPrarticlePosCuda[tid].z,
				//	pstPrarticlePosCuda[tid].abExternalSide[0],
				//	pstPrarticlePosCuda[tid].abExternalSide[1],
				//	pstPrarticlePosCuda[tid].abExternalSide[2],
				//	pstPrarticlePosCuda[tid].abExternalSide[3],
				//	pstPrarticlePosCuda[tid].abExternalSide[4],
				//	pstPrarticlePosCuda[tid].abExternalSide[5],
				//	pstPrarticlePosCuda[tid].sStoneType,
				//	tid);

				//}
				//else
				//{
				//	//printf("%3d\t%3d\t%3d\t%d%d%d%d%d%d\t%d\tNo-%d\n", pstPrarticlePosCuda[tid].x, pstPrarticlePosCuda[tid].y, pstPrarticlePosCuda[tid].z,
				//	//pstPrarticlePosCuda[tid].abExternalSide[0],
				//	//pstPrarticlePosCuda[tid].abExternalSide[1],
				//	//pstPrarticlePosCuda[tid].abExternalSide[2],
				//	//pstPrarticlePosCuda[tid].abExternalSide[3],
				//	//pstPrarticlePosCuda[tid].abExternalSide[4],
				//	//pstPrarticlePosCuda[tid].abExternalSide[5],
				//	//pstPrarticlePosCuda[tid].sStoneType,
				//	//tid);
				//}

			
		}
	}


	

	//printf("Afte:%03d->%f\n",tid, pstPrarticlePosCuda[tid].fPorosity);

}

__global__ void kernelReCalcExternalSide(int nStep, 
										 int nPrarticlePosCntCuda, ST_PARTICLE_POS_CUDA	*pstPrarticlePosCuda
										 ,int nXFileVoxCnt, int nYFileVoxCnt)
{ 
	// ������ �����尡 ���ÿ� ó���Ѵ�. // ���� threadIdx(������ �ε���)�� ���ؼ� ��������� �����Ѵ�. 
	int tid = blockIdx.x * blockDim.x + threadIdx.x + nStep;

	if(tid >= nPrarticlePosCntCuda)
		return;

	//pstPrarticlePosCuda[tid].bCheck = true;


	//pstPrarticlePosCuda[tid].abExternalSide[0];
	//memset(pstPrarticlePosCuda[tid].abExternalSide, false, sizeof(bool) * 6);
	

	//printf("tid : %d\n", tid);
		
	//! �����¿�յ��� ������ �ִ��� Ȯ���ϰ� �ܺ� ���� �ܸ��� üũ�Ѵ�. 

	int nIdx = tid + (nXFileVoxCnt * nYFileVoxCnt);
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		//printf("INNNNN2\n\n\n");
		if(pstPrarticlePosCuda[nIdx].sStoneType == -1)
		{
			pstPrarticlePosCuda[tid].abExternalSide[0] = true;
		}
		else
		{
			pstPrarticlePosCuda[tid].abExternalSide[0] = false;
		}
	}
	
	
	
	nIdx = tid - (nXFileVoxCnt * nYFileVoxCnt);
	//printf("Idx : %d\ttid : %d\n", nIdx, tid);
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		//printf("INNNNN1\n\n\n");
		
		if(pstPrarticlePosCuda[nIdx].sStoneType == -1)
		{
			pstPrarticlePosCuda[tid].abExternalSide[1] = true;
		}
		else
		{
			pstPrarticlePosCuda[tid].abExternalSide[1] = false;
		}
	}

	
	
	nIdx = tid - nXFileVoxCnt;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		//printf("INNNNN3\n\n\n");
		if(pstPrarticlePosCuda[nIdx].sStoneType == -1)
		{
			pstPrarticlePosCuda[tid].abExternalSide[2] = true;
		}
		else
		{
			pstPrarticlePosCuda[tid].abExternalSide[2] = false;
		}
	}

	nIdx = tid + nXFileVoxCnt;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		//printf("INNNNN4\n\n\n");
		if(pstPrarticlePosCuda[nIdx].sStoneType == -1)
		{
			pstPrarticlePosCuda[tid].abExternalSide[3] = true;
		}
		else
		{
			pstPrarticlePosCuda[tid].abExternalSide[3] = false;
		}
	}

	nIdx = tid - 1;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType == -1)
		{
			pstPrarticlePosCuda[tid].abExternalSide[4] = true;
		}
		else
		{
			pstPrarticlePosCuda[tid].abExternalSide[4] = false;
		}
	}
	
	nIdx = tid + 1;
	if(nIdx <= nPrarticlePosCntCuda && nIdx >= 0)
	{
		if(pstPrarticlePosCuda[nIdx].sStoneType == -1)
		{
			pstPrarticlePosCuda[tid].abExternalSide[5] = true;
		}
		else
		{
			pstPrarticlePosCuda[tid].abExternalSide[5] = false;
		}
	}

	

	//if(pstPrarticlePosCuda[tid].sStoneType == 0 &&
	//	(pstPrarticlePosCuda[tid].abExternalSide[0] || pstPrarticlePosCuda[tid].abExternalSide[1] ||
	//			pstPrarticlePosCuda[tid].abExternalSide[2] || pstPrarticlePosCuda[tid].abExternalSide[3] ||
	//			pstPrarticlePosCuda[tid].abExternalSide[4] || pstPrarticlePosCuda[tid].abExternalSide[5] )) //! �����ε� �ܺο� ��������� �ܺη� ����
	//		{
	//			pstPrarticlePosCuda[tid].sStoneType = -1;
	//	}


}


void CGPUCalcRockAgingInner::SetInnderVoxelData(int nRepeatCnt, int nPrarticlePosCnt, ST_PARTICLE_POS_CUDA	*pstPrarticlePos, ST_PARTICLE_POS_CUDA	*pstPrarticlePosMask)
{
	//! ���� ����
	ST_PARTICLE_POS_CUDA *pstPrarticlePosCuda;
	ST_PARTICLE_POS_CUDA *pstPrarticlePosCudaMask;
	//! ���� ����
	//int *pnPrarticlePosCntCuda;

	// cudaMalloc(destination, number of byte)�� device�� �޸𸮸� �Ҵ��Ѵ�.
	int nSizeCnt = sizeof(ST_PARTICLE_POS_CUDA);
	if ( cudaSuccess != cudaMalloc(&pstPrarticlePosCuda, nSizeCnt*nPrarticlePosCnt))
	{
		printf( "Error! Malloc \n" );
	}
	


	if ( cudaSuccess != cudaMalloc(&pstPrarticlePosCudaMask, nSizeCnt*nPrarticlePosCnt))
	{
		printf( "Error! Malloc \n" );
	}


	if ( cudaSuccess != cudaMemset(pstPrarticlePosCudaMask, NULL, nSizeCnt*nPrarticlePosCnt))
	{
		printf( "Error! Memset \n" );
	}


	/*if( cudaSuccess != cudaMalloc(&pnPrarticlePosCntCuda, sizeof(int)))
	{
		printf( "Error! Malloc \n" );

		Sleep(1000);
	}*/
	//else
	//{
	//	printf( "ErrorGOOD \n" );

	//}


	// cudaMemcpy(destination, source, number of byte, cudaMemcpyHostToDevice)�� ȣ��Ʈ���� ����̽��� �޸𸮸� ī���Ѵ�.
	if ( cudaSuccess != cudaMemcpy(pstPrarticlePosCuda, pstPrarticlePos, nSizeCnt*nPrarticlePosCnt, cudaMemcpyHostToDevice))
	{
		printf( "Error! Copy1 \n" );
	}

	//cudaMemcpy(pnPrarticlePosCntCuda, &nPrarticlePosCnt, sizeof(int), cudaMemcpyHostToDevice);

	//if ( cudaSuccess != cudaMemcpy(bVoxelYResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
	//{
	//	printf( "Error! Y VoxelState Memcpy \n" );
	//}
	
	//! �ּ� �����
	int nThreadCnt = 32; // 32�� ���� ó��
	//int nSharedMemoryCnt = lcm(nThreadCnt, 6); //92
	int nBlockCnt = (nPrarticlePosCnt / nThreadCnt) + 1;
	//kernelCalcRocking<<<nBlockCnt, nThreadCnt, nThreadCnt * 7>>>(nThreadCnt, nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, m_fCalcWaterInnerAbsorption, m_fCalcLayerWaterAborption, m_fCalcWaterChange);
	//int nBlockStep = 256;
	int nBlockStep = 128;
	
	for(int a = 0; a< nRepeatCnt; a++)
	{
		int nStep = 0;

		/// �ΰ��� ������ ó���� �Ǿ���Ѵ�.
		//------------------------------------------------------------------------
		//! ���� �޸𸮸� ���� ��� ó���Ǵ� ��ƾ
		nStep = 0;
		for(int n = 0; n < nBlockCnt; n += nBlockStep)
		{
			int nBlockStepTemp = nBlockStep;	
			if(nBlockCnt - n < nBlockStep)
				nBlockStepTemp = nBlockCnt - n;
			kernelCalcRocking<<<nBlockStepTemp, nThreadCnt>>>(nStep, nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask, 
				m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, 
				m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, 
				m_fCalcWaterInnerAbsorption, 
				m_fCalcLayerWaterAborption, m_fCalcWaterChange);	

			nStep += nBlockStepTemp * nThreadCnt;

			if(n % dfCALC_BLOCK_CNT == dfCALC_BLOCK_CNT - 1)
			{
				cudaDeviceSynchronize();
			}
		}
		//------------------------------------------------------------------------

		//------------------------------------------------------------------------
		//! ���� �޸𸮸� �Ϻζ� �����Ǵ� ��ƾ ��������
		//------------------------------------------------------------------------

		

		//int nBlockStepTemp = nBlockStep;	
		//kernelCalcRocking<<<nBlockStepTemp, nThreadCnt>>>(nStep, nPrarticlePosCnt, pstPrarticlePosCuda/*, pstPrarticlePosCudaMask*/, 
		//	m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, 
		//	m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, 
		//	m_fCalcWaterInnerAbsorption, 
		//	m_fCalcLayerWaterAborption, m_fCalcWaterChange);	
		//
		//
		//nStep += nBlockStepTemp * nThreadCnt;


		//nBlockStepTemp = nBlockStep;	
		//kernelCalcRocking<<<nBlockStepTemp, nThreadCnt>>>(nStep, nPrarticlePosCnt, pstPrarticlePosCuda/*, pstPrarticlePosCudaMask*/, 
		//	m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, 
		//	m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, 
		//	m_fCalcWaterInnerAbsorption, 
		//	m_fCalcLayerWaterAborption, m_fCalcWaterChange);	

		
		
		nStep = 0;
		for(int n = 0; n < nBlockCnt; n += nBlockStep)
		{
			int nBlockStepTemp = nBlockStep;	
			if(nBlockCnt - n < nBlockStep)
				nBlockStepTemp = nBlockCnt - n;
			
			kernelCalcRockingMasking<<<nBlockStepTemp, nThreadCnt>>>(nStep, nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask);
			nStep += nBlockStepTemp * nThreadCnt;

			if(n % dfCALC_BLOCK_CNT == dfCALC_BLOCK_CNT - 1)
			{
				cudaDeviceSynchronize();
			}

		}

		nStep = 0;
		for(int n = 0; n < nBlockCnt; n += nBlockStep)
		{
			int nBlockStepTemp = nBlockStep;	
			if(nBlockCnt - n < nBlockStep)
				nBlockStepTemp = nBlockCnt - n;
			
			kernelReCalcExternalSide<<<nBlockStepTemp, nThreadCnt>>>(nStep, nPrarticlePosCnt, pstPrarticlePosCuda, m_nXFileVoxCnt, m_nYFileVoxCnt);
			nStep += nBlockStepTemp * nThreadCnt;

			if(n % dfCALC_BLOCK_CNT == dfCALC_BLOCK_CNT - 1)
			{
				cudaDeviceSynchronize();
			}
		}

		printf("\n\n");

		//Sleep(100);
	}
		

	

	//cudaDeviceSynchronize();

	//cudaDeviceSynchronize();
	//cudaStreamSynchronize(cudaStream);


	//for(int n = 0; n < nPrarticlePosCnt; n++)
	//{
	//	printf("Acess idx : %d -> Porosity:%f,%f\n", n, pstPrarticlePosCudaMask[n].fPorosity, pstPrarticlePosCudaMask[n].fHaveWater);
	//}
	//printf("TEST\n");
	
	//memset(pstPrarticlePos, NULL, nSizeCnt*nPrarticlePosCnt);

	if ( cudaSuccess != cudaMemcpy(pstPrarticlePos, pstPrarticlePosCuda, nSizeCnt*nPrarticlePosCnt, cudaMemcpyDeviceToHost))
	{
		printf( "Error! Copy2 \n" );
	}

	//cudaDeviceSynchronize();

	cudaFree(pstPrarticlePosCuda);
	//cudaFree(pstPrarticlePosCudaMask);
	//cudaFree(pnPrarticlePosCntCuda);

	//pstPrarticlePosMask[0].x = 1;



}


