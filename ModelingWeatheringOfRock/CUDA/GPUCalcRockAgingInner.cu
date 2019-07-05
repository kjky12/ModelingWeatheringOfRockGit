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


__global__ void kernelCalcRocking(int nPrarticlePosCntCuda, ST_PARTICLE_POS	*pstPrarticlePosCuda, ST_PARTICLE_POS	*pstPrarticlePosCudaMask, 
								  int nX, int nY, int nZ,
								  float fCoefficient, float fTopRate, float fSideRate, float fBottomRate,
								  float fCalcWaterInnerAbsorption, float fCalcLayerWaterAborption, float fCalcWaterChange
								  )
{ 
	// ������ �����尡 ���ÿ� ó���Ѵ�. // ���� threadIdx(������ �ε���)�� ���ؼ� ��������� �����Ѵ�. 
	//int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nExternalSideIdx = threadIdx.x;
    //printf( "x:%d\n", nPrarticlePosCntCuda);

	if(tid > nPrarticlePosCntCuda)
		return;

	
    


	//printf( "IN_OUT : %d\n", pstPrarticlePosCuda[tid].bInOut);
	if(pstPrarticlePosCuda[tid].bInOut == true) //! �ܺδ� ���� �Ի�ر�
	{
		__shared__ float			fPorosity;
		fPorosity = 0.0;


		if(pstPrarticlePosCuda[tid].abExternalSide[nExternalSideIdx] == TRUE)
		{
			//switch(stParticlePos.vecExternalSide[nEx])	//[0:��,1:��,2:��,3:��,4:��,5:��]
			switch(nExternalSideIdx)	//[0:��,1:��,2:��,3:��,4:��,5:��]
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

		pstPrarticlePosCudaMask[tid].fPorosity = fPorosity;
		printf( "OuterMask : %d\n", tid);

		//fPorosity ���ط� >= fGranularDisintegration �Ի�ر� ���ް�
		//if(pstPrarticlePosCuda[tid].fPorosity >= pstPrarticlePosCuda[tid].fGranularDisintegration) // �Ի�ر� ���ް��� �����Ͽ� ����
		//{
		//	

		//	//stParticlePos.bUse = false;

		//	//strKey.Format(L"%d-%d-%d",x,y,z);

		//	//vecDeleParticle.push_back(strKey);

		//	//break;
		//}
	}
	else //! ���� ó��
	{
		if(pstPrarticlePosCuda[tid].sStoneType == 0) //! ���ظ� ó���ؾ��Ѵ�.
		{
			printf( "InnerMask Check : %d\n", tid);


			//__shared__ unsigned int sdata[6];

			//1. ���� ����� = ��������� - {(�ִ� ���̾� Idx - ���� ���̾� Idx) * ���̾ ���� ������ * ��������� }
			float fHaveWaterTemp = fCalcWaterInnerAbsorption - ( (/*sMaxLayerIdx -*/ pstPrarticlePosCuda[tid].sLayerIdx) * fCalcLayerWaterAborption * fCalcWaterInnerAbsorption );
			
			//! 0���� ������ ������ ���� ������
			if(fHaveWaterTemp < 0.0)
				fHaveWaterTemp = 0.0;

			__shared__ int			nBreakCnt;
			nBreakCnt = 0;

			//3. if(���� ��ȭ�� * ���� ��â�� > 1.0)
			if(((pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) * fCalcWaterChange) > 1.0)
			{
				int nPos = tid;
				switch(nExternalSideIdx)	//[0:��,1:��,2:��,3:��,4:��,5:��]
				{
				case 0:
					nPos += (nX * nY);
					break;
				case 1:
					nPos -= (nX * nY);
					break;
				case 2:
					nPos += nX;
					break;
				case 3:
					nPos -= nX;
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

				nBreakCnt++;

				__syncthreads();


				pstPrarticlePosCudaMask[nPos].fHaveWater = (pstPrarticlePosCuda[tid].fHaveWater + fHaveWaterTemp) / (float)(nBreakCnt + 1);
				printf( "InnerMask : %d\n", nPos);
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
	cudaMemcpy(pstPrarticlePosCuda, pstPrarticlePos, nSizeCnt*nPrarticlePosCnt, cudaMemcpyHostToDevice);
	//cudaMemcpy(pnPrarticlePosCntCuda, &nPrarticlePosCnt, sizeof(int), cudaMemcpyHostToDevice);



	//if ( cudaSuccess != cudaMemcpy(bVoxelYResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
	//{
	//	printf( "Error! Y VoxelState Memcpy \n" );
	//}

	
	int nBlockCnt = nPrarticlePosCnt;

	

	kernelCalcRocking<<<nBlockCnt, 6>>>(nPrarticlePosCnt, pstPrarticlePosCuda, pstPrarticlePosCudaMask, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, m_fCoefficient, m_fTopRate, m_fSideRate, m_fBottomRate, m_fCalcWaterInnerAbsorption, m_fCalcLayerWaterAborption, m_fCalcWaterChange);
	//kernelCalcRocking<<<nBlockCnt, 32>>>(*pnPrarticlePosCntCuda, pstPrarticlePosCuda);

	
	cudaMemcpy(pstPrarticlePosMask, pstPrarticlePosCudaMask, nSizeCnt*nPrarticlePosCnt, cudaMemcpyDeviceToHost);


	cudaFree(pstPrarticlePosCuda);
	cudaFree(pstPrarticlePosCudaMask);
	//cudaFree(pnPrarticlePosCntCuda);

	int a= 0;


}

