#include "GPUCalcRockAgingInner.cuh"
#include "cuda.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Windows.h"

#define dfSHARED_MEMORY_SIZE	32*7

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


//__global__ void kernelCalcRocking(int nPrarticlePosCntCuda, ST_PARTICLE_POS	*pstPrarticlePosCuda, ST_PARTICLE_POS	*pstPrarticlePosCudaMask, 
//								  int nX, int nY, int nZ,
//								  float fCoefficient, float fTopRate, float fSideRate, float fBottomRate,
//								  float fCalcWaterInnerAbsorption, float fCalcLayerWaterAborption, float fCalcWaterChange
//								  )
//{ 
//	// ������ �����尡 ���ÿ� ó���Ѵ�. // ���� threadIdx(������ �ε���)�� ���ؼ� ��������� �����Ѵ�. 
//	//int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
//	int tid = blockIdx.x * blockDim.x + threadIdx.x;
//	int nExternalSideIdx = threadIdx.x;
//    //printf( "x:%d\n", nPrarticlePosCntCuda);
//
//	if(tid > nPrarticlePosCntCuda)
//		return;
//
//	
//    
//
//
//	//printf( "IN_OUT : %d\n", pstPrarticlePosCuda[tid].bInOut);
//	if(pstPrarticlePosCuda[tid].bInOut == true) //! �ܺδ� ���� �Ի�ر�
//	{
//		__shared__ float			fPorosity;
//		fPorosity = 0.0;
//
//
//		if(pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx] == TRUE)
//		{
//			//switch(stParticlePos.vecExternalSide[nEx])	//[0:��,1:��,2:��,3:��,4:��,5:��]
//			switch(nExternalSideIdx)	//[0:��,1:��,2:��,3:��,4:��,5:��]
//			{
//			case 0:
//				{
//					fPorosity += (fCoefficient * fTopRate);
//				}
//				break;
//			case 1:
//				{
//					fPorosity += (fCoefficient * fBottomRate);
//				}
//				break;
//			case 2:
//			case 3:
//			case 4:
//			case 5:
//				{
//					fPorosity += (fCoefficient * fSideRate);
//				}
//				break;
//			default:
//				break;
//			}
//
//		}
//
//        __syncthreads();
//
//		pstPrarticlePosCudaMask[tid].fPorosity = fPorosity;
//		printf( "OuterMask : %d\n", tid);
//
//		//fPorosity ���ط� >= fGranularDisintegration �Ի�ر� ���ް�
//		//if(pstPrarticlePosCuda[tid].fPorosity >= pstPrarticlePosCuda[tid].fGranularDisintegration) // �Ի�ر� ���ް��� �����Ͽ� ����
//		//{
//		//	
//
//		//	//stParticlePos.bUse = false;
//
//		//	//strKey.Format(L"%d-%d-%d",x,y,z);
//
//		//	//vecDeleParticle.push_back(strKey);
//
//		//	//break;
//		//}
//	}
//	else //! ���� ó��
//	{
//		if(pstPrarticlePosCuda[tid].sStoneType == 0) //! ���ظ� ó���ؾ��Ѵ�.
//		{
//			printf( "InnerMask Check : %d\n", tid);
//
//
//			//__shared__ unsigned int sdata[6];
//
//			//1. ���� ����� = ��������� - {(�ִ� ���̾� Idx - ���� ���̾� Idx) * ���̾ ���� ������ * ��������� }
//			float fHaveWaterTemp = fCalcWaterInnerAbsorption - ( (/*sMaxLayerIdx -*/ pstPrarticlePosCuda[tid].sLayerIdx) * fCalcLayerWaterAborption * fCalcWaterInnerAbsorption );
//			
//			//! 0���� ������ ������ ���� ������
//			if(fHaveWaterTemp < 0.0)
//				fHaveWaterTemp = 0.0;
//
//			__shared__ float			fPorosity[6];
//			memset(fPorosity, NULL, sizeof(float) * 6);
//			//fPorosity = 0.0;
//			//__syncthreads();
//
//			//3. if(���� ��ȭ�� * ���� ��â�� > 1.0)
//			if(((pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) * fCalcWaterChange) > 1.0)
//			{
//				int nPos = tid;
//				switch(nExternalSideIdx)	//[0:��,1:��,2:��,3:��,4:��,5:��]
//				{
//				case 0:
//					nPos += (nX * nY);
//					break;
//				case 1:
//					nPos -= (nX * nY);
//					break;
//				case 2:
//					nPos += nX;
//					break;
//				case 3:
//					nPos -= nX;
//					break;
//				case 4:
//					nPos -= 1;
//					break;
//				case 5:
//					nPos += 1;
//					break;
//				default:
//					break;
//				}
//
//				if(nPos > nPrarticlePosCntCuda)
//					return;
//
//				fPorosity[nExternalSideIdx] = (fCalcWaterChange / 5.0);
//
//				__syncthreads();
//
//				pstPrarticlePosCudaMask[nPos].fPorosity += fPorosity[nExternalSideIdx];
//
//				//if(pstPrarticlePosCudaMask[nPos].fPorosity >= pstPrarticlePosCudaMask[nPos].fGranularDisintegration) // �Ի�ر� ���ް��� �����Ͽ� ����
//				//{
//				//	nBreakCnt++;
//
//				//	__syncthreads();
//				//	pstPrarticlePosCudaMask[nPos].fHaveWater = (pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) / (float)(nBreakCnt + 1);
//				//	printf( "InnerMask : %d\n", nPos);
//				//}
//			
//
//				//pstPrarticlePosCudaMask[nPos]
//
//			}
//
//		}
//	}
//
//	/*if(pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx] == true)
//	{
//
//	}*/
//
//
//	//pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx];
//
//    //printf( "x:%d\ty:%d\tz:%d\tTid : %d\n", pstPrarticlePosCuda[tid].x, pstPrarticlePosCuda[tid].y, pstPrarticlePosCuda[tid].z,  tid);
//
//
//} 
//
//
//
//void CGPUCalcRockAgingInner::SetInnderVoxelData(int nPrarticlePosCnt, ST_PARTICLE_POS	*pstPrarticlePos, ST_PARTICLE_POS	*pstPrarticlePosMask)
//{
//	//! ���� ����
//	ST_PARTICLE_POS *pstPrarticlePosCuda;
//	ST_PARTICLE_POS *pstPrarticlePosCudaMask;
//	//! ���� ����
//	//int *pnPrarticlePosCntCuda;
//
//	// cudaMalloc(destination, number of byte)�� device�� �޸𸮸� �Ҵ��Ѵ�.
//	int nSizeCnt = sizeof(ST_PARTICLE_POS);
//	if ( cudaSuccess != cudaMalloc(&pstPrarticlePosCuda, nSizeCnt*nPrarticlePosCnt))
//	{
//		printf( "Error! Malloc \n" );
//	}
//
//	if ( cudaSuccess != cudaMalloc(&pstPrarticlePosCudaMask, nSizeCnt*nPrarticlePosCnt))
//	{
//		printf( "Error! Malloc \n" );
//	}
//
//
//	if ( cudaSuccess != cudaMemset(pstPrarticlePosCudaMask, NULL, nSizeCnt*nPrarticlePosCnt))
//	{
//		printf( "Error! Memset \n" );
//	}
//
//	
//
//
//	/*if( cudaSuccess != cudaMalloc(&pnPrarticlePosCntCuda, sizeof(int)))
//	{
//		printf( "Error! Malloc \n" );
//
//		Sleep(1000);
//	}*/
//	//else
//	//{
//	//	printf( "ErrorGOOD \n" );
//
//	//}
//
//
//	
//	
//	// cudaMemcpy(destination, source, number of byte, cudaMemcpyHostToDevice)�� ȣ��Ʈ���� ����̽��� �޸𸮸� ī���Ѵ�.
//	cudaMemcpy(pstPrarticlePosCuda, pstPrarticlePos, nSizeCnt*nPrarticlePosCnt, cudaMemcpyHostToDevice);
//	//cudaMemcpy(pnPrarticlePosCntCuda, &nPrarticlePosCnt, sizeof(int), cudaMemcpyHostToDevice);
//
//
//
//	//if ( cudaSuccess != cudaMemcpy(bVoxelYResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
//	//{
//	//	printf( "Error! Y VoxelState Memcpy \n" );
//	//}
//
//	
//	int nBlockCnt = nPrarticlePosCnt;
//	kernelCalcRocking<<<nBlockCnt, 6>>>(nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, m_fCalcWaterInnerAbsorption, m_fCalcLayerWaterAborption, m_fCalcWaterChange);
//	
//
//	
//	cudaMemcpy(pstPrarticlePosMask, pstPrarticlePosCudaMask, nSizeCnt*nPrarticlePosCnt, cudaMemcpyDeviceToHost);
//
//
//	cudaFree(pstPrarticlePosCuda);
//	cudaFree(pstPrarticlePosCudaMask);
//	//cudaFree(pnPrarticlePosCntCuda);
//
//	int a= 0;
//
//
//}


