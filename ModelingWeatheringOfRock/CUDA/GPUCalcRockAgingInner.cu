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


__global__ void kernelCalcRocking(int nPrarticlePosCntCuda, ST_PARTICLE_POS	*pstPrarticlePosCuda, 
								  int nX, int nY, int nZ,
								  float fCoefficient, float fTopRate, float fSideRate, float fBottomRate
								  )
{ 
	// 수많은 스레드가 동시에 처리한다. // 따라서 threadIdx(스레드 인덱스)를 통해서 스레드들을 구별한다. 
	//int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nExternalSideIdx = threadIdx.x;
    //printf( "x:%d\n", nPrarticlePosCntCuda);

	if(tid > nPrarticlePosCntCuda)
		return;

	
    __shared__ unsigned int sdata[6];


	if(pstPrarticlePosCuda[tid].bInOut == true) //! 외부는 기존 입상붕괴
	{
		__shared__ float			fPorosity;

		if(pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx] == TRUE)
		{
			//switch(stParticlePos.vecExternalSide[nEx])	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
			switch(nExternalSideIdx)	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
			{
			case 0:
				{
					fPorosity += (fCoefficient * fTopRate);
				}
				break;
			case 1:
				{
					fPorosity += (fCoefficient * fBottomRate);
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				{
					fPorosity += (fCoefficient * fSideRate);
				}
				break;
			default:
				break;
			}

		}

        __syncthreads();

		pstPrarticlePosCuda[tid].fPorosity += fPorosity;
		//fPorosity 공극률 >= fGranularDisintegration 입상붕괴 도달값
		if(pstPrarticlePosCuda[tid].fPorosity >= pstPrarticlePosCuda[tid].fGranularDisintegration) // 입상붕괴 도달값에 도달하여 제거
		{
			

			//stParticlePos.bUse = false;

			//strKey.Format(L"%d-%d-%d",x,y,z);

			//vecDeleParticle.push_back(strKey);

			//break;
		}

		;

	}
	else
	{

	}

	if(pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx] == true)
	{

	}


	pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx];

    printf( "x:%d\ty:%d\tz:%d\tTid : %d\n", pstPrarticlePosCuda[tid].x, pstPrarticlePosCuda[tid].y, pstPrarticlePosCuda[tid].z,  tid);


} 



void CGPUCalcRockAgingInner::SetInnderVoxelData(int nPrarticlePosCnt, ST_PARTICLE_POS	*pstPrarticlePos)
{
	//! 복셀 정보
	ST_PARTICLE_POS *pstPrarticlePosCuda;
	//! 복셀 개수
	int *pnPrarticlePosCntCuda;

	// cudaMalloc(destination, number of byte)로 device의 메모리를 할당한다.
	int nSizeCnt = sizeof(ST_PARTICLE_POS);
	if ( cudaSuccess != cudaMalloc(&pstPrarticlePosCuda, nSizeCnt*nPrarticlePosCnt))
	{
		printf( "Error! Malloc \n" );
	}

	if( cudaSuccess != cudaMalloc(&pnPrarticlePosCntCuda, sizeof(int)))
	{
		printf( "Error! Malloc \n" );

		Sleep(1000);
	}
	//else
	//{
	//	printf( "ErrorGOOD \n" );

	//}


	
	
	// cudaMemcpy(destination, source, number of byte, cudaMemcpyHostToDevice)로 호스트에서 디바이스로 메모리를 카피한다.
	cudaMemcpy(pstPrarticlePosCuda, pstPrarticlePos, nSizeCnt*nPrarticlePosCnt, cudaMemcpyHostToDevice);
	cudaMemcpy(pnPrarticlePosCntCuda, &nPrarticlePosCnt, sizeof(int), cudaMemcpyHostToDevice);



	//if ( cudaSuccess != cudaMemcpy(bVoxelYResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
	//{
	//	printf( "Error! Y VoxelState Memcpy \n" );
	//}

	
	int nBlockCnt = nPrarticlePosCnt;

	

	kernelCalcRocking<<<nBlockCnt, 6>>>(nPrarticlePosCnt, pstPrarticlePosCuda, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate);
	//kernelCalcRocking<<<nBlockCnt, 32>>>(*pnPrarticlePosCntCuda, pstPrarticlePosCuda);



	cudaFree(pstPrarticlePosCuda);
	cudaFree(pnPrarticlePosCntCuda);

	int a= 0;


}

