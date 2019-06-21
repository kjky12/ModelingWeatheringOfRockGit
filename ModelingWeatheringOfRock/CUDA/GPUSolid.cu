#include "GPUSolid.cuh"
#include "cuda.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


#define SIZE 1024

CGPUSolid::CGPUSolid(void)
{

}

CGPUSolid::~CGPUSolid(void)
{

}




__global__ void sum_Kernel(int a, int b, int* c)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	c[tid] = a + b;
}



int CGPUSolid::sum_cuda(int a, int b, int *c)
{
	int *f;

	cudaMalloc(&f, sizeof(int) * 1);
	cudaMemcpy(f, c, sizeof(int) * 1, cudaMemcpyHostToDevice);

	sum_Kernel << <1, 1>> > (a,b, f);

	cudaMemcpy(c, f, sizeof(int) * 1, cudaMemcpyDeviceToHost);

	cudaFree(f);


	return true;
}






__global__ void VectorAdd(int *a, int *b, int *c, int n)
{ 
	// 수많은 스레드가 동시에 처리한다. // 따라서 threadIdx(스레드 인덱스)를 통해서 스레드들을 구별한다. 
	int i = threadIdx.x; 
	printf("threadIdx.x : %d, n : %d\n", i, n); 
	for (i = 0; i < n; i++) 
	{ 
		c[i] = a[i] + b[i]; 
		printf("%d = %d + %d\n", c[i], a[i], b[i]); 
	} 
} 



int CGPUSolid::Sum_TEST(int *a, int *b, int *c)
{
	int *d_a, *d_b, *d_c;

	// cudaMalloc(destination, number of byte)로 device의 메모리를 할당한다.
	cudaMalloc(&d_a, SIZE*sizeof(int));
	cudaMalloc(&d_b, SIZE*sizeof(int));
	cudaMalloc(&d_c, SIZE*sizeof(int));
	
	
	// cudaMemcpy(destination, source, number of byte, cudaMemcpyHostToDevice)로 호스트에서 디바이스로 메모리를 카피한다.
	cudaMemcpy(d_a, a, SIZE*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, b, SIZE*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_c, c, SIZE*sizeof(int), cudaMemcpyHostToDevice);
	
	// 함수 호출을 위해서 새로운 신텍스 요소를 추가할 필요가 있다.
	// 첫번째 parameter는 블럭의 수이다. 예제에서는 스레드 블럭이 하나이다.
	// SIZE는 1024개의 스레드를 의미한다.
	VectorAdd << < 1, SIZE >> >(d_a, d_b, d_c, SIZE);
	
	
	//cudaMemcpy(source, destination, number of byte, cudaMemDeviceToHost)로 디바이스의 메모리(연산 결과 데이터)를 호스트에 카피한다.
	cudaMemcpy(a, d_a, SIZE*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(b, d_b, SIZE*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(c, d_c, SIZE*sizeof(int), cudaMemcpyDeviceToHost);
	

	for (int i = 0; i<SIZE; ++i)
		printf("c[%d] = %d\n", i, c[i]);

	// cudaFree(d_a)를 통해 디바이스의 메모리를 할당 해제
	cudaFree(d_a);
	cudaFree(d_b);
	cudaFree(d_c);
return 0;
}


void CGPUSolid::GpuInfoPrint()
{
	cudaDeviceProp  prop;

    int count = 0;
    cudaGetDeviceCount( &count );

    for (int i=0; i< count; i++) 
	{
        cudaGetDeviceProperties( &prop, i );
        printf( "   --- General Information for device %d ---\n", i );
        printf( "Name:  %s\n", prop.name );
        printf( "Compute capability:  %d.%d\n", prop.major, prop.minor );
        printf( "Clock rate:  %d\n", prop.clockRate );
        printf( "Device copy overlap:  " );
        if (prop.deviceOverlap)
            printf( "Enabled\n" );
        else
            printf( "Disabled\n");
        printf( "Kernel execution timeout :  " );
        if (prop.kernelExecTimeoutEnabled)
            printf( "Enabled\n" );
        else
            printf( "Disabled\n" );
		printf( "\n" );

        printf( "   --- Memory Information for device %d ---\n", i );
        printf( "Total global mem:  %ld\n", prop.totalGlobalMem );
        printf( "Total constant Mem:  %ld\n", prop.totalConstMem );
        printf( "Max mem pitch:  %ld\n", prop.memPitch );
        printf( "Texture Alignment:  %ld\n", prop.textureAlignment );
		printf( "\n" );

        printf( "   --- MP Information for device %d ---\n", i );
        printf( "Multiprocessor count:  %d\n", prop.multiProcessorCount );
        printf( "Shared mem per mp:  %ld\n", prop.sharedMemPerBlock );
        printf( "Registers per mp:  %d\n", prop.regsPerBlock );
        printf( "Threads in warp:  %d\n", prop.warpSize );
        printf( "Max threads per block:  %d\n", prop.maxThreadsPerBlock );
        printf( "Max thread dimensions:  (%d, %d, %d)\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2] );
        printf( "Max grid dimensions:  (%d, %d, %d)\n",prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2] );
        printf( "\n" );


		m_nMaxBlockSize = prop.maxGridSize[0];
		if(m_nMaxBlockSize > prop.maxGridSize[1])
			m_nMaxBlockSize = prop.maxGridSize[1];
		if(m_nMaxBlockSize > prop.maxGridSize[2])
			m_nMaxBlockSize = prop.maxGridSize[2];

		m_nMaxThreadSize = prop.maxThreadsPerBlock ;
    }

    return ;

}



__global__ void UseCheckX(bool *bValue, bool *bResult, int nSize)
{ 
	// 수많은 스레드가 동시에 처리한다. // 따라서 threadIdx(스레드 인덱스)를 통해서 스레드들을 구별한다. 
	//int i = threadIdx.x; 
	//printf("threadIdx.x : %d, n : %d\n", i, n); 
	//for (i = 0; i < n; i++) 
	//{ 
	//	c[i] = a[i] + b[i]; 
	//	printf("%d = %d + %d\n", c[i], a[i], b[i]); 
	//} 

	int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int nPos = tid * nSize;

	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\ttid : %02d\tPos : %03d\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos);


	int nOutCnt = 0;
	int nStartIdx = -1, nEndIdx = -1;
	int x = 0;
	for(x = 0 ; x < nSize ; x++)
	{
		if(bValue[nPos + x])	// 첫 데이터를 찾는다.
		{
			nStartIdx = x;
			break;
		}
		else
			nOutCnt++;
	}

	if(nSize != nOutCnt)
	{
		for(x = nSize-1 ; x >=0  ; x--)
		{
			if(bValue[nPos + x])	// 첫 데이터를 찾는다.
			{
				nEndIdx = x;
				break;
			}
		}

		
	}

	if(nStartIdx != -1 && nEndIdx != -1)
	{
		//printf("S : %d\t, E : %d\n", nStartIdx, nEndIdx );
		for(unsigned int s = nStartIdx; s <= nEndIdx ; s++)
		{
			
			bResult[nPos + s] = 1;
			//pParent->ReplaceFileInfo(s,y,z,1);
		}
	}


	//c[tid] = a[tid] + b[tid];
} 


void CGPUSolid::CUDA_SolidVoxelizationX(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelXResult)
{
	//m_nMaxBlockSize = 15;
	//m_nMaxThreadSize = 16;

	const unsigned int unOneThreadSize = m_nXFileVoxCnt;
	const unsigned int unBlockSize = m_nYFileVoxCnt * m_nZFileVoxCnt;
	unsigned int unOneStep  = m_nMaxBlockSize * unOneThreadSize;
	if(unOneStep > unBlockSize * unOneThreadSize)
		unOneStep = unBlockSize * unOneThreadSize;
			
	unsigned int unStep = 0;

	while (unStep < unVoxelSize)
	{
		unsigned int unCpySize = 0;
		//! 디바이스에게 메모리 카피
		if(unVoxelSize - unStep < unOneStep)
		{
			printf("OH!");
			unCpySize = unVoxelSize - unStep;
		}
		else
			unCpySize = unOneStep;

		bool *cubVoxelState;
		bool *cubVoxelResultSize;

		cudaMalloc(&cubVoxelState, unCpySize*sizeof(bool));
		cudaMalloc(&cubVoxelResultSize, unCpySize*sizeof(bool));

		//! 디바이스에게 메모리 카피
		cudaMemcpy(cubVoxelState, bVoxelState + unStep, unCpySize*sizeof(bool), cudaMemcpyHostToDevice);
		
		UseCheckX<<<unCpySize / unOneThreadSize,1>>>(cubVoxelState, cubVoxelResultSize, unOneThreadSize);


		cudaMemcpy(bVoxelXResult + unStep, cubVoxelResultSize, unCpySize*sizeof(bool), cudaMemcpyDeviceToHost);

		unStep += unCpySize;

		cudaFree(cubVoxelState);
		cudaFree(cubVoxelResultSize);

	}

	//return true;
}


__global__ void UseCheckY(bool *bValue, bool *bResult, int nX, int nY, int nZ, int unBlockStep)
{ 
	int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int nDiv = 0;
	int nNmg = 0;
	int nStad = nX;

	int nPos = 0;
	if((unBlockStep + tid) > 0)
	{
		nDiv = (unBlockStep + tid) / nStad;
		nNmg = (unBlockStep + tid) % nStad;
	}
	//nPos = tid + nDiv * (nStad * nY)  + nNmg;
	nPos = nDiv * (nStad * nY)  + nNmg;

	int nOutCnt = 0;
	int nStartIdx = -1, nEndIdx =-1;
	int nIdx = 0;
	for(nIdx = 0 ; nIdx < nY ; nIdx++)
	{
		//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\ttid : %02d\tPos : %03d\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos + (nIdx * nX));
		//printf("tid : %02d,\tPos : %02d,\tIdx*nStad : %02d,\tIdx %02d\n", tid, nPos, (nIdx * nStad), nPos + (nIdx * nStad));
		if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
		{
			nStartIdx = nIdx * nStad;
			break;
		}
		else
			nOutCnt++;
	}

	if(nY != nOutCnt)
	{
		for(nIdx = nY-1 ; nIdx >=0  ; nIdx--)
		{
			//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\ttid : %02d\tPos : %03d\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos + (nIdx * nX));

			//printf("tid : %02d,\tPos : %02d,\tIdx*nStad : %02d,\tIdx %02d\n", tid, nPos, (nIdx * nStad), nPos + (nIdx * nStad));
			if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
			{
				nEndIdx = nIdx * nStad;
				break;
			}
		}
	}

	if(nStartIdx != -1 && nEndIdx != -1)
	{
		//printf("tid: %03d,\t S : %d\t, E : %d\t, OutCnt : %d\n", tid, nStartIdx, nEndIdx, nOutCnt );
		for(unsigned int s = nStartIdx; s <= nEndIdx ; s= s + nStad)
		{
			//printf("%d\t", s);
			bResult[nPos + s] = 1;
			//pParent->ReplaceFileInfo(s,y,z,1);
		}
	}
	

	
} 

void CGPUSolid::CUDA_SolidVoxelizationY(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelYResult)
{
	//m_nMaxBlockSize = 15;
	//m_nMaxThreadSize = 16;

	const unsigned int unOneThreadSize = m_nYFileVoxCnt;
	const unsigned int unMaxBlockCnt = m_nMaxBlockSize;
	unsigned int unOneStepBlcokCnt  = m_nXFileVoxCnt * m_nZFileVoxCnt;
	if(unOneStepBlcokCnt > unMaxBlockCnt)
	{
		unOneStepBlcokCnt = unMaxBlockCnt;
	}
			
	const unsigned int unMaxBlockSize = unVoxelSize / m_nYFileVoxCnt;

	unsigned int unBlockStep = 0;

	bool *cubVoxelState;
	bool *cubVoxelResultSize;

	if ( cudaSuccess != cudaMalloc(&cubVoxelState, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Y VoxelState Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultSize, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Y VoxelResult Malloc \n" );
	}

	//! 디바이스에게 메모리 카피
	cudaMemcpy(cubVoxelState, bVoxelState, unVoxelSize*sizeof(bool), cudaMemcpyHostToDevice);

	while (unBlockStep <  unMaxBlockSize)
	{
		if(unOneStepBlcokCnt > unMaxBlockSize - unBlockStep)
			unOneStepBlcokCnt = unMaxBlockSize - unBlockStep;
		UseCheckY<<<unOneStepBlcokCnt,1>>>(cubVoxelState, cubVoxelResultSize, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, unBlockStep);
		if ( cudaSuccess != cudaGetLastError() )
		{
			printf( "Error! Y BlockStep : %d \n", unBlockStep);
		}
		unBlockStep += unOneStepBlcokCnt;
		
	}
	
	if ( cudaSuccess != cudaMemcpy(bVoxelYResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
	{
		printf( "Error! Y VoxelState Memcpy \n" );
	}

	cudaFree(cubVoxelState);
	cudaFree(cubVoxelResultSize);
}


__global__ void UseCheckZ(bool *bValue, bool *bResult, int nX, int nY, int nZ, int unBlockStep)
{ 
	int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	int nDiv = 0;
	int nNmg = 0;
	int nStad = nX*nY;

	int nPos = 0;
	if((unBlockStep + tid) > 0)
	{
		nDiv = (unBlockStep + tid) / nStad;
		nNmg = (unBlockStep + tid) % nStad;
	}
	//nPos = tid + nDiv * (nStad * nZ)  + nNmg;
	nPos = nDiv * (nStad * nZ)  + nNmg;

	int nOutCnt = 0;
	int nStartIdx = -1, nEndIdx =-1;
	int nIdx = 0;
	for(nIdx = 0 ; nIdx < nZ ; nIdx++)
	{
		//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\ttid : %02d\tPos : %03d\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos + (nIdx * (nX*nY)));
		//printf("tid : %02d,\tPos : %02d,\tIdx*nStad : %02d,\tIdx %02d\n", tid, nPos, (nIdx * nStad), nPos + (nIdx * nStad));
		if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
		{
			nStartIdx = nIdx * nStad;
			break;
		}
		else
			nOutCnt++;
	}

	if(nZ != nOutCnt)
	{
		for(nIdx = nZ-1 ; nIdx >=0  ; nIdx--)
		{
			//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\ttid : %02d\tPos : %03d\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos + (nIdx * (nX*nY)));

			//printf("tid : %02d,\tPos : %02d,\tIdx*nStad : %02d,\tIdx %02d\n", tid, nPos, (nIdx * nStad), nPos + (nIdx * nStad));
			if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
			{
				nEndIdx = nIdx * nStad;
				break;
			}
		}
	}

	//printf("tid: %03d,\t S : %d\t, E : %d\t, OutCnt : %d\n", tid, nStartIdx, nEndIdx, nOutCnt );
	if(nStartIdx != -1 && nEndIdx != -1)
	{
		//printf("tid: %03d,\t S : %d\t, E : %d\t, OutCnt : %d\n", tid, nStartIdx, nEndIdx, nOutCnt );
		for(unsigned int s = nStartIdx; s <= nEndIdx ; s= s + nStad)
		{
			//printf("%d\t", s);
			bResult[nPos + s] = 1;
			//pParent->ReplaceFileInfo(s,y,z,1);
		}
	}

	
} 

void CGPUSolid::CUDA_SolidVoxelizationZ(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelZResult)
{
	//m_nMaxBlockSize = 15;
	//m_nMaxThreadSize = 16;

	const unsigned int unOneThreadSize = m_nZFileVoxCnt;
	const unsigned int unMaxBlockCnt = m_nMaxBlockSize;	
	unsigned int unOneStepBlcokCnt  = m_nXFileVoxCnt * m_nYFileVoxCnt;
	//unsigned int unOneStepBlcokCnt  = 1;
	if(unOneStepBlcokCnt > unMaxBlockCnt)
	{
		unOneStepBlcokCnt = unMaxBlockCnt;
	}

	const unsigned int unMaxBlockSize = unVoxelSize / m_nZFileVoxCnt;

	unsigned int unBlockStep = 0;

	bool *cubVoxelState;
	bool *cubVoxelResultSize;

	if ( cudaSuccess != cudaMalloc(&cubVoxelState, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Z VoxelState Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultSize, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Z VoxelResult Malloc \n" );
	}

	//! 디바이스에게 메모리 카피
	cudaMemcpy(cubVoxelState, bVoxelState, unVoxelSize*sizeof(bool), cudaMemcpyHostToDevice);

	while (unBlockStep <  unMaxBlockSize)
	{
		if(unOneStepBlcokCnt > unMaxBlockSize - unBlockStep)
			unOneStepBlcokCnt = unMaxBlockSize - unBlockStep;
		UseCheckZ<<<unOneStepBlcokCnt,1>>>(cubVoxelState, cubVoxelResultSize, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, unBlockStep);
		if ( cudaSuccess != cudaGetLastError() )
		{
			printf( "Error! Z BlockStep : %d \n", unBlockStep);
		}
		unBlockStep += unOneStepBlcokCnt;
	}
	


	if ( cudaSuccess != cudaMemcpy(bVoxelZResult, cubVoxelResultSize, unVoxelSize*sizeof(bool)*sizeof(bool), cudaMemcpyDeviceToHost))
	{
		printf( "Error! Z VoxelState Memcpy \n" );
	}


	cudaFree(cubVoxelState);
	cudaFree(cubVoxelResultSize);
}


void CGPUSolid::CUDA_SolidVoxelizationZTest(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelZResult, unsigned int unBlockStep)
{
	//m_nMaxBlockSize = 15;
	//m_nMaxThreadSize = 16;

	const unsigned int unMaxBlockCnt = m_nMaxBlockSize;	
	//unsigned int unOneStepBlcokCnt  = m_nXFileVoxCnt * m_nYFileVoxCnt;
	unsigned int unOneStepBlcokCnt  = 1;
	if(unOneStepBlcokCnt > unMaxBlockCnt)
	{
		unOneStepBlcokCnt = unMaxBlockCnt;
	}

	const unsigned int unMaxBlockSize = unVoxelSize / m_nZFileVoxCnt;


	bool *cubVoxelState;
	bool *cubVoxelResultSize;

	if ( cudaSuccess != cudaMalloc(&cubVoxelState, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Z VoxelState Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultSize, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Z VoxelResult Malloc \n" );
	}

	//! 디바이스에게 메모리 카피
	cudaMemcpy(cubVoxelState, bVoxelState, unVoxelSize*sizeof(bool), cudaMemcpyHostToDevice);
	cudaMemcpy(cubVoxelResultSize, bVoxelZResult, unVoxelSize*sizeof(bool), cudaMemcpyHostToDevice);

	//while (unBlockStep <  unMaxBlockSize)
	//{
		//if(unOneStepBlcokCnt > unMaxBlockSize - unBlockStep)
		//	unOneStepBlcokCnt = unMaxBlockSize - unBlockStep;
		UseCheckZ<<<unOneStepBlcokCnt,1>>>(cubVoxelState, cubVoxelResultSize, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, unBlockStep);
		if ( cudaSuccess != cudaGetLastError() )
		{
			printf( "Error! Z BlockStep : %d \n", unBlockStep);
		}
	//unBlockStep += unOneStepBlcokCnt;
	//}
	

	


	if ( cudaSuccess != cudaMemcpy(bVoxelZResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
	{
		printf( "Error! Z VoxelState Memcpy \n" );
	}

	//for (int v= 0; v < unVoxelSize; v ++)
	//{
	//	printf("%d\n", bVoxelZResult[v]);
	//}


	cudaFree(cubVoxelState);
	cudaFree(cubVoxelResultSize);
}

__global__ void MergeXYZBig(bool *bVoxelStateX, bool *bVoxelStateY, bool *bVoxelStateZ, bool *bVoxelStateTotal, unsigned int unOneStep)
{ 
	//printf("gogogog\n");
	unsigned int  tid = blockDim.x * blockIdx.x + threadIdx.x;// + unStep;
	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\tthreadIdx.y :%d\t tid : %02d\n", blockIdx.x, blockDim.x, threadIdx.x, threadIdx.y,tid);
	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\t tid : %02ld\n", blockIdx.x, blockDim.x, threadIdx.x, tid);


	if(unOneStep > tid)
	{
		//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\tthreadIdx.y :%d\t tid : %02d\n", blockIdx.x, blockDim.x, threadIdx.x, threadIdx.y,tid);

		/*__shared__ bool b[3];
		b[0] = bVoxelStateX[tid];
		b[1] = bVoxelStateY[tid];
		b[2] = bVoxelStateZ[tid];*/

		//if(bVoxelStateX[tid] && bVoxelStateY[tid] && bVoxelStateZ[tid])
		//	bVoxelStateTotal[tid] = true;

		bVoxelStateTotal[tid] = bVoxelStateX[tid] & bVoxelStateY[tid] & bVoxelStateZ[tid];


		//if(bVoxelStateX[tid] && bVoxelStateY[tid] && bVoxelStateZ[tid])
		//	bVoxelStateTotal[tid] = true;

	}
	
	


} 

void CGPUSolid::CUDA_SolidVoxelizationTotal(unsigned int  unVoxelSize, bool *bVoxelStateX, bool *bVoxelStateY, bool *bVoxelStateZ, bool *bVoxelStateTotal)
{
	//m_nMaxBlockSize = 32;
	//m_nMaxThreadSize = 32;

	unsigned int unOneStep  = m_nMaxBlockSize * m_nMaxThreadSize;
	if(unOneStep > unVoxelSize)
		unOneStep = unVoxelSize;
			
	unsigned int unStep = 0;
	unsigned int unIdx = 0;

	while (unStep < unVoxelSize)
	{
		unsigned int unCpySize = 0;
		//! 디바이스에게 메모리 카피
		if(unVoxelSize - unStep < unOneStep)
			unCpySize = unVoxelSize - unStep;
		else
			unCpySize = unOneStep;

		bool *cubVoxelStateX;
		bool *cubVoxelStateY, 
		bool *cubVoxelStateZ;
		bool *cubVoxelResultSize;
		cudaMalloc(&cubVoxelStateX, unCpySize*sizeof(bool));
		cudaMalloc(&cubVoxelStateY, unCpySize*sizeof(bool));
		cudaMalloc(&cubVoxelStateZ, unCpySize*sizeof(bool));
		cudaMalloc(&cubVoxelResultSize, unCpySize*sizeof(bool));

		

		cudaMemcpy(cubVoxelStateX, bVoxelStateX + (unOneStep*unIdx), unCpySize*sizeof(bool), cudaMemcpyHostToDevice);
		cudaMemcpy(cubVoxelStateY, bVoxelStateY + (unOneStep*unIdx), unCpySize*sizeof(bool), cudaMemcpyHostToDevice);
		cudaMemcpy(cubVoxelStateZ, bVoxelStateZ + (unOneStep*unIdx), unCpySize*sizeof(bool), cudaMemcpyHostToDevice);
		
		//printf("%d\n", unStep);
		MergeXYZBig<<<m_nMaxBlockSize,m_nMaxThreadSize>>>(cubVoxelStateX, cubVoxelStateY, cubVoxelStateZ, cubVoxelResultSize, unCpySize);

		cudaMemcpy(bVoxelStateTotal + (unOneStep*unIdx), cubVoxelResultSize, unCpySize*sizeof(bool), cudaMemcpyDeviceToHost);

		unStep += unCpySize;
		unIdx += 1;

		cudaFree(cubVoxelStateX);
		cudaFree(cubVoxelStateY);
		cudaFree(cubVoxelStateZ);
		cudaFree(cubVoxelResultSize);
	}

	



	//return true;
}




__global__ void UseCheckXYZ(bool *bValue, bool *bResultX, bool *bResultY, bool *bResultZ, int nX, int nY, int nZ, int unBlockStep)
{ 
	unsigned int  tid = blockDim.x * blockIdx.x / 3;/* + threadIdx.x;*/// + unStep;

	int nDiv = 0;
	int nNmg = 0;
	//! 각 축별로 계산시 첫 IDx + ?? 의 기준..?
	int nStad = 0;
	//! 계산 할 축의 개수를 넣어줌
	int nX_Y_Z = 0;
	bool *bResult = NULL;

	if(threadIdx.x == 0) // X축
	{
		//nStad = nX;
		nStad = 1;
		nX_Y_Z = nX;
		bResult = bResultX;

		if(nY * nZ <= tid)
			return;
	}
	else if(threadIdx.x == 1) // Y축
	{
		nStad = nX;
		nX_Y_Z = nY;
		bResult = bResultY;

		if(nX * nZ <= tid)
			return;
	}
	else //if(threadIdx.x == 2) Z축
	{
		nStad = nX*nY;
		nX_Y_Z = nZ;
		bResult = bResultZ;
		
		if(nX * nY <= tid)
			return;
	}


	//! 아래는 계산..
	if((unBlockStep + tid) > 0)
	{
		nDiv = (unBlockStep + tid) / nStad;
		nNmg = (unBlockStep + tid) % nStad;
	}
	int nPos = 0;
	nPos = nDiv * (nStad * nX_Y_Z)  + nNmg;


	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\t tid : %02d\tPos : %02d\t", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos);
	//printf("nAxis : %02d\tnStad : %02d\tnX_Y_Z : %02d\t nDiv : %02d\tnNmg : %02d\tPos : %02d\t\n", threadIdx.x, nStad, nX_Y_Z, nDiv, nNmg, nPos);

	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\t tid : %02d\tPos : %02d\tnAxis : %02d\tnStad : %02d\tnX_Y_Z : %02d\t nDiv : %02d\tnNmg : %02d\tPos : %02d\t\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos, threadIdx.x, nStad, nX_Y_Z, nDiv, nNmg, nPos);

	//return;

	//printf("threadIdx.x : %02d\ttid : %02ld\n", threadIdx.x, tid);
	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\ttid : %02d\tPos : %03d\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos);
	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\t tid : %02ld\n", blockIdx.x, blockDim.x, threadIdx.x, tid);
	//return;


	int nOutCnt = 0;
	int nStartIdx = -1, nEndIdx =-1;
	int nIdx = 0;
	for(nIdx = 0 ; nIdx < nX_Y_Z ; nIdx++)
	{
		if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
		{
			nStartIdx = nIdx * nStad;
			break;
		}
		else
			nOutCnt++;
	}

	if(nX_Y_Z != nOutCnt)
	{
		for(nIdx = nX_Y_Z-1 ; nIdx >=0  ; nIdx--)
		{
			if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
			{
				nEndIdx = nIdx * nStad;
				break;
			}
		}
	}

	if(nStartIdx != -1 && nEndIdx != -1)
	{
		//printf("S : %d\t, E : %d\n", nStartIdx, nEndIdx );
		for(unsigned int s = nStartIdx; s <= nEndIdx ; s= s + nStad)
		{
			bResult[nPos + s] = 1;
		}
	}
}



void CGPUSolid::CUDA_SolidVoxelizationXYZ(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelResultTotal)
{
	const unsigned int unMaxBlockCnt = m_nMaxBlockSize;

	const unsigned int unOneThreadSizeX = m_nXFileVoxCnt;
	const unsigned int unOneStepBlcokCntX  = m_nYFileVoxCnt * m_nZFileVoxCnt;

	const unsigned int unOneThreadSizeY = m_nYFileVoxCnt;
	const unsigned int unOneStepBlcokCntY  = m_nXFileVoxCnt * m_nZFileVoxCnt;

	const unsigned int unOneThreadSizeZ = m_nZFileVoxCnt;
	const unsigned int unOneStepBlcokCntZ  = m_nXFileVoxCnt * m_nYFileVoxCnt;


	//! 각 축별로 가장 큰 블록을 기준으로 처리한다.
	unsigned int unMaxBlockSize = 0;
	unMaxBlockSize = unOneStepBlcokCntX;
	if(unMaxBlockSize < unOneStepBlcokCntY)
	{
		unMaxBlockSize = unOneStepBlcokCntY;
		printf( "Big Y \n" );
	}
	if(unMaxBlockSize < unOneStepBlcokCntZ)
	{
		unMaxBlockSize = unOneStepBlcokCntZ;
		printf( "Big Z \n" );
	}

	//! 만들수 있는 최대 블록 수를 넘으면 최대 블록수만큼만 처리하기위해
	unsigned int unOneStepBlcokCnt  = unMaxBlockSize;
	if(unOneStepBlcokCnt > unMaxBlockCnt)
		unOneStepBlcokCnt = unMaxBlockCnt;



	////////////////////////////////////////////////////////////////////////////////////
	//! 메모리 할당
	bool *cubVoxelState;
	bool *cubVoxelResultX;
	bool *cubVoxelResultY;
	bool *cubVoxelResultZ;

	if ( cudaSuccess != cudaMalloc(&cubVoxelState, unVoxelSize*sizeof(bool)))
	{
		printf( "Error!  VoxelState Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultX, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! X VoxelResult Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultY, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! X VoxelResult Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultZ, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! X VoxelResult Malloc \n" );
	}
	cudaMemcpy(cubVoxelState, bVoxelState, unVoxelSize*sizeof(bool), cudaMemcpyHostToDevice);

	////////////////////////////////////////////////////////////////////////////////////


	//현재 처리된 블록의 스탭
	unsigned int unBlockStep = 0;
	////////////////////////////////////////////////////////////////////////////////////
	// ! X, Y, Z Inner Voxel 생성!!
	while (unBlockStep <  unMaxBlockSize)
	{
		if(unOneStepBlcokCnt > unMaxBlockSize - unBlockStep)
			unOneStepBlcokCnt = unMaxBlockSize - unBlockStep;

		UseCheckXYZ<<<unOneStepBlcokCnt,3>>>(cubVoxelState, cubVoxelResultX, cubVoxelResultY, cubVoxelResultZ, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, unBlockStep);
		if ( cudaSuccess != cudaGetLastError() )
		{
			printf( "Error! BlockStep : %d \n", unBlockStep);
		}
		unBlockStep += unOneStepBlcokCnt;
	}


	printf( "\n\n\n");
	
	
	// 임시
	//cudaMemcpy(bVoxelResultTotal, cubVoxelResultZ, unVoxelSize, cudaMemcpyDeviceToHost);

	//! 필요 없는 메모리는 삭제후 토탈 영역을 메모리 할당해준다.
	cudaFree(cubVoxelState);


	//return ;

	////////////////////////////////////////////////////////////////////////////////////
	//! Total 영역을 계산해준다
	unsigned int unOneStep  = m_nMaxBlockSize * m_nMaxThreadSize;
	if(unOneStep > unVoxelSize)
		unOneStep = unVoxelSize;

	unsigned int unIdx = 0;
	unsigned int unStep = 0;
	while (unStep < unVoxelSize)
	{
		unsigned int unCpySize = 0;
		//! 디바이스에게 메모리 카피
		if(unVoxelSize - unStep < unOneStep)
			unCpySize = unVoxelSize - unStep;
		else
			unCpySize = unOneStep;

		bool *cubVoxelTotalResult;
		if ( cudaSuccess != cudaMalloc(&cubVoxelTotalResult, unCpySize*sizeof(bool)))
		{
			printf( "Error!  VoxelTotal Malloc \n" );
		}
		MergeXYZBig<<<m_nMaxBlockSize,m_nMaxThreadSize>>>(cubVoxelResultX, cubVoxelResultY, cubVoxelResultZ, cubVoxelTotalResult, unCpySize);

		cudaMemcpy(bVoxelResultTotal + (unOneStep*unIdx), cubVoxelTotalResult, unCpySize*sizeof(bool), cudaMemcpyDeviceToHost);
		cudaFree(cubVoxelTotalResult);

		unStep += unCpySize;
		unIdx += 1;
	}
	
	printf( "\n\n\n");

	
	cudaFree(cubVoxelResultX);
	cudaFree(cubVoxelResultY);
	cudaFree(cubVoxelResultZ);
	

}



__global__ void UseCheckXYZBlock(bool *bValue, bool *bResultX, bool *bResultY, bool *bResultZ, int nX, int nY, int nZ, int unBlockStep)
{ 
	unsigned int  tid = blockDim.x * blockIdx.x + threadIdx.x;// + unStep;

	

	//! 블록별 3의 배수로 만들어서 처리 하기 때문
	int nAxis = blockIdx.x % 3;
	int nAxisDiv = blockIdx.x / 3;

	////////////////////////////////////////////////
	//! 블록별로 축이 같아야하기때문 blockDim.x = 32 임... 워프개수
	tid = tid - (blockDim.x * blockIdx.x) + (nAxisDiv * blockDim.x);


	int nDiv = 0;
	int nNmg = 0;
	//! 각 축별로 계산시 첫 IDx + ?? 의 기준..?
	int nStad = 0;
	//! 계산 할 축의 개수를 넣어줌
	int nX_Y_Z = 0;
	bool *bResult = NULL;

	if(nAxis == 0) // X축
	{
		//nStad = nX;
		nStad = 1;
		nX_Y_Z = nX;
		bResult = bResultX;

		if(nY * nZ <= tid)
			return;
	}
	else if(nAxis == 1) // Y축
	{
		nStad = nX;
		nX_Y_Z = nY;
		bResult = bResultY;

		if(nX * nZ <= tid)
			return;
	}
	else //if(nAxis == 2) Z축
	{
		nStad = nX*nY;
		nX_Y_Z = nZ;
		bResult = bResultZ;
		
		if(nX * nY <= tid)
			return;

	}

	//! 아래는 계산..
	if((unBlockStep + tid) > 0)
	{
		nDiv = (unBlockStep + tid) / nStad;
		nNmg = (unBlockStep + tid) % nStad;
	}



	int nPos = 0;
	nPos = nDiv * (nStad * nX_Y_Z)  + nNmg;

	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\t tid : %02d\tPos : %02d\t\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos);
	//printf("nAxis : %02d\tnStad : %02d\tnX_Y_Z : %02d\t nDiv : %02d\tnNmg : %02d\tPos : %02d\t\n", nAxis, nStad, nX_Y_Z, nDiv, nNmg, nPos);
	//printf("blockIdx.x : %02d\tblockDim.x : %02d\tthreadIdx.x : %02d\t tid : %02d\tPos : %02d\tnAxis : %02d\tnStad : %02d\tnX_Y_Z : %02d\t nDiv : %02d\tnNmg : %02d\tPos : %02d\t\n", blockIdx.x, blockDim.x, threadIdx.x, tid, nPos, nAxis, nStad, nX_Y_Z, nDiv, nNmg, nPos);

	//return;

	int nOutCnt = 0;
	int nStartIdx = -1, nEndIdx =-1;
	int nIdx = 0;
	for(nIdx = 0 ; nIdx < nX_Y_Z ; nIdx++)
	{
		if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
		{
			nStartIdx = nIdx * nStad;
			break;
		}
		else
			nOutCnt++;
	}

	if(nX_Y_Z != nOutCnt)
	{
		for(nIdx = nX_Y_Z-1 ; nIdx >=0  ; nIdx--)
		{
			if(bValue[nPos + (nIdx * nStad)])	// 첫 데이터를 찾는다.
			{
				nEndIdx = nIdx * nStad;
				break;
			}
		}
	}

	if(nStartIdx != -1 && nEndIdx != -1)
	{
		//printf("S : %d\t, E : %d\n", nStartIdx, nEndIdx );
		for(unsigned int s = nStartIdx; s <= nEndIdx ; s= s + nStad)
		{
			bResult[nPos + s] = 1;
		}
	}
}



void CGPUSolid::CUDA_SolidVoxelizationXYZBlock(unsigned int  unVoxelSize, bool *bVoxelState, bool *bVoxelResultTotal)
{
	const unsigned int unMaxBlockCnt = m_nMaxBlockSize;

	const unsigned int unOneThreadSizeX = m_nXFileVoxCnt;
	const unsigned int unOneStepBlcokCntX  = m_nYFileVoxCnt * m_nZFileVoxCnt;

	const unsigned int unOneThreadSizeY = m_nYFileVoxCnt;
	const unsigned int unOneStepBlcokCntY  = m_nXFileVoxCnt * m_nZFileVoxCnt;

	const unsigned int unOneThreadSizeZ = m_nZFileVoxCnt;
	const unsigned int unOneStepBlcokCntZ  = m_nXFileVoxCnt * m_nYFileVoxCnt;


	//! 각 축별로 가장 큰 블록을 기준으로 처리한다.
	unsigned int unMaxBlockSize = 0;
	unMaxBlockSize = unOneStepBlcokCntX;
	if(unMaxBlockSize < unOneStepBlcokCntY)
	{
		unMaxBlockSize = unOneStepBlcokCntY;
		printf( "Big Y \n" );
	}
	if(unMaxBlockSize < unOneStepBlcokCntZ)
	{
		unMaxBlockSize = unOneStepBlcokCntZ;
		printf( "Big Z \n" );
	}
	
	//! 축이 3개니깐!
	unMaxBlockSize = ((unMaxBlockSize / 32) + 1) * 3;
	//unMaxBlockSize = ((unMaxBlockSize / 32) + 1) * 3;

	//! 만들수 있는 최대 블록 수를 넘으면 최대 블록수만큼만 처리하기위해 -> +1은 마지막도 처리를 해주어야해서..
	unsigned int unOneStepBlcokCnt  = unMaxBlockSize;
	if(unOneStepBlcokCnt > unMaxBlockCnt)
		unOneStepBlcokCnt = unMaxBlockCnt;



	////////////////////////////////////////////////////////////////////////////////////
	//! 메모리 할당
	bool *cubVoxelState;
	bool *cubVoxelResultX;
	bool *cubVoxelResultY;
	bool *cubVoxelResultZ;

	if ( cudaSuccess != cudaMalloc(&cubVoxelState, unVoxelSize*sizeof(bool)))
	{
		printf( "Error!  VoxelState Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultX, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! X VoxelResult Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultY, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Y VoxelResult Malloc \n" );
	}
	if ( cudaSuccess != cudaMalloc(&cubVoxelResultZ, unVoxelSize*sizeof(bool)))
	{
		printf( "Error! Z VoxelResult Malloc \n" );
	}
	cudaMemcpy(cubVoxelState, bVoxelState, unVoxelSize*sizeof(bool), cudaMemcpyHostToDevice);

	////////////////////////////////////////////////////////////////////////////////////
	//현재 처리된 블록의 스탭
	unsigned int unBlockStep = 0;
	////////////////////////////////////////////////////////////////////////////////////
	// ! X, Y, Z Inner Voxel 생성!!
	while (unBlockStep <  unMaxBlockSize)
	{
		if(unOneStepBlcokCnt > unMaxBlockSize - unBlockStep)
			unOneStepBlcokCnt = unMaxBlockSize - unBlockStep;

		UseCheckXYZBlock<<<unOneStepBlcokCnt,32>>>(cubVoxelState, cubVoxelResultX, cubVoxelResultY, cubVoxelResultZ, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, unBlockStep);
		if ( cudaSuccess != cudaGetLastError() )
		{
			printf( "Error! BlockStep : %d \n", unBlockStep);
		}
		unBlockStep += unOneStepBlcokCnt;
	}


	// 임시
//	cudaMemcpy(bVoxelResultTotal, cubVoxelResultZ, unVoxelSize, cudaMemcpyDeviceToHost);


	//! 필요 없는 메모리는 삭제후 토탈 영역을 메모리 할당해준다.
	cudaFree(cubVoxelState);


	//return ;

	////////////////////////////////////////////////////////////////////////////////////
	//! Total 영역을 계산해준다
	unsigned int unOneStep  = m_nMaxBlockSize * m_nMaxThreadSize;
	if(unOneStep > unVoxelSize)
		unOneStep = unVoxelSize;

	unsigned int unIdx = 0;
	unsigned int unStep = 0;
	while (unStep < unVoxelSize)
	{
		unsigned int unCpySize = 0;
		//! 디바이스에게 메모리 카피
		if(unVoxelSize - unStep < unOneStep)
			unCpySize = unVoxelSize - unStep;
		else
			unCpySize = unOneStep;

		bool *cubVoxelTotalResult = NULL;
		if ( cudaSuccess != cudaMalloc(&cubVoxelTotalResult, unCpySize*sizeof(bool)))
		{
			printf( "Error!  VoxelTotal Malloc \n" );
		}
		MergeXYZBig<<<m_nMaxBlockSize,m_nMaxThreadSize>>>(cubVoxelResultX, cubVoxelResultY, cubVoxelResultZ, cubVoxelTotalResult, unCpySize);

		cudaMemcpy(bVoxelResultTotal + (unOneStep*unIdx), cubVoxelTotalResult, unCpySize*sizeof(bool), cudaMemcpyDeviceToHost);
		cudaFree(cubVoxelTotalResult);

		unStep += unCpySize;
		unIdx += 1;
	}
	
	
	cudaFree(cubVoxelResultX);
	cudaFree(cubVoxelResultY);
	cudaFree(cubVoxelResultZ);
	

}