__device__ void ViewMaskData(ST_PARTICLE_POS	*pstPrarticlePosCudaMask, int tid, int nPrarticlePosCntCuda, int nXFileVoxCnt, int nYFileVoxCnt)
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

__device__ void InputMaskDataToSharedMem(ST_PARTICLE_POS	*pstPrarticlePosCudaMask, ST_PARTICLE_POS_UNIT_PROCESS	*astParticle_pos_unitProcess, int tid, int threadIdx, int nPrarticlePosCntCuda, int nXFileVoxCnt, int nYFileVoxCnt)
{

	pstPrarticlePosCudaMask[tid].fPorosity += astParticle_pos_unitProcess[threadIdx].fPorosity;
	pstPrarticlePosCudaMask[tid].fHaveWater += astParticle_pos_unitProcess[threadIdx].fHaveWater;


	int nValue = tid + (nXFileVoxCnt * nYFileVoxCnt);
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		pstPrarticlePosCudaMask[nValue].fPorosity += astParticle_pos_unitProcess[threadIdx+ 1].fPorosity;
		pstPrarticlePosCudaMask[nValue].fHaveWater += astParticle_pos_unitProcess[threadIdx+ 1].fHaveWater;

	

	}

	nValue = tid - (nXFileVoxCnt * nYFileVoxCnt);
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{

		pstPrarticlePosCudaMask[nValue].fPorosity += astParticle_pos_unitProcess[threadIdx+ 2].fPorosity;
		pstPrarticlePosCudaMask[nValue].fHaveWater += astParticle_pos_unitProcess[threadIdx+ 2].fHaveWater;

	
	}

	nValue = tid + nXFileVoxCnt;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		pstPrarticlePosCudaMask[nValue].fPorosity += astParticle_pos_unitProcess[threadIdx+ 3].fPorosity;
		pstPrarticlePosCudaMask[nValue].fHaveWater += astParticle_pos_unitProcess[threadIdx+ 3].fHaveWater;

		
	}

	nValue = tid - nXFileVoxCnt;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		pstPrarticlePosCudaMask[nValue].fPorosity += astParticle_pos_unitProcess[threadIdx+ 4].fPorosity;
		pstPrarticlePosCudaMask[nValue].fHaveWater += astParticle_pos_unitProcess[threadIdx+ 4].fHaveWater;

	}

	nValue = tid - 1;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		
		pstPrarticlePosCudaMask[nValue].fPorosity += astParticle_pos_unitProcess[threadIdx+ 5].fPorosity;
		pstPrarticlePosCudaMask[nValue].fHaveWater += astParticle_pos_unitProcess[threadIdx+ 5].fHaveWater;


	}

	nValue = tid + 1;
	if(nValue >= 0 && nValue < nPrarticlePosCntCuda)
	{
		pstPrarticlePosCudaMask[nValue].fPorosity += astParticle_pos_unitProcess[threadIdx+ 6].fPorosity;
		pstPrarticlePosCudaMask[nValue].fHaveWater += astParticle_pos_unitProcess[threadIdx+ 6].fHaveWater;
	}

	
	

}


