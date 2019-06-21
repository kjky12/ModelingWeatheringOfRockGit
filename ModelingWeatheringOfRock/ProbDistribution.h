// ProbDistribution.h: interface for the CProbDistribution class.
// Probability Distribution

#if !defined(AFX_PROBDISTRIBUTION_H__BC8CB960_F42C_4C45_8F0E_95D32E734CA7__INCLUDED_)
#define AFX_PROBDISTRIBUTION_H__BC8CB960_F42C_4C45_8F0E_95D32E734CA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProbDistribution  
{
public:
	double GetDistParam();

/*****************************************************************************************************/	
	int probType;
	//	0 : constant
	//	1 : user-defined
	//	2 : uniform
	//	3 : gaussian
	//	4 : ralyeigh
	//	5 : uniform polar distance
	//	6 : uniform polar angle
	//	7 : user-defined [stair]
	//	8 : discrete uniform 

	double constant;			//Constant
	double mean;				//Mean
	double stdDev;				//��(Log-normal fading)
	double min;					//Min
	double max;					//Max
	double maxDistance;			//MaxDistance
	double maxAngle;			//MaxAngle
	double step;				//Step
	double Rate;				//Rate
	double *xValue;				//userDefine�� ��쿡 ����ϴ� �Ķ���� �� x, x���� �ݵ�� �������� ���ĵǾ� �ԷµǾ�� �Ѵ�.
	double *yProb;				//userDefine�� ��쿡 ����ϴ� �Ķ���� �� y
	int uCount;					//userDefine�� ��� ������ ��
/*****************************************************************************************************/

	double CalDiscreteUniform();
	double CalUniformPolarAngle();
	double CalUniformPolar();
	double CalRayleigh();
	double CalGaussian();
	double CalUniform();	
	double CalConstant();
	double CalUserDefine();
	double CalUserDefineStair();
	double CalExponential();

//	void SetDistribution(LibDistribution dist);

	CProbDistribution();

//	���� :	Ȯ�� ���� �Ķ���� ���� Ŭ���� ������, user defined�� ��� 
//			type = 1 �̰ų� 7�� ��쿡 ���
//	�Է� :	�Ķ���� type, userDefine, userDefine[stair]�� ����� �Ķ���� ����, x�� set, y�� set
//	��� :	Ŭ���� ���� ���� ����
	CProbDistribution(int pType, int defiendCount, double *x, double *y);

//	���� :	Ȯ�� ���� �Ķ���� ���� Ŭ���� ������, user defined�� �ƴ� ��� 
//			type = 1 �� 7�� �ƴ� ��쿡 ���
//	�Է� :	�Ķ���� type, Constant, Mean, ��, Min, Max, MaxDistance, MaxAngle, Step
//	��� :	Ŭ���� ���� ���� ����
	CProbDistribution(int _pType, double _constant = 0.0, double _mean = 0.0, double _stdDev = 0.0, double _min = 0.0, double _max = 0.0, double _maxDistance = 0.0, double _maxAngle = 0.0, double _step = 0.0, double _rate = 0.0);
	
	

	virtual ~CProbDistribution();
};

#endif // !defined(AFX_PROBDISTRIBUTION_H__BC8CB960_F42C_4C45_8F0E_95D32E734CA7__INCLUDED_)

