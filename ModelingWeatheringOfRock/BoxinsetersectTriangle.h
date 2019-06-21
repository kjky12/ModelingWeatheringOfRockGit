#pragma once

#include <stdio.h>
#include <stdlib.h>

// #include <vector>
 #include <limits>




struct Vector {
	float x, y, z;       

	Vector(void)
	{        
	}

	Vector(float a, float b, float c)
	{
		x=a;            
		y=b;
		z=c;            
	}

	Vector operator-(Vector &v2)
	{
		Vector v3;
		v3.x = x - v2.x;
		v3.y = y - v2.y;
		v3.z = z - v2.z;
		return v3;
	}

}; 

struct Point {
	float x, y, z;  

	Point(void)
	{        
	}

	Point(float a, float b, float c)
	{
		x=a;            
		y=b;
		z=c;            
	}

	Vector operator-(Point &p2)
	{
		Vector v3;
		v3.x = x - p2.x;
		v3.y = y - p2.y;
		v3.z = z - p2.z;
		return v3;
	}

}; 

struct Sphere {
	Point c; // Sphere center
	float r; // Sphere radius
};

// 
// Vector operator*(float scale, Vector v)
// {
// 	Vector v3;
// 	v3.x = scale*v.x;
// 	v3.y = scale*v.y;
// 	v3.z = scale*v.z;
// 	return v3;
// }

// Point operator+(Point p, Vector v)
// {
// 	Point p2;
// 	p2.x = p.x + v.x;
// 	p2.y = p.y + v.y;
// 	p2.z = p.z + v.z;
// 	return p2;
// }
// 
// Point operator+(Point p, Point p2)
// {
// 	Point p3;
// 	p3.x = p.x + p2.x;
// 	p3.y = p.y + p2.y;
// 	p3.z = p.z + p2.z;
// 	return p3;
// }
// 
// Point operator*(float scale, Point p)
// {
// 	Point p2;
// 	p2.x = scale*p.x;
// 	p2.y = scale*p.y;
// 	p2.z = scale*p.z;
// 	return p2;
// }


class BoxinsetersectTriangle
{
public:
	BoxinsetersectTriangle(void);
	~BoxinsetersectTriangle(void);


	float Dot(Vector v1, Vector v2);

	float Dot(Point p1, Vector v2);
	Vector Cross(Vector v1, Vector v2);

	float getmin(const vector<Point> &points, Vector axis);
	float getmax(const vector<Point> &points, Vector axis);
	bool isect(const vector<Point> &points1, const vector<Point> &points2, Vector axis);
	bool isectboxtri(float center[3], float r[3], float triverts[3][3]);

	
};