__global__ void kernelCalcRocking(int nThreadCnt, 
								  int nPrarticlePosCntCuda, ST_PARTICLE_POS	*pstPrarticlePosCuda, ST_PARTICLE_POS	*pstPrarticlePosCudaMask, 
								  int nXFileVoxCnt, int nYFileVoxCnt, int nZFileVoxCnt,
								  float fCoefficient, float fTopRate, float fSideRate, float fBottomRate,
								  float fCalcWaterInnerAbsorption, float fCalcLayerWaterAborption, float fCalcWaterChange
								  )
{ 
	//return;

	 // ������ �����尡 ���ÿ� ó���Ѵ�. // ���� threadIdx(������ �ε���)�� ���ؼ� ��������� �����Ѵ�. 
	//int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int tid = blockIdx.x * blockDim.x + threadIdx.x;

	//if(tid > nPrarticlePosCntCuda)
	//	return;

	
	//return;
	//int nExternalSideIdx = threadIdx.x % 6;
    //printf( "x:%d\n", nPrarticlePosCntCuda);

	if(tid > nPrarticlePosCntCuda)
		return;

	//printf("tid : %d\n", tid);

	//extern __shared__ ST_PARTICLE_POS_UNIT_PROCESS			astParticle_pos_unitProcess[]; //->0:����, 1:��,2:��,3:��,4:��,5:��,6��
	//__shared__ ST_PARTICLE_POS_UNIT_PROCESS			astParticle_pos_unitProcess[32 * 7]; //->0:����, 1:��,2:��,3:��,4:��,5:��,6��
	__shared__ ST_PARTICLE_POS_UNIT_PROCESS			astParticle_pos_unitProcess[dfSHARED_MEMORY_SIZE]; //->0:����, 1:��,2:��,3:��,4:��,5:��,6��
	memset(astParticle_pos_unitProcess, NULL, sizeof(ST_PARTICLE_POS_UNIT_PROCESS) * dfSHARED_MEMORY_SIZE);
	//memset(astParticle_pos_unitProcess, NULL, sizeof(ST_PARTICLE_POS_UNIT_PROCESS) * lcm(nThreadCnt, 6));
	__syncthreads();

	//ViewMaskData(pstPrarticlePosCudaMask, tid, nPrarticlePosCntCuda, nXFileVoxCnt, nYFileVoxCnt);


	//printf( "IN_OUT : %d\n", pstPrarticlePosCuda[tid].bInOut);
	if(pstPrarticlePosCuda[tid].bInOut == true) //! �ܺδ� ���� �Ի�ر�
	{
		//! �ܺ� �Ի�ر��� �������� ó���Ѵ�.(SHARED ����ŷ ���� �޸𸮿� ó��!)
		 astParticle_pos_unitProcess[threadIdx.x * 7].fPorosity = (pstPrarticlePosCuda[tid].abExternalSide[0] * fCoefficient * fTopRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[1] * fCoefficient * fBottomRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[2] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[3] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[4] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[5] * fCoefficient * fSideRate);
		
		
		//printf( "OuterMask : %d -> Porosity:%f\n", tid, astParticle_pos_unitProcess[threadIdx.x].fPorosity);
	}
	else //! ���� ó��
	{
		if(pstPrarticlePosCuda[tid].sStoneType == 0) //! ���ظ� ó���ؾ��Ѵ�.
		{
			//printf( "InnerMask Check : %d\n", tid);


			//__shared__ unsigned int sdata[6];

			//1. ���� ����� = ��������� - {(�ִ� ���̾� Idx - ���� ���̾� Idx) * ���̾ ���� ������ * ��������� }
			float fHaveWaterTemp = fCalcWaterInnerAbsorption - ( (/*sMaxLayerIdx -*/ pstPrarticlePosCuda[tid].sLayerIdx) * fCalcLayerWaterAborption * fCalcWaterInnerAbsorption );
			
			//! 0���� ������ ������ ���� ������
			if(fHaveWaterTemp < 0.0)
				fHaveWaterTemp = 0.0;

			//fPorosity = 0.0;
			//__syncthreads();

			//! ����ŷ��
			astParticle_pos_unitProcess[threadIdx.x].fHaveWater += fHaveWaterTemp;

			//printf( "Inner : %d -> Water:%f\n", tid, astParticle_pos_unitProcess[threadIdx.x].fHaveWater);

			//(pstPrarticlePosCuda[tid].abExternalSide[0] * fCoefficient * fTopRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[1] * fCoefficient * fBottomRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[2] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[3] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[4] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[5] * fCoefficient * fSideRate);


			//3. if(���� ��ȭ�� * ���� ��â�� > 1.0)
			if(((pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) * fCalcWaterChange) > 1.0)
			{
				//! ��
				astParticle_pos_unitProcess[threadIdx.x + 1].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				astParticle_pos_unitProcess[threadIdx.x + 2].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				astParticle_pos_unitProcess[threadIdx.x + 3].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				astParticle_pos_unitProcess[threadIdx.x + 4].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				astParticle_pos_unitProcess[threadIdx.x + 5].fPorosity += (fCalcWaterChange / 5.0);
				//! ��
				astParticle_pos_unitProcess[threadIdx.x + 6].fPorosity += (fCalcWaterChange / 5.0);

			}

		}
	}



	pstPrarticlePosCudaMask[tid].fPorosity += astParticle_pos_unitProcess[threadIdx.x].fPorosity;
	pstPrarticlePosCudaMask[tid].fHaveWater += astParticle_pos_unitProcess[threadIdx.x].fHaveWater;
	
	pstPrarticlePosCudaMask[tid + (nXFileVoxCnt * nYFileVoxCnt)].fPorosity += astParticle_pos_unitProcess[threadIdx.x + 1].fPorosity;
	pstPrarticlePosCudaMask[tid + (nXFileVoxCnt * nYFileVoxCnt)].fHaveWater += astParticle_pos_unitProcess[threadIdx.x + 1].fHaveWater;

	pstPrarticlePosCudaMask[tid - (nXFileVoxCnt * nYFileVoxCnt)].fPorosity += astParticle_pos_unitProcess[threadIdx.x + 2].fPorosity;
	pstPrarticlePosCudaMask[tid - (nXFileVoxCnt * nYFileVoxCnt)].fHaveWater += astParticle_pos_unitProcess[threadIdx.x + 2].fHaveWater;

	pstPrarticlePosCudaMask[tid + nXFileVoxCnt].fPorosity += astParticle_pos_unitProcess[threadIdx.x + 3].fPorosity;
	pstPrarticlePosCudaMask[tid + nXFileVoxCnt].fHaveWater += astParticle_pos_unitProcess[threadIdx.x + 3].fHaveWater;

	pstPrarticlePosCudaMask[tid - nXFileVoxCnt].fPorosity += astParticle_pos_unitProcess[threadIdx.x + 4].fPorosity;
	pstPrarticlePosCudaMask[tid - nXFileVoxCnt].fHaveWater += astParticle_pos_unitProcess[threadIdx.x + 4].fHaveWater;

	pstPrarticlePosCudaMask[tid - 1].fPorosity += astParticle_pos_unitProcess[threadIdx.x + 5].fPorosity;
	pstPrarticlePosCudaMask[tid - 1].fHaveWater += astParticle_pos_unitProcess[threadIdx.x + 5].fHaveWater;

	pstPrarticlePosCudaMask[tid + 1].fPorosity += astParticle_pos_unitProcess[threadIdx.x + 6].fPorosity;
	pstPrarticlePosCudaMask[tid + 1].fHaveWater += astParticle_pos_unitProcess[threadIdx.x + 6].fHaveWater;
	


} 


