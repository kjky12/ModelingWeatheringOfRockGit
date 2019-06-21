// ProbDistribution.cpp: implementation of the CProbDistribution class.
//

#include "stdafx.h"
#include "ProbDistribution.h"

#include "math.h"

#ifdef _AFXDLL
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProbDistribution::CProbDistribution()
{
	xValue = NULL;
	yProb = NULL;
}

//	개요 :	확률 분포 파라미터 설정 클래스 생성자, user defined인 경우 
//			type = 1 이거나 7인 경우에 사용
//	입력 :	파라미터 type, userDefine, userDefine[stair]인 경우의 파라미터 갯수, x값 set, y값 set
//	출력 :	클래스 정상 생성 여부
CProbDistribution::CProbDistribution(int pType, int defiendCount, double *x, double *y)
{
	if ( pType == 1 || pType == 7 )
	{
		if ( defiendCount =! 0 )
		{
			xValue = new double[defiendCount];
			yProb  = new double[defiendCount];

			for ( int i = 0; i < defiendCount; i++ )
			{
				xValue[i] = x[i];
				yProb[i] = y[i];
			}
		}
		else	
		{
			//	?	
		}
	}
	else
	{
		//	?
	}

	xValue = NULL;
	yProb = NULL;
}

// void CProbDistribution::SetDistribution(LibDistribution dist)
// {
// 	probType = dist.type;
// 	constant = dist.constant;
// 	mean = dist.mean;	
// 	stdDev = dist.stdDev;	
// 	min = dist.min;		
// 	max = dist.max;		
// 	maxDistance = dist.maxDist;
// 	maxAngle = dist.maxAngle;
// 	step = dist.step;
// 	
// 	if (dist.type == 1 || dist.type  == 7 )
// 	{
// 		xValue = new double[dist.userDefine.count];
// 		yProb = new double[dist.userDefine.count];
// 		for (int i = 0; i < dist.userDefine.count; i++)
// 		{
// 			xValue[i] = dist.userDefine.value[i];
// 			yProb[i] = dist.userDefine.prob[i];
// 		}
// 		uCount = dist.userDefine.count;
// 	}	
// 	else
// 	{
// 		xValue = NULL;
// 		yProb = NULL;
// 		uCount = 0;
// 	}
// 	
// }
//	개요 :	확률 분포 파라미터 설정 클래스 생성자, user defined이 아닌 경우 
//			type = 1 도 7도 아닌 경우에 사용
//	입력 :	파라미터 type, Constant, Mean, σ, Min, Max, MaxDistance, MaxAngle, Step
//	출력 :	클래스 정상 생성 여부
CProbDistribution::CProbDistribution(int _pType, double _constant, double _mean, double _stdDev, double _min, double _max, double _maxDistance, double _maxAngle, double _step, double _rate)
{
	probType = _pType;
	constant = _constant;
	mean = _mean;	
	stdDev = _stdDev;	
	min = _min;		
	max = _max;		
	maxDistance = _maxDistance;
	maxAngle = _maxAngle;
	step = _step;	
	Rate = _rate;

	xValue = NULL;
	yProb = NULL;
}

CProbDistribution::~CProbDistribution()
{
	if ( xValue != NULL )
	{
		delete [] xValue;
	}
	if ( yProb != NULL )
	{
		delete [] yProb;
	}
}

double CProbDistribution::CalConstant()
{
	return constant;
}

double CProbDistribution::CalUniform()
{
	double dDy = double(rand( )%10000)/10000.0;		
	dDy = min + (max - min ) * dDy;
	return dDy;
}

double CProbDistribution::CalGaussian()
{
	double dDy = double(rand( )%10000)/10000.0;
    double dValue1 = dDy * 2.0 - 1.0;
    double dIerf = sqrt(acos((double)-1)) / 2.0 * (dValue1 + acos((double)-1) * pow(dValue1, 3.0) / 12.0 + 7.0 * pow(acos((double)-1), 2.0) * pow(dValue1, 5.0) / 480.0 + 127.0 * pow(acos((double)-1), 3.0) *
            pow(dValue1, 7.0) / 40320.0 + 4369.0 * pow(acos((double)-1), 4.0) * pow(dValue1, 9.0) / 5806080.0 + 34807.0 * pow(acos((double)-1), 5.0) * pow(dValue1, 11.0) / 182476800.0);
    dDy = dIerf * sqrt(2.0) * stdDev + mean;
	return dDy;
}

