//CGPUSurface.cuh 파일 내에는 아래와 같이 써준다.



#pragma once

#include <cuda_runtime.h>


#define dfMAX_PART_CNT		4
#define dfMAX_FACE_CNT		1024
#define dfMAX_VERTEX_CNT	512




struct sFaceCuda
{
	int n;

	int v[5];	//vertices
	int vt[5];	//text coords
	int vn[5];	//normals

	sFaceCuda() {
		memset (this, 0, sizeof(sFaceCuda));
	}
};

struct sPartCuda
{
	char name[256];
	int nUseFaceCnt;
	sFaceCuda faces[dfMAX_FACE_CNT];

	sPartCuda() {
		name[0] = 0;
	}
};

struct sPartDataCuda
{
	float fMaxX;
	float fMaxY;
	float fMaxZ;
	float fMinX;
	float fMinY;
	float fMinZ;

	sPartDataCuda() {
		fMaxX = -99999.9;
		fMaxY = -99999.9;
		fMaxZ = -99999.9;
		fMinX = 99999.9;
		fMinY = 99999.9;
		fMinZ = 99999.9;
	}
};


struct sVertexCuda
{
	float x, y, z;
};


#ifdef __cplusplus 
extern "C" {//<-- extern 시작
#endif

class CGPUSurface
{

	public:
	CGPUSurface(void);
	virtual ~CGPUSurface(void);

	//! 해상도 나누는 개수
	int			g_nDivideCnt;
	//! 선택된 축
	int			g_nSelectAxis;
	//! 복셀 크기 인듯
	float		g_fRectSize;

	void SetGlobalValue(int		nDivideCnt, int		nSelectAxis, float	fRectSize);

	////////////////////////////////////////
	//Object 복셀화 변수
	int m_nUseVertex;
	sVertexCuda vertexes[dfMAX_VERTEX_CNT];

	int m_nUsePart;
	sPartCuda parts[dfMAX_PART_CNT];

	float m_fObjMaxX;
	float m_fObjMaxY;
	float m_fObjMaxZ;
	float m_fObjMinX;
	float m_fObjMinY;
	float m_fObjMinZ;

	void SetObjectValue(float fObjMaxX, float fObjMaxY,float fObjMaxZ,	float fObjMinX,	float fObjMinY,	float fObjMinZ,
		int nVretexesSize , char *pVertex, int nPartSize, char *pParts);

	//////////////////////////////////////////
	// etc
	unsigned long long m_nXFileVoxCnt;
	unsigned long long m_nYFileVoxCnt;
	unsigned long long m_nZFileVoxCnt;
	void SetGlobalEtc(unsigned long long nXFileVoxCnt,	unsigned long long nYFileVoxCnt,	unsigned long long nZFileVoxCnt);


	////////////////////////////////////////
	// Surface execute
	void SetSurfaceVoxelization();
};



#ifdef __cplusplus 

}

#endif


