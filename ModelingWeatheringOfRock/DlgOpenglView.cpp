// DlgOpenglView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelingWeatheringOfRock.h"
#include "DlgOpenglView.h"
#include "ObjLoader.h"
#include <gl/glut.h>

// CDlgOpenglView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgOpenglView, CDialog)

CDlgOpenglView::CDlgOpenglView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOpenglView::IDD, pParent)
{

	m_fScale = 1.0;
	m_XRot = 0.0;
	m_YRot = 0.0;
	m_ZRot = 0.0;

	m_XTran = 0.0;
	m_YTran = 0.0;
	m_ZTran = 0.0;

	m_f3DScale = 1.0;

	m_bResizing=true;

	m_bComplete = FALSE;

	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;

	m_bTranObj = FALSE;
	m_bViewAxis = FALSE;
}

CDlgOpenglView::~CDlgOpenglView()
{
}

void CDlgOpenglView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_VIEW_SIZE, m_staticViewSize);
}


BEGIN_MESSAGE_MAP(CDlgOpenglView, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgOpenglView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgOpenglView::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()

	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CDlgOpenglView 메시지 처리기입니다.

void CDlgOpenglView::OnBnClickedOk()
{
	return;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

void CDlgOpenglView::OnBnClickedCancel()
{
	return;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void CDlgOpenglView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	m_wgl.Begin(&dc);
	if (m_bResizing)
	{
		CRect rect;
		GetClientRect(&rect);

		glViewport(0,0, rect.Width(), rect.Height());
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45,rect.Width()/(double)rect.Height(),0.001,10000.0);
		

// 		m_combo3DViewEye.AddString(L"User");
// 		m_combo3DViewEye.AddString(L"Top");
// 		m_combo3DViewEye.AddString(L"Bottom");
// 		m_combo3DViewEye.AddString(L"Left");
// 		m_combo3DViewEye.AddString(L"Right");
// 		m_combo3DViewEye.AddString(L"Front");
// 		m_combo3DViewEye.AddString(L"Back");

		float fScale = m_f3DScale / 10.0;

		switch (g_n3DShowEye)
		{
		case 0:
			gluLookAt(5.0+fScale,5.0+fScale,5.0+fScale,0,0,0,0,0,1);
			break;
		case 1:
			gluLookAt(0.0,0.0,5.0+fScale,0,0,0,0,1,0);
			break;
		case 2:
			gluLookAt(0.0,0.0,-5.0-fScale,0,0,0,0,1,0);
			break;
		case 3:
			gluLookAt(5.0+fScale,0.0,0.0,0,0,0,0,0,1);
			break;
		case 4:
			gluLookAt(-5.0-fScale,0.0,0.0,0,0,0,0,0,1);
			break;
		case 5:
			gluLookAt(0.0,5.0+fScale,0.0,0,0,0,0,0,1);
			break;
		case 6:
			gluLookAt(0.0,-5.0-fScale,0.0,0,0,0,0,0,1);
			break;
		default:
			gluLookAt(5.0+fScale,5.0+fScale,5.0+fScale,0,0,0,0,0,1);
			break;
		}
		
		glMatrixMode(GL_MODELVIEW);
	}

	if(g_n3DShowType == 0)
		DrawFileGLScene();		// 계산용
	else if(g_n3DShowType == 1)
		DrawFileExternScene();	// 외부 노출 개수 보기 위해
	else if(g_n3DShowType == 2)
		DrawFileColor();	// 입자별 색상을 보기위해
	else if(g_n3DShowType == 3)
		DrawFilePorosity();	// 공극보기
	else
		DrawFileObjScene(); // 오브젝트 보기위해
	
	//
	
	GetParent()->SendMessage(WM_CHANGE_POS_ROT_MSG,0,0);
	//()
	//DrawGLScene();

	//DrawVoxelScene();

	m_wgl.End();
}



void CDlgOpenglView::OnDraw(CDC* pDC)
{

	
}


int CDlgOpenglView::DrawVoxelScene()
{

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();						// Reset The View

	glRotatef(m_XRot,1.0f,0.0f,0.0f);
	glRotatef(m_YRot,0.0f,1.0f,0.0f);
	glRotatef(m_ZRot,0.0f,0.0f,1.0f);
	glScalef(m_fScale,m_fScale,m_fScale);

	glTranslatef(m_XTran,m_YTran,m_ZTran);


	//	Set up some nice attributes for drawing the grid.
	glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);

	//	Create the grid.

	glBegin(GL_LINES);

	for (int i = -100; i <= 100; i++)
	{
		glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
		glVertex3f((float)i, -100, 0 );
		glVertex3f((float)i, 100, 0 );
		glVertex3f(-100, (float)i, 0);
		glVertex3f(100, (float)i, 0);
	}

	glEnd();

	//	Create the axies.
	glBegin(GL_LINES);

	glColor4f(1, 0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(100, 0, 0);

	glColor4f(0, 1, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 100, 0);

	glColor4f(0, 0, 1, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 100);

	glEnd();


	float fColor = 0.0;

	if(!m_bComplete)
	{

		if(g_bVoxelComplete)
		{
			glNewList(10,GL_COMPILE_AND_EXECUTE);

// 			for(int n = 0 ; n < 8 ; n++)
// 			{

				for(int v = 0 ; v < g_CompletevecVoxelPos.size() * ((float)g_nSlideView / 100.0) ; v++)
				{
					
					glPushMatrix();
					glTranslatef(g_CompletevecVoxelPos[v].x,g_CompletevecVoxelPos[v].y,g_CompletevecVoxelPos[v].z);
					//glColor3f((float)((rand()%150)/255.0),(float)((rand()%255)/255.0),(float)((rand()%255)/255.0));
					
					//fColor = (float)(((rand()%150) + 100)/255.0);
					fColor = (float)(((rand()%200) + 50 )/255.0);
					//glColor3f((float)(((rand()%150) + 100)/255.0),(float)(((rand()%150) + 100)/255.0),(float)(((rand()%150) + 100)/255.0));
					glColor3f(fColor,fColor,fColor);
					glutSolidCube (g_fRectSize);
					glPopMatrix();
				}
				
//			}

			glEndList();

			m_bComplete = TRUE;
		}

		
	}
	else
		glCallList(10);



	glPopAttrib();
	glFlush();

//	m_rtri+=5.0f;						// Increase The Rotation Variable For The Triangle 
//	m_rquad-=0.1f;						// Decrease The Rotation Variable For The Quad 
	return TRUE;						// Keep Going
}



int CDlgOpenglView::DrawGLScene()
{

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();						// Reset The View



	glRotatef(m_XRot,1.0f,0.0f,0.0f);
	glRotatef(m_YRot,0.0f,1.0f,0.0f);
	glRotatef(m_ZRot,0.0f,0.0f,1.0f);
	glScalef(m_fScale,m_fScale,m_fScale);

	glTranslatef(m_XTran,m_YTran,m_ZTran);

	//	Set up some nice attributes for drawing the grid.
	glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);

	//	Create the grid.

	glBegin(GL_LINES);

	for (int i = -10; i <= 10; i++)
	{
		glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
		glVertex3f((float)i, -10, 0 );
		glVertex3f((float)i, 10, 0 );
		glVertex3f(-10, (float)i, 0);
		glVertex3f(10, (float)i, 0);
	}

	glEnd();

	//	Create the axies.
	glBegin(GL_LINES);

	glColor4f(1, 0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(15, 0, 0);

	glColor4f(0, 1, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 15, 0);

	glColor4f(0, 0, 1, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 15);

	glEnd();





	//	glRotatef(m_rtri,0.0f,1.0f,0.0f);		// Rotate The Pyramid On It's Y Axis
	//	glTranslatef(-5.0f,0.0f,-5.0f);			// Move Left And Into The Screen

	if(!m_bComplete)
	{
		map<int,vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>>::iterator	iterBaseRockData;
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
		ST_PARTICLE_POS stParticlePos;
		vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>	VecZBaseRockParticle;

		//float fColor = 0.0f;
		int nRed = 0;int nGreen = 0;int nBlue = 0;

		iterBaseRockData = g_MapBaseRockData.find(0);

		if(iterBaseRockData != g_MapBaseRockData.end())
		{
			VecZBaseRockParticle = iterBaseRockData->second;


			glNewList(10,GL_COMPILE_AND_EXECUTE);

			for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
			{
				stParticlePos = iterOutsideData->second;

				if(stParticlePos.iExternalSideCnt <= 0)
					continue;

				if(VecZBaseRockParticle[stParticlePos.z][stParticlePos.y][stParticlePos.x].bUse)
				{
					glPushMatrix();

					glTranslatef(stParticlePos.x/10.0f,stParticlePos.y/10.0f,stParticlePos.z/10.0f);			// Move Left And Into The Screen

					gf_GetEfieldColor(VecZBaseRockParticle[stParticlePos.z][stParticlePos.y][stParticlePos.x].fPorosity,nRed,nGreen,nBlue);

					glColor3f(float(nRed / 255.0),float(nGreen / 255.0),float(nBlue / 255.0));
					glutSolidCube (0.1);
					glPopMatrix();
				}
			}

			glEndList();
		}





		m_bComplete = TRUE;
	}
	else
		glCallList(10);



	glPopAttrib();
	glFlush();

	//	m_rtri+=5.0f;						// Increase The Rotation Variable For The Triangle 
	//	m_rquad-=0.1f;						// Decrease The Rotation Variable For The Quad 
	return TRUE;						// Keep Going
}

int CDlgOpenglView::DrawFileGLScene()
{

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();						// Reset The View



	//	Set up some nice attributes for drawing the grid.
	glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);

	//	Create the grid.

// 	glBegin(GL_LINES);
// 
// 	for (int i = -10; i <= 10; i++)
// 	{
// 		glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
// 		glVertex3f((float)i, -10, 0 );
// 		glVertex3f((float)i, 10, 0 );
// 		glVertex3f(-10, (float)i, 0);
// 		glVertex3f(10, (float)i, 0);
// 	}
// 
// 	glEnd();
// 
	//	Create the axies.

		
	glRotatef(m_XRot,1.0f,0.0f,0.0f);
	glRotatef(m_YRot,0.0f,1.0f,0.0f);
	glRotatef(m_ZRot,0.0f,0.0f,1.0f);
	glScalef(m_fScale,m_fScale,m_fScale);

	
	glTranslatef(m_XTran,m_YTran,m_ZTran);

	if(m_bViewAxis)
	{
		glBegin(GL_LINES);

		glColor4f(1, 0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(600, 0, 0);

		glColor4f(0, 1, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 600, 0);

		glColor4f(0, 0, 1, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 600);

		glEnd();
	}

	glTranslatef(-1.0 * g_nXVoxCnt/2.0,-1.0 * g_nYVoxCnt/2.0,-1.0 * g_nZVoxCnt/2.0);

	if(!m_bComplete)
	{
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
		ST_PARTICLE_POS stParticlePos;
		vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>	VecZBaseRockParticle;

		//float fColor = 0.0f;
		int nRed = 0;int nGreen = 0;int nBlue = 0;
	
		glNewList(10,GL_COMPILE_AND_EXECUTE);

		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
		{
			stParticlePos = iterOutsideData->second;

// 			if(stParticlePos.iExternalSideCnt <= 0)
// 				continue;


			glPushMatrix();

			glTranslatef((stParticlePos.x - g_nXVoxCnt)/10.0f,(stParticlePos.y- g_nYVoxCnt)/10.0f,(stParticlePos.z- g_nZVoxCnt)/10.0f);			// Move Left And Into The Screen

			gf_GetEfieldColor(stParticlePos.fPorosity,nRed,nGreen,nBlue);

			glColor3f(float(nRed / 255.0),float(nGreen / 255.0),float(nBlue / 255.0));
			glutSolidCube (0.1);
			glPopMatrix();
		}

		glEndList();

		m_bComplete = TRUE;
	}
	else
		glCallList(10);



	glPopAttrib();
	glFlush();

	//	m_rtri+=5.0f;						// Increase The Rotation Variable For The Triangle 
	//	m_rquad-=0.1f;						// Decrease The Rotation Variable For The Quad 
	return TRUE;						// Keep Going
}


int CDlgOpenglView::DrawFileExternScene()
{

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();						// Reset The View


	//	Set up some nice attributes for drawing the grid.
	glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);

	//	Create the grid.

// 	glBegin(GL_LINES);
// 
// 	for (int i = -10; i <= 10; i++)
// 	{
// 		glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
// 		glVertex3f((float)i, -10, 0 );
// 		glVertex3f((float)i, 10, 0 );
// 		glVertex3f(-10, (float)i, 0);
// 		glVertex3f(10, (float)i, 0);
// 	}
// 
// 	glEnd();
// 
// 	//	Create the axies.

	

	glRotatef(m_XRot,1.0f,0.0f,0.0f);
	glRotatef(m_YRot,0.0f,1.0f,0.0f);
	glRotatef(m_ZRot,0.0f,0.0f,1.0f);
	glScalef(m_fScale,m_fScale,m_fScale);

	
	
	glTranslatef(m_XTran,m_YTran,m_ZTran);


	if(m_bViewAxis)
	{
		glBegin(GL_LINES);

		glColor4f(1, 0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(600, 0, 0);

		glColor4f(0, 1, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 600, 0);

		glColor4f(0, 0, 1, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 600);

		glEnd();
	}

//	glTranslatef(-1.0 * g_nXVoxCnt/2.0,-1.0 * g_nYVoxCnt/2.0,-1.0 * g_nZVoxCnt/2.0);

	if(!m_bComplete)
	{
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
		ST_PARTICLE_POS stParticlePos;
		vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>	VecZBaseRockParticle;

		//float fColor = 0.0f;
		int nRed = 0;int nGreen = 0;int nBlue = 0;

		glNewList(10,GL_COMPILE_AND_EXECUTE);

		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
		{
			stParticlePos = iterOutsideData->second;

// 			if(stParticlePos.bInOut == false)
// 				continue;

			if(stParticlePos.iExternalSideCnt <= 0)
				continue;

			//if(VecZBaseRockParticle[stParticlePos.z][stParticlePos.y][stParticlePos.x].bUse)
			{
				glPushMatrix();

				glTranslatef((stParticlePos.x - g_nXVoxCnt)/10.0f,(stParticlePos.y- g_nYVoxCnt)/10.0f,(stParticlePos.z- g_nZVoxCnt)/10.0f);			// Move Left And Into The Screen

				//gf_GetEfieldColor(stParticlePos.fPorosity,nRed,nGreen,nBlue);

				
				//switch ((int)stParticlePos.vecExternalSide.size())
				switch ((int)stParticlePos.iExternalSideCnt)
				{
				case 1: 
					nRed = 63;
					nGreen = 72;
					nBlue = 204;
					break;
				case 2: 
					nRed = 0;
					nGreen = 162;
					nBlue = 232;
					break;
				case 3: 
					nRed = 34;
					nGreen = 177;
					nBlue = 76;
					break;
				case 4: 
					nRed = 255;
					nGreen = 242;
					nBlue = 0;
					break;
				case 5: 
					nRed = 255;
					nGreen = 127;
					nBlue = 39;
					break;
				case 6: 
					nRed = 237;
					nGreen = 28;
					nBlue = 36;
					break;
				default:
					nRed = 255;
					nGreen = 255;
					nBlue = 255;
					break;
				}
				glColor3f(float(nRed / 255.0),float(nGreen / 255.0),float(nBlue / 255.0));
				glutSolidCube (0.1);
				glPopMatrix();
			}
		}

		glEndList();

		m_bComplete = TRUE;
	}
	else
		glCallList(10);



	glPopAttrib();
	glFlush();

	//	m_rtri+=5.0f;						// Increase The Rotation Variable For The Triangle 
	//	m_rquad-=0.1f;						// Decrease The Rotation Variable For The Quad 
	return TRUE;						// Keep Going
}


int CDlgOpenglView::DrawFileColor()
{

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();						// Reset The View


	//	Set up some nice attributes for drawing the grid.
	glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);

	//	Create the grid.

	// 	glBegin(GL_LINES);
	// 
	// 	for (int i = -10; i <= 10; i++)
	// 	{
	// 		glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
	// 		glVertex3f((float)i, -10, 0 );
	// 		glVertex3f((float)i, 10, 0 );
	// 		glVertex3f(-10, (float)i, 0);
	// 		glVertex3f(10, (float)i, 0);
	// 	}
	// 
	// 	glEnd();
	// 
	// 	//	Create the axies.



	glRotatef(m_XRot,1.0f,0.0f,0.0f);
	glRotatef(m_YRot,0.0f,1.0f,0.0f);
	glRotatef(m_ZRot,0.0f,0.0f,1.0f);
	glScalef(m_fScale,m_fScale,m_fScale);



	glTranslatef(m_XTran,m_YTran,m_ZTran);


	if(m_bViewAxis)
	{
		glBegin(GL_LINES);

		glColor4f(1, 0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(600, 0, 0);

		glColor4f(0, 1, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 600, 0);

		glColor4f(0, 0, 1, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 600);

		glEnd();
	}

	//	glTranslatef(-1.0 * g_nXVoxCnt/2.0,-1.0 * g_nYVoxCnt/2.0,-1.0 * g_nZVoxCnt/2.0);

	if(!m_bComplete)
	{
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
		ST_PARTICLE_POS stParticlePos;
		vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>	VecZBaseRockParticle;

		//float fColor = 0.0f;
		int nRed = 0;int nGreen = 0;int nBlue = 0;
		float fAlpha = 0.0;

		glNewList(10,GL_COMPILE_AND_EXECUTE);

		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
		{
			stParticlePos = iterOutsideData->second;

 			if(stParticlePos.iExternalSideCnt <= 0)
 				continue;

// 			if(stParticlePos.bInOut == false)
//				continue;


			//if(VecZBaseRockParticle[stParticlePos.z][stParticlePos.y][stParticlePos.x].bUse)
			{
				glPushMatrix();

				glTranslatef((stParticlePos.x - g_nXVoxCnt)/10.0f,(stParticlePos.y- g_nYVoxCnt)/10.0f,(stParticlePos.z- g_nZVoxCnt)/10.0f);			// Move Left And Into The Screen

				//gf_GetEfieldColor(stParticlePos.fPorosity,nRed,nGreen,nBlue);

				switch ((int)stParticlePos.sStoneType) // 0:공극, 1:장석, 2:석영, 3:사장석, 4:흑운모, 5:티타나이트, 6:각섬석, 7:녹렴석, 8:녹니석]
				{
				case 0: 
					nRed = 255;
					nGreen = 0;
					nBlue = 0;
					fAlpha = 1.0;
// 					nRed = 255;
// 					nGreen = 255;
// 					nBlue = 255;
// 
// 					fAlpha = 0.25;
					break;
				case 1: 
					nRed = 211;
					nGreen = 165;
					nBlue = 136;
					fAlpha = 1.0;
					break;
				case 2: 
					nRed = 192;
					nGreen = 198;
					nBlue = 198;
					fAlpha = 1.0;
					break;
				case 3: 
					nRed = 213;
					nGreen = 211;
					nBlue = 217;
					fAlpha = 1.0;
					break;
				case 4: 
					nRed = 37;
					nGreen = 39;
					nBlue = 34;
					fAlpha = 0.9;
					break;
				case 5: 
					nRed = 109;
					nGreen = 149;
					nBlue = 43;
					fAlpha = 0.9;
					break;
				case 6: 
					nRed = 32;
					nGreen = 32;
					nBlue = 39;
					fAlpha = 0.8;
					break;
				case 7: 
					nRed = 181;
					nGreen = 171;
					nBlue = 90;
					fAlpha = 0.9;
					break;
				case 8: 
					nRed = 136;
					nGreen = 139;
					nBlue = 106;
					fAlpha = 0.9;
					break;
				default:
					nRed = 255;
					nGreen = 255;
					nBlue = 255;
					fAlpha = 1.0;
					break;
				}

				GLfloat mat_ambient[] = { 0,0,0,0.0 };
				GLfloat mat_diffuse[] = { 0.8,0.8,0.8,1.0 };
				GLfloat mat_emission[] = { 0,0,0,0.0 };
				GLfloat mat_specular[] = { 0.7,0.7,0.7,1.0 };
				GLfloat f_chlorite_lumi[] = { 0.0 };


				glColor4f(float(nRed / 255.0),float(nGreen / 255.0),float(nBlue / 255.0), fAlpha);
// 				glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
// 				glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
// 				glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
// 				//glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
// 				glMaterialfv(GL_FRONT, GL_SHININESS, f_chlorite_lumi);

				glutSolidCube (0.1);
				glPopMatrix();
			}
		}

		glEndList();

		m_bComplete = TRUE;
	}
	else
		glCallList(10);



	glPopAttrib();
	glFlush();

	//	m_rtri+=5.0f;						// Increase The Rotation Variable For The Triangle 
	//	m_rquad-=0.1f;						// Decrease The Rotation Variable For The Quad 
	return TRUE;						// Keep Going
}

int CDlgOpenglView::DrawFilePorosity()
{

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();						// Reset The View


	//	Set up some nice attributes for drawing the grid.
	glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);

	//	Create the grid.

	// 	glBegin(GL_LINES);
	// 
	// 	for (int i = -10; i <= 10; i++)
	// 	{
	// 		glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
	// 		glVertex3f((float)i, -10, 0 );
	// 		glVertex3f((float)i, 10, 0 );
	// 		glVertex3f(-10, (float)i, 0);
	// 		glVertex3f(10, (float)i, 0);
	// 	}
	// 
	// 	glEnd();
	// 
	// 	//	Create the axies.



	glRotatef(m_XRot,1.0f,0.0f,0.0f);
	glRotatef(m_YRot,0.0f,1.0f,0.0f);
	glRotatef(m_ZRot,0.0f,0.0f,1.0f);
	glScalef(m_fScale,m_fScale,m_fScale);



	glTranslatef(m_XTran,m_YTran,m_ZTran);


	if(m_bViewAxis)
	{
		glBegin(GL_LINES);

		glColor4f(1, 0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(600, 0, 0);

		glColor4f(0, 1, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 600, 0);

		glColor4f(0, 0, 1, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 600);

		glEnd();
	}

	//	glTranslatef(-1.0 * g_nXVoxCnt/2.0,-1.0 * g_nYVoxCnt/2.0,-1.0 * g_nZVoxCnt/2.0);

	if(!m_bComplete)
	{
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
		ST_PARTICLE_POS stParticlePos;
		vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>	VecZBaseRockParticle;

		//float fColor = 0.0f;
		int nRed = 0;int nGreen = 0;int nBlue = 0;
		float fAlpha = 0.0;

		glNewList(10,GL_COMPILE_AND_EXECUTE);

		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
		{
			stParticlePos = iterOutsideData->second;

			// 			if(stParticlePos.iExternalSideCnt <= 0)
			// 				continue;


			//if(VecZBaseRockParticle[stParticlePos.z][stParticlePos.y][stParticlePos.x].bUse)
			{
				if(stParticlePos.sStoneType == 0)
				{
					nRed = 255;
					nGreen = 0;
					nBlue = 0;
					fAlpha = 0.8;

				}
				else
				{
					continue;
					nRed = 255;
					nGreen = 255;
					nBlue = 255;
					fAlpha = 0.1;
				}


				glPushMatrix();

				glTranslatef((stParticlePos.x - g_nXVoxCnt)/10.0f,(stParticlePos.y- g_nYVoxCnt)/10.0f,(stParticlePos.z- g_nZVoxCnt)/10.0f);			// Move Left And Into The Screen

				//gf_GetEfieldColor(stParticlePos.fPorosity,nRed,nGreen,nBlue);

			

				GLfloat mat_ambient[] = { 0,0,0,0.0 };
				GLfloat mat_diffuse[] = { 0.8,0.8,0.8,1.0 };
				GLfloat mat_emission[] = { 0,0,0,0.0 };
				GLfloat mat_specular[] = { 0.7,0.7,0.7,1.0 };
				GLfloat f_chlorite_lumi[] = { 0.0 };


				glColor4f(float(nRed / 255.0),float(nGreen / 255.0),float(nBlue / 255.0), fAlpha);
				glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
				//glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
				glMaterialfv(GL_FRONT, GL_SHININESS, f_chlorite_lumi);

				glutSolidCube (0.1);
				glPopMatrix();
			}
		}

		glEndList();

		m_bComplete = TRUE;
	}
	else
		glCallList(10);



	glPopAttrib();
	glFlush();

	//	m_rtri+=5.0f;						// Increase The Rotation Variable For The Triangle 
	//	m_rquad-=0.1f;						// Decrease The Rotation Variable For The Quad 
	return TRUE;						// Keep Going

}

void CDlgOpenglView::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	m_bResizing = true;
}