double CProbDistribution::CalRayleigh()
{
	double dDy = double(rand( )%10000)/10000.0;
    dDy = min + sqrt(-2.0 * pow(stdDev, 2.0) * log( (1 - dDy) ) );	//check
	return dDy;
}

double CProbDistribution::CalUniformPolar()
{
	double dDy = double(rand( )%10000)/10000.0;
    dDy = maxDistance * dDy;
	return dDy;
}

double CProbDistribution::CalUniformPolarAngle()
{
	double dDy = double(rand( )%10000)/10000.0;
	dDy = (maxAngle * -1.0) + (maxAngle - (maxAngle * -1.0) ) * dDy;
	return dDy;
}

double CProbDistribution::CalDiscreteUniform()
{
	double dDy = rand() % (int)( (max - min) / step + 1 );
    dDy = dDy * step + min;
	return dDy;
}

double CProbDistribution::CalUserDefine()
{	
	double r=(rand()%100)+1;
	double ran=r/100.0;
	double result = 0.0;
	
	for( int a=0; a < uCount; a++)
	{
		if(a==0)
		{
			if(yProb[a] >= ran)
			{
				result=xValue[a];
				break;
			}
		}
		if(a==uCount-1)
		{
			if(yProb[a] <= ran)
				result=xValue[a];
			else
			{
				result=0;
			}
			break;
		}

		if(yProb[a] == ran)
		{
			result=xValue[a];
			break;
		}
		else if(yProb[a] < ran && yProb[a+1]> ran)
		{
			result=xValue[a]+(xValue[a+1]-xValue[a])*((ran-yProb[a])/(yProb[a+1]-yProb[a]));
			break;
		}
	}

	return result;
}

double CProbDistribution::CalUserDefineStair()
{		
	double r=(rand()%100)+1;
	double ran=r/100.0;
	double result = 0.0;
	double x = 0.0;
	double y = 0.0;
	
	for(int a = 0; a<uCount; a++)
	{
		for(int b=a;b<uCount;b++)
		{
			if(yProb[a] > yProb[b])
			{
				y=yProb[a];
				yProb[a]=xValue[b];
				yProb[b]=y;

				x=xValue[a];
				xValue[a]=xValue[b];
				xValue[b]=x;
			}
		}
	}
	for(int a=0;a<uCount;a++)
	{
		if(a==uCount-1)
		{
			if(yProb[a] <= ran)	
				result=xValue[a];
			else
			{
				result=0;
			}
			break;
		}
		if(a==0)
		{
			if(yProb[a] >= ran)
			{
				result=xValue[a];
				break;
			}
		}
		if(yProb[a]*100.0 == ran)
		{
			result=xValue[a];
			break;
		}
		else if(yProb[a] < ran && yProb[a+1]> ran)
		{
			result=xValue[a+1];
			break;
		}
	}

	return result;
}

 double CProbDistribution::CalExponential()
 {
	 double dRate =Rate;

 	 double dDy = double(rand( )%10000)/10000.0;
	 //m_dDy = Math.Log((1 - m_dDy), Math.Exp(1.0)) / dRate * -1.0;
	 dDy = log10( exp(1.0) ) / log10(1 - dDy) / dRate * -1.0;

 	 return dDy;
 }

double CProbDistribution::GetDistParam()
{
	

	double paramV = 0.0;
	if ( probType == 0 )
	{
		paramV = CalConstant();
	}
	else if ( probType == 1 )
	{
		paramV = CalUserDefine();
	}
	else if ( probType == 2 )
	{
		paramV = CalUniform();
	}
	else if ( probType == 3 )
	{
		paramV = CalGaussian();
	}
	else if ( probType == 4 )
	{
		paramV = CalRayleigh();
	}
	else if ( probType == 5 )
	{
		paramV = CalExponential();
	}
	else if ( probType == 6 )
	{
		paramV = CalUniformPolar();
	}
	else if ( probType == 7 )
	{
		paramV = CalUniformPolarAngle();
	}
	else if ( probType == 8 )
	{
		paramV = CalUserDefineStair();
	}
	else if ( probType == 9 )
	{
		paramV = CalDiscreteUniform();
	}
	else
	{
		//	debug
	}
	return paramV;
}