void CGPUCalcRockAgingInner::SetInnderVoxelData(int nPrarticlePosCnt, ST_PARTICLE_POS	*pstPrarticlePos, ST_PARTICLE_POS	*pstPrarticlePosMask)
{
	//! ���� ����
	ST_PARTICLE_POS *pstPrarticlePosCuda;
	ST_PARTICLE_POS *pstPrarticlePosCudaMask;
	//! ���� ����
	//int *pnPrarticlePosCntCuda;

	// cudaMalloc(destination, number of byte)�� device�� �޸𸮸� �Ҵ��Ѵ�.
	int nSizeCnt = sizeof(ST_PARTICLE_POS);
	if ( cudaSuccess != cudaMalloc(&pstPrarticlePosCuda, nSizeCnt*nPrarticlePosCnt))
	{
		printf( "Error! Malloc \n" );
	}

	if ( cudaSuccess != cudaMalloc(&pstPrarticlePosCudaMask, nSizeCnt*nPrarticlePosCnt))
	{
		printf( "Error! Malloc \n" );
	}


	/*if ( cudaSuccess != cudaMemset(pstPrarticlePosCudaMask, NULL, nSizeCnt*nPrarticlePosCnt))
	{
		printf( "Error! Memset \n" );
	}*/


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
	cudaMemcpy(pstPrarticlePosCuda, pstPrarticlePos, nSizeCnt*nPrarticlePosCnt, cudaMemcpyHostToDevice);
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
	kernelCalcRocking<<<nBlockCnt, nThreadCnt>>>(nThreadCnt, nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, m_fCalcWaterInnerAbsorption, m_fCalcLayerWaterAborption, m_fCalcWaterChange);
	//kernelCalcRocking<<<4, nThreadCnt, nThreadCnt * 7>>>(nThreadCnt, nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, m_fCalcWaterInnerAbsorption, m_fCalcLayerWaterAborption, m_fCalcWaterChange);
	
	//cudaDeviceSynchronize();
	//cudaStreamSynchronize(cudaStream);

	printf("TEST\n");
	
	if ( cudaSuccess != cudaMemcpy(pstPrarticlePosMask, pstPrarticlePosCudaMask, nSizeCnt*nPrarticlePosCnt, cudaMemcpyDeviceToHost))
	{
		printf( "Error! Copy \n" );
	}


	cudaFree(pstPrarticlePosCuda);
	cudaFree(pstPrarticlePosCudaMask);
	//cudaFree(pnPrarticlePosCntCuda);

	//pstPrarticlePosMask[0].x = 1;


	int a= 0;


}