BOOL CDlgOpenglView::OnEraseBkgnd(CDC* pDC) 
{
	return true;
}

void CDlgOpenglView::OnDestroy() 
{
	CDialog::OnDestroy();



}
BOOL CDlgOpenglView::OnInitDialog()
{
	CDialog::OnInitDialog();

	CClientDC dc(this);

	m_wgl.Begin(&dc);

	glShadeModel(GL_SMOOTH);						// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);			// Black Background
	glClearDepth(1.0f);								// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Really Nice Perspective Calculations

	m_wgl.End();

	return 0;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgOpenglView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	if(m_bTranObj)		// 객체 이동
	{
		if(m_bLButtonDown)
		{

			switch (g_n3DShowEye)
			{
			case 1:		// Top
				m_XTran -= ((m_ptLBDown.x - point.x) / 10.0);
				m_YTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 2:		// bottom
				m_XTran += ((m_ptLBDown.x - point.x) / 10.0);
				m_YTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 3:		// left
				m_YTran -= ((m_ptLBDown.x - point.x) / 10.0);
				m_ZTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 4:		// right
				m_YTran += ((m_ptLBDown.x - point.x) / 10.0);
				m_ZTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 5:		// front
				m_XTran += ((m_ptLBDown.x - point.x) / 10.0);
				m_ZTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 6:		// back
				m_XTran -= ((m_ptLBDown.x - point.x) / 10.0);
				m_ZTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			default:
				m_XTran -= ((m_ptLBDown.x - point.x) / 10.0);
				m_YTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			}

			

			m_ptLBDown = point;
			OnPaint();
		}

// 		if(m_bRButtonDown)
// 		{
// 			m_XTran += ((m_ptRBDown.x - point.x) / 10.0);
// 			m_YTran += ((m_ptRBDown.y - point.y) / 10.0);
// 
// 			m_ptRBDown = point;
// 			OnPaint();
// 		}
	}
	else// 객체 회전
	{
		if(m_bLButtonDown)
		{

			switch (g_n3DShowEye)
			{
			case 1:		// Top
				m_YRot -= ((m_ptLBDown.x - point.x) / 10.0);
				m_XRot -= ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 2:		// bottom
				m_YRot -= ((m_ptLBDown.x - point.x) / 10.0);
				m_XRot += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 3:		// left
				m_ZRot -= ((m_ptLBDown.x - point.x) / 10.0);
				m_YRot -= ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 4:		// right
				m_ZRot -= ((m_ptLBDown.x - point.x) / 10.0);
				m_YRot += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 5:		// front
				m_ZRot -= ((m_ptLBDown.x - point.x) / 10.0);
				m_XRot += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			case 6:		// back
				m_ZRot -= ((m_ptLBDown.x - point.x) / 10.0);
				m_XRot -= ((m_ptLBDown.y - point.y) / 10.0);
				break;
			default:
				m_XTran -= ((m_ptLBDown.x - point.x) / 10.0);
				m_YTran += ((m_ptLBDown.y - point.y) / 10.0);
				break;
			}


// 			m_ZRot += ((m_ptLBDown.x - point.x) / 10.0);
// 			m_fScale += ((m_ptLBDown.y - point.y) / 1000.0);

			m_ptLBDown = point;
			OnPaint();
		}

// 		if(m_bRButtonDown)
// 		{
// 			m_XRot += ((m_ptRBDown.x - point.x) / 10.0);
// 			m_YRot += ((m_ptRBDown.y - point.y) / 10.0);
// 
// 			m_ptRBDown = point;
// 			OnPaint();
// 		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgOpenglView::OnLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_ptLBDown = point;
	m_bLButtonDown = TRUE;
	CDialog::OnNcLButtonDown(nHitTest, point);
}

void CDlgOpenglView::OnLButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_ptLBDown = point;
	m_bLButtonDown = FALSE;
	CDialog::OnNcLButtonUp(nHitTest, point);
}

