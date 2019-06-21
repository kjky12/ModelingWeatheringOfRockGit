#include "GPUSurface.cuh"
#include "cuda.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


#define SIZE 1024

CGPUSurface::CGPUSurface(void)
{
	//! 해상도 나누는 개수
	g_nDivideCnt = 0;
	//! 선택된 축
	g_nSelectAxis = 0;
	//! 복셀 크기 인듯
	g_fRectSize = 0.0;
	////////////////////////////////////////
	//Object 복셀화 변수
	m_nUseVertex = 0;
	memset(vertexes, NULL, sizeof(sVertexCuda) * dfMAX_VERTEX_CNT);

	m_nUsePart = 0;
	memset(parts, NULL, sizeof(sPartCuda) * dfMAX_PART_CNT);

	m_fObjMaxX = 0.0;
	m_fObjMaxY = 0.0;
	m_fObjMaxZ = 0.0;
	m_fObjMinX = 0.0;
	m_fObjMinY = 0.0;
	m_fObjMinZ = 0.0;

	//////////////////////////////////////////
	// etc
	m_nXFileVoxCnt = 0;
	m_nYFileVoxCnt = 0;
	m_nZFileVoxCnt = 0;

}

CGPUSurface::~CGPUSurface(void)
{

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 표면 복셀

void CGPUSurface::SetGlobalValue(int		nDivideCnt, int		nSelectAxis, float	fRectSize)
{
	g_nDivideCnt = nDivideCnt;
	g_nSelectAxis = nSelectAxis;
	g_fRectSize = fRectSize;

}


void CGPUSurface::SetObjectValue(float fObjMaxX, float fObjMaxY,float fObjMaxZ,	float fObjMinX,	float fObjMinY,	float fObjMinZ,
		int nVretexesSize , char *pVertex, int nPartSize, char *pParts)
{
	m_fObjMaxX = fObjMaxX;
	m_fObjMaxY = fObjMaxY;
	m_fObjMaxZ = fObjMaxZ;
	m_fObjMinX = fObjMinX;
	m_fObjMinY = fObjMinY;
	m_fObjMinZ = fObjMinZ;

	m_nUseVertex = nVretexesSize;
	memcpy(vertexes, pVertex, sizeof(sVertexCuda) * nVretexesSize);

	m_nUsePart = nPartSize;
	memcpy(parts, pParts, sizeof(sPartCuda) * nPartSize);
}


void CGPUSurface::SetGlobalEtc(unsigned long long nXFileVoxCnt,	unsigned long long nYFileVoxCnt,	unsigned long long nZFileVoxCnt)
{
	m_nXFileVoxCnt = nXFileVoxCnt;
	m_nYFileVoxCnt = nYFileVoxCnt;
	m_nZFileVoxCnt = nZFileVoxCnt;
}



__global__ void CreateFaceArea(sFaceCuda* sFaceData, sVertexCuda* sVertexData, sPartDataCuda*	cusPartDataResult)
{ 
	int tid = blockIdx.x * blockDim.x+ threadIdx.x;	
	
	printf("blockIdx : %d\t blockDim : %d\t threadIdx : %d\t tid : %d\n", blockIdx.x, blockDim.x, threadIdx.x, tid);

	for (int j=0,jn= sFaceData[tid].n; j<jn; ++j) 
	{
		int &v  = sFaceData[tid].v[j];
		//if (0 < v && v <= vt_size) 
		{
			sVertexCuda &vt = sVertexData[v - 1];

			//triverts[j][0] = vt.x;
			//triverts[j][1] = vt.y;
			//triverts[j][2] = vt.z;

			//if(PartData.fMaxX < vt.x)
			//	PartData.fMaxX = vt.x;
			//if(PartData.fMaxY < vt.y)
			//	PartData.fMaxY = vt.y;
			//if(PartData.fMaxZ < vt.z)
			//	PartData.fMaxZ = vt.z;

			//if(PartData.fMinX > vt.x)
			//	PartData.fMinX = vt.x;
			//if(PartData.fMinY > vt.y)
			//	PartData.fMinY = vt.y;
			//if(PartData.fMinZ > vt.z)
			//	PartData.fMinZ = vt.z;

			//nPartCnt++;
		}

		//TRACE("\tv:%d\t", v);

	}

	sFaceData[tid].v[0];
	sFaceData[tid].v[1];
	sFaceData[tid].v[2];
	sFaceData[tid].v[3];
	sFaceData[tid].v[4];


}

void CGPUSurface::SetSurfaceVoxelization()
{
	m_nUsePart;
	parts[0].nUseFaceCnt;

	for(int nPart = 0; nPart < m_nUsePart; nPart++)
	{
		sFaceCuda* cusFaceData = NULL;
		sVertexCuda* cusVertexData = NULL;

		sPartDataCuda*	cusPartDataCuda = NULL;

		if ( cudaSuccess != cudaMalloc(&cusFaceData, parts[nPart].nUseFaceCnt*sizeof(sFaceCuda) ) )
		{
			printf( "Error! Face MemCpy \n" );
		}
		if ( cudaSuccess != cudaMalloc(&cusVertexData, m_nUseVertex*sizeof(sVertexCuda) ) )
		{
			printf( "Error! Vertex MemCpy \n" );
		}
		if ( cudaSuccess != cudaMalloc(&cusPartDataCuda, parts[nPart].nUseFaceCnt*sizeof(sPartDataCuda) ) )
		{
			printf( "Error! Face MemCpy \n" );
		}
		

		cudaMemcpy(cusFaceData, &parts[nPart], parts[nPart].nUseFaceCnt*sizeof(sFaceCuda), cudaMemcpyHostToDevice);
		cudaMemcpy(cusVertexData, &vertexes, m_nUseVertex*sizeof(sVertexCuda), cudaMemcpyHostToDevice);

		CreateFaceArea<<<parts[nPart].nUseFaceCnt,1>>>(cusFaceData, cusVertexData, cusPartDataCuda);
		if ( cudaSuccess != cudaGetLastError() )
		{
			printf( "Error! Kernel : %d \n");
		}

		cudaFree(cusFaceData);
		cudaFree(cusVertexData);
	}



	//const unsigned int unOneThreadSize = m_nYFileVoxCnt;
	//const unsigned int unMaxBlockCnt = m_nMaxBlockSize;
	//unsigned int unOneStepBlcokCnt  = m_nXFileVoxCnt * m_nZFileVoxCnt;
	//if(unOneStepBlcokCnt > unMaxBlockCnt)
	//{
	//	unOneStepBlcokCnt = unMaxBlockCnt;
	//}
	//		
	//const unsigned int unMaxBlockSize = unVoxelSize / m_nYFileVoxCnt;

	//unsigned int unBlockStep = 0;

	//bool *cubVoxelState;
	//bool *cubVoxelResultSize;

	//if ( cudaSuccess != cudaMalloc(&cubVoxelState, unVoxelSize*sizeof(bool)))
	//{
	//	printf( "Error! Y VoxelState Malloc \n" );
	//}
	//if ( cudaSuccess != cudaMalloc(&cubVoxelResultSize, unVoxelSize*sizeof(bool)))
	//{
	//	printf( "Error! Y VoxelResult Malloc \n" );
	//}

	////! 디바이스에게 메모리 카피
	//cudaMemcpy(cubVoxelState, bVoxelState, unVoxelSize*sizeof(bool), cudaMemcpyHostToDevice);

	//while (unBlockStep <  unMaxBlockSize)
	//{
	//	if(unOneStepBlcokCnt > unMaxBlockSize - unBlockStep)
	//		unOneStepBlcokCnt = unMaxBlockSize - unBlockStep;
	//	CreateFaceArea<<<unOneStepBlcokCnt,1>>>(cubVoxelState, cubVoxelResultSize, m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, unBlockStep);
	//	if ( cudaSuccess != cudaGetLastError() )
	//	{
	//		printf( "Error! Y BlockStep : %d \n", unBlockStep);
	//	}
	//	unBlockStep += unOneStepBlcokCnt;
	//	
	//}
	//
	//if ( cudaSuccess != cudaMemcpy(bVoxelYResult, cubVoxelResultSize, unVoxelSize*sizeof(bool), cudaMemcpyDeviceToHost))
	//{
	//	printf( "Error! Y VoxelState Memcpy \n" );
	//}

	//cudaFree(cubVoxelState);
	//cudaFree(cubVoxelResultSize);


}