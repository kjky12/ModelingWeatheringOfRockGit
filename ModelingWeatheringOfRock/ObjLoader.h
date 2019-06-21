#include <vector>

using namespace std;

struct sVertex
{
	float x, y, z;
};

struct sTexCoord
{
	float u, v;
};

struct sMaterial
{	
	char name[256];
	int illum;
	char map_Kd[256];
	float Ns;
	float Ni;
	float d;
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Tf[3];
	unsigned int texture;

	sMaterial() {
		memset (this, 0, sizeof(sMaterial));
	}
};

struct sFace
{
	int n;

	int v[5];	//vertices
	int vt[5];	//text coords
	int vn[5];	//normals

	sFace() {
		memset (this, 0, sizeof(sFace));
	}
};

struct sPart
{
	char name[256];
	vector<sFace> faces;

	sPart() {
		name[0] = 0;
	}
};

struct sPartData
{
	float fMaxX;
	float fMaxY;
	float fMaxZ;
	float fMinX;
	float fMinY;
	float fMinZ;

	sPartData() {
		fMaxX = -99999.9;
		fMaxY = -99999.9;
		fMaxZ = -99999.9;
		fMinX = 99999.9;
		fMinY = 99999.9;
		fMinZ = 99999.9;
	}
};



class CObjLoader
{
public:
	CObjLoader ();

	void loadMaterialsTexture ();
	bool Load (char *objfile, char *mtlfile = NULL);
	void Draw (float scale = 1.f);
	bool interSection(sVertex center);

	bool interSection1();

	void CaclSection(int nPartCnt,float triverts[6][3],sPartData PartData);

	float m_fMaxX;
	float m_fMaxY;
	float m_fMaxZ;
	float m_fMinX;
	float m_fMinY;
	float m_fMinZ;

	vector<sVertex> vertexes;
	vector<sPart> parts;
private:
	vector<sMaterial> materials;

	
	vector<sTexCoord> texcoords;
	vector<sVertex> normals;

	

	bool loadObjects (char *fileName);
	bool loadMaterials (char *fileName);

	int findMaterialIndex(char *name);
	bool loadTexture (char *fileName, unsigned int *texture);

private:	
	char _work_path[_MAX_DIR];
	bool _loaded;
};