void CDlgOpenglView::OnRButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_ptRBDown = point;
	m_bRButtonDown = TRUE;
	CDialog::OnNcRButtonDown(nHitTest, point);
}

void CDlgOpenglView::OnRButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_ptRBDown = point;
	m_bRButtonDown = FALSE;
	CDialog::OnNcRButtonUp(nHitTest, point);
}


int CDlgOpenglView::DrawFileObjScene()
{

	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();						// Reset The View

	

	
	//	Set up some nice attributes for drawing the grid.
	glPushAttrib(GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);


// 	glBegin(GL_LINES);
// 
// 	for (int i = -10; i <= 10; i++)
// 	{
// 		glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
// 		glVertex3f((float)i, -10, 0 );
// 		glVertex3f((float)i, 10, 0 );
// 		glVertex3f(-10, (float)i, 0);
// 		glVertex3f(10, (float)i, 0);
// 	}
// 
// 	glEnd();
// 
	

	glRotatef(m_XRot,1.0f,0.0f,0.0f);
	glRotatef(m_YRot,0.0f,1.0f,0.0f);
	glRotatef(m_ZRot,0.0f,0.0f,1.0f);
	glScalef(m_fScale,m_fScale,m_fScale);

	glTranslatef(m_XTran,m_YTran,m_ZTran);


	if(m_bViewAxis)
	{
		glBegin(GL_LINES);

		glColor4f(1, 0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(600, 0, 0);

		glColor4f(0, 1, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 600, 0);

		glColor4f(0, 0, 1, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 600);

		glEnd();
	}

	if(!m_bComplete)
	{

		float triverts[6][3];
		CObjLoader _obj1;
		//	_obj1.Load ("Model/LegoBrick2_1withHole.obj");
		_obj1.Load (CT2A(g_strFilePathName));

		float r[3];

		int vt_size = _obj1.vertexes.size ();
	
		glNewList(10,GL_COMPILE_AND_EXECUTE);

		for (unsigned int h=0; h<_obj1.parts.size (); ++h) 
		{
			vector<sFace> &faces = _obj1.parts[h].faces;

			int nFaceCnt = (int)faces.size();

			for (unsigned int i=0 , ip= nFaceCnt ; i<ip; ++i) 
			{
				int nPartCnt = 0;

				glColor3f(0.2, 0.2, 0.2);
				glBegin(GL_LINE_STRIP);
				
				for (int j=0, jn=faces[i].n; j<jn; ++j) 
				{
					int &v  = faces[i].v[j];
					if (0 < v && v <= vt_size) 
					{
						sVertex &vt = _obj1.vertexes[v - 1];

						glVertex3f(vt.x, vt.y, vt.z);

	// 					triverts[j][0] = vt.x;
	// 					triverts[j][1] = vt.y;
	// 					triverts[j][2] = vt.z;

						if(PartData.fMaxX < vt.z)
							PartData.fMaxX = vt.z;
						if(PartData.fMaxY < vt.y)
							PartData.fMaxY = vt.y;

						if(PartData.fMinX > vt.z)
							PartData.fMinX = vt.z;
						if(PartData.fMinY > vt.y)
							PartData.fMinY = vt.y;

					}
				}
				glEnd();
			}
		}
		glEndList();

		m_bComplete = TRUE;
	}
	else
	{
//		glTranslatef(0.0,0.0f,-1.0 * (PartData.fMaxX - PartData.fMinX) / 2.0);
		glCallList(10);
	}

	glPopAttrib();
	glFlush();

	return TRUE;						// Keep Going
}


