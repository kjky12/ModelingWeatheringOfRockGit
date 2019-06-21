#include "StdAfx.h"
#include "BoxinsetersectTriangle.h"


//#include <iostream>
// using std::cin;
// using std::cout;
// using std::endl;
using std::numeric_limits;
using namespace std;


BoxinsetersectTriangle::BoxinsetersectTriangle(void)
{
}

BoxinsetersectTriangle::~BoxinsetersectTriangle(void)
{
}





float BoxinsetersectTriangle::Dot(Vector v1, Vector v2)
{
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

float BoxinsetersectTriangle::Dot(Point p1, Vector v2)
{
	return (p1.x*v2.x + p1.y*v2.y + p1.z*v2.z);
}

Vector BoxinsetersectTriangle::Cross(Vector v1, Vector v2)
{
	Vector v3;       
	v3.x = v1.y*v2.z - v2.y*v1.z;
	v3.y = v1.z*v2.x - v1.x*v2.z;
	v3.z = v1.x*v2.y - v1.y*v2.x;
	return v3;
}




float BoxinsetersectTriangle::getmin(const vector<Point> &points, Vector axis)
{
	float min = (std::numeric_limits<float>::max)(); 

	for (int ctr = 0; ctr < points.size(); ctr++)
	{
		float dotprod = Dot(points[ctr], axis);
		if (dotprod < min) min = dotprod;
	}
	return min;
}

float BoxinsetersectTriangle::getmax(const vector<Point> &points, Vector axis)
{
	float max = -(std::numeric_limits<float>::max)(); 

	for (int ctr = 0; ctr < points.size(); ctr++)
	{
		float dotprod = Dot(points[ctr], axis);
		if (dotprod > max) max = dotprod;
	}
	return max;
}

bool BoxinsetersectTriangle::isect(const vector<Point> &points1, const vector<Point> &points2, Vector axis)
{
	if (getmin(points1, axis) > getmax(points2, axis)) return false;
	if (getmax(points1, axis) < getmin(points2, axis)) return false;
	return true;     
}


bool BoxinsetersectTriangle::isectboxtri(float center[3], float r[3], float triverts[3][3])
{

	vector<Point> boxpoints;
	boxpoints.push_back(Point(center[0]+r[0], center[1]+r[1], center[2]+r[2]));
	boxpoints.push_back(Point(center[0]+r[0], center[1]+r[1], center[2]-r[2]));
	boxpoints.push_back(Point(center[0]+r[0], center[1]-r[1], center[2]+r[2]));
	boxpoints.push_back(Point(center[0]+r[0], center[1]-r[1], center[2]-r[2]));
	boxpoints.push_back(Point(center[0]-r[0], center[1]+r[1], center[2]+r[2]));
	boxpoints.push_back(Point(center[0]-r[0], center[1]+r[1], center[2]-r[2]));
	boxpoints.push_back(Point(center[0]-r[0], center[1]-r[1], center[2]+r[2]));
	boxpoints.push_back(Point(center[0]-r[0], center[1]-r[1], center[2]-r[2]));

	vector<Point> tripoints;
	tripoints.push_back(Point(triverts[0][0], triverts[0][1], triverts[0][2]));
	tripoints.push_back(Point(triverts[1][0], triverts[1][1], triverts[1][2]));
	tripoints.push_back(Point(triverts[2][0], triverts[2][1], triverts[2][2]));

	// test the x, y, and z axes
	if (!isect(boxpoints, tripoints, Vector(1, 0, 0))) return false;
	if (!isect(boxpoints, tripoints, Vector(0, 1, 0))) return false;
	if (!isect(boxpoints, tripoints, Vector(0, 0, 1))) return false;

	// test the triangle normal
	Vector triedge1 = tripoints[1] - tripoints[0];
	Vector triedge2 = tripoints[2] - tripoints[1];
	Vector trinormal = Cross(triedge1, triedge2);
	if (!isect(boxpoints, tripoints, trinormal)) return false;

	// test the 9 edge cross products
	Vector triedge3 = tripoints[0] - tripoints[2];

	Vector boxedge1 = Vector(1, 0, 0);
	Vector boxedge2 = Vector(0, 1, 0);
	Vector boxedge3 = Vector(0, 0, 1);

	if (!isect(boxpoints, tripoints, Cross(boxedge1, triedge1))) return false;
	if (!isect(boxpoints, tripoints, Cross(boxedge1, triedge2))) return false;
	if (!isect(boxpoints, tripoints, Cross(boxedge1, triedge3))) return false;

	if (!isect(boxpoints, tripoints, Cross(boxedge2, triedge1))) return false;
	if (!isect(boxpoints, tripoints, Cross(boxedge2, triedge2))) return false;
	if (!isect(boxpoints, tripoints, Cross(boxedge2, triedge3))) return false;

	if (!isect(boxpoints, tripoints, Cross(boxedge3, triedge1))) return false;
	if (!isect(boxpoints, tripoints, Cross(boxedge3, triedge2))) return false;
	if (!isect(boxpoints, tripoints, Cross(boxedge3, triedge3))) return false;

	return true;

}
// 
// int main(int argc, char *argv[])
// {
// 
// 	float center[3] = {2.0, 2.0, 0.0};
// 	float r[3] = {0.2, 0.2, 0.2};
// 
// 
// 	float triverts[3][3] = {
// 		{0.000000, 0.0, 0.00},
// 		{3.0, 0.0, 0.0},
// 		{0.0, 3.0, 0.0}
// 	};
// 
// 	cout << isectboxtri(center, r, triverts) << endl;
// 
// 	system("PAUSE");
// 	return EXIT_SUCCESS;
// }

