#include "GPUCalcRockAgingInner.cuh"
#include "cuda.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Windows.h"


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
//	// 수많은 스레드가 동시에 처리한다. // 따라서 threadIdx(스레드 인덱스)를 통해서 스레드들을 구별한다. 
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
//	if(pstPrarticlePosCuda[tid].bInOut == true) //! 외부는 기존 입상붕괴
//	{
//		__shared__ float			fPorosity;
//		fPorosity = 0.0;
//
//
//		if(pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx] == TRUE)
//		{
//			//switch(stParticlePos.vecExternalSide[nEx])	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
//			switch(nExternalSideIdx)	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
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
//		//fPorosity 공극률 >= fGranularDisintegration 입상붕괴 도달값
//		//if(pstPrarticlePosCuda[tid].fPorosity >= pstPrarticlePosCuda[tid].fGranularDisintegration) // 입상붕괴 도달값에 도달하여 제거
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
//	else //! 내부 처리
//	{
//		if(pstPrarticlePosCuda[tid].sStoneType == 0) //! 공극만 처리해야한다.
//		{
//			printf( "InnerMask Check : %d\n", tid);
//
//
//			//__shared__ unsigned int sdata[6];
//
//			//1. 수분 흡수량 = 수분흡수율 - {(최대 레이어 Idx - 현재 레이어 Idx) * 레이어별 수분 차감률 * 수분흡수율 }
//			float fHaveWaterTemp = fCalcWaterInnerAbsorption - ( (/*sMaxLayerIdx -*/ pstPrarticlePosCuda[tid].sLayerIdx) * fCalcLayerWaterAborption * fCalcWaterInnerAbsorption );
//			
//			//! 0보다 적으면 수분이 들어가지 않은것
//			if(fHaveWaterTemp < 0.0)
//				fHaveWaterTemp = 0.0;
//
//			__shared__ float			fPorosity[6];
//			memset(fPorosity, NULL, sizeof(float) * 6);
//			//fPorosity = 0.0;
//			//__syncthreads();
//
//			//3. if(수분 포화도 * 수분 팽창률 > 1.0)
//			if(((pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) * fCalcWaterChange) > 1.0)
//			{
//				int nPos = tid;
//				switch(nExternalSideIdx)	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
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
//				//if(pstPrarticlePosCudaMask[nPos].fPorosity >= pstPrarticlePosCudaMask[nPos].fGranularDisintegration) // 입상붕괴 도달값에 도달하여 제거
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
//	//! 복셀 정보
//	ST_PARTICLE_POS *pstPrarticlePosCuda;
//	ST_PARTICLE_POS *pstPrarticlePosCudaMask;
//	//! 복셀 개수
//	//int *pnPrarticlePosCntCuda;
//
//	// cudaMalloc(destination, number of byte)로 device의 메모리를 할당한다.
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
//	// cudaMemcpy(destination, source, number of byte, cudaMemcpyHostToDevice)로 호스트에서 디바이스로 메모리를 카피한다.
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


