
// ModelingWeatheringOfRock.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CModelingWeatheringOfRockApp:
// �� Ŭ������ ������ ���ؼ��� ModelingWeatheringOfRock.cpp�� �����Ͻʽÿ�.
//

class CModelingWeatheringOfRockApp : public CWinApp
{
public:
	CModelingWeatheringOfRockApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CModelingWeatheringOfRockApp theApp;