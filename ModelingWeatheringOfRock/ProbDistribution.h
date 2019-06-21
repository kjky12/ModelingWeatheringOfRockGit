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
	double stdDev;				//σ(Log-normal fading)
	double min;					//Min
	double max;					//Max
	double maxDistance;			//MaxDistance
	double maxAngle;			//MaxAngle
	double step;				//Step
	double Rate;				//Rate
	double *xValue;				//userDefine인 경우에 사용하는 파라미터 값 x, x값은 반드시 오름차순 정렬되어 입력되어야 한다.
	double *yProb;				//userDefine인 경우에 사용하는 파라미터 값 y
	int uCount;					//userDefine인 경우 데이터 수
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

//	개요 :	확률 분포 파라미터 설정 클래스 생성자, user defined인 경우 
//			type = 1 이거나 7인 경우에 사용
//	입력 :	파라미터 type, userDefine, userDefine[stair]인 경우의 파라미터 갯수, x값 set, y값 set
//	출력 :	클래스 정상 생성 여부
	CProbDistribution(int pType, int defiendCount, double *x, double *y);

//	개요 :	확률 분포 파라미터 설정 클래스 생성자, user defined이 아닌 경우 
//			type = 1 도 7도 아닌 경우에 사용
//	입력 :	파라미터 type, Constant, Mean, σ, Min, Max, MaxDistance, MaxAngle, Step
//	출력 :	클래스 정상 생성 여부
	CProbDistribution(int _pType, double _constant = 0.0, double _mean = 0.0, double _stdDev = 0.0, double _min = 0.0, double _max = 0.0, double _maxDistance = 0.0, double _maxAngle = 0.0, double _step = 0.0, double _rate = 0.0);
	
	

	virtual ~CProbDistribution();
};

#endif // !defined(AFX_PROBDISTRIBUTION_H__BC8CB960_F42C_4C45_8F0E_95D32E734CA7__INCLUDED_)