__global__ void kernelCalcRocking(int nThreadCnt, 
								  int nPrarticlePosCntCuda, ST_PARTICLE_POS	*pstPrarticlePosCuda, ST_PARTICLE_POS	*pstPrarticlePosCudaMask, 
								  int nXFileVoxCnt, int nYFileVoxCnt, int nZFileVoxCnt,
								  float fCoefficient, float fTopRate, float fSideRate, float fBottomRate,
								  float fCalcWaterInnerAbsorption, float fCalcLayerWaterAborption, float fCalcWaterChange
								  )
{ 
	 // 수많은 스레드가 동시에 처리한다. // 따라서 threadIdx(스레드 인덱스)를 통해서 스레드들을 구별한다. 
	//int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nExternalSideIdx = threadIdx.x;
    //printf( "x:%d\n", nPrarticlePosCntCuda);

	if(tid > nPrarticlePosCntCuda)
		return;
	
	__shared__ ST_PARTICLE_POS_UNIT_PROCESS			astParticle_pos_unitProcess[32 * 6];
	memset(astParticle_pos_unitProcess, NULL, sizeof(ST_PARTICLE_POS_UNIT_PROCESS) * 32 * 6);
	__syncthreads();

	//printf( "IN_OUT : %d\n", pstPrarticlePosCuda[tid].bInOut);
	if(pstPrarticlePosCuda[tid].bInOut == true) //! 외부는 기존 입상붕괴
	{
		//! 외부 입상붕괴를 수식으로 처리한다.(SHARED 마스킹 전용 메모리에 처리!)
		 astParticle_pos_unitProcess[threadIdx.x].fPorosity = (pstPrarticlePosCuda[tid].abExternalSide[0] * fCoefficient * fTopRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[1] * fCoefficient * fBottomRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[2] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[3] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[4] * fCoefficient * fSideRate)
			 +  (pstPrarticlePosCuda[tid].abExternalSide[5] * fCoefficient * fSideRate);
		
		//pstPrarticlePosCudaMask[tid].fPorosity = fPorosity;
		printf( "OuterMask : %d\n", tid);
	}
	else //! 내부 처리
	{
		if(pstPrarticlePosCuda[tid].sStoneType == 0) //! 공극만 처리해야한다.
		{
			printf( "InnerMask Check : %d\n", tid);


			//__shared__ unsigned int sdata[6];

			//1. 수분 흡수량 = 수분흡수율 - {(최대 레이어 Idx - 현재 레이어 Idx) * 레이어별 수분 차감률 * 수분흡수율 }
			float fHaveWaterTemp = fCalcWaterInnerAbsorption - ( (/*sMaxLayerIdx -*/ pstPrarticlePosCuda[tid].sLayerIdx) * fCalcLayerWaterAborption * fCalcWaterInnerAbsorption );
			
			//! 0보다 적으면 수분이 들어가지 않은것
			if(fHaveWaterTemp < 0.0)
				fHaveWaterTemp = 0.0;

			//fPorosity = 0.0;
			//__syncthreads();



			//(pstPrarticlePosCuda[tid].abExternalSide[0] * fCoefficient * fTopRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[1] * fCoefficient * fBottomRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[2] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[3] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[4] * fCoefficient * fSideRate)
			// +  (pstPrarticlePosCuda[tid].abExternalSide[5] * fCoefficient * fSideRate);


			//3. if(수분 포화도 * 수분 팽창률 > 1.0)
			if(((pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) * fCalcWaterChange) > 1.0)
			{
				int nPos = tid;
				switch(nExternalSideIdx)	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
				{
				case 0:
					nPos += (nXFileVoxCnt * nYFileVoxCnt);
					break;
				case 1:
					nPos -= (nXFileVoxCnt * nYFileVoxCnt);
					break;
				case 2:
					nPos += nXFileVoxCnt;
					break;
				case 3:
					nPos -= nXFileVoxCnt;
					break;
				case 4:
					nPos -= 1;
					break;
				case 5:
					nPos += 1;
					break;
				default:
					break;
				}

				if(nPos > nPrarticlePosCntCuda)
					return;

				//fPorosity[nExternalSideIdx] = (fCalcWaterChange / 5.0);

				__syncthreads();

				pstPrarticlePosCudaMask[nPos].fPorosity += fPorosity[nExternalSideIdx];

				//if(pstPrarticlePosCudaMask[nPos].fPorosity >= pstPrarticlePosCudaMask[nPos].fGranularDisintegration) // 입상붕괴 도달값에 도달하여 제거
				//{
				//	nBreakCnt++;

				//	__syncthreads();
				//	pstPrarticlePosCudaMask[nPos].fHaveWater = (pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) / (float)(nBreakCnt + 1);
				//	printf( "InnerMask : %d\n", nPos);
				//}
			

				//pstPrarticlePosCudaMask[nPos]

			}

		}
	}

	/*if(pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx] == true)
	{

	}*/


	//pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx];

    //printf( "x:%d\ty:%d\tz:%d\tTid : %d\n", pstPrarticlePosCuda[tid].x, pstPrarticlePosCuda[tid].y, pstPrarticlePosCuda[tid].z,  tid);


} 



void CGPUCalcRockAgingInner::SetInnderVoxelData(int nPrarticlePosCnt, ST_PARTICLE_POS	*pstPrarticlePos, ST_PARTICLE_POS	*pstPrarticlePosMask)
{
	//! 복셀 정보
	ST_PARTICLE_POS *pstPrarticlePosCuda;
	ST_PARTICLE_POS *pstPrarticlePosCudaMask;
	//! 복셀 개수
	//int *pnPrarticlePosCntCuda;

	// cudaMalloc(destination, number of byte)로 device의 메모리를 할당한다.
	int nSizeCnt = sizeof(ST_PARTICLE_POS);
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


	
	
	// cudaMemcpy(destination, source, number of byte, cudaMemcpyHostToDevice)로 호스트에서 디바이스로 메모리를 카피한다.
	cudaMemcpy(pstPrarticlePosCuda, pstPrarticlePos, nSizeCnt*nPrarticlePosCnt, cudaMemcpyHostToDevice);
	//cudaMemcpy(pnPrarticlePosCntCuda, &nPrarticlePosCnt, sizeof(int), cudaMemcpyHostToDevice);



	//if ( cudaSuccess != cudaMemcpy(bVoxelYResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
	//{
	//	printf( "Error! Y VoxelState Memcpy \n" );
	//}

	int nThreadCnt = 32;
	int nBlockCnt = (nPrarticlePosCnt / nThreadCnt) + 1;
	kernelCalcRocking<<<nBlockCnt, nThreadCnt * 6>>>(nThreadCnt, nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, m_fCalcWaterInnerAbsorption, m_fCalcLayerWaterAborption, m_fCalcWaterChange);
	

	
	cudaMemcpy(pstPrarticlePosMask, pstPrarticlePosCudaMask, nSizeCnt*nPrarticlePosCnt, cudaMemcpyDeviceToHost);


	cudaFree(pstPrarticlePosCuda);
	cudaFree(pstPrarticlePosCudaMask);
	//cudaFree(pnPrarticlePosCntCuda);

	int a= 0;


}
