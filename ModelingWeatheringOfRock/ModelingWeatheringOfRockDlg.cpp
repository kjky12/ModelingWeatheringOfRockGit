
// ModelingWeatheringOfRockDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ModelingWeatheringOfRock.h"
#include "ModelingWeatheringOfRockDlg.h"
#include "afxdialogex.h"



#include "BoxinsetersectTriangle.h"
#include <process.h>
#include <algorithm>
#include <fstream>

//! 파이 차트 그래프 클래스
#include "DlgPieDemoDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int			m_nThreadAnalNum;
static int			m_nThreadFinshNum;
static unsigned long long			m_nSolidTotalCnt;

void ThreadSurfaceVoxelization( void* pArguments );

void ThreadSolidVoxelizationX( void* pArguments );
void ThreadSolidVoxelizationY( void* pArguments );
void ThreadSolidVoxelizationZ( void* pArguments );
void ThreadTotalSolidVoxelization( void* pArguments );

//! 외부 단면 계산 스레드
void ThreadCalcExternalSide( void* pArguments );
//! 입상붕괴 시뮬레이션
void ThreadCalcRockAging( void* pArguments );





CString LogPath;
int write_log_file(CString strLog)
{
	CFile logfile;

	if (logfile.Open(LogPath, CFile::modeCreate|CFile::modeNoTruncate | CFile::modeWrite) == FALSE)
	{
		return(1);
	}

	logfile.SeekToEnd();

//	SYSTEMTIME lpSystemTime;
	// 	GetLocalTime(&lpSystemTime); // 현재 날짜, 시간 가져오는 함수
// 	CStringA l_str;
// 	l_str.Format("%02d.%02d.%04d - %02d:%02d:%02d:%03d : ",lpSystemTime.wDay,lpSystemTime.wMonth,lpSystemTime.wYear,lpSystemTime.wHour,lpSystemTime.wMinute,lpSystemTime.wSecond, lpSystemTime.wMilliseconds);
// 
// 
// 	logfile.Write(l_str, l_str.GetLength());
	strLog = strLog + L"\r\n";
	logfile.Write(strLog, strLog.GetLength() * sizeof(TCHAR));
	//logfile.Write((const void *)"\r\n",1);
	logfile.Close();
	return(0);
}

CString GetProgPath()
{
	// get root of Dscorder application
	TCHAR szTemp[300];
	::GetModuleFileName(NULL, szTemp, 300);
	CString path = szTemp;
	if (0 < path.ReverseFind('\\')) {
		path = path.Left(path.ReverseFind('\\'));
	}
	return path;
}


int create_log_file()
{
	SYSTEMTIME lpSystemTime;
	GetLocalTime(&lpSystemTime); // 현재 날짜, 시간 가져오는 함수
	CString l_str;
	l_str.Format(L"%04d%02d%02d%02d%02d%02d",lpSystemTime.wYear,lpSystemTime.wMonth,lpSystemTime.wDay,lpSystemTime.wHour,lpSystemTime.wMinute,lpSystemTime.wSecond);

	CFile logfile;

	LogPath.Format(_T("%s\\%sLogFile.csv"), GetProgPath(), l_str);

	if (logfile.Open(LogPath, CFile::modeCreate|CFile::modeNoTruncate| CFile::modeWrite) == FALSE)
	{
		return(1);
	}
	WORD wd = 0xFEFF;   //unicode 식별자
	logfile.Write(&wd,2);   //unicode 식별자 파일 처음에 쓰자
	//logfile.SeekToEnd();
	logfile.Close();

	return(0);
}



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CModelingWeatheringOfRockDlg 대화 상자



CModelingWeatheringOfRockDlg::CModelingWeatheringOfRockDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelingWeatheringOfRockDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_n3DSeeType = 0;
	g_n3DSeeEye = 1;


	m_nCalcTotalCnt = 0;	// 계산 입자 개수
	m_nCalcTryCnt = 1;		// 계산 횟수


	m_bStopCalc = FALSE;
	m_bTriangle = FALSE;
	m_pDlgOpenglView = NULL;
	m_pDlgOpenglViewerDialog = NULL;
	m_pDlgColorChart = NULL;

	m_nXFileVoxCnt = 0;
	m_nYFileVoxCnt = 0;
	m_nZFileVoxCnt = 0;

	m_bSimulIdx = false;

	for (int v= 0; v < dfTOTAL_COUNT; v++)
	{
		m_bDataTmp[v] = NULL;
	}
	
	m_setDeleteParticle.clear();


	for (int i = 0; i < dfSTONE_TYPE_CNT; i++)
		m_pPDGranularDisintegration[i] = NULL;

}

void CModelingWeatheringOfRockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editXSize);
	DDX_Control(pDX, IDC_EDIT2, m_editYSize);
	DDX_Control(pDX, IDC_EDIT3, m_editZSize);
	DDX_Control(pDX, IDC_EDIT_GR_DIS_CNT, m_editGrDisCnt);
	DDX_Control(pDX, IDC_EDIT_GR_TOTAL_CNT, m_editGrTotalCnt);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT, m_editCalcTryCnt);
	//	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
	DDX_Control(pDX, IDC_FRAME_VIEW, m_frameView);
	DDX_Control(pDX, IDC_EDIT4, m_editParticleSize);
	DDX_Control(pDX, IDC_EDIT_CALC_POROSITY, m_editCalcPorosity);
	DDX_Control(pDX, IDC_EDIT_CALC_MULTI, m_editCalcMulti);
	DDX_Control(pDX, IDC_STATIC_COLOR_CHART, m_staticColorChart);
	//DDX_Control(pDX, IDC_SLIDER1, m_SliderCtrl3DSlideView);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT2, m_editXData);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT3, m_editYData);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT4, m_editZData);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT5, m_editSolidView);
	DDX_Control(pDX, IDC_EDIT_GR_DIS_CNT2, m_EditStopSimulCnt);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT6, m_editXAxiDivide);
	DDX_Control(pDX, IDC_COMBO1, m_comboShow3DType);
	DDX_Control(pDX, IDC_COMBO2, m_combo3DViewEye);
	DDX_Control(pDX, IDC_SPIN1, m_SpinCtrl);
	DDX_Control(pDX, IDC_EDIT5, m_editScaleData);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT7, m_editProcessStatus);
	DDX_Control(pDX, IDC_CHECK1, m_checkSaveStep);
	DDX_Control(pDX, IDC_CHECK2, m_checkRenderObj);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT8, m_editStepCnt);
	DDX_Control(pDX, IDC_CHECK3, m_checkTranObj);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT9, m_editXRot);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT10, m_editYRot);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT11, m_editZRot);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT12, m_editXTran);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT13, m_editYTran);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT14, m_editZTran);
	DDX_Control(pDX, IDC_CHECK4, m_checkViewAxis);
	DDX_Control(pDX, IDC_EDIT_GR_DIS_CNT3, m_editTotalSolidCnt);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT15, m_editVoxelAxisCnt);
	DDX_Control(pDX, IDC_COMBO3, m_comboSelectAxis);
	DDX_Control(pDX, IDC_EDIT_CALC_MULTI2, m_editTopRate);
	DDX_Control(pDX, IDC_EDIT_CALC_MULTI3, m_editSideRate);
	DDX_Control(pDX, IDC_EDIT_CALC_MULTI4, m_editBottomRate);
	DDX_Control(pDX, IDC_EDIT_CALC_TRY_CNT16, m_editSaveInterval);
	DDX_Control(pDX, IDC_EDIT_INIT_POROSITY, m_editInitPorosity);
	DDX_Control(pDX, IDC_EDIT_CALC_POROSITY2, m_editGranularDisintegration);
	DDX_Control(pDX, IDC_COMBO_DATA_PROCESS, m_comboDataProcess);
	//DDX_Control(pDX, IDC_CHECK5, m_cudaCheck);
	DDX_Control(pDX, IDC_COMBO_USE_CUDA, m_comboUseCuda);

	DDX_Text(pDX,IDC_EDIT_FELDSPAR_PER	  ,m_strEditFeldsparPer);
	DDX_Text(pDX,IDC_EDIT_QUARTZ_PER	  ,m_strEditQuartzPer);
	DDX_Text(pDX,IDC_EDIT_PLAGIOCLASE_PER ,m_strEditPlagioclasePer);
	DDX_Text(pDX,IDC_EDIT_BIOTITE_PER	  ,m_strEditBiotitePer);
	DDX_Text(pDX,IDC_EDIT_TITANITE_PER	  ,m_strEditTitanitePer);
	DDX_Text(pDX,IDC_EDIT_HORNBLENDE_PER  ,m_strEditHornblendePer);
	DDX_Text(pDX,IDC_EDIT_EPIDOTE_PER	  ,m_strEditEpidotePer);
	DDX_Text(pDX,IDC_EDIT_CHLORITE_PER	  ,m_strEditChloritePer);
	DDX_Text(pDX,IDC_EDIT_POROSITY_PER	  ,m_strEditPorosityPer);
	DDX_Text(pDX,IDC_EDIT_FELDSPAR_DIS	  ,m_strEditFeldsparDis);
	DDX_Text(pDX,IDC_EDIT_QUARTZ_DIS	  ,m_strEditQuartzDis);
	DDX_Text(pDX,IDC_EDIT_PLAGIOCLASE_DIS ,m_strEditPlagioclaseDis);
	DDX_Text(pDX,IDC_EDIT_BIOTITE_DIS	  ,m_strEditBiotiteDis);
	DDX_Text(pDX,IDC_EDIT_TITANITE_DIS	  ,m_strEditTitaniteDis);
	DDX_Text(pDX,IDC_EDIT_HORNBLENDE_DIS  ,m_strEditHornblendeDis);
	DDX_Text(pDX,IDC_EDIT_EPIDOTE_DIS	  ,m_strEditEpidoteDis);
	DDX_Text(pDX,IDC_EDIT_CHLORITE_DIS	  ,m_strEditChloriteDis);
	DDX_Text(pDX,IDC_EDIT_FELDSPAR_DEL	  ,m_strEditFeldsparDel);
	DDX_Text(pDX,IDC_EDIT_QUARTZ_DEL	  ,m_strEditQuartzDel);
	DDX_Text(pDX,IDC_EDIT_PLAGIOCLASE_DEL ,m_strEditPlagioclaseDel);
	DDX_Text(pDX,IDC_EDIT_BIOTITE_DEL	  ,m_strEditBiotiteDel);
	DDX_Text(pDX,IDC_EDIT_TITANITE_DEL	  ,m_strEditTitaniteDel);
	DDX_Text(pDX,IDC_EDIT_HORNBLENDE_DEL  ,m_strEditHornblendeDel);
	DDX_Text(pDX,IDC_EDIT_EPIDOTE_DEL	  ,m_strEditEpidoteDel);
	DDX_Text(pDX,IDC_EDIT_CHLORITE_DEL	  ,m_strEditChloriteDel);
	DDX_Text(pDX,IDC_EDIT_POROSITY_CNT	  ,m_strEditPorosityCnt);

	DDX_Control(pDX, IDC_EDIT_CALC_WATER_INNER_ABSORPTION, m_editCalcWaterInnerAbsorption);
	DDX_Control(pDX, IDC_EDIT_CALC_LAYER_WATER_INNER_ABSORPTION, m_editCalcLayerWaterAborption);
	DDX_Control(pDX, IDC_EDIT_CALC_WATER_CHANGE, m_editCalcWaterChange);
	DDX_Control(pDX, IDC_COMBO_USE_BLOCK_THREAD, m_comboBlockThreadSel);
	DDX_Control(pDX, IDC_COMBO_PROCESS_CALC_ROCKING, m_comboProcessCalcRocking);
}































BEGIN_MESSAGE_MAP(CModelingWeatheringOfRockDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MODEL_DATA, &CModelingWeatheringOfRockDlg::OnBnClickedButtonCreateModelData)
	ON_BN_CLICKED(IDC_BUTTON_CALC_EXTERNAL_SIDE, &CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcExternalSide)
	ON_BN_CLICKED(IDC_BUTTON_CALC_ROCK_AGING, &CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAging)
	ON_WM_DESTROY()

	ON_MESSAGE(WM_FINISH_SIM_ONE_STEP_MSG,CModelingWeatheringOfRockDlg::OnCalcRockAgingMsg)
	ON_MESSAGE(WM_FINISH_VOXEL_MSG,CModelingWeatheringOfRockDlg::OnFinshVoxelMsg)
	ON_MESSAGE(WM_FINISH_SOLID_VOXEL_MSG,CModelingWeatheringOfRockDlg::OnFinshSolidVoxelMsg)

	ON_MESSAGE(WM_CHANGE_POS_ROT_MSG,CModelingWeatheringOfRockDlg::OnChangePosRotMsg)


	ON_BN_CLICKED(IDC_BUTTON_CALC_ROCK_AGING_CANCEL, &CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAgingCancel)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MODEL_DATA2, &CModelingWeatheringOfRockDlg::OnBnClickedButtonCreateModelData2)
	ON_BN_CLICKED(IDOK, &CModelingWeatheringOfRockDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OBJ_FILE_MODELING, &CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling)
//	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &CModelingWeatheringOfRockDlg::OnTRBNThumbPosChangingSlider1)
	//ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CModelingWeatheringOfRockDlg::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_BUTTON_SOLID_DATA, &CModelingWeatheringOfRockDlg::OnBnClickedButtonSolidData)
	ON_BN_CLICKED(IDC_BUTTON_CALC_ROCK_AGING2, &CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAging2)
	ON_BN_CLICKED(IDC_BUTTON_CALC_ROCK_AGING3, &CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAging3)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CModelingWeatheringOfRockDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON_SOLID_DATA2, &CModelingWeatheringOfRockDlg::OnBnClickedButtonSolidData2)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CModelingWeatheringOfRockDlg::OnCbnSelchangeCombo2)
	
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CModelingWeatheringOfRockDlg::OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_BUTTON_OBJ_FILE_MODELING3, &CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling3)
	ON_BN_CLICKED(IDC_CHECK3, &CModelingWeatheringOfRockDlg::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_BUTTON_OBJ_FILE_MODELING4, &CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling4)
	ON_BN_CLICKED(IDC_CHECK4, &CModelingWeatheringOfRockDlg::OnBnClickedCheck4)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CModelingWeatheringOfRockDlg::OnCbnSelchangeCombo3)
	ON_EN_CHANGE(IDC_EDIT_CALC_TRY_CNT16, &CModelingWeatheringOfRockDlg::OnEnChangeEditCalcTryCnt16)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA_PROCESS, &CModelingWeatheringOfRockDlg::OnCbnSelchangeComboDataProcess)
	ON_BN_CLICKED(IDC_BUTTON1, &CModelingWeatheringOfRockDlg::OnBnClickedButton1)
	
	ON_BN_CLICKED(IDC_BUTTON3, &CModelingWeatheringOfRockDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CModelingWeatheringOfRockDlg::OnBnClickedButton4)

	ON_MESSAGE(WM_SIMULATION_VOXEL,&CModelingWeatheringOfRockDlg::OnSimulationMsg)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_RATE_STONE, &CModelingWeatheringOfRockDlg::OnBnClickedButtonViewRateStone)
	ON_BN_CLICKED(IDC_BUTTON_SEE, &CModelingWeatheringOfRockDlg::OnBnClickedButtonSee)
	ON_CBN_SELCHANGE(IDC_COMBO_USE_BLOCK_THREAD, &CModelingWeatheringOfRockDlg::OnCbnSelchangeComboUseBlockThread)
	ON_CBN_SELCHANGE(IDC_COMBO_PROCESS_CALC_ROCKING, &CModelingWeatheringOfRockDlg::OnCbnSelchangeComboProcessCalcRocking)
	
	ON_BN_CLICKED(IDC_BUTTON5, &CModelingWeatheringOfRockDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON_OBJ_FILE_MODELING5, &CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling5)
	ON_BN_CLICKED(IDC_BUTTON6, &CModelingWeatheringOfRockDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CModelingWeatheringOfRockDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CModelingWeatheringOfRockDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// CModelingWeatheringOfRockDlg 메시지 처리기

BOOL CModelingWeatheringOfRockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//! 프로그램 설정
	m_comboDataProcess.AddString(L"Memory process");
	m_comboDataProcess.AddString(L"File process");
	m_comboDataProcess.SetCurSel(dfDATA_PROCESS_MEMORY);
	m_nComboProcessSel = dfDATA_PROCESS_MEMORY;


// 	m_comboUseCuda.AddString(L"CPU로직-CPU처리");
// 	m_comboUseCuda.AddString(L"GPU로직-GPU처리");
	m_comboUseCuda.AddString(L"CPU");
	m_comboUseCuda.AddString(L"GPU");
	m_comboUseCuda.SetCurSel(dfLOGIC_CPU_CPU);
	//m_comboUseCuda.SetCurSel(dfLOGIC_GPU_GPU);
	m_nComboLogicProcess = m_comboUseCuda.GetCurSel();
	
	m_comboBlockThreadSel.AddString(L"축별 Block[3개]");
	m_comboBlockThreadSel.AddString(L"축별 Thread[3개]");
	m_comboBlockThreadSel.SetCurSel(dfLOGIC_CPU_CPU);
	m_nComboBlockThreadSel = m_comboBlockThreadSel.GetCurSel();


	m_editInitPorosity.SetWindowTextW(L"1");
	m_editGranularDisintegration.SetWindowTextW(L"7");
	m_editCalcPorosity.SetWindowTextW(L"0.5");

	m_comboProcessCalcRocking.AddString(L"GPU");
	m_comboProcessCalcRocking.AddString(L"CPU");
	m_comboProcessCalcRocking.SetCurSel(0);

	gf_Init();

	m_SpinCtrl.SetRange32(-1000,1000);
	m_SpinCtrl.SetPos(0);

	m_comboShow3DType.AddString(L"물성값");
	m_comboShow3DType.AddString(L"외부 노출 단면");
	m_comboShow3DType.AddString(L"입자 색상");
	m_comboShow3DType.AddString(L"공극 보기");
	m_comboShow3DType.AddString(L"폴리곤");
	m_comboShow3DType.SetCurSel(1);
	g_n3DShowType = m_comboShow3DType.GetCurSel();
	
	m_combo3DViewEye.AddString(L"User");
	m_combo3DViewEye.AddString(L"Top");
	m_combo3DViewEye.AddString(L"Bottom");
	m_combo3DViewEye.AddString(L"Left");
	m_combo3DViewEye.AddString(L"Right");
	m_combo3DViewEye.AddString(L"Front");
	m_combo3DViewEye.AddString(L"Back");
	m_combo3DViewEye.SetCurSel(0);

	m_comboSelectAxis.SetCurSel(0);

	m_editScaleData.SetWindowText(L"0");
	m_editXSize.SetWindowText(L"0.01");
	m_editYSize.SetWindowText(L"0.01");
	m_editZSize.SetWindowText(L"0.01");
	m_editParticleSize.SetWindowText(L"1");
	g_fRockParticleSize = 1.0;

	m_EditStopSimulCnt.SetWindowText(L"200");
//	m_editCalcPorosity.SetWindowText(L"0.06016");
	m_editCalcMulti.SetWindowText(L"1.0");

	m_editCalcWaterInnerAbsorption.SetWindowText(L"0.8");
	m_editCalcLayerWaterAborption.SetWindowText(L"0.2");
	m_editCalcWaterChange.SetWindowText(L"1.0905"); //0도일때 부피 커짐
	//m_editCalcWaterChange.SetWindowText(L"1.0901"); //-4도일때 부피 커짐


	m_editTopRate.SetWindowText(L"1.0");
	m_editSideRate.SetWindowText(L"0.3");
	m_editBottomRate.SetWindowText(L"0.05");

	m_fTopRate = 1.0;
	m_fSideRate = 0.3;
	m_fBottomRate = 0.05;

	m_editSaveInterval.SetWindowText(L"10");

	//m_ProgressCtrl.SetRange(0,100);


	m_checkSaveStep.SetCheck(TRUE);
	m_checkRenderObj.SetCheck(TRUE);

	CRect rcTmp;
	m_frameView.GetWindowRect(&rcTmp);
	ScreenToClient(&rcTmp);


	if(!m_pDlgOpenglView)
	{
		m_pDlgOpenglView = new CDlgOpenglView();
		m_pDlgOpenglView->Create(IDD_OPENGL_VIEW_DIALOG,this);

		m_pDlgOpenglView->MoveWindow(rcTmp);
		m_pDlgOpenglView->ShowWindow(SW_SHOW);

	}


	if(!m_pDlgOpenglViewerDialog)
	{
		m_pDlgOpenglViewerDialog = new CDlgOpenglViewerDialog();
		m_pDlgOpenglViewerDialog->Create(IDD_OPENGL_VIEWER_DIALOG,this);
		m_pDlgOpenglViewerDialog->ShowWindow(SW_HIDE);
// 		m_pDlgOpenglView->MoveWindow(rcTmp);
// 		m_pDlgOpenglView->ShowWindow(SW_SHOW);
	}


	m_staticColorChart.GetWindowRect(&rcTmp);
	ScreenToClient(&rcTmp);

	if(!m_pDlgColorChart)
	{
		m_pDlgColorChart = new CDlgColorChart(this, 0);
		m_pDlgColorChart->Create(IDD_COLOR_CHART_DIALOG,this);



		m_pDlgColorChart->MoveWindow(rcTmp);
		m_pDlgColorChart->ShowWindow(SW_SHOW);

	}

	// 슬라이더 컨트롤 설정
	//m_SliderCtrl3DSlideView.SetRange(0,100);



	// 공극률 설정을 위한 확률변수 설정
	double dInitPorosity = 0.005670416;
	CProbDistribution PDPorosity(4,0.0,0.0,0.0001,dInitPorosity,0.0,0.0,0.0,0.0,0.0);


	// 입상붕괴 도달값 계산을 위한 확률변수 설정
	double dGranularDisintegration = 1.0;
	CProbDistribution PDGranularDisintegration(3,0.0,dGranularDisintegration,0.001,0.0,0.0,0.0,0.0,0.0,0.0);


	
	int n = 0;
	for(n = 0 ; n < 100 ; n++)
	{
		float fPorosity = PDPorosity.GetDistParam();									// 공극률
		float fGranularDisintegration = PDGranularDisintegration.GetDistParam();		// 입상붕괴 도달값 

		TRACE2("%f	%f\n",fPorosity,fGranularDisintegration);
	}


	//! gpu 상태 얻어오기
	OnBnClickedButton3();

	//! 로그 파일 생성
	create_log_file();

	UpdateData(TRUE);

	m_strEditFeldsparDis= L"6";						//장석
	m_strEditQuartzDis= L"7";					    //석영
	m_strEditPlagioclaseDis= L"5";				    //사장석
	m_strEditBiotiteDis= L"5.5";				    //흑운모
	m_strEditTitaniteDis= L"3";					    //티타나이트
	m_strEditHornblendeDis= L"4";					//각섬석
	m_strEditEpidoteDis= L"5";						//녹렴석
	m_strEditChloriteDis= L"6";

	/*입자별 비율 초기화*/
	m_strEditFeldsparPer= L"40";					//장석
	m_strEditQuartzPer= L"30";						//석영
	m_strEditPlagioclasePer= L"20";				//사장석
	m_strEditBiotitePer= L"4";						//흑운모
	m_strEditTitanitePer= L"1";					//티타나이트
	m_strEditHornblendePer= L"2";					//각섬석
	m_strEditEpidotePer= L"1";						//녹렴석
	m_strEditChloritePer= L"1";					//녹니석
	m_strEditPorosityPer= L"1";					//공극

	UpdateData(FALSE);


	InitEditValue();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CModelingWeatheringOfRockDlg::InitEditValue()
{
	UpdateData(TRUE);

	/*입자별 삭제된 복셀수 초기화*/
	m_strEditFeldsparDel= L"0";						//장석
	m_strEditQuartzDel= L"0";						//석영
	m_strEditPlagioclaseDel= L"0";				    //사장석
	m_strEditBiotiteDel= L"0";						//흑운모
	m_strEditTitaniteDel= L"0";					    //티타나이트
	m_strEditHornblendeDel= L"0";					//각섬석
	m_strEditEpidoteDel= L"0";						//녹렴석
	m_strEditChloriteDel= L"0";					    //녹니석


	m_strEditPorosityCnt= L"0";					    //공극

	UpdateData(FALSE);

}

void CModelingWeatheringOfRockDlg::OnDestroy()
{
	CDialog::OnDestroy();

	M_W_DELETE(m_pDlgOpenglView);
	M_W_DELETE(m_pDlgOpenglViewerDialog);
	
	M_W_DELETE(m_pDlgColorChart);

	for (int v= 0; v < dfTOTAL_COUNT; v++)
	{
		M_A_DELETE(m_bDataTmp[v]);
	}

	for (int i = 0; i < dfSTONE_TYPE_CNT; i++)
		M_DELETE(m_pPDGranularDisintegration[i]);
}


void CModelingWeatheringOfRockDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CModelingWeatheringOfRockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}



}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CModelingWeatheringOfRockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void ThreadCreateModelData( void* pArguments )
{
	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;

	float fXSize = 0.0;
	float fYSize = 0.0;
	float fZSize = 0.0;

	CString strTmp = L"";
	pParent->m_editXSize.GetWindowText(strTmp);
	fXSize = _wtof(strTmp) * (1000.0 / g_fRockParticleSize);
	pParent->m_editYSize.GetWindowText(strTmp);
	fYSize = _wtof(strTmp) * (1000.0 / g_fRockParticleSize);
	pParent->m_editZSize.GetWindowText(strTmp);
	fZSize = _wtof(strTmp) * (1000.0 / g_fRockParticleSize);



	ST_BASE_ROCK_PARTICLE stBaseRockParticle;

	// 입상붕괴 도달값 계산을 위한 확률변수 설정
	CString strGranularDisintegration = L"";
	pParent->m_editGranularDisintegration.GetWindowText(strGranularDisintegration);
	double dGranularDisintegration = _wtof(strGranularDisintegration);
	CProbDistribution PDGranularDisintegration(3,0.0,dGranularDisintegration,0.001,0.0,0.0,0.0,0.0,0.0,0.0);
	// 공극률 설정을 위한 확률변수 설정
	CString strInitData = L"";
	pParent->m_editInitPorosity.GetWindowText(strInitData);
	double dInitPorosity = _wtof(strInitData);
	CProbDistribution PDPorosity(4,0.0,0.0,0.0001,dInitPorosity,0.0,0.0,0.0,0.0,0.0);



	map<int,vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>>::iterator	iterBaseRockData;	
	vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>	VecZBaseRockParticle;
	vector<vector<ST_BASE_ROCK_PARTICLE>>			VecYBaseRockParticle;
	vector<ST_BASE_ROCK_PARTICLE>					VecXBaseRockParticle;

	VecZBaseRockParticle.clear();
	
	if(!pParent->m_bTriangle)
	{
		for(int z = 0 ; z < fZSize ; z++)
		{
			VecYBaseRockParticle.clear();
			for(int y = 0 ; y < fYSize ; y++)
			{
				VecXBaseRockParticle.clear();
				for(int x = 0 ; x < fXSize ; x++)
				{

					memset(&stBaseRockParticle,NULL,sizeof(ST_BASE_ROCK_PARTICLE));
					stBaseRockParticle.bUse = true;																// 사용여부 
					//				int nColor = rand()%255;
					//				stBaseRockParticle.crColor = RGB(nColor,nColor,nColor);										// 색상
					stBaseRockParticle.fPorosity = PDPorosity.GetDistParam();									// 공극률
					stBaseRockParticle.fGranularDisintegration = PDGranularDisintegration.GetDistParam();		// 입상붕괴 도달값 

					VecXBaseRockParticle.push_back(stBaseRockParticle);


				}
				VecYBaseRockParticle.push_back(VecXBaseRockParticle);
			}
			VecZBaseRockParticle.push_back(VecYBaseRockParticle);

//			pParent->m_ProgressCtrl.SetPos((z/fZSize)*40);
		}
	}
	else
	{

		for(int z = 0 ; z < fZSize ; z++)
		{
			VecYBaseRockParticle.clear();
			for(int y = 0 ; y < fYSize ; y++)
			{
				VecXBaseRockParticle.clear();
				for(int x = 0 ; x < fXSize ; x++)
				{

					memset(&stBaseRockParticle,NULL,sizeof(ST_BASE_ROCK_PARTICLE));


					if(fXSize - z > x)
					{
						stBaseRockParticle.bUse = true;																// 사용여부 
						stBaseRockParticle.fPorosity = PDPorosity.GetDistParam();									// 공극률
						stBaseRockParticle.fGranularDisintegration = PDGranularDisintegration.GetDistParam();		// 입상붕괴 도달값 
					}
					else
					{
						stBaseRockParticle.bUse = false;																// 사용여부 
						stBaseRockParticle.fPorosity = 0.0;
						stBaseRockParticle.fGranularDisintegration = 0.0;
					}

					VecXBaseRockParticle.push_back(stBaseRockParticle);


				}
				VecYBaseRockParticle.push_back(VecXBaseRockParticle);
			}
			VecZBaseRockParticle.push_back(VecYBaseRockParticle);

//			pParent->m_ProgressCtrl.SetPos((z/fZSize)*40);
		}

	}

	g_MapBaseRockData.insert(make_pair(0,VecZBaseRockParticle));

//	pParent->m_ProgressCtrl.SetPos(100);


	pParent->SendMessage(WM_FINISH_SIM_ONE_STEP_MSG,1,0);
	
}




void CModelingWeatheringOfRockDlg::OnBnClickedButtonCreateModelData()
{


	CString strTmp = L"";
	m_editParticleSize.GetWindowText(strTmp);
	g_fRockParticleSize = _wtof(strTmp);

	g_MapBaseRockData.clear();
	g_MapOutsideData.clear();
	m_mapInsertParticle.clear();
	m_nCalcTotalCnt = 0;	// 계산 입자 개수
	m_nCalcTryCnt = 0;		// 계산 횟수

	m_bTriangle = TRUE;

	m_bStopCalc = FALSE;
	_beginthread( ThreadCreateModelData, 0, this);

	

}

void CModelingWeatheringOfRockDlg::OnBnClickedButtonCreateModelData2()
{
	CString strTmp = L"";
	m_editParticleSize.GetWindowText(strTmp);
	g_fRockParticleSize = _wtof(strTmp);


	g_MapBaseRockData.clear();
	g_MapOutsideData.clear();
	m_mapInsertParticle.clear();
	m_nCalcTotalCnt = 0;	// 계산 입자 개수
	m_nCalcTryCnt = 0;		// 계산 횟수

	m_bTriangle = FALSE;

	m_bStopCalc = FALSE;
	_beginthread( ThreadCreateModelData, 0, this);
}



// 솔리드 파일에서 내부 셀 사용 여부 적용 [2/9/2014 ART]
void CModelingWeatheringOfRockDlg::DeleteCellToSolidFile(int nX, int nY, int nZ)
{
	CString strSolPath = gf_GetModulePath() + L"SolidPos_del.dat";
	FILE *p_Solfile = _wfopen(strSolPath, L"r+b"); 

	int nYVoxCnt = 0;
	int nXVoxCnt = 0;
	int nZVoxCnt = 0;
	bool bSol = false;
	if(NULL != p_Solfile)
	{ 

		int nSeekSize = (sizeof(int) * 3) + (sizeof(bool) *((nY *(nXVoxCnt * nZVoxCnt)) + (nX * nZVoxCnt) + nZ)) ;
		if(nSeekSize > 0)
		{
			fread(&bSol, sizeof(bool), 1, p_Solfile); 
			bSol = false;
			fwrite(&bSol, sizeof(bool), 1, p_Solfile); 
		}
		fclose(p_Solfile); 
	}

}


unsigned int CModelingWeatheringOfRockDlg::GetTotalCntFromSolidFile()
{
	
	CString strSolPath = gf_GetModulePath() + L"SolidVoxel.dat";
	FILE *p_Solfile = _wfopen(strSolPath, L"rb"); 

	unsigned long long nTotalCnt = 0;
	unsigned long long nXVoxCnt = 11;
	unsigned long long nYVoxCnt = 29;
	unsigned long long nZVoxCnt = 40;


	
		if(NULL != p_Solfile)
		{
			fread(&nXVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 
			fread(&nYVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 
			fread(&nZVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 

			unsigned long long nSeekSize = (sizeof(unsigned long long) * 3) + (sizeof(bool) * (nXVoxCnt *nYVoxCnt * nZVoxCnt));
			_fseeki64(p_Solfile,nSeekSize ,SEEK_SET);
			fread(&nTotalCnt, sizeof(unsigned long long), 1, p_Solfile); 

			fclose(p_Solfile); 
		}

	
	CString strTmp = L"";

	strTmp.Format(L"X : %d, Y : %d, Z : %d",nXVoxCnt,nYVoxCnt,nZVoxCnt);
	m_editVoxelAxisCnt.SetWindowTextW(strTmp);

	return nTotalCnt;
}

// 솔리드 파일에서 내부 셀 사용 여부 판단 [2/9/2014 ART]
bool CModelingWeatheringOfRockDlg::GetUsedCellFromSolidFile(int nX, int nY, int nZ)
{
	

	set<CString>::iterator iterTmp;
	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTmp;


		CString strKey;
		strKey.Format(L"%d-%d-%d",nX,nY,nZ);
		iterTmp = m_setDeleteParticle.find(strKey);

		if(iterTmp != m_setDeleteParticle.end())
		{
			return false;
		}
	
	unsigned long long nXVoxCnt = 0;
	unsigned long long nYVoxCnt = 0;
	unsigned long long nZVoxCnt = 0;
	bool nSol = 0;


	FILE *p_Solfile = NULL; 
	// 	iterOutsideDataTmp = g_MapOutsideData.find(strKey);
	// 	
	// 	if(iterOutsideDataTmp != g_MapOutsideData.end())
	// 		return false;

	//! 메모리 처리
	if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{
		nXVoxCnt= m_nXFileVoxCnt;
		nYVoxCnt= m_nYFileVoxCnt;
		nZVoxCnt= m_nZFileVoxCnt;
	}
	else //! 파일 처리
	{
		CString strSolPath = gf_GetModulePath() + L"SolidVoxel.dat";
		p_Solfile = _wfopen(strSolPath, L"rb"); 

		if(NULL != p_Solfile)
		{
			fread(&nXVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 
			fread(&nYVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 
			fread(&nZVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 	 
		}
	}

	if((nY < nYVoxCnt && nX< nXVoxCnt && nZ < nZVoxCnt) && (nY >=0 && nX >=0 && nZ >=0))
	{
		int nGap = (sizeof(unsigned long long) * 3);
		unsigned long long nSeekSize = nGap + (sizeof(bool) * (nX + (nXVoxCnt * nY) + (nXVoxCnt * nYVoxCnt * nZ)));
		//! 메모리 처리
		if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
		{
			nSol = g_vecVoxelTotal[nSeekSize - nGap];
		}
		else //! 파일 처리
		{
			_fseeki64(p_Solfile,nSeekSize ,SEEK_SET);
			fread(&nSol, sizeof(bool), 1, p_Solfile); 
		}
	}
	else
	{
		nSol = 0;
	}

	if(NULL != p_Solfile)
		fclose(p_Solfile); 



	

	return nSol;
}

//! 외부 단면 계산(상하 좌우 앞뒤를 확인하여 근접 복셀의 유무로 외부 복셀인지 판단.)
void CModelingWeatheringOfRockDlg::CalcExternalSide(int x,int y,int z,int nXVoxCnt, int nYVoxCnt, int nZVoxCnt, ST_PARTICLE_POS& stParticlePos)
{
	float fXSize = 0.0;
	float fYSize = 0.0;
	float fZSize = 0.0;
	
	CString strKey = L"";
	CString strTmp = L"";

	stParticlePos.iExternalSideCnt = 0;
	//stParticlePos.vecExternalSide.clear();
	memset(stParticlePos.abExternalSide, 0, 6);
	//stParticlePos.iExternalSideCnt = 6;
	

	//if(z-1 >= 0 && z-1 < nZVoxCnt)
	{
		if(!GetUsedCellFromSolidFile(x,y,z-1))
		{
			stParticlePos.iExternalSideCnt++;
			//stParticlePos.vecExternalSide.push_back(1);
			stParticlePos.abExternalSide[1] = TRUE;
		}
	}

	//if(z+1 > 0 && z+1 < nZVoxCnt)
	{
		if(!GetUsedCellFromSolidFile(x,y,z+1))
		{
			stParticlePos.iExternalSideCnt++;
			//stParticlePos.vecExternalSide.push_back(0);
			stParticlePos.abExternalSide[0] = TRUE;
		}
	}

	//if(y-1 >= 0 && y-1 < nYVoxCnt)
	{
		if(!GetUsedCellFromSolidFile(x,y-1,z))
		{
			stParticlePos.iExternalSideCnt++;
			//stParticlePos.vecExternalSide.push_back(2);
			stParticlePos.abExternalSide[2] = TRUE;
		}
	}

	//if(y+1 > 0 && y+1 < nYVoxCnt)
	{
		if(!GetUsedCellFromSolidFile(x,y+1,z))
		{
			stParticlePos.iExternalSideCnt++;
			//stParticlePos.vecExternalSide.push_back(3);
			stParticlePos.abExternalSide[3] = TRUE;
		}
	}

	//if(x-1 >= 0 && x-1 < nXVoxCnt)
	{
		if(!GetUsedCellFromSolidFile(x-1,y,z))
		{
			stParticlePos.iExternalSideCnt++;
			//stParticlePos.vecExternalSide.push_back(4);
			stParticlePos.abExternalSide[4] = TRUE;
		}
	}

	//if(x+1 > 0 && x+1 < nXVoxCnt)
	{
		if(!GetUsedCellFromSolidFile(x+1,y,z))
		{
			stParticlePos.iExternalSideCnt++;
			//stParticlePos.vecExternalSide.push_back(5);
			stParticlePos.abExternalSide[5] = TRUE;
		}
	}
}

//! 외부 단면 계산 스레드
void ThreadCalcExternalSide( void* pArguments )
{
	/*
	Detail:	복셀화 이후 입자 위치 구조체의 데이터를 입력한다.
	사용여부 , 공극률(랜덤 분포도 - 라일리), 외부 노출 단면 개수, 외부 노출 단면 [0:상,1:하,2:좌,3:우,4:앞,5:뒤], 입상붕괴 도달값 (랜덤 분포도 - 가우시안)
	*/

	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;


	CString strProcessStatus;
	strProcessStatus.Format(L"Calculation of Exposed Faces to Exterior");
	pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);


	CString strNumPath = gf_GetModulePath() + L"SurfaceNum.dat";
	FILE *p_Numfile =  NULL; 

	g_MapOutsideData.clear();
	CString strKey = L"";

	// 입상붕괴 도달값 계산을 위한 확률변수 설정
// 	CString strGranularDisintegration = L"";
// 	pParent->m_editGranularDisintegration.GetWindowText(strGranularDisintegration);
// 	double dGranularDisintegration = _wtof(strGranularDisintegration);
// 	CProbDistribution PDGranularDisintegration(3,0.0,dGranularDisintegration,0.001,0.0,0.0,0.0,0.0,0.0,0.0);
	// 공극률 설정을 위한 확률변수 설정
	CString strInitData = L"";
	pParent->m_editInitPorosity.GetWindowText(strInitData);
	double dInitPorosity = _wtof(strInitData);
	CProbDistribution PDPorosity(4,0.0,0.0,0.0001,dInitPorosity,0.0,0.0,0.0,0.0,0.0);

	map<int,int>::iterator iterStoneCnt;
	map<int,int>::iterator iterStoneLayerCnt;

	ST_VOXEL_FILE_POS stVoxelFilePos;
	map<CString,ST_VOXEL_3D_POS>::iterator	iterCompleteVoxelPos;


	int nTotalCnt = 0;
	g_nXVoxCnt = pParent->m_nXFileVoxCnt / 2;
	g_nYVoxCnt = pParent->m_nYFileVoxCnt / 2;
	g_nZVoxCnt = pParent->m_nZFileVoxCnt / 2;

	//! 메모리 처리
	if(pParent->m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{
		//nTotalCnt = (int)g_vecCompleteVoxelPosSolid.size();
		nTotalCnt = (int)g_vecCompleteVoxelPos.size();
		
	}
	else // 파일 처리
	{
		p_Numfile = _wfopen(strNumPath, L"rb"); 
		if(NULL != p_Numfile )
		{ 
			fread(&pParent->m_nXFileVoxCnt, sizeof(int), 1, p_Numfile); 
			fread(&pParent->m_nYFileVoxCnt, sizeof(int), 1, p_Numfile); 
			fread(&pParent->m_nZFileVoxCnt, sizeof(int), 1, p_Numfile); 

			fread(&nTotalCnt, sizeof(int), 1, p_Numfile); 
		}
	}


	//float fMaxDistance = sqrt(pow(pParent->m_nXFileVoxCnt / 2.0, 2) + pow(pParent->m_nYFileVoxCnt / 2.0, 2) + pow(pParent->m_nZFileVoxCnt / 2.0, 2) );
	//! 기준을 가장 긴 축으로 해서 만들어야 레이어 개수가 딱 맞는다.
	float nMax = 0.0;
	nMax =  max(pParent->m_nXFileVoxCnt, pParent->m_nYFileVoxCnt);
	nMax =  max(nMax, pParent->m_nZFileVoxCnt);
	float fMaxDistance = nMax / 2.0;

	//! 외부만드는 횟수로 레이어를 나누어주기위해
	int nIndexNum = 0;
	//////////////////////////////////////////////////////////////////////////
	// 첫 외부는 파일 및 기존 메모리에서 사용한다.
	for(int nVox = 0  ; nVox < nTotalCnt  ; nVox++)
	{
		ST_PARTICLE_POS stParticlePos;

		//! 메모리 처리
		if(pParent->m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
		{
			stVoxelFilePos = g_vecNumData[nVox];
			//stVoxelFilePos.nX = g_vecCompleteVoxelPosSolid[nVox].nX;
			//stVoxelFilePos.nY = g_vecCompleteVoxelPosSolid[nVox].nY;
			//stVoxelFilePos.nZ = g_vecCompleteVoxelPosSolid[nVox].nZ;
		}
		else //파일처리
		{
			if(NULL != p_Numfile )
			{ 
				fread(&stVoxelFilePos, sizeof(ST_VOXEL_FILE_POS), 1, p_Numfile); 
			}
		}

		stParticlePos.x = stVoxelFilePos.nX;
		stParticlePos.y = stVoxelFilePos.nY;
		stParticlePos.z = stVoxelFilePos.nZ;

		//! 외부 단면을 보고 스톤 타입도 일부 정해줘야함..(외부에는 공극이 있는게 말이 안되니깐...)
		pParent->CalcExternalSide(stParticlePos.x,stParticlePos.y,stParticlePos.z,pParent->m_nXFileVoxCnt,pParent->m_nYFileVoxCnt,pParent->m_nZFileVoxCnt,stParticlePos);

		stParticlePos.sLayerIdx = nIndexNum;

		strKey.Format(L"%d-%d-%d",stParticlePos.x,stParticlePos.y,stParticlePos.z);

		g_MapOutsideData.insert(make_pair(strKey,stParticlePos));		//! 중심 레이어
		



		//////////////////////////////////////////////////////////////////////////
		//! 레이어 카운트
		iterStoneLayerCnt = pParent->m_mapStoneLayerCnt.find(stParticlePos.sLayerIdx);
		if(iterStoneLayerCnt != pParent->m_mapStoneLayerCnt.end())
			iterStoneLayerCnt->second++;
		else
			pParent->m_mapStoneLayerCnt.insert(make_pair(stParticlePos.sLayerIdx, 1));

		//////////////////////////////////////////////////////////////////////////
		//! 레이어 카운트
		strProcessStatus.Format(L"Layer%2d, Init Progress : %.2f %%", nIndexNum, float((float)nVox / (float)nTotalCnt) * 100.0);
		pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);


		if(NULL != p_Numfile )
			fclose(p_Numfile); 
	}
	nIndexNum++;


	//! 여기서 옵션을 넣어서 내부도 만드는지 외부만 만드는지!
	if(g_MapOutsideData.size() > 0)
	{
		//////////////////////////////////////////////////////////////////////////
		//! 여기서 나머지 내부 레이어를 생성해 준다.
		ST_PARTICLE_POS stParticlePos;
		map<CString,ST_PARTICLE_POS>		mapOutsideDataTemp = g_MapOutsideData;
		while (mapOutsideDataTemp.size() > 0)
		{

			pParent->m_mapInsertParticle.clear();

			map<CString,ST_PARTICLE_POS>::iterator iterOutsideData;
			for (iterOutsideData = mapOutsideDataTemp.begin()  ; iterOutsideData != mapOutsideDataTemp.end()  ; iterOutsideData++)
			{
				pParent->ReCalcExternalSide(iterOutsideData->second.x, iterOutsideData->second.y, iterOutsideData->second.z, mapOutsideDataTemp);	// 입자 제거후 주변 입자의 외부 노출 갯수 재계산

				strKey.Format(L"%d-%d-%d",iterOutsideData->second.x, iterOutsideData->second.y, iterOutsideData->second.z);
				pParent->m_setDeleteParticle.insert(strKey);
			}


			map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTmp;
			map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataDel;
			int nVoxCnt = 1;
			for(iterOutsideDataTmp = pParent->m_mapInsertParticle.begin() ; iterOutsideDataTmp != pParent->m_mapInsertParticle.end() ; iterOutsideDataTmp++)
			{
				iterOutsideDataDel = g_MapOutsideData.find(iterOutsideDataTmp->first);
				if(iterOutsideDataDel != g_MapOutsideData.end())
				{
					g_MapOutsideData.erase(iterOutsideDataDel);
				}

				iterOutsideDataTmp->second.sLayerIdx = nIndexNum;

				g_MapOutsideData.insert(make_pair(iterOutsideDataTmp->first,iterOutsideDataTmp->second));


				//////////////////////////////////////////////////////////////////////////
				//! 레이어 카운트
				iterStoneLayerCnt = pParent->m_mapStoneLayerCnt.find(iterOutsideDataTmp->second.sLayerIdx);
				if(iterStoneLayerCnt != pParent->m_mapStoneLayerCnt.end())
					iterStoneLayerCnt->second++;
				else
					pParent->m_mapStoneLayerCnt.insert(make_pair(iterOutsideDataTmp->second.sLayerIdx, 1));

				//////////////////////////////////////////////////////////////////////////
				//! 레이어 카운트
				strProcessStatus.Format(L"Layer%2d, Init Progress : %.2f %%", nIndexNum, float((float)nVoxCnt++ / (float)pParent->m_mapInsertParticle.size()) * 100.0);
				pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);

			}

			nIndexNum++;

			//! 다시할때는 추가된 부분을 빼고 다시 한다.
			mapOutsideDataTemp = pParent->m_mapInsertParticle;

		}

	}



	//! 입상 붕괴 계산시 외부 접촉 단면을 위해 추가해주어야함
	pParent->m_setDeleteParticle.clear();


	srand((unsigned int)time(NULL));
	pParent->InitStonRate();
	ST_PARTICLE_POS stParticlePos;
	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
	int nIntexTemp = 1;
	nTotalCnt = g_MapOutsideData.size();
	for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
	{
		pParent->CalcExternalSide(iterOutsideData->second.x, iterOutsideData->second.y, iterOutsideData->second.z ,pParent->m_nXFileVoxCnt, pParent->m_nYFileVoxCnt, pParent->m_nZFileVoxCnt, iterOutsideData->second);

		if(iterOutsideData->second.iExternalSideCnt == 0)	iterOutsideData->second.bInOut = false;
		else												iterOutsideData->second.bInOut = true;

		iterOutsideData->second.sStoneType = pParent->GetStoneType(&iterOutsideData->second);

		iterOutsideData->second.fPorosity = PDPorosity.GetDistParam();																		// 공극률
		iterOutsideData->second.fGranularDisintegration = pParent->m_pPDGranularDisintegration[iterOutsideData->second.sStoneType]->GetDistParam();		// 입상붕괴 도달값 

		//! 공극의 경우에만 수분 포화도를 양수로 넣는다.
		if(iterOutsideData->second.sStoneType == 0)
			iterOutsideData->second.fHaveWater = 0.0;
		else
			iterOutsideData->second.fHaveWater = 0.0;

		//////////////////////////////////////////////////////////////////////////
		//! 입자 종류 카운트
		iterStoneCnt = pParent->m_mapStoneTypeCnt.find(iterOutsideData->second.sStoneType);
		if(iterStoneCnt != pParent->m_mapStoneTypeCnt.end())
			iterStoneCnt->second++;
		else
			pParent->m_mapStoneTypeCnt.insert(make_pair(iterOutsideData->second.sStoneType, 1));

		stParticlePos = iterOutsideData->second;

		//////////////////////////////////////////////////////////////////////////
		//! 레이어 카운트
		strProcessStatus.Format(L"Recalc External Cnt, Progress : %.2f %%", float((float)nIntexTemp++ / (float)nTotalCnt) * 100.0);
		pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);

	}

	//////////////////////////////////////////////////////////////////////////
	//! 가시화를 위한 가시화 변수 초기화 6방향
// 	int nShowCnt = 6;
// 	g_MapShowData.clear();
// 	g_mapShowDataLink.clear();
// 	for (int v=  0; v < nShowCnt; v++)
// 	{
// 		map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA> mapShowData;
// 		g_MapShowData.insert(make_pair(v,mapShowData));
//  		vector<vector<ST_VOXEL_3D_POS_VIEWER>>						vecShowDataLink;	//! 가시화를위해 연결..
//  		g_mapShowDataLink.insert(make_pair(v,vecShowDataLink));
// 		pParent->InitShowData(v);
// 
// 		strProcessStatus.Format(L"Show : %d", v);
// 		pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);
// 
// 	}



	pParent->SendMessage(WM_FINISH_SOLID_VOXEL_MSG,4,0);	
}


//! 가시화 데이터 초기화 방향 - 0:상, 1:하, 2:좌, 3:우, 4:앞, 5:뒤
void CModelingWeatheringOfRockDlg::InitShowData(int nShow)
{
	//nShow : 0:상, 1:하, 2:좌, 3:우, 4:앞, 5:뒤

	//ST_VOXEL_3D_POS_VIEWER	stVoxel3dPosViewer;

	ST_VOXEL_3D_POS_VIEWER														stVoxel3dPosViewerTemp;
	map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>::iterator						iterShowDataTemp;
	map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>::iterator						iterShowDataTempFind;


	//! 방향 데이터
	map<int, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>>::iterator				iterShowData;
	iterShowData = g_MapShowData.find(nShow);
	if(iterShowData == g_MapShowData.end())
		return;

	iterShowData->second.clear();


	//! 방향에 따른 iterator
	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
	ST_SHOW_DATA	stShowDataTemp;
	for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
	{
		//stVoxel3dPosViewerTemp.z = iterOutsideData->second.z;
		
		if(iterOutsideData->second.sStoneType == 0)
			stShowDataTemp.nPorosityCnt = 1;
		else
			stShowDataTemp.nPorosityCnt = 0;

		stShowDataTemp.nDepthOrign = 1;
		stShowDataTemp.nDepth = 1;


		switch (nShow)
		{
		case  0: //0:상
		case  1: //1:하
// 		case  6: //6:상 - 현재 상태
// 		case  7: //7:하 - 현재 상태
			{

				if(nShow == 0)
				{
					stVoxel3dPosViewerTemp.fHori = iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.y;


					stShowDataTemp.nStartIdxCnt = m_nZFileVoxCnt - iterOutsideData->second.z;
					stShowDataTemp.nStartIdxCntOrign = m_nZFileVoxCnt - iterOutsideData->second.z;
				}
				else
				{
					stVoxel3dPosViewerTemp.fHori = m_nXFileVoxCnt - iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.y;


					stShowDataTemp.nStartIdxCnt = iterOutsideData->second.z;
					stShowDataTemp.nStartIdxCntOrign = iterOutsideData->second.z;
				}
				
			}
			break;
		case  2: //2:좌
		case  3: //3:우
// 		case  8: //8:좌 - 현재 상태
// 		case  9: //9:우 - 현재 상태

			{

				if(nShow == 2)
				{
					stVoxel3dPosViewerTemp.fHori = iterOutsideData->second.y;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = m_nXFileVoxCnt - iterOutsideData->second.x;
					stShowDataTemp.nStartIdxCntOrign = m_nXFileVoxCnt - iterOutsideData->second.x;
				}
				else
				{
					stVoxel3dPosViewerTemp.fHori = m_nYFileVoxCnt - iterOutsideData->second.y;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = iterOutsideData->second.x;
					stShowDataTemp.nStartIdxCntOrign = iterOutsideData->second.x;

				}
					

			}
			break;
		case  4: //4:앞
		case  5: //5:뒤
// 		case  10: //10:앞 - 현재 상태
// 		case  11: //11:뒤 - 현재 상태

			{


				if(nShow == 4)
				{
					stVoxel3dPosViewerTemp.fHori = m_nXFileVoxCnt - iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.z;
					//stVoxel3dPosViewerTemp.fVert = m_nZFileVoxCnt - iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = m_nYFileVoxCnt - iterOutsideData->second.y;
					stShowDataTemp.nStartIdxCntOrign = m_nYFileVoxCnt - iterOutsideData->second.y;
				}
				else
				{
					stVoxel3dPosViewerTemp.fHori = iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = iterOutsideData->second.y;
					stShowDataTemp.nStartIdxCntOrign = iterOutsideData->second.y;
				}

			}
			break;

		default:
			break;
		}

		iterShowDataTemp = iterShowData->second.find(stVoxel3dPosViewerTemp);
		if(iterShowDataTemp != iterShowData->second.end())
		{
			iterShowDataTemp->second.nPorosityCnt += stShowDataTemp.nPorosityCnt;
			iterShowDataTemp->second.nDepthOrign += stShowDataTemp.nDepthOrign;
			iterShowDataTemp->second.nDepth += stShowDataTemp.nDepth;

			if(iterShowDataTemp->second.nStartIdxCnt >  stShowDataTemp.nStartIdxCnt)
			{
				iterShowDataTemp->second.nStartIdxCnt = stShowDataTemp.nStartIdxCnt;
				iterShowDataTemp->second.nStartIdxCntOrign = stShowDataTemp.nStartIdxCnt;
			}

			//  [5/29/2019 kjky12] 비율 계산
			iterShowDataTemp->second.dRate = (double)(iterShowDataTemp->second.nDepth - iterShowDataTemp->second.nPorosityCnt) / (double)iterShowDataTemp->second.nDepthOrign * 100.0;
		}
		else //! 없으면 입력
		{
			iterShowData->second.insert(make_pair(stVoxel3dPosViewerTemp, stShowDataTemp));
		}
	}


	//! 방향 데이터
	map<int, vector<vector<ST_VOXEL_3D_POS_VIEWER>>>::iterator				iterShowDataLink;	//! 가시화를위해 연결..
	iterShowDataLink = g_mapShowDataLink.find(nShow);
	if(iterShowDataLink == g_mapShowDataLink.end())
		return;

	set<ST_VOXEL_3D_POS_VIEWER>				setVoxel3dPosViewer;
	set<ST_VOXEL_3D_POS_VIEWER>::iterator	iterVoxel3dPosViewer;

	for (iterShowDataTemp = iterShowData->second.begin() ; iterShowDataTemp != iterShowData->second.end(); iterShowDataTemp++)
	{
		ST_VOXEL_3D_POS_VIEWER	stVoxel3dPosViewer = iterShowDataTemp->first;

// 		iterVoxel3dPosViewer = setVoxel3dPosViewer.find(stVoxel3dPosViewer);
// 		if(iterVoxel3dPosViewer != setVoxel3dPosViewer.end()) //! 벌써 처리 한경우는 스킵
// 			continue;

		setVoxel3dPosViewer.clear();


		int nStartIdx = iterShowDataTemp->second.nStartIdxCntOrign;

		setVoxel3dPosViewer.insert(stVoxel3dPosViewer);

		vector<ST_VOXEL_3D_POS_VIEWER>		vecTemp;
		vecTemp.push_back(stVoxel3dPosViewer);


		int nArountCnt = SameStartIdxFind(nStartIdx, stVoxel3dPosViewer, iterShowData->second, vecTemp, setVoxel3dPosViewer);

		
// 		for (int nArountTemp = 0; nArountTemp < vecTemp.size(); nArountTemp++) 
// 		{
// 			if(vecTemp[nArountTemp].vecEmpty.size() == 0)
// 			{
// 
// 			}
// 		}


		if(vecTemp.size() == 1)
		{
			for (int nArountTemp = 0; nArountTemp < 4; nArountTemp++) 
			{
				vecTemp[0].vecEmpty.push_back(nArountTemp);
			}
		}

		iterShowDataLink->second.push_back(vecTemp);

	}
}


int CModelingWeatheringOfRockDlg::SameStartIdxFind(int nStartIdx, ST_VOXEL_3D_POS_VIEWER	&stVoxel3dPosViewer, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA> &mapShowData, vector<ST_VOXEL_3D_POS_VIEWER>&	vecVoxel3DPosViewer, set<ST_VOXEL_3D_POS_VIEWER>		&setVoxel3dPosViewer)
{
	int nArountTemp = 0;

	map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>::iterator			iterShowDataTempFind;
	ST_VOXEL_3D_POS_VIEWER	stVoxel3dPosViewerOrigin = stVoxel3dPosViewer;
	ST_VOXEL_3D_POS_VIEWER	stVoxel3dPosViewerTemp;
	set<ST_VOXEL_3D_POS_VIEWER>::iterator	iterVoxel3dPosViewer;

	stVoxel3dPosViewerTemp = stVoxel3dPosViewerOrigin;
	stVoxel3dPosViewerTemp.fHori += 1;
	stVoxel3dPosViewerTemp.fVert += 0;

	iterVoxel3dPosViewer = setVoxel3dPosViewer.find(stVoxel3dPosViewerTemp);
	if(iterVoxel3dPosViewer == setVoxel3dPosViewer.end()) //! 벌써 처리 한경우는 스킵
	{
		iterShowDataTempFind = mapShowData.find(stVoxel3dPosViewerTemp);
		if(iterShowDataTempFind != mapShowData.end())
		{
			if(nStartIdx == iterShowDataTempFind->second.nStartIdxCnt)
			{
				setVoxel3dPosViewer.insert(stVoxel3dPosViewerTemp);

				SameStartIdxFind(nStartIdx, stVoxel3dPosViewerTemp, mapShowData, vecVoxel3DPosViewer, setVoxel3dPosViewer);

				vecVoxel3DPosViewer.push_back(stVoxel3dPosViewerTemp);
				//vecVoxel3DPosViewer.push_back(stVoxel3dPosViewerTemp);

			}
			else
			{
				stVoxel3dPosViewer.vecEmpty.push_back(1);
			}
		}
		else
		{
			stVoxel3dPosViewer.vecEmpty.push_back(1);
		}
 	}
// 	else
// 	{
// 		nArountTemp++;
// 		//stVoxel3dPosViewer.vecEmpty.push_back(1);
// 	}

	stVoxel3dPosViewerTemp = stVoxel3dPosViewerOrigin;
	stVoxel3dPosViewerTemp.fHori -= 1;
	stVoxel3dPosViewerTemp.fVert += 0;

	iterVoxel3dPosViewer = setVoxel3dPosViewer.find(stVoxel3dPosViewerTemp);
	if(iterVoxel3dPosViewer == setVoxel3dPosViewer.end()) //! 벌써 처리 한경우는 스킵
	{
		iterShowDataTempFind = mapShowData.find(stVoxel3dPosViewerTemp);
		if(iterShowDataTempFind != mapShowData.end())
		{
			if(nStartIdx == iterShowDataTempFind->second.nStartIdxCnt)
			{
				setVoxel3dPosViewer.insert(stVoxel3dPosViewerTemp);

				SameStartIdxFind(nStartIdx, stVoxel3dPosViewerTemp, mapShowData, vecVoxel3DPosViewer, setVoxel3dPosViewer);

				vecVoxel3DPosViewer.push_back(stVoxel3dPosViewerTemp);

			}
			else
			{
				stVoxel3dPosViewer.vecEmpty.push_back(0);
			}
		}
		else
		{
			stVoxel3dPosViewer.vecEmpty.push_back(0);
		}
 	}
// 	else
// 	{
// 		nArountTemp++;
// 		//stVoxel3dPosViewer.vecEmpty.push_back(0);
// 	}

	stVoxel3dPosViewerTemp = stVoxel3dPosViewerOrigin;
	stVoxel3dPosViewerTemp.fHori += 0;
	stVoxel3dPosViewerTemp.fVert += 1;

	iterVoxel3dPosViewer = setVoxel3dPosViewer.find(stVoxel3dPosViewerTemp);
	if(iterVoxel3dPosViewer == setVoxel3dPosViewer.end()) //! 벌써 처리 한경우는 스킵
	{
		iterShowDataTempFind = mapShowData.find(stVoxel3dPosViewerTemp);
		if(iterShowDataTempFind != mapShowData.end())
		{
			if(nStartIdx == iterShowDataTempFind->second.nStartIdxCnt)
			{
				setVoxel3dPosViewer.insert(stVoxel3dPosViewerTemp);

				SameStartIdxFind(nStartIdx, stVoxel3dPosViewerTemp, mapShowData, vecVoxel3DPosViewer, setVoxel3dPosViewer);

				vecVoxel3DPosViewer.push_back(stVoxel3dPosViewerTemp);

			}
			else
			{
				stVoxel3dPosViewer.vecEmpty.push_back(2);
				
			}
		}
		else
		{
			stVoxel3dPosViewer.vecEmpty.push_back(2);

		}

 	}
// 	else
// 	{
// 		nArountTemp++;
// 		//stVoxel3dPosViewer.vecEmpty.push_back(2);
// 	}

	stVoxel3dPosViewerTemp = stVoxel3dPosViewerOrigin;
	stVoxel3dPosViewerTemp.fHori += 0;
	stVoxel3dPosViewerTemp.fVert -= 1;

	iterVoxel3dPosViewer = setVoxel3dPosViewer.find(stVoxel3dPosViewerTemp);
	if(iterVoxel3dPosViewer == setVoxel3dPosViewer.end()) //! 벌써 처리 한경우는 스킵
	{
		iterShowDataTempFind = mapShowData.find(stVoxel3dPosViewerTemp);
		if(iterShowDataTempFind != mapShowData.end())
		{
			if(nStartIdx == iterShowDataTempFind->second.nStartIdxCnt)
			{
				setVoxel3dPosViewer.insert(stVoxel3dPosViewerTemp);

				SameStartIdxFind(nStartIdx, stVoxel3dPosViewerTemp, mapShowData, vecVoxel3DPosViewer, setVoxel3dPosViewer);

				vecVoxel3DPosViewer.push_back(stVoxel3dPosViewerTemp);

			}
			else
			{
				stVoxel3dPosViewer.vecEmpty.push_back(3);
				//nCnt++;
			}
		}
		else
		{
			stVoxel3dPosViewer.vecEmpty.push_back(3);
			//nCnt++;
		}
 	}
// 	else
// 	{
// 		nArountTemp++;
// 		//stVoxel3dPosViewer.vecEmpty.push_back(3);
// 	}
	
	return nArountTemp;
}

void CModelingWeatheringOfRockDlg::UpdateShowData(int nShow)
{
	//nShow : 0:상, 1:하, 2:좌, 3:우, 4:앞, 5:뒤
	ST_VOXEL_3D_POS_VIEWER														stVoxel3dPosViewerTemp;
	map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>::iterator						iterShowDataTemp;
	map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>::iterator						iterShowDataTemp2;


	// map< 방향, <배열 XYZ 위치  / 입자 위치 정보> >
	map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>				mapShowData;	



	//! 방향에 따른 iterator
	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
	ST_SHOW_DATA	stShowDataTemp;
	for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
	{
		//stVoxel3dPosTemp.z = iterOutsideData->second.z;

		if(iterOutsideData->second.sStoneType == 0)
			stShowDataTemp.nPorosityCnt = 1;
		else
			stShowDataTemp.nPorosityCnt = 0;

		stShowDataTemp.nDepthOrign = 1;
		stShowDataTemp.nDepth = 1;


		switch (nShow)
		{
		case  0: //0:상
		case  1: //1:하
			// 		case  6: //6:상 - 현재 상태
			// 		case  7: //7:하 - 현재 상태
			{

				if(nShow == 0)
				{
					stVoxel3dPosViewerTemp.fHori = iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.y;


					stShowDataTemp.nStartIdxCnt = m_nZFileVoxCnt - iterOutsideData->second.z;
					stShowDataTemp.nStartIdxCntOrign = m_nZFileVoxCnt - iterOutsideData->second.z;
				}
				else
				{
					stVoxel3dPosViewerTemp.fHori = m_nXFileVoxCnt - iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = m_nYFileVoxCnt - iterOutsideData->second.y;


					stShowDataTemp.nStartIdxCnt = iterOutsideData->second.z;
					stShowDataTemp.nStartIdxCntOrign = iterOutsideData->second.z;
				}

			}
			break;
		case  2: //2:좌
		case  3: //3:우
			// 		case  8: //8:좌 - 현재 상태
			// 		case  9: //9:우 - 현재 상태

			{

				if(nShow == 2)
				{
					stVoxel3dPosViewerTemp.fHori = iterOutsideData->second.y;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = m_nXFileVoxCnt - iterOutsideData->second.x;
					stShowDataTemp.nStartIdxCntOrign = m_nXFileVoxCnt - iterOutsideData->second.x;
				}
				else
				{
					stVoxel3dPosViewerTemp.fHori = m_nYFileVoxCnt - iterOutsideData->second.y;
					stVoxel3dPosViewerTemp.fVert = m_nZFileVoxCnt - iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = iterOutsideData->second.x;
					stShowDataTemp.nStartIdxCntOrign = iterOutsideData->second.x;

				}


			}
			break;
		case  4: //4:앞
		case  5: //5:뒤
			// 		case  10: //10:앞 - 현재 상태
			// 		case  11: //11:뒤 - 현재 상태

			{


				if(nShow == 4)
				{
					stVoxel3dPosViewerTemp.fHori = m_nXFileVoxCnt - iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = m_nZFileVoxCnt - iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = m_nYFileVoxCnt - iterOutsideData->second.y;
					stShowDataTemp.nStartIdxCntOrign = m_nYFileVoxCnt - iterOutsideData->second.y;
				}
				else
				{
					stVoxel3dPosViewerTemp.fHori = iterOutsideData->second.x;
					stVoxel3dPosViewerTemp.fVert = iterOutsideData->second.z;


					stShowDataTemp.nStartIdxCnt = iterOutsideData->second.y;
					stShowDataTemp.nStartIdxCntOrign = iterOutsideData->second.y;
				}

			}
			break;

		default:
			break;
		}


		iterShowDataTemp = mapShowData.find(stVoxel3dPosViewerTemp);
		if(iterShowDataTemp != mapShowData.end())
		{
			iterShowDataTemp->second.nPorosityCnt += stShowDataTemp.nPorosityCnt;
			iterShowDataTemp->second.nDepthOrign += stShowDataTemp.nDepthOrign;
			iterShowDataTemp->second.nDepth += stShowDataTemp.nDepth;

			if(iterShowDataTemp->second.nStartIdxCnt >  stShowDataTemp.nStartIdxCnt)
			{
				iterShowDataTemp->second.nStartIdxCnt = stShowDataTemp.nStartIdxCnt;
				iterShowDataTemp->second.nStartIdxCntOrign = stShowDataTemp.nStartIdxCnt;
			}

		}
		else //! 없으면 입력
		{
			mapShowData.insert(make_pair(stVoxel3dPosViewerTemp, stShowDataTemp));
		}
	}


	//! 방향 데이터
	map<int, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA>>::iterator				iterShowData;
	iterShowData = g_MapShowData.find(nShow);
	if(iterShowData == g_MapShowData.end())
		return;

	for(iterShowDataTemp = mapShowData.begin(); iterShowDataTemp != mapShowData.end(); iterShowDataTemp++)
	{
		iterShowDataTemp2 = iterShowData->second.find(iterShowDataTemp->first);

		if(iterShowDataTemp2 == iterShowData->second.end())
			continue;;

		iterShowDataTemp2->second.nDepth = iterShowDataTemp->second.nDepth;

		iterShowDataTemp2->second.nPorosityCnt = iterShowDataTemp->second.nPorosityCnt;

		iterShowDataTemp2->second.nStartIdxCnt = iterShowDataTemp->second.nStartIdxCnt;

		//  [5/29/2019 kjky12] 비율 계산
		iterShowDataTemp2->second.dRate = (double)(iterShowDataTemp2->second.nDepth - iterShowDataTemp2->second.nPorosityCnt) / (double)iterShowDataTemp2->second.nDepthOrign * 100.0;
	}

}

//! 외부 노출 단면 계산 버튼 클릭
void CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcExternalSide()
{
	m_nCalcTotalCnt = 0;	// 계산 입자 개수
	m_nCalcTryCnt = 0;		// 계산 횟수

	m_bTriangle = TRUE;
	m_bStopCalc = FALSE;

	CString strTmp = L"";

	//! 메모리 처리
	if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{
		m_nCalcTotalCnt = m_nSolidTotalCnt; //GetTotalCntFromSolidFile();
		m_setDeleteParticle.clear();

	}
	else  //! 파일 처리
	{
		gf_CreateTextFile(gf_GetModulePath() + L"OutsideCnt.txt");
		gf_CreateTextFile(gf_GetModulePath() + L"DeleteCnt.txt");


		//CopyFile(gf_GetModulePath() + L"SolidVoxel.dat", gf_GetModulePath() + L"SolidPos_del.dat", FALSE);	
		m_setDeleteParticle.clear();

		m_nCalcTotalCnt = GetTotalCntFromSolidFile();
	
	}

	
	strTmp.Format(L"%d",m_nCalcTotalCnt);
	m_editTotalSolidCnt.SetWindowTextW(strTmp);

	RefreshMessage();
	
	srand((unsigned int)time(NULL));
	InitStonRate();
	InitEditValue();
	SetProbDistributionValue();
	InitStoneCnt();
	_beginthread( ThreadCalcExternalSide, 0, this);

	
}


void CModelingWeatheringOfRockDlg::InitStonRate(bool bClear)
{
	int nEditFeldsparPer = _wtoi(m_strEditFeldsparPer);
	int nEditQuartzPer = _wtoi(m_strEditQuartzPer);
	int nEditPlagioclasePer = _wtoi(m_strEditPlagioclasePer);
	int nEditBiotitePer = _wtoi(m_strEditBiotitePer);
	int nEditTitanitePer = _wtoi(m_strEditTitanitePer);
	int nEditHornblendePer = _wtoi(m_strEditHornblendePer);
	int nEditEpidotePer = _wtoi(m_strEditEpidotePer);
	int nEditChloritePer = _wtoi(m_strEditChloritePer);
	int nEditPorosityPer = _wtoi(m_strEditPorosityPer);
	int nRatePercentAll = _wtoi(m_strEditFeldsparPer) + _wtoi(m_strEditQuartzPer) + _wtoi(m_strEditPlagioclasePer) + _wtoi(m_strEditBiotitePer) + _wtoi(m_strEditTitanitePer) + _wtoi(m_strEditHornblendePer) + _wtoi(m_strEditEpidotePer) + _wtoi(m_strEditChloritePer) + _wtoi(m_strEditPorosityPer);


	int nStonTyDisate[dfSTONE_TYPE_CNT] = {
		nEditPorosityPer,		
		nEditFeldsparPer,
		nEditQuartzPer,
		nEditPlagioclasePer,
		nEditBiotitePer,
		nEditTitanitePer,
		nEditHornblendePer,
		nEditEpidotePer,
		nEditChloritePer
	};

	if(bClear)
		m_mapInitStoneTypeCnt.clear();
	for (int v = 0; v < dfSTONE_TYPE_CNT; v++)
	{
		m_mapInitStoneTypeCnt.insert(make_pair(v, 0));
	}


	for (int v = 0; v < dfSTONE_TYPE_CNT; v++)
	{
		map<int,int>::iterator	iterInitStoneTypeCnt = m_mapInitStoneTypeCnt.find(v);
		for (int a = 0; a < nStonTyDisate[v]; a++)
		{
			iterInitStoneTypeCnt->second++;
		}
	}

}


LRESULT CModelingWeatheringOfRockDlg::OnCalcRockAgingMsg(WPARAM wParam, LPARAM lParam)
{


	if(lParam == 999) // GPU처리;
	{
		SetStoneTypeCntView();	

		OnCbnSelchangeCombo1();


		CString strTmp;
		//strTmp.Format(L"%d (%.02f%%)",(int)m_setDeleteParticle.size(), (float)((float)m_setDeleteParticle.size()/(float)m_nCalcTotalCnt) * 100.0);
		m_editGrDisCnt.SetWindowText(L"-");

		//strTmp.Format(L"%d (%.02f%%)",(int)g_MapOutsideData.size(), (float)((float)g_MapOutsideData.size()/(float)m_nCalcTotalCnt) * 100.0);
		m_editGrTotalCnt.SetWindowText(L"-");

		strTmp.Format(L"%d",m_nCalcTryCnt);	
		m_editCalcTryCnt.SetWindowText(strTmp);


	}
	else
	{
		CString strTemp;
		strTemp.Format(L"CPU - CalcRockAging End(%d)", m_nCalcTryCnt);
		ShowTraceTime(strTemp, 1);


		CString strInput = L"";
		m_EditStopSimulCnt.GetWindowText(strInput);


		SetStoneTypeCntView();	

		OnCbnSelchangeCombo1();



		CString strTmp;
		strTmp.Format(L"%d (%.02f%%)",(int)m_setDeleteParticle.size(), (float)((float)m_setDeleteParticle.size()/(float)m_nCalcTotalCnt) * 100.0);
		m_editGrDisCnt.SetWindowText(strTmp);

		strTmp.Format(L"%d (%.02f%%)",(int)g_MapOutsideData.size(), (float)((float)g_MapOutsideData.size()/(float)m_nCalcTotalCnt) * 100.0);
		m_editGrTotalCnt.SetWindowText(strTmp);

		strTmp.Format(L"%d",m_nCalcTryCnt);		
		m_editCalcTryCnt.SetWindowText(strTmp);


		//! 여기가 CPU처리
		if((int)wParam == 1)
		{
			//OnBnClickedButtonCalcExternalSide();
		}
		else
		{
			OnBnClickedButtonCalcRockAging();
		}
	}

	


	return FALSE;
}

bool CModelingWeatheringOfRockDlg::CalcRockAgingSetData()
{
	

// 	if(m_checkSaveStep.GetCheck() && !(m_nCalcTryCnt % m_nSaveInterval))
// 		SaveStepOutsideData(m_nCalcTryCnt);

	SetProbDistributionValue();


	CString strInput = L"";

	m_editTopRate.GetWindowText(strInput);
	m_fTopRate = _wtof(strInput);
	m_editSideRate.GetWindowText(strInput);
	m_fSideRate = _wtof(strInput);
	m_editBottomRate.GetWindowText(strInput);
	m_fBottomRate = _wtof(strInput);

	if(g_MapOutsideData.size() <= 0)
		return false;
// 	strInput.Format(L"%d\n",g_MapOutsideData.size());
// 	gf_InsertTextFile(gf_GetModulePath() + L"OutsideCnt.txt",strInput);
// 
// 	strInput.Format(L"%d\n",m_setDeleteParticle.size());
// 	gf_InsertTextFile(gf_GetModulePath() + L"DeleteCnt.txt",strInput);


	m_EditStopSimulCnt.GetWindowText(strInput);

	if(m_nCalcTryCnt > _wtoi(strInput) -1)
	{
		UpdateMainView(FALSE);
		return false;
	}

	if(m_bStopCalc)
	{
		return false;
	}


	return true;
}

//! 시작 버튼
void CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAging()
{
	CString strTemp = L"";
	m_bStopCalc = FALSE;

 	if(m_comboProcessCalcRocking.GetCurSel() == 0) //GPU
 	{
		m_EditStopSimulCnt.GetWindowTextW(strTemp);

		CalcRockingGpu(_wtoi(strTemp) - m_nCalcTryCnt);
 	}
 	else//CPU
	{



		if(CalcRockAgingSetData())
		{
			strTemp.Format(L"CPU - CalcRockAging Start(%d)", m_nCalcTryCnt);
			ShowTraceTime(strTemp);

			_beginthread( ThreadCalcRockAging, 0, this);
		}
		else
		{

			int a= 0;
		}

	}



}


//! 재수행 버튼
void CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAging2()
{
	m_bStopCalc = FALSE;

// 	if(m_comboProcessCalcRocking.GetCurSel() == 0) //GPU
// 	{
// 
// 	}
// 	else//CPU
// 	{
		if(CalcRockAgingSetData())
		{

			_beginthread( ThreadCalcRockAging, 0, this);
		}

//	}

	//OnBnClickedButtonCalcRockAging();
	//_beginthread( ThreadCalcRockAging, 0, this);
}


//! 한번 수행 버튼
void CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAging3()
{

	m_bStopCalc = TRUE;
	CalcRockAgingSetData();

	if(m_comboProcessCalcRocking.GetCurSel() == 0) //GPU
	{
		CalcRockingGpu(1);
	}
	else//CPU
	{
		CString strTemp = L"";
		strTemp.Format(L"%d", m_nCalcTryCnt+1);
		m_EditStopSimulCnt.SetWindowText(strTemp);
		_beginthread( ThreadCalcRockAging, 0, this);

	}

}

//! 입상붕괴 시뮬레이션 수행
void ThreadCalcRockAging( void* pArguments )
{
	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;

	//pParent->ShowTraceTime(L"CPU - CalcRockAging Start");

	float fXSize = 0.0;
	float fYSize = 0.0;
	float fZSize = 0.0;

	CString strKey = L"";
	CString strTmp = L"";
	CString strTmp1 = L"";
	CString strTmp2 = L"";

	map<int,int>::iterator iterStoneCnt;

	ST_BASE_ROCK_PARTICLE stBaseRockParticle;

	map<int,vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>>::iterator	iterBaseRockData;	
	vector<vector<vector<ST_BASE_ROCK_PARTICLE>>>	VecZBaseRockParticle;

	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTmp;
	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTmpInner;
	ST_PARTICLE_POS stParticlePos;

	//! 외부에 공극이 있는경우 연결되있는 공극을 전부 없애기 위해..
	vector<CString> vecDeleParticleInsideCheck;
	vecDeleParticleInsideCheck.clear();
	//! 삭제되는 외부 복셀 데이터
	vector<CString> vecDeleParticle;
	vecDeleParticle.clear();
	//! 변경되는 내부 공극 복셀 데이터
	vector<ST_PARTICLE_POS> vecPorosity;
	vecPorosity.clear();


	//! 공극률 개수
	pParent->m_editCalcPorosity.GetWindowText(strTmp);
	//! 가속 계수
	pParent->m_editCalcMulti.GetWindowText(strTmp1);

	float fCoefficient = _wtof(strTmp) * _wtof(strTmp1);	// 공극률 계수값 (공극률 * 가속 계수)

	//////////////////////////////////////////////////////////////////////////
	//! 수분 흡수율 -> 공극에만 적용되고 공극에서 6방향으로!!
	pParent->m_editCalcWaterInnerAbsorption.GetWindowText(strTmp);
	//! 레이별 수분부피차감률
	pParent->m_editCalcLayerWaterAborption.GetWindowText(strTmp1);
	//! 수분 부피 팽창률
	pParent->m_editCalcWaterChange.GetWindowText(strTmp2);

	//! 수분 흡수율
	float fCalcWaterInnerAbsorption =  _wtof(strTmp);
	//! 레이어별 수분 부피 차감율
	float fCalcLayerWaterAborption = _wtof(strTmp1);
	//! 수분 팽창
	float fCalcWaterChange =  _wtof(strTmp2);

	//////////////////////////////////////////////////////////////////////////

	//pParent->m_nCalcTryCnt++;

	//iterBaseRockData = g_MapBaseRockData.find(0);

	//! 현재 데이터의 최대 레이어
	short sMaxLayerIdx = -1;
	//! 최대 레이어를 확인한다.
// 	if(pParent->m_mapStoneLayerCnt.size() > 0)
// 	{
// 		map<int,int>::iterator	iterStoneLayerCnt = pParent->m_mapStoneLayerCnt.end();
// 		for (iterStoneLayerCnt = pParent->m_mapStoneLayerCnt.begin() ; iterStoneLayerCnt != pParent->m_mapStoneLayerCnt.end(); iterStoneLayerCnt++)
// 		{
// 			if(iterStoneLayerCnt->second <= 0)
// 				continue;
// 
// 			if(sMaxLayerIdx < iterStoneLayerCnt->first)
// 				sMaxLayerIdx = iterStoneLayerCnt->first;
// 		}
// 	}
	


	CString strTemp;
	//if(iterBaseRockData != g_MapBaseRockData.end())
	pParent->m_EditStopSimulCnt.GetWindowText(strTemp);
	for (pParent->m_nCalcTryCnt; pParent->m_nCalcTryCnt < _wtoi(strTemp); pParent->m_nCalcTryCnt++)
	{
		//VecZBaseRockParticle = iterBaseRockData->second;

		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
		{
			
			stParticlePos = iterOutsideData->second;

			int x = stParticlePos.x;
			int y = stParticlePos.y;
			int z = stParticlePos.z;
				if(stParticlePos.bInOut == true) //! 외부는 기존 입상붕괴
				{
					//! 6면이 다 외부인 경우는 삭제해준다.(하늘에 붕 떠있어서), 
					if(stParticlePos.iExternalSideCnt == 6) 
					{
						//stParticlePos.bUse = false;

						strKey.Format(L"%d-%d-%d",x,y,z);

						vecDeleParticle.push_back(strKey);

						continue;;
					}

					//공극의 경우 외부에 있을수 없다.
					if(stParticlePos.sStoneType == 0)
					{
						//stParticlePos.bUse = false;

						strKey.Format(L"%d-%d-%d",x,y,z);

						vecDeleParticleInsideCheck.push_back(strKey);

						continue;;
					}

			

					//! 임시로
					//continue;

					//for(int nEx = 0 ; nEx < stParticlePos.vecExternalSide.size() ; nEx++)	// 외부 노출 단면 개수만큼 계산
					for(int nEx = 0 ; nEx < 6 ; nEx++)	// 외부 노출 단면 개수만큼 계산
					{

						if(stParticlePos.abExternalSide[nEx] == FALSE)
							continue;

						//switch(stParticlePos.vecExternalSide[nEx])	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
						switch(nEx)	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
						{
						case 0:
							{
								stParticlePos.fPorosity += (fCoefficient * pParent->m_fTopRate);
							}
							break;
						case 1:
							{
								stParticlePos.fPorosity += (fCoefficient * pParent->m_fBottomRate);
							}
							break;
						case 2:
						case 3:
						case 4:
						case 5:
							{
								stParticlePos.fPorosity += (fCoefficient * pParent->m_fSideRate);
							}
							break;
						default:
							break;
						}

						//fPorosity 공극률 >= fGranularDisintegration 입상붕괴 도달값
						if(stParticlePos.fPorosity >= stParticlePos.fGranularDisintegration) // 입상붕괴 도달값에 도달하여 제거
						{
							//stParticlePos.bUse = false;

							strKey.Format(L"%d-%d-%d",x,y,z);

							vecDeleParticle.push_back(strKey);

							break;
						}
					}
				}
				else //! 내부는 공극의 경우에 주변에 값을 입력한다.
				{
					if(stParticlePos.sStoneType != 0) //! 공극만 처리해야한다.
						continue;
					
					

					//! 수분 흡수율
					fCalcWaterInnerAbsorption;
					//! 레이어별 수분 부피 차감율
					fCalcLayerWaterAborption;
					//! 수분 팽창
					fCalcWaterChange;
					//! 최대 레이어
					//sMaxLayerIdx;

					/*[2/11/2019 kjky12] 
					1. 수분 흡수량 = 수분흡수율 - {(최대 레이어 Idx - 현제 레이어 Idx) * 레이어별 수분 차감률 * 수분흡수율 }
					그리고 수분 흡수량의 최대는 1.0이고 최소는 0.0이다. 그리고 결과값이 -일 경우 0.0으로 수분이 흡수되지 않은걸로 판단.

					2. 수분 포화도 += 수분 흡수량

					3.if(수분 포화도 * 수분 팽창률 > 1.0)
					{
					여기서 입상붕괴치를 값을 넣어 주어야될듯
					}
					*/ 
					//1. 수분 흡수량 = 수분흡수율 - {(최대 레이어 Idx - 현제 레이어 Idx) * 레이어별 수분 차감률 * 수분흡수율 }
					float fHaveWaterTemp = fCalcWaterInnerAbsorption - ( (/*sMaxLayerIdx -*/ stParticlePos.sLayerIdx) * fCalcLayerWaterAborption * fCalcWaterInnerAbsorption );
					
					//! 0보다 적으면 수분이 들어가지 않은것
					if(fHaveWaterTemp < 0.0)
						fHaveWaterTemp = 0.0;

					//2. 수분 포화도 += 수분 흡수량
					stParticlePos.fHaveWater += fHaveWaterTemp;

					//! 수분 포화도의 최대값은 1.0이다.
					if(stParticlePos.fHaveWater > 1.0)
						stParticlePos.fHaveWater = 1.0;

					//3. if(수분 포화도 * 수분 팽창률 > 1.0)
					if((stParticlePos.fHaveWater * fCalcWaterChange) > 1.0)
					{
						vector<ST_PARTICLE_POS> vecPorosityTemp;
						vecPorosityTemp.clear();

						float fTotalHaveWater = 0.0;
						for(int nEx = 0 ; nEx < 6 ; nEx++)	// 외부 노출 단면 개수만큼 계산
						{
							int nX = x;
							int nY = y;
							int nZ = z;

							switch(nEx)	//[0:상,1:하,2:좌,3:우,4:앞,5:뒤]
							{
							case 0:
								nZ += 1;
								break;
							case 1:
								nZ -= 1;
								break;
							case 2:
								nY += 1;
								break;
							case 3:
								nY -= 1;
								break;
							case 4:
								nX -= 1;
								break;
							case 5:
								nX += 1;
								break;
							default:
								break;
							}

							if(x != nX || y != nY || z != nZ) //! 기존 위치와 다른게 있는것만
							{
								strKey.Format(L"%d-%d-%d",nX,nY,nZ);


								iterOutsideDataTmpInner = g_MapOutsideData.find(strKey);
								if(iterOutsideDataTmpInner != g_MapOutsideData.end())
								{
									iterOutsideDataTmpInner->second.fPorosity += (fCalcWaterChange / 5.0);

									if(iterOutsideDataTmpInner->second.fPorosity >= iterOutsideDataTmpInner->second.fGranularDisintegration) // 입상붕괴 도달값에 도달하여 제거
									{
										if(iterOutsideDataTmpInner->second.fHaveWater < 0)
											iterOutsideDataTmpInner->second.fHaveWater = 0;

										//! 여기서 바꾸게되면 중복으로 계산이 될 수 있기 떄문에 계산하지 않는다.
										vecPorosity.push_back(iterOutsideDataTmpInner->second);

									}
//  [8/16/2019 kjky12] 이건 추후에 입상붕괴 알고리즘이 추가되는경우 해줘야된다고 생각함
// 									//! 주위 입자 타입이 공극이면 무시해준다.
// 									if(iterOutsideDataTmpInner->second.sStoneType == 0)
// 									{
// 										//! 공극 수분율을 합쳐주기 위해서 밑에서 무시해준다.
// 										if(iterOutsideDataTmpInner->second.fHaveWater < 0)
// 											iterOutsideDataTmpInner->second.fHaveWater = 0;
// 										fTotalHaveWater += iterOutsideDataTmpInner->second.fHaveWater;
// 										//! 여기서 바꾸게되면 중복으로 계산이 될 수 있기 떄문에 계산하지 않는다. ->주변이 공극이니까 그 공긍게 대한 수분 함수율을 합쳐준다
// 										vecPorosityTemp.push_back(iterOutsideDataTmpInner->second);
// 
// 										continue;
// 									}
// 									
// 									// [2/11/2019 kjky12] 여기서 인근 값을 찾게되면 입상붕괴를 수행해야한다... 정의가 필요하다
// 									//! 일단 부피팽창률로한다
// 									iterOutsideDataTmpInner->second.fPorosity += (fCalcWaterChange / 5.0);
// 
// 									if(iterOutsideDataTmpInner->second.fPorosity >= iterOutsideDataTmpInner->second.fGranularDisintegration) // 입상붕괴 도달값에 도달하여 제거
// 									{
// 										if(iterOutsideDataTmpInner->second.fHaveWater < 0)
// 											iterOutsideDataTmpInner->second.fHaveWater = 0;
// 										fTotalHaveWater += iterOutsideDataTmpInner->second.fHaveWater;
// 										//! 여기서 바꾸게되면 중복으로 계산이 될 수 있기 떄문에 계산하지 않는다.
// 										vecPorosityTemp.push_back(iterOutsideDataTmpInner->second);
// 									}

								}

							}

						}
						/*
//  [8/16/2019 kjky12] 이건 추후에 입상붕괴 알고리즘이 추가되는경우 해줘야된다고 생각함
// 						//////////////////////////////////////////////////////////////////////////
// 						//! 여기서 입자 -> 공극으로 바뀐 항목에 수분 포화도를 n으로 나누어 넣어준다.
// 						int nChangePorosity = vecPorosityTemp.size();
// 						for (int nT = 0; nT < nChangePorosity; nT++)
// 						{
// 							strKey.Format(L"%d-%d-%d", vecPorosityTemp[nT].x, vecPorosityTemp[nT].y, vecPorosityTemp[nT].z);
// 							iterOutsideDataTmp = g_MapOutsideData.find(strKey);
// 							if(iterOutsideDataTmp != g_MapOutsideData.end())
// 							{
// 								//! 주위 공극과 수분율이 합쳐진다.
// 								iterOutsideDataTmp->second.fHaveWater = (fTotalHaveWater + stParticlePos.fHaveWater) / (float)(nChangePorosity + 1);
// 
// 								//! 주위 입자 타입이 공극이면 수분율만 합치고 무시해준다.
// 								if(iterOutsideDataTmp->second.sStoneType == 0)
// 									continue;
// 
// 								vecPorosity.push_back(iterOutsideDataTmp->second);
// 							}
// 
// 						}

						stParticlePos.fHaveWater = (fTotalHaveWater + stParticlePos.fHaveWater) / (float)(nChangePorosity + 1);
						*/
						stParticlePos.fHaveWater = 1.0;

					}
				}
			
			iterOutsideData->second = stParticlePos;
		}

		//	iterBaseRockData->second = VecZBaseRockParticle;


		// 	pParent->ShowTraceTime(L"CPU - CalcRockAging End", 1);
		// 	pParent->ShowTraceTime(L"CPU - Else Start");

		vector<ST_DELETE_VOXEL_POS> deleteVoxelPos;

		ST_DELETE_VOXEL_POS stDeleteVoxelPos;
		set<CString>::iterator	iterDeleteParticle;
		//////////////////////////////////////////////////////////////////////////
		//! 외부에 입상 붕괴 처리
		deleteVoxelPos.clear();
		int v = 0 ;
		for(v = 0 ; v < vecDeleParticle.size() ; v++)
		{
			bool bDelCheck = false;
			iterDeleteParticle = pParent->m_setDeleteParticle.find(vecDeleParticle[v]);
			if(iterDeleteParticle != pParent->m_setDeleteParticle.end()) //! 기존에 먼저 삭제 된적 있으면 무시...(이게 앞에서 미리 내부 복셀을 모두 생성하는 경우 처리되느 중복을 없애기위해)
				bDelCheck = true;

			iterOutsideDataTmp = g_MapOutsideData.find(vecDeleParticle[v]);
			if(iterOutsideDataTmp != g_MapOutsideData.end())
			{
				stDeleteVoxelPos.nX = iterOutsideDataTmp->second.x;
				stDeleteVoxelPos.nY = iterOutsideDataTmp->second.y;
				stDeleteVoxelPos.nZ = iterOutsideDataTmp->second.z;

				iterStoneCnt = pParent->m_mapStoneTypeCnt.find(iterOutsideDataTmp->second.sStoneType);
				iterStoneCnt->second--;

				//			pParent->DeleteCellToSolidFile(iterOutsideDataTmp->second.x,iterOutsideDataTmp->second.y,iterOutsideDataTmp->second.z);
				g_MapOutsideData.erase(iterOutsideDataTmp);

				if(!bDelCheck)
					deleteVoxelPos.push_back(stDeleteVoxelPos);

			}

			//strKey.Format(L"%d-%d-%d",x,y,z);
			//! 내부 복셀을 미리 생성하는 경우 모든 복셀이 들어 있다.
			if(!bDelCheck)
				pParent->m_setDeleteParticle.insert(vecDeleParticle[v]);
		}

		//////////////////////////////////////////////////////////////////////////
		//! 외부와 연결되있는공극을 확인하고 공극을 삭제해준다.
		set<CString>::iterator iterTemp;
		set<CString> setTemp;
		for(v = 0 ; v < vecDeleParticleInsideCheck.size() ; v++)
		{
			setTemp.clear();
			setTemp =  pParent->CalcDeleteParticleInside(vecDeleParticleInsideCheck[v]);

			for (iterTemp = setTemp.begin(); iterTemp != setTemp.end(); iterTemp++)
			{
				bool bDelCheck = false;
				iterDeleteParticle = pParent->m_setDeleteParticle.find(vecDeleParticleInsideCheck[v]);
				if(iterDeleteParticle != pParent->m_setDeleteParticle.end()) //! 기존에 먼저 삭제 된적 있으면 무시...(이게 앞에서 미리 내부 복셀을 모두 생성하는 경우 처리되느 중복을 없애기위해)
					bDelCheck = true;

				iterOutsideDataTmp = g_MapOutsideData.find(*iterTemp);
				if(iterOutsideDataTmp != g_MapOutsideData.end())
				{
					stDeleteVoxelPos.nX = iterOutsideDataTmp->second.x;
					stDeleteVoxelPos.nY = iterOutsideDataTmp->second.y;
					stDeleteVoxelPos.nZ = iterOutsideDataTmp->second.z;

					iterStoneCnt = pParent->m_mapStoneTypeCnt.find(iterOutsideDataTmp->second.sStoneType);
					iterStoneCnt->second--;

					//			pParent->DeleteCellToSolidFile(iterOutsideDataTmp->second.x,iterOutsideDataTmp->second.y,iterOutsideDataTmp->second.z);
					g_MapOutsideData.erase(iterOutsideDataTmp);

					if(!bDelCheck)
						deleteVoxelPos.push_back(stDeleteVoxelPos);
				}


				if(!bDelCheck)
					pParent->m_setDeleteParticle.insert(vecDeleParticleInsideCheck[v]);

			}

		}



		//////////////////////////////////////////////////////////////////////////
		//! 내부 입자 -> 공극 변환 처리(위에서 전부 계산 이후 처리 해야 중복 수행 방지됨)
		for (int v = 0; v < vecPorosity.size(); v++)
		{
			strKey.Format(L"%d-%d-%d", vecPorosity[v].x, vecPorosity[v].y, vecPorosity[v].z);


			iterOutsideDataTmp = g_MapOutsideData.find(strKey);
			if(iterOutsideDataTmp != g_MapOutsideData.end())
			{
				//! 변경 전 개수 빼기
				iterStoneCnt = pParent->m_mapStoneTypeCnt.find(iterOutsideDataTmp->second.sStoneType);
				iterStoneCnt->second--;

				iterOutsideDataTmp->second.sStoneType = 0;

				//! 변경 후 개수 추가
				iterStoneCnt = pParent->m_mapStoneTypeCnt.find(iterOutsideDataTmp->second.sStoneType);
				iterStoneCnt->second++;
			}
		}


		//  [2/5/2019 kjky12] 전체 복셀을 미리 생성하기 때문에 삭제 및 재계산을 할 필요가 없다.
		for(int d = 0 ; d < deleteVoxelPos.size() ; d++)
		{
			// 입자 제거후 주변 입자의 외부 노출 갯수 재계산
			pParent->ReCalcExternalSide(deleteVoxelPos[d].nX,deleteVoxelPos[d].nY,deleteVoxelPos[d].nZ, g_MapOutsideData);	// 입자 제거후 주변 입자의 외부 노출 갯수 재계산
		}


		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataDel;
		for(iterOutsideDataTmp = pParent->m_mapInsertParticle.begin() ; iterOutsideDataTmp != pParent->m_mapInsertParticle.end() ; iterOutsideDataTmp++)
		{
			iterOutsideDataDel = g_MapOutsideData.find(iterOutsideDataTmp->first);
			if(iterOutsideDataDel != g_MapOutsideData.end())
			{
				g_MapOutsideData.erase(iterOutsideDataDel);
			}

			g_MapOutsideData.insert(make_pair(iterOutsideDataTmp->first,iterOutsideDataTmp->second));
		}


		vecDeleParticle.clear();
		pParent->m_mapInsertParticle.clear();

	}

// 	int nShowCnt = 6;
// 	//g_MapShowData.clear();
// 	for (int v=  0; v < nShowCnt; v++)
// 	{
// // 		map<ST_VOXEL_3D_POS, ST_SHOW_DATA> mapShowData;
// // 		g_MapShowData.insert(make_pair(v,mapShowData));
// 		pParent->UpdateShowData(v);
// 	}



	pParent->SendMessage(WM_FINISH_SIM_ONE_STEP_MSG,pParent->m_bStopCalc,0);


	//pParent->ShowTraceTime(L"CPU - Else End", 1);


}


set<CString> CModelingWeatheringOfRockDlg::CalcDeleteParticleInside(CString strPos)
{
	set<CString>		setDelDataParticleInside;
	setDelDataParticleInside.insert(strPos);


	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTmp;

	iterOutsideDataTmp = g_MapOutsideData.find(strPos);
	if(iterOutsideDataTmp != g_MapOutsideData.end())
	{
		int nValue[6][3] = {
			{0,0,-1},
			{0,0,1},
			{0,1,0},
			{0,-1,0},
			{1,0,0},
			{-1,0,0}
		};

		for (int a =0; a< 6; a++)
		{
			CString strKey = L"";
			strKey.Format(L"%d-%d-%d", iterOutsideDataTmp->second.x + nValue[a][0], iterOutsideDataTmp->second.y + nValue[a][1], iterOutsideDataTmp->second.z + nValue[a][2]);

			map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTemp = g_MapOutsideData.find(strKey);
			if(iterOutsideDataTemp != g_MapOutsideData.end())
			{
				if(iterOutsideDataTemp->second.sStoneType == 0)
				{
					setDelDataParticleInside.insert(strPos);

					CalcDeleteParticleInsideDetail(strKey, setDelDataParticleInside);
				}
			}
		}

	}

// 	if(setDelDataParticleInside.size() > 0)
// 	{
// 		g_MapOutsideData.erase()
// 
// 
// 	}

	return setDelDataParticleInside;
}


void CModelingWeatheringOfRockDlg::CalcDeleteParticleInsideDetail(CString strPos, set<CString>		&setDelDataParticleInside)
{


	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTmp;
	iterOutsideDataTmp = g_MapOutsideData.find(strPos);
	if(iterOutsideDataTmp != g_MapOutsideData.end())
	{
		int nValue[6][3] = {
			{0,0,-1},
			{0,0,1},
			{0,1,0},
			{0,-1,0},
			{1,0,0},
			{-1,0,0}
		};

		for (int a =0; a< 6; a++)
		{
			CString strKey = L"";
			strKey.Format(L"%d-%d-%d", iterOutsideDataTmp->second.x + nValue[a][0], iterOutsideDataTmp->second.y + nValue[a][1], iterOutsideDataTmp->second.z + nValue[a][2]);

			map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataTemp = g_MapOutsideData.find(strKey);
			if(iterOutsideDataTemp != g_MapOutsideData.end())
			{
				if(iterOutsideDataTemp->second.sStoneType == 0)
				{
					

					set<CString>::iterator iterDelDataParticleInside = setDelDataParticleInside.find(strKey);
					if(iterDelDataParticleInside != setDelDataParticleInside.end())
					{

					}
					else
					{
						//! 미리 넣은적 없을때만 재계산
						setDelDataParticleInside.insert(iterOutsideDataTemp->first);
						CalcDeleteParticleInsideDetail(strKey, setDelDataParticleInside);
					}

				}
			}
		}

	}


}


void CModelingWeatheringOfRockDlg::ReCalcExternalSide(int x,int y,int z, map<CString,ST_PARTICLE_POS>	&mapOutsideData)
{

	CString strKey = L"";
	CString strTmp = L"";
	
	// 입상붕괴 도달값 계산을 위한 확률변수 설정
// 	CString strGranularDisintegration = L"";
// 	m_editGranularDisintegration.GetWindowText(strGranularDisintegration);
// 	double dGranularDisintegration = _wtof(strGranularDisintegration);
// 	CProbDistribution PDGranularDisintegration(3,0.0,dGranularDisintegration,0.001,0.0,0.0,0.0,0.0,0.0,0.0);
	
	// 공극률 설정을 위한 확률변수 설정
	CString strInitData = L"";
	m_editInitPorosity.GetWindowText(strInitData);
	double dInitPorosity = _wtof(strInitData);
	CProbDistribution PDPorosity(4,0.0,0.0,0.0001,dInitPorosity,0.0,0.0,0.0,0.0,0.0);

	float fMaxDistance = sqrt(pow(m_nXFileVoxCnt / 2.0, 2) + pow(m_nYFileVoxCnt / 2.0, 2) + pow(m_nZFileVoxCnt / 2.0, 2) );


	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideDataDel;

	int nValue[6][3] = {
		{0,0,-1},
		{0,0,1},
		{0,1,0},
		{0,-1,0},
		{1,0,0},
		{-1,0,0}
	};

	//! 6방향 처리
	for (int v = 0; v < 6; v++)
	{
		int nPlusX = nValue[v][0];
		int nPlusY = nValue[v][1];
		int nPlusZ = nValue[v][2];
		strKey.Format(L"%d-%d-%d",x + nPlusX,y  + nPlusY, z  + nPlusZ);

		if(GetUsedCellFromSolidFile(x + nPlusX,y  + nPlusY, z  + nPlusZ))
		{
			ST_PARTICLE_POS stParticlePos;

			stParticlePos.x = x + nPlusX;
			stParticlePos.y = y + nPlusY;
			stParticlePos.z = z + nPlusZ;
			CalcExternalSide(x + nPlusX,y  + nPlusY, z  + nPlusZ,m_nXFileVoxCnt, m_nYFileVoxCnt, m_nZFileVoxCnt, stParticlePos);

			if(stParticlePos.iExternalSideCnt == 0)	
				stParticlePos.bInOut = false;
			else									
				stParticlePos.bInOut = true;

			iterOutsideDataDel = mapOutsideData.find(strKey);
			if(iterOutsideDataDel != mapOutsideData.end())
			{
				stParticlePos.sStoneType = iterOutsideDataDel->second.sStoneType;

				stParticlePos.fPorosity = iterOutsideDataDel->second.fPorosity;
				stParticlePos.fGranularDisintegration = iterOutsideDataDel->second.fGranularDisintegration;
				
// 				//! 중심 레이어
 				stParticlePos.sLayerIdx = iterOutsideDataDel->second.sLayerIdx;

				//! 수분포화도
				if(stParticlePos.sStoneType == 0)
					stParticlePos.fHaveWater = iterOutsideDataDel->second.fHaveWater;
				else
					stParticlePos.fHaveWater = 0.0; //! 공극에만 의미가 있기때문

			}
			else
			{
				stParticlePos.sStoneType = GetStoneType(&stParticlePos);

				stParticlePos.fPorosity = PDPorosity.GetDistParam();									// 공극률
				stParticlePos.fGranularDisintegration = m_pPDGranularDisintegration[stParticlePos.sStoneType]->GetDistParam();		// 입상붕괴 도달값 

				//! 중심 레이어
				//stParticlePos.sLayerIdx = stParticlePos.fCenterRange / (fMaxDistance / 10.0);
				stParticlePos.sLayerIdx = -1;

				//! 수분포화도
				if(stParticlePos.sStoneType == 0)
					stParticlePos.fHaveWater = 0.0;
				else
					stParticlePos.fHaveWater = -1.0; //! 공극에만 의미가 있기때문

			}


			m_mapInsertParticle.insert(make_pair(strKey,stParticlePos));
		}
	}


}




void CModelingWeatheringOfRockDlg::OnBnClickedButtonCalcRockAgingCancel()
{
	m_bStopCalc = TRUE;

	m_editGrDisCnt.SetWindowText(L"");
	m_editGrTotalCnt.SetWindowText(L"");
	m_editCalcTryCnt.SetWindowText(L"");
}



void CModelingWeatheringOfRockDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

/*iType : 0 : surface, 1: solid X, 2: solid Y, 3: solid Z*/
void CModelingWeatheringOfRockDlg::ReplaceFileInfo(unsigned int nX,unsigned int nY,unsigned int nZ,short iType)
{
	int nZGapSize = m_nZFileVoxCnt/dfTHREAD_MAX_CNT;

	int nIndex = nZ / nZGapSize;
	int nZMod = nZ % nZGapSize;
		
	vector<bool>	*vecBChk = NULL;
	vector<bool>	*vecBChkT = NULL;
	CString strFileName = L"";
	switch (iType)
	{
	case 0:
		vecBChk = &g_vecVoxelState;
		
//		vecBChk = &g_vecVoxelDivState[nIndex];
// 		vecBChkT = &g_vecVoxelDivState[nIndex];
// 		vecBChkT->at(lSeekIndex) = 1;


		strFileName.Format(L"SurfaceVoxel%02d.dat",nIndex);
		break;
	case 1:
		vecBChk = &g_vecVoxelXState;
		strFileName.Format(L"SolidXVoxel%02d.dat",nIndex);
		break;
	case 2:
		vecBChk = &g_vecVoxelYState;
		strFileName.Format(L"SolidYVoxel%02d.dat",nIndex);
		break;
	case 3:
		vecBChk = &g_vecVoxelZState;
		strFileName.Format(L"SolidZVoxel%02d.dat",nIndex);
		break;
	case 4:
		vecBChk = &g_vecVoxelTotal;
		strFileName.Format(L"SolidVoxel%02d.dat",nIndex);
		break;
	default:
		break;
	}

	unsigned long long lSeekIndex = nX + (m_nXFileVoxCnt * nY) + (m_nXFileVoxCnt * m_nYFileVoxCnt * nZMod);
	//! 메모리 처리
	if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{
		bool chTmp = 1;
		//  [4/4/2018 kjky12] 각 영역별로 나눈후 합치려했으나 이상해서 접음
// 		if(iType == 0)
// 		{
// 			vecBChk->at(lSeekIndex) = chTmp;
// 		}
// 		else
// 		{
			unsigned long long  Idx = lSeekIndex + (nIndex * m_nDivFileSize);
			vecBChk->at(Idx) = chTmp;
//		}
 		
 		

	}
	else // 파일 처리
	{
		CString strNumPath = gf_GetModulePath() + strFileName;

		basic_ofstream< bool > fileStream( strNumPath , ios::out | ios::in | ios::binary );    

		fileStream.seekp( lSeekIndex );
		bool chTmp = 1;
		fileStream.put( chTmp );
		fileStream.close();
	}

	



}


bool CModelingWeatheringOfRockDlg::VexelXDataSort(ST_VOXEL_3D_POS &s, ST_VOXEL_3D_POS &t)
{
	/** 
	@details 적용 시간에 따른 벡터 정렬 함수
	@param[in] s : 비교 대상 구조체
	@param[in] t : 비교 대상 구조체
	@return 비교 결과
	*/
	return s.x < t.x;
	//return s.nX > t.nX;
}

bool CModelingWeatheringOfRockDlg::VexelYDataSort(ST_VOXEL_3D_POS &s, ST_VOXEL_3D_POS &t)
{
	/** 
	@details 적용 시간에 따른 벡터 정렬 함수
	@param[in] s : 비교 대상 구조체
	@param[in] t : 비교 대상 구조체
	@return 비교 결과
	*/
	return s.y < t.y;
	//return s.nY > t.nY;
}

bool CModelingWeatheringOfRockDlg::VexelZDataSort(ST_VOXEL_3D_POS &s, ST_VOXEL_3D_POS &t)
{
	/** 
	@details 적용 시간에 따른 벡터 정렬 함수
	@param[in] s : 비교 대상 구조체
	@param[in] t : 비교 대상 구조체
	@return 비교 결과
	*/
	return s.z < t.z;
	//return s.nZ > t.nZ;
}


void CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling()
{
	g_vecVoxelState.clear();
	g_vecCompleteVoxelPosSolid.clear();

	m_nComboLogicProcess = m_comboUseCuda.GetCurSel();
	if(m_nComboLogicProcess == dfLOGIC_CPU_CPU)
	{
		g_vecVoxelXState.clear();
		g_vecVoxelYState.clear();
		g_vecVoxelZState.clear();
		g_vecVoxelTotal.clear();
	}
	else 
	{
		g_vecVoxelTotalCuda.clear();

	}
	

	

	CString strTmp;
	m_editXAxiDivide.GetWindowText(strTmp);

	g_nDivideCnt = _wtoi(strTmp) - 4;
	
	if(m_nSimulIdx <= 1 && g_strFilePathName.IsEmpty())
	{
		g_strFilePathName =L"";

		CFileDialog FileDlg(TRUE, _T("obj"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("obj File(*.obj)|*.obj||") );
		if (FileDlg.DoModal() == IDOK )
		{
			g_strFilePathName = FileDlg.GetPathName();
		}

		if(g_strFilePathName.IsEmpty())
			return;
	}

	m_nThreadAnalNum = 0;
	m_nThreadFinshNum = 0;

	g_bVoxelComplete = FALSE;




	CObjLoader _obj1;
	_obj1.Load (CT2A(g_strFilePathName));

	float nGap = 0.0;

	if(g_nSelectAxis == 0)
	{
		nGap = (_obj1.m_fMaxX - _obj1.m_fMinX) / (float)g_nDivideCnt;
		
		m_nXFileVoxCnt = g_nDivideCnt + 4 ;
		m_nYFileVoxCnt = ((_obj1.m_fMaxY - _obj1.m_fMinY) / nGap) + 4;
		m_nZFileVoxCnt = ((_obj1.m_fMaxZ - _obj1.m_fMinZ) / nGap) + 4;
	}
	else if(g_nSelectAxis == 1)
	{
		nGap = (_obj1.m_fMaxY - _obj1.m_fMinY) / (float)g_nDivideCnt;
		m_nXFileVoxCnt = ((_obj1.m_fMaxX - _obj1.m_fMinX) / nGap) + 4;
		m_nYFileVoxCnt = g_nDivideCnt + 4 ;
		m_nZFileVoxCnt = ((_obj1.m_fMaxZ - _obj1.m_fMinZ) / nGap) + 4;
	}
	else
	{
		nGap = (_obj1.m_fMaxZ - _obj1.m_fMinZ) / (float)g_nDivideCnt;

		m_nXFileVoxCnt = ((_obj1.m_fMaxX - _obj1.m_fMinX) / nGap) + 4;
		m_nYFileVoxCnt = ((_obj1.m_fMaxY - _obj1.m_fMinY) / nGap) + 4;
		m_nZFileVoxCnt = g_nDivideCnt + 4 ;
	}


	int nRDD = m_nZFileVoxCnt % dfTHREAD_MAX_CNT;
	if(m_nZFileVoxCnt % dfTHREAD_MAX_CNT)
	{
		m_nZFileVoxCnt += (dfTHREAD_MAX_CNT - (m_nZFileVoxCnt % dfTHREAD_MAX_CNT));
	}

//! 복셀 개수 확인
// 	CString strTrace = L"";
// 	strTmp.Format(L"X: %d\t", m_nXFileVoxCnt);
// 	strTrace+=strTmp;
// 	strTmp.Format(L"Y: %d\t", m_nYFileVoxCnt);
// 	strTrace+=strTmp;
// 	strTmp.Format(L"Z: %d", m_nZFileVoxCnt);
// 	strTrace+=strTmp;
// 	TRACE(strTrace);
// 
	//PostMessage(WM_SIMULATION_VOXEL, m_nSimulIdx++, m_nSimulObjectIdx);
// 
// 	return;

	m_nDivFileSize = (((m_nXFileVoxCnt) * (m_nYFileVoxCnt) * (m_nZFileVoxCnt)) / dfTHREAD_MAX_CNT);

	//! 메모리 처리
	if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{
// 		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
// 		{
// 			g_vecVoxelDivState[n].resize(m_nDivFileSize);
// 		}
		g_vecVoxelState.resize(m_nXFileVoxCnt* m_nYFileVoxCnt * m_nZFileVoxCnt);

	}
	else
	{
		bool* bInitVoxel = new bool[m_nDivFileSize];
		memset(bInitVoxel,NULL, m_nDivFileSize);
		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
		{
			CString strFileName = L"";
			strFileName.Format(L"SurfaceVoxel%02d.dat",n);
			CString strNumPath = gf_GetModulePath() + strFileName;
			FILE *p_Numfile = _wfopen(strNumPath, L"wb"); 


			if(NULL != p_Numfile)
			{ 
				fwrite(bInitVoxel, sizeof(bool), m_nDivFileSize, p_Numfile); 
			}

			fclose(p_Numfile);
		}
		M_A_DELETE(bInitVoxel);
	}
	

	CString strTempT = L"";
	strTempT.Format(L"%s", g_strFilePathName);
	ShowTraceTime(strTempT);
	
	strTempT.Format(L"pixel : %s, Thread %d", strTmp, dfTHREAD_MAX_CNT);
	ShowTraceTime(strTempT);
	int nMax = m_nXFileVoxCnt*m_nYFileVoxCnt*m_nZFileVoxCnt;
	strTmp.Format(L"x-%ld", m_nXFileVoxCnt);	strTempT = strTmp;
	strTmp.Format(L",y-%ld", m_nYFileVoxCnt);	strTempT += strTmp;
	strTmp.Format(L",z-%ld", m_nZFileVoxCnt);	strTempT += strTmp;
	strTmp.Format(L",all-%ld", nMax);			strTempT += strTmp;
	ShowTraceTime(strTempT);
	ShowTraceTime(L"Voxel Start");
	
	ShowTraceTime(L"Start Surface->Voxel");

	m_obj1 = _obj1;
	//m_obj1.Load (CT2A(g_strFilePathName));
	for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
	{
		_beginthread( ThreadSurfaceVoxelization, 0, this);
		m_nThreadAnalNum++;
		Sleep(1);
	}

	//////////////////////////////////////////////////////////////////////////
	//Surface에 필요한거 넣어주기
// 	struct sFaceCudaTmp
// 	{
// 		int n;
// 
// 		int v[5];	//vertices
// 		int vt[5];	//text coords
// 		int vn[5];	//normals
// 
// 		sFaceCudaTmp() {
// 			memset (this, 0, sizeof(sFaceCuda));
// 		}
// 	};
// 
// 	struct sPartCudaTmp
// 	{
// 		char name[256];
// 		int nUseFaceCnt;
// 		sFaceCudaTmp faces[dfMAX_FACE_CNT];
// 
// 		sPartCudaTmp() {
// 			name[0] = 0;
// 		}
// 	};
// 
// 
// 	m_GPUSurface.SetGlobalValue(g_nDivideCnt, g_nSelectAxis, g_fRectSize);
// 		
// 
// 	sVertex	sVertexTmp[dfMAX_VERTEX_CNT] = {NULL,};
// 	copy(m_obj1.vertexes.begin(), m_obj1.vertexes.end(), sVertexTmp);
// 	
// 
// 
// 	sPartCudaTmp	sPartCudaTmp1[dfMAX_PART_CNT];
// 	memset(sPartCudaTmp1, NULL, sizeof(sPartCudaTmp) * dfMAX_PART_CNT);
// 	for (int v=  0; v < dfMAX_PART_CNT && v < m_obj1.parts.size(); v++)
// 	{
// 		memcpy(sPartCudaTmp1[v].name, m_obj1.parts[v].name, 256);
// 		sPartCudaTmp1[v].nUseFaceCnt = m_obj1.parts[v].faces.size();
// 		copy(m_obj1.parts[v].faces.begin(), m_obj1.parts[v].faces.end(), (sFace*)sPartCudaTmp1[v].faces);
// 	}
//  
//  	m_GPUSurface.SetObjectValue(m_obj1.m_fMaxX,	m_obj1.m_fMaxY,	m_obj1.m_fMaxZ,	m_obj1.m_fMinX,	m_obj1.m_fMinY,	m_obj1.m_fMinZ,
//  		m_obj1.vertexes.size(), (char*)sVertexTmp, m_obj1.parts.size(), (char*)sPartCudaTmp1);
// 
// 
// 	m_GPUSurface.SetGlobalEtc(m_nXFileVoxCnt,	m_nYFileVoxCnt,	m_nZFileVoxCnt);
// 
// 	m_GPUSurface.SetSurfaceVoxelization();


}


void ThreadSurfaceVoxelization( void* pArguments )
{
	int nNum = m_nThreadAnalNum-1;


	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;

	pParent->m_editProcessStatus.SetWindowTextW(L"Start Calculate");

	BoxinsetersectTriangle BoxinsetersectTriangle1;

	//! 부모를 사용하지 않기위해 추가함..
	CObjLoader m_obj1 = pParent->m_obj1;
	int nZGapSize = pParent->m_nZFileVoxCnt/dfTHREAD_MAX_CNT;
	unsigned long long  m_nXFileVoxCnt = pParent->m_nXFileVoxCnt;
	unsigned long long  m_nYFileVoxCnt = pParent->m_nYFileVoxCnt;

	//CObjLoader _obj1;
	//	_obj1.Load ("Model/LegoBrick2_1withHole.obj");


	float fGap = 0.0;

	if(g_nSelectAxis == 0)
		fGap = (m_obj1.m_fMaxX - m_obj1.m_fMinX) / float(g_nDivideCnt);
	else if(g_nSelectAxis == 1)
		fGap = (m_obj1.m_fMaxY - m_obj1.m_fMinY) / float(g_nDivideCnt);
	else
		fGap = (m_obj1.m_fMaxZ - m_obj1.m_fMinZ) / float(g_nDivideCnt);

	float triverts[6][3];


	float r[3];
	g_fRectSize = fGap;
	r[0] = g_fRectSize/ 2.0;
	r[1] = g_fRectSize/ 2.0;
	r[2] = g_fRectSize/ 2.0;


	int vt_size = m_obj1.vertexes.size ();

	int nPartSize = m_obj1.parts.size ();

	for (unsigned int h=0; h<nPartSize; ++h) 
	{
		vector<sFace> &faces = m_obj1.parts[h].faces;

		int nFaceCnt = (int)faces.size();
		int nDiv = (int)faces.size() / dfTHREAD_MAX_CNT;
		int nAddLast = 0;

		if(nDiv <= 0)
			nDiv = 1;

		if((nNum+1) * nDiv > nFaceCnt)
			continue;

		if(nNum == dfTHREAD_MAX_CNT - 1)
			nAddLast = nFaceCnt - (nDiv * dfTHREAD_MAX_CNT);

		for (unsigned int i=(nNum * nDiv) , ip= ((nNum+1) * nDiv) + nAddLast/*faces.size ()*/; i<ip; ++i) 
		{
// 			TRACE("i:%d\n",i);
// 			continue;

			//TRACE("i:%d",i);

			sPartData PartData;
			//! nPartCnt는 메시의 꼭지점 개수이다(nPartCnt =3 일 경우 삼각메시).
			int nPartCnt = 0;
			for (int j=0,jn= faces[i].n; j<jn; ++j) 
			{
				int &v  = faces[i].v[j];
				if (0 < v && v <= vt_size) 
				{
					sVertex &vt = m_obj1.vertexes[v - 1];

					triverts[j][0] = vt.x;
					triverts[j][1] = vt.y;
					triverts[j][2] = vt.z;

					if(PartData.fMaxX < vt.x)
						PartData.fMaxX = vt.x;
					if(PartData.fMaxY < vt.y)
						PartData.fMaxY = vt.y;
					if(PartData.fMaxZ < vt.z)
						PartData.fMaxZ = vt.z;

					if(PartData.fMinX > vt.x)
						PartData.fMinX = vt.x;
					if(PartData.fMinY > vt.y)
						PartData.fMinY = vt.y;
					if(PartData.fMinZ > vt.z)
						PartData.fMinZ = vt.z;

					nPartCnt++;
				}

				//TRACE("\tv:%d\t", v);

			}
			
// 			TRACE("\n");
// 			TRACE("minX:%f\tmaxX:%f\tminY:%f\tmaxY:%f\tminZ:%f\tmaxZ:%f\n", PartData.fMinX, PartData.fMaxX, PartData.fMinY, PartData.fMaxY, PartData.fMinZ, PartData.fMaxZ);
// 
// 			continue;


			int nXMin = ((PartData.fMinX-m_obj1.m_fMinX) / fGap) - 2;
			int nXMax = ((PartData.fMaxX-m_obj1.m_fMinX) / fGap) + 2;

			int nYMin = ((PartData.fMinY-m_obj1.m_fMinY) / fGap) - 2;
			int nYMax = ((PartData.fMaxY-m_obj1.m_fMinY) / fGap) + 2;

			int nZMin = ((PartData.fMinZ-m_obj1.m_fMinZ) / fGap) - 2;
			int nZMax = ((PartData.fMaxZ-m_obj1.m_fMinZ) / fGap) + 2;

			//TRACE("x:%d,%d\ty:%d,%d\tz:%d,%d\t\n",nXMin,nXMax,nYMin,nYMax,nZMin,nZMax);
// 
// 			continue;

			int nGap = 0;
			//! nPartCnt 메시는 최소 삼각메시이므로...
			for(int n = 3 ; n <= nPartCnt ; n++)
			{

				float Triver[3][3] = {0.0,};
				for(int v = 0 ; v < 3 ; v++)
				{
					if(v == 0)
					{
						Triver[v][0] = triverts[v][0];
						Triver[v][1] = triverts[v][1];
						Triver[v][2] = triverts[v][2];
					}
					else
					{
						Triver[v][0] = triverts[v+nGap][0];
						Triver[v][1] = triverts[v+nGap][1];
						Triver[v][2] = triverts[v+nGap][2];
					}
				}
				nGap++;

				float fZ = m_obj1.m_fMinZ +((float)nZMin * fGap);
				for(int nZCnt = nZMin; nZCnt <= nZMax; nZCnt++)
				{
					float fY = m_obj1.m_fMinY +((float)nYMin * fGap)  ;
					for(int nYCnt = nYMin; nYCnt <= nYMax; nYCnt++)
					{
						float fX = m_obj1.m_fMinX +((float)nXMin * fGap)  ;
						for(int nXCnt = nXMin; nXCnt <= nXMax; nXCnt++)
						{
							//! 해당 복셀의 중심 좌표
							float afCenter[3];
							afCenter[0] = fX;
							afCenter[1] = fY;
							afCenter[2] = fZ;


							//해당 복셀의 인덱스
							//nXCnt, nYCnt, nZCnt

							if(BoxinsetersectTriangle1.isectboxtri(afCenter, r, Triver))
							{
								pParent->ReplaceFileInfo(nXCnt+2,nYCnt+2,nZCnt+2);
								// 									TRACE3("TRUE %d,%d,%d\n",nXCnt,nYCnt,nZCnt);
							}
							else
							{
								// 									TRACE3("FALSE %d,%d,%d\n",nXCnt,nYCnt,nZCnt);
							}

							//TRACE("Check fX:%f nXCnt:%d \t fY:%f nYCnt:%d \t fZ:%f nZCnt:%d \t\n", fX, nXCnt, fY, nYCnt, fZ, nZCnt);

							//	nXCnt++;
							fX+=fGap;
						}
						//nYCnt++;
						fY+=fGap;
					}
					//nZCnt++;
					fZ+=fGap;
				}
			}
		}
	}

	m_nThreadFinshNum++;

	if(m_nThreadFinshNum == dfTHREAD_MAX_CNT)
	{

		// 		pParent->m_fMaxX=pParent->m_obj1.m_fMaxX;
		// 		pParent->m_fMaxY=pParent->m_obj1.m_fMaxY;
		// 		pParent->m_fMaxZ=pParent->m_obj1.m_fMaxZ;
		// 		pParent->m_fMinX=pParent->m_obj1.m_fMinX;
		// 		pParent->m_fMinY=pParent->m_obj1.m_fMinY;
		// 		pParent->m_fMinZ=pParent->m_obj1.m_fMinZ;

		pParent->SendMessage(WM_FINISH_VOXEL_MSG,0,0);
	}
	//int d = 0;
	CString strProcessStatus;
	strProcessStatus.Format(L"Progress : %.2f %%",float((float)m_nThreadFinshNum / (float)dfTHREAD_MAX_CNT)*100.0);
	pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);
	//pParent->m_ProgressCtrl.SetPos(m_nThreadFinshNum * 10);
}


void CModelingWeatheringOfRockDlg::SetGpuData()
{
	m_GPUSolid.m_nXFileVoxCnt = m_nXFileVoxCnt;
	m_GPUSolid.m_nYFileVoxCnt = m_nYFileVoxCnt;
	m_GPUSolid.m_nZFileVoxCnt = m_nZFileVoxCnt;

	m_GPUCalcRockAgingInner.m_nXFileVoxCnt = m_nXFileVoxCnt;
	m_GPUCalcRockAgingInner.m_nYFileVoxCnt = m_nYFileVoxCnt;
	m_GPUCalcRockAgingInner.m_nZFileVoxCnt = m_nZFileVoxCnt;

	CString strInput = L"";
	m_editTopRate.GetWindowText(strInput);
	m_GPUCalcRockAgingInner.m_fTopRate = _wtof(strInput);
	
	m_editSideRate.GetWindowText(strInput);
	m_GPUCalcRockAgingInner.m_fSideRate = _wtof(strInput);
	
	m_editBottomRate.GetWindowText(strInput);
	m_GPUCalcRockAgingInner.m_fBottomRate = _wtof(strInput);

	CString strTmp;
	CString strTmp1;
	CString strTmp2;
	
	//! 공극률 개수
	m_editCalcPorosity.GetWindowText(strTmp);
	//! 가속 계수
	m_editCalcMulti.GetWindowText(strTmp1);

	float fCoefficient = _wtof(strTmp) * _wtof(strTmp1);	// 공극률 계수값 (공극률 * 가속 계수)
	m_GPUCalcRockAgingInner.m_fCoefficient = fCoefficient;


	//////////////////////////////////////////////////////////////////////////
	//! 수분 흡수율 -> 공극에만 적용되고 공극에서 6방향으로!!
	m_editCalcWaterInnerAbsorption.GetWindowText(strTmp);
	//! 레이별 수분부피차감률
	m_editCalcLayerWaterAborption.GetWindowText(strTmp1);
	//! 수분 부피 팽창률
	m_editCalcWaterChange.GetWindowText(strTmp2);

	//! 수분 흡수율
	m_GPUCalcRockAgingInner.m_fCalcWaterInnerAbsorption =  _wtof(strTmp);
	//! 레이어별 수분 부피 차감율
	m_GPUCalcRockAgingInner.m_fCalcLayerWaterAborption = _wtof(strTmp1);
	//! 수분 팽창
	m_GPUCalcRockAgingInner.m_fCalcWaterChange =  _wtof(strTmp2);

}

LRESULT CModelingWeatheringOfRockDlg::OnFinshVoxelMsg(WPARAM wParam, LPARAM lParam)
{
	//_endthread();

	CString strProcessStatus;
	strProcessStatus.Format(L"Start Surface Voxel Clearing");
	m_editProcessStatus.SetWindowTextW(strProcessStatus);

	ShowTraceTime(L"End Surface->Voxel", 1);


// 	//! 복셀부분만하게함..
// 	PostMessage(WM_SIMULATION_VOXEL, m_nSimulIdx++, 0);
// 	return 1;


	ShowTraceTime(L"Start Thread Voxel - 1");


	g_vecCompleteVoxelPos.clear();

	CString strFileName = L"";
	CString strSurPath1 = L"";
	FILE *p_Devfile = NULL;

	if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{
// 		g_vecVoxelState.resize(m_nXFileVoxCnt* m_nYFileVoxCnt * m_nZFileVoxCnt);
// 
// 		int nStep = 0;
// 		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
// 		{
// 			copy(g_vecVoxelDivState[n].begin(), g_vecVoxelDivState[n].begin() + m_nDivFileSize, g_vecVoxelState.begin() + nStep);
// 
// 			nStep += m_nDivFileSize;
// 
// 			g_vecVoxelDivState[n].clear();
// 		}
	}
	else  //! 파일 처리만 해줌
	{
		// 표면 복셀 처리된 파일 합치는 부분!!!	
		strFileName.Format(L"SurfaceVoxel.dat");
		strSurPath1 = gf_GetModulePath() + strFileName;
		p_Devfile = _wfopen(strSurPath1, L"wb"); 

		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
		{
			strFileName.Format(L"SurfaceVoxel%02d.dat",n);
			strSurPath1 = gf_GetModulePath() + strFileName;
			FILE *p_Surfile = _wfopen(strSurPath1, L"rb"); 
			bool* fdata = new bool[m_nDivFileSize];

			if(NULL != p_Surfile)
			{ 
				fread(fdata, sizeof(bool), m_nDivFileSize, p_Surfile); 

				if(NULL != p_Devfile)
				{ 
					fwrite(fdata, sizeof(bool), m_nDivFileSize, p_Devfile); 
				}
			}
			M_A_DELETE(fdata);
			fclose(p_Surfile);
		}
		fclose(p_Devfile);


		strFileName.Format(L"SurfaceVoxel.dat");
		strSurPath1 = gf_GetModulePath() + strFileName;
		p_Devfile = _wfopen(strSurPath1, L"rb"); 
	}


	// 	int nXHalfCnt = m_nXFileVoxCnt/2;
	// 	int nYHalfCnt = m_nYFileVoxCnt/2;
	// 	int nZHalfCnt = m_nZFileVoxCnt/2;


	bool bTRUE = false;
	unsigned long long nSeekSize = 0;

	ST_DELETE_VOXEL_POS stDeleteVoxelPos;
	for(int nZ = 0 ; nZ < m_nZFileVoxCnt ; nZ++)
	{
		for(int nY = 0 ; nY < m_nYFileVoxCnt ; nY++)
		{
			for(int nX = 0 ; nX < m_nXFileVoxCnt ; nX++)
			{

				nSeekSize = nX  + (m_nXFileVoxCnt * nY) + (m_nXFileVoxCnt * m_nYFileVoxCnt * nZ);

				//! 메모리 처리
				if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
				{
					bTRUE = g_vecVoxelState[nSeekSize];
				}
				else //파일 처리
				{
					_fseeki64(p_Devfile,nSeekSize ,SEEK_SET);
					fread(&bTRUE, sizeof(bool), 1, p_Devfile); 
				}

				if(bTRUE)
				{
					stDeleteVoxelPos.nX = nX;
					stDeleteVoxelPos.nY = nY;
					stDeleteVoxelPos.nZ = nZ;
					g_vecCompleteVoxelPos.push_back(stDeleteVoxelPos);
				}
			}
		}
	}

	if(p_Devfile)
		fclose(p_Devfile);

	
	ShowTraceTime(L"End Thread Voxel - 1", 1);
	ShowTraceTime(L"Start Thread Voxel - 2");


	strProcessStatus.Format(L"Save Surface Voxel ");
	m_editProcessStatus.SetWindowTextW(strProcessStatus);

	m_nSolidVoxelCnt = 0;
	CString strNumPath = gf_GetModulePath() + L"SurfaceNum.dat";
	FILE *p_Numfile = NULL; 

	CString strSurPath = gf_GetModulePath() + L"SurfacePos.dat";
	FILE *p_Surfile = NULL; 

	g_vecNumData.clear();
	g_vecSurData.clear();



	// 입상붕괴 도달값 계산을 위한 확률변수 설정
	CString strGranularDisintegration = L"";
	m_editGranularDisintegration.GetWindowText(strGranularDisintegration);
	double dGranularDisintegration = _wtof(strGranularDisintegration);
	CProbDistribution PDGranularDisintegration(3,0.0,dGranularDisintegration,0.001,0.0,0.0,0.0,0.0,0.0,0.0);
	// 공극률 설정을 위한 확률변수 설정
	CString strInitData = L"";
	m_editInitPorosity.GetWindowText(strInitData);
	double dInitPorosity = _wtof(strInitData);
	CProbDistribution PDPorosity(4,0.0,0.0,0.0001,dInitPorosity,0.0,0.0,0.0,0.0,0.0);

	int nSize = (int)g_vecCompleteVoxelPos.size();		

	//! 파일 처리
	if(m_nComboProcessSel == dfDATA_PROCESS_FILE)
	{
		p_Numfile = _wfopen(strNumPath, L"wb"); 
		p_Surfile = _wfopen(strSurPath, L"wb");

		if(NULL != p_Numfile || NULL != p_Surfile)
		{ 
			fwrite(&m_nXFileVoxCnt, sizeof(int), 1, p_Numfile); 
			fwrite(&m_nYFileVoxCnt, sizeof(int), 1, p_Numfile); 
			fwrite(&m_nZFileVoxCnt, sizeof(int), 1, p_Numfile); 
			fwrite(&nSize, sizeof(int), 1, p_Numfile); 

			fwrite(&m_nXFileVoxCnt, sizeof(int), 1, p_Surfile); 
			fwrite(&m_nYFileVoxCnt, sizeof(int), 1, p_Surfile); 
			fwrite(&m_nZFileVoxCnt, sizeof(int), 1, p_Surfile); 
			fwrite(&nSize, sizeof(int), 1, p_Surfile); 
		}
	}

	int nProcessCnt = 0;
	for(int v = 0 ; v <  nSize ; v++)
	{
		ST_VOXEL_FILE_POS stVoxelFilePos;
		stVoxelFilePos.nX = g_vecCompleteVoxelPos[v].nX;
		stVoxelFilePos.nY = g_vecCompleteVoxelPos[v].nY;
		stVoxelFilePos.nZ = g_vecCompleteVoxelPos[v].nZ;
		stVoxelFilePos.fPorosity				= 0;									// 공극률
		stVoxelFilePos.fGranularDisintegration = 0;		// 입상붕괴 도달값  ;
		//stVoxelFilePos.fPorosity				= PDPorosity.GetDistParam();									// 공극률
		//stVoxelFilePos.fGranularDisintegration = PDGranularDisintegration.GetDistParam();		// 입상붕괴 도달값  ;

		if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
		{
			g_vecNumData.push_back(stVoxelFilePos);
		}
		else
		{
			if(NULL != p_Numfile || NULL != p_Surfile)
				fwrite(&stVoxelFilePos, sizeof(ST_VOXEL_FILE_POS), 1, p_Numfile); 
		}


		strProcessStatus.Format(L"Save Surface Voxel Progress : %.2f %%",float((float)nProcessCnt++ / (float)nSize)*100.0);
		m_editProcessStatus.SetWindowTextW(strProcessStatus);
	}
	if(NULL != p_Numfile) 
		fclose(p_Numfile); 
	if(NULL != p_Surfile)
		fclose(p_Surfile); 


	strProcessStatus.Format(L"Save Complete");
	m_editProcessStatus.SetWindowTextW(strProcessStatus);


	
	ShowTraceTime(L"End Thread Voxel - 2", 1);
	ShowTraceTime(L"Start Thread Voxel - 3");

	//! 메모리 처리
	if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{

		if(m_nComboLogicProcess == dfLOGIC_GPU_GPU) //GPU 전체 로직
		{
			g_vecVoxelTotalCuda.resize(m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt);

			for (int v= 0; v < dfTOTAL_COUNT; v++)
			{
				M_A_DELETE(m_bDataTmp[v]);
				m_bDataTmp[v] = new bool[m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt];
				memset(m_bDataTmp[v], NULL, m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt);
			}
		}
		else
		{
			g_vecVoxelXState.resize(m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt);
			g_vecVoxelYState.resize(m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt);
			g_vecVoxelZState.resize(m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt);
			g_vecVoxelTotal.resize(m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt);

		}
	}
	else
	{
		// 솔리드 복셀화를 위한 파일 생성
		bool* bInitVoxel = new bool[m_nDivFileSize];
		memset(bInitVoxel,NULL, m_nDivFileSize);

		CString strFileNameX = L"";
		CString strFileNameY = L"";
		CString strFileNameZ = L"";

		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
		{

			strFileNameX.Format(L"SolidXVoxel%02d.dat",n);
			strFileNameY.Format(L"SolidYVoxel%02d.dat",n);
			strFileNameZ.Format(L"SolidZVoxel%02d.dat",n);
			strFileName.Format(L"SolidVoxel%02d.dat",n);

			strNumPath = gf_GetModulePath() + strFileNameX;
			FILE *p_NumfileX = _wfopen(strNumPath, L"wb"); 
			strNumPath = gf_GetModulePath() + strFileNameY;
			FILE *p_NumfileY = _wfopen(strNumPath, L"wb"); 
			strNumPath = gf_GetModulePath() + strFileNameZ;
			FILE *p_NumfileZ = _wfopen(strNumPath, L"wb"); 

			strNumPath = gf_GetModulePath() + strFileName;
			FILE *p_Numfile = _wfopen(strNumPath, L"wb"); 

			if(NULL != p_NumfileX)
			{ 
				fwrite(bInitVoxel, sizeof(bool), m_nDivFileSize, p_NumfileX); 
			}
			if(NULL != p_NumfileY)
			{ 
				fwrite(bInitVoxel, sizeof(bool), m_nDivFileSize, p_NumfileY); 
			}
			if(NULL != p_NumfileZ)
			{ 
				fwrite(bInitVoxel, sizeof(bool), m_nDivFileSize, p_NumfileZ); 
			}

			if(NULL != p_Numfile)
			{ 
				fwrite(bInitVoxel, sizeof(bool), m_nDivFileSize, p_Numfile); 
			}

			fclose(p_NumfileX);
			fclose(p_NumfileY);
			fclose(p_NumfileZ);
			fclose(p_Numfile);
		}

		M_A_DELETE(bInitVoxel);
	}
	
	ShowTraceTime(L"End Thread Voxel - 3", 1);

	m_nThreadAnalNum = 0;
	m_nThreadFinshNum = 0;


	CString strLog = L"";
	if(m_nComboLogicProcess == dfLOGIC_GPU_GPU) //GPU 전체 로직
	{
		//ShowTraceTime(L"Start XYZ Voxel(GPU)");
		// 		if(m_nComboBlockThreadSel == 0)
		// 		{
		// 			m_GPUSolid.CUDA_SolidVoxelizationXYZBlock(nSizeT, m_bDataTmp[dfVOXEL_IDX], m_bDataTmp[dfTOTAL_IDX]);
		// 
		// 		}
		// 		else
		// 		{
		// 
		// 			m_GPUSolid.CUDA_SolidVoxelizationXYZ(nSizeT, m_bDataTmp[dfVOXEL_IDX], m_bDataTmp[dfTOTAL_IDX]);
		// 		}


		ShowTraceTime(L"GPU-GPU");
		ShowTraceTime(L"Start Inner Voxel");

		SetGpuData();

		int nSizeT = g_vecVoxelState.size();
		memset(m_bDataTmp[dfVOXEL_IDX], NULL, nSizeT);
		memset(m_bDataTmp[dfTOTAL_IDX], NULL, nSizeT);
		copy(g_vecVoxelState.begin(), g_vecVoxelState.begin() + nSizeT, m_bDataTmp[dfVOXEL_IDX]);
		ShowTraceTime(L"External Voxel CPU->GPU COPY" , 1);

		ShowTraceTime(L"Start Inner Voxel -> Version 1");

		memset(m_bDataTmp[dfTOTAL_IDX], NULL, nSizeT);
		m_GPUSolid.CUDA_SolidVoxelizationXYZ(nSizeT, m_bDataTmp[dfVOXEL_IDX], m_bDataTmp[dfTOTAL_IDX]);
		std::copy(m_bDataTmp[dfTOTAL_IDX], m_bDataTmp[dfTOTAL_IDX]+nSizeT, g_vecVoxelTotalCuda.begin());
		
		ShowTraceTime(L"End Inner Voxel -> Version 1" , 1);


		Sleep(500);

		ShowTraceTime(L"Start Inner Voxel -> Version 2");

		memset(m_bDataTmp[dfTOTAL_IDX], NULL, nSizeT);
		m_GPUSolid.CUDA_SolidVoxelizationXYZBlock(nSizeT, m_bDataTmp[dfVOXEL_IDX], m_bDataTmp[dfTOTAL_IDX]);
		std::copy(m_bDataTmp[dfTOTAL_IDX], m_bDataTmp[dfTOTAL_IDX]+nSizeT, g_vecVoxelTotalCuda.begin());

		ShowTraceTime(L"End Inner Voxel -> Version 2" , 1);

		int nErrorCnt = 0;
		for (int v = 0; v < g_vecVoxelTotalCuda.size(); v++)
		{
			//if(g_vecVoxelTotalCudaTemp[v] != g_vecVoxelTotalCuda[v])
			if(g_vecVoxelTotalCuda[v] != g_vecVoxelTotal[v])
			{
				nErrorCnt++;
			}
		}
		CString strTempT = L"";
		if(nErrorCnt > 0)
		{
			strTempT.Format(L"Error Cnt : %d\n", nErrorCnt);
		}
		else
		{
			strTempT.Format(L"GOOD");
		}
		ShowTraceTime(strTempT);
		ShowTraceTime(L"\n", 2);


		SendMessage(WM_FINISH_SOLID_VOXEL_MSG,3,0);
	}
	else
	{
		ShowTraceTime(L"CPU-CPU");
		ShowTraceTime(L"Start Inner Voxel");


		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
		{
			_beginthread( ThreadSolidVoxelizationX, 0, this);
			m_nThreadAnalNum++;
			Sleep(1);
		}
	}


	return 0;
}

void ThreadSolidVoxelizationX( void* pArguments )
{
	int nNum = m_nThreadAnalNum-1;

	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;

	CString strProcessStatus;
	strProcessStatus.Format(L"X Axis start");
	pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);


	CString strFileName = L"";
	strFileName.Format(L"SurfaceVoxel.dat");
	CString strSurPath1 = gf_GetModulePath() + strFileName;
	FILE *p_Devfile = _wfopen(strSurPath1, L"rb"); 

	bool bTRUE = false;

	int nDiv = pParent->m_nZFileVoxCnt / dfTHREAD_MAX_CNT;
	int nAddLast = 0;

	if(nDiv <= 0)
		nDiv = 1;

	if(nNum == dfTHREAD_MAX_CNT - 1)
		nAddLast = pParent->m_nZFileVoxCnt - (nDiv * dfTHREAD_MAX_CNT);

	bool* bData = new bool[pParent->m_nXFileVoxCnt];

	if(!((nNum+1) * nDiv > pParent->m_nZFileVoxCnt))
	{
		for (unsigned int z=(nNum * nDiv) , ip= ((nNum+1) * nDiv) + nAddLast; z<ip ; z++) 
		{
			for(unsigned int y = 0 ; y < pParent->m_nYFileVoxCnt ; y++)
			{
				//! 메모리 처리
				unsigned long long nSeekSize = 0  + (pParent->m_nXFileVoxCnt * y) + (pParent->m_nXFileVoxCnt * pParent->m_nYFileVoxCnt * z);
				
				unsigned int nOutCnt = 0;
				int nStartIdx = -1;
				int nEndIdx = -1;
				unsigned int x = 0 ;

				if(pParent->m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
				{
					copy(g_vecVoxelState.begin() + nSeekSize, g_vecVoxelState.begin() + nSeekSize + pParent->m_nXFileVoxCnt, bData);
				}
				else // 파일처리
				{
					_fseeki64(p_Devfile, nSeekSize, SEEK_SET);
					fread(bData, sizeof(bool), pParent->m_nXFileVoxCnt, p_Devfile); 
				}

				
				for(x = 0 ; x < pParent->m_nXFileVoxCnt ; x++)
				{
					if(bData[x])	// 첫 데이터를 찾는다.
					{
						nStartIdx = x;
						break;
					}
					else
						nOutCnt++;
				}

				if(pParent->m_nXFileVoxCnt != nOutCnt)
				{
					for(x = pParent->m_nXFileVoxCnt-1 ; x >=0  ; x--)
					{
						if(bData[x])	// 첫 데이터를 찾는다.
						{
							nEndIdx = x;
							break;
						}
					}					
				}

				if(nStartIdx != -1 && nEndIdx != -1)
				{
					for(unsigned int s = nStartIdx; s <= nEndIdx ; s++)
					{
						pParent->ReplaceFileInfo(s,y,z,1);
					}
				}

			}

			strProcessStatus.Format(L"X Axis Solid Progress[%d] : %.2f %%",nNum,float((float)z / (float)pParent->m_nZFileVoxCnt) * 100.0);
			pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);
		}

	}
	M_A_DELETE(bData);

	if(p_Devfile)
		fclose(p_Devfile);

	//	M_A_DELETE(bData);

	m_nThreadFinshNum++;
	TRACE1("TRUE %d\n",nNum);

	if(m_nThreadFinshNum == dfTHREAD_MAX_CNT)
	{
		pParent->SendMessage(WM_FINISH_SOLID_VOXEL_MSG,0,0);
	}
}

void ThreadSolidVoxelizationY( void* pArguments )
{
	int nNum = m_nThreadAnalNum-1;

	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;


	CString strProcessStatus;
	strProcessStatus.Format(L"Y Axis Start");
	pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);


	CString strFileName = L"";
	strFileName.Format(L"SurfaceVoxel.dat");
	CString strSurPath1 = gf_GetModulePath() + strFileName;
	FILE *p_Devfile = _wfopen(strSurPath1, L"rb"); 

	bool bTRUE = false;

	int nDiv = pParent->m_nZFileVoxCnt / dfTHREAD_MAX_CNT;
	int nAddLast = 0;

	if(nDiv <= 0)
		nDiv = 1;

	if(nNum == dfTHREAD_MAX_CNT - 1)
		nAddLast = pParent->m_nZFileVoxCnt - (nDiv * dfTHREAD_MAX_CNT);


	bool* bData = new bool[pParent->m_nYFileVoxCnt];
	unsigned long long  nSeekSize = 0;
	if(!((nNum+1) * nDiv > pParent->m_nZFileVoxCnt))
	{
		for (int z=(nNum * nDiv) , ip= ((nNum+1) * nDiv) + nAddLast; z<ip ; z++) 
		{
			for(int x = 0 ; x < pParent->m_nXFileVoxCnt ; x++)
			{
				unsigned int y = 0 ;

				for(y = 0 ; y < pParent->m_nYFileVoxCnt ; y++)	
				{
					nSeekSize = x  + (pParent->m_nXFileVoxCnt * y) + (pParent->m_nXFileVoxCnt * pParent->m_nYFileVoxCnt * z);
					if(pParent->m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
					{
						bData[y] = g_vecVoxelState[nSeekSize];
					}
					else // 파일처리
					{
						_fseeki64(p_Devfile,nSeekSize ,SEEK_SET);
						fread(&bData[y], sizeof(bool), 1, p_Devfile); 
					}
					//TRACE("%d\n", nSeekSize);
				}
				//TRACE("\n\n");


				unsigned int nOutCnt = 0;
				int nStartIdx = -1;
				int nEndIdx = -1;
				
				for(y = 0 ; y < pParent->m_nYFileVoxCnt ; y++)
				{
					if(bData[y])	// 첫 데이터를 찾는다.
					{
						nStartIdx = y;
						break;
					}
					else
						nOutCnt++;
				}

				if(pParent->m_nYFileVoxCnt != nOutCnt)
				{
					for(y = pParent->m_nYFileVoxCnt-1 ; y >=0  ; y--)
					{
						if(bData[y])	// 첫 데이터를 찾는다.
						{
							nEndIdx = y;
							break;
						}
					}				
				}


				if(nStartIdx != -1 && nEndIdx != -1)
				{
					for(unsigned int s = nStartIdx; s <= nEndIdx ; s++)
					{
						pParent->ReplaceFileInfo(x,s,z,2);
					}
				}
				

				
				
			}

			strProcessStatus.Format(L"Y Axis Solid Voxelization Progress[%d] : %.2f %%",nNum,float((float)z / (float)pParent->m_nZFileVoxCnt) * 100.0);
			pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);
		}

	}
	
	M_A_DELETE(bData);
	if(p_Devfile)
		fclose(p_Devfile);


	m_nThreadFinshNum++;
	TRACE1("TRUE %d\n",nNum);

	if(m_nThreadFinshNum == dfTHREAD_MAX_CNT)
	{
		pParent->SendMessage(WM_FINISH_SOLID_VOXEL_MSG,1,0);
	}
	
}

void ThreadSolidVoxelizationZ( void* pArguments )
{
	int nNum = m_nThreadAnalNum-1;

	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;

	CString strProcessStatus;
	strProcessStatus.Format(L"Z Axis Start");
	pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);

	CString strFileName = L"";
	strFileName.Format(L"SurfaceVoxel.dat");
	CString strSurPath1 = gf_GetModulePath() + strFileName;
	FILE *p_Devfile = _wfopen(strSurPath1, L"rb"); 

	bool bTRUE = false;

	int nDiv = pParent->m_nYFileVoxCnt / dfTHREAD_MAX_CNT;
	int nAddLast = 0;

	if(nDiv <= 0)
		nDiv = 1;

	if(nNum == dfTHREAD_MAX_CNT - 1)
		nAddLast = pParent->m_nYFileVoxCnt - (nDiv * dfTHREAD_MAX_CNT);

	bool* bData = new bool[pParent->m_nZFileVoxCnt];
	unsigned long long nSeekSize = 0;

	if(!((nNum+1) * nDiv > pParent->m_nYFileVoxCnt))
	{
		for (int y=(nNum * nDiv) , ip= ((nNum+1) * nDiv) + nAddLast; y<ip ; y++) 
		{
			for(int x = 0 ; x < pParent->m_nXFileVoxCnt ; x++)
			{
				unsigned int z = 0 ;
				for(z = 0 ; z < pParent->m_nZFileVoxCnt ; z++)	
				{
					nSeekSize = x  + (pParent->m_nXFileVoxCnt * y) + (pParent->m_nXFileVoxCnt * pParent->m_nYFileVoxCnt * z);

					if(pParent->m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
					{
						bData[z] = g_vecVoxelState[nSeekSize ];
					}
					else // 파일처리
					{
						_fseeki64(p_Devfile,nSeekSize ,SEEK_SET);
						fread(&bData[z], sizeof(bool), 1, p_Devfile); 
					}
					
					//TRACE("%d\n", nSeekSize);
				}
				//TRACE("\n");


				unsigned int nOutCnt = 0;
				int nStartIdx = -1;
				int nEndIdx = -1;

				for(z = 0 ; z < pParent->m_nZFileVoxCnt ; z++)
				{
					if(bData[z])	// 첫 데이터를 찾는다.
					{
						nStartIdx = z;
						break;
					}
					else
						nOutCnt++;
				}

				if(pParent->m_nZFileVoxCnt != nOutCnt)
				{
					for(z = pParent->m_nZFileVoxCnt-1 ; z >=0  ; z--)
					{
						if(bData[z])	// 첫 데이터를 찾는다.
						{
							nEndIdx = z;
							break;
						}
					}
					
				}

				if(nStartIdx != -1 && nEndIdx != -1)
				{
					for(unsigned int s = nStartIdx; s <= nEndIdx ; s++)
					{
						pParent->ReplaceFileInfo(x,y,s,3);
					}
				}
				
			}

			strProcessStatus.Format(L"Z Axis Solid Voxelization Progress[%d] : %.2f %%",nNum,float((float)y / (float)pParent->m_nYFileVoxCnt) * 100.0);
			pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);
		}
	}

	M_A_DELETE(bData);
	if(p_Devfile)
		fclose(p_Devfile);

	m_nThreadFinshNum++;
	TRACE1("TRUE %d\n",nNum);

	if(m_nThreadFinshNum == dfTHREAD_MAX_CNT)
	{
		pParent->SendMessage(WM_FINISH_SOLID_VOXEL_MSG,2,0);
	}
	
}


LRESULT CModelingWeatheringOfRockDlg::OnFinshSolidVoxelMsg(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 0)
	{
		//! 파일 처리
		if(m_nComboProcessSel == dfDATA_PROCESS_FILE)
		{
			// 솔리드 복셀 처리된 파일 합치는 부분!!!	
			CString strFileName = L"";
			strFileName.Format(L"SolidXVoxel.dat");
			CString strSurPath1 = gf_GetModulePath() + strFileName;
			FILE *p_Devfile = _wfopen(strSurPath1, L"wb"); 

			for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
			{
				strFileName.Format(L"SolidXVoxel%02d.dat",n);
				strSurPath1 = gf_GetModulePath() + strFileName;
				FILE *p_Surfile = _wfopen(strSurPath1, L"rb"); 
				bool* fdata = new bool[m_nDivFileSize];

				if(NULL != p_Surfile)
				{ 
					fread(fdata, sizeof(bool), m_nDivFileSize, p_Surfile); 

					if(NULL != p_Devfile)
					{ 
						fwrite(fdata, sizeof(bool), m_nDivFileSize, p_Devfile); 
					}
				}
				M_A_DELETE(fdata);
				fclose(p_Surfile);
			}
			fclose(p_Devfile);
		}

		//PostMessage(WM_SIMULATION_VOXEL, m_nSimulIdx++, m_nSimulObjectIdx);


		//ShowTraceTime(L"Start Y Voxel");

		m_nThreadAnalNum = 0;
		m_nThreadFinshNum = 0;
		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
		{
			_beginthread( ThreadSolidVoxelizationY, 0, this);
			m_nThreadAnalNum++;
			Sleep(1);
		}


	}
	else if(wParam == 1)
	{
		//ShowTraceTime(L"End Y Voxel", 1);

// 		int nErrorCnt = 0;
// 		for (int v = 0; v < g_vecVoxelYState.size(); v++)
// 		{
// 			if(g_vecVoxelYState[v] != g_vecVoxelYStateCuda[v])
// 			{
// 				nErrorCnt++;
// 				// 				CString strFormat = L"";
// 				// 				strFormat.Format(L"DataIdx : %d\n", v);
// 				// 				TRACE(strFormat);
// 			}
// 		}
// 		CString strTempT = L"";
// 		if(nErrorCnt > 0)
// 		{
// 			strTempT.Format(L"Y : Error Cnt : %d", nErrorCnt);
// 			ShowTraceTime(strTempT , 2);
// 		}
// 		else
// 		{
// 			strTempT.Format(L"Y : GOOD");
// 			ShowTraceTime(strTempT, 2);
// 		}
// 		g_vecVoxelYStateCuda.clear();




		//! 파일 처리
		if(m_nComboProcessSel == dfDATA_PROCESS_FILE)
		{
			// 솔리드 복셀 처리된 파일 합치는 부분!!!	
			CString strFileName = L"";
			strFileName.Format(L"SolidYVoxel.dat");
			CString strSurPath1 = gf_GetModulePath() + strFileName;
			FILE *p_Devfile = _wfopen(strSurPath1, L"wb"); 

			for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
			{
				strFileName.Format(L"SolidYVoxel%02d.dat",n);
				strSurPath1 = gf_GetModulePath() + strFileName;
				FILE *p_Surfile = _wfopen(strSurPath1, L"rb"); 
				bool* fdata = new bool[m_nDivFileSize];

				if(NULL != p_Surfile)
				{ 
					fread(fdata, sizeof(bool), m_nDivFileSize, p_Surfile); 

					if(NULL != p_Devfile)
					{ 
						fwrite(fdata, sizeof(bool), m_nDivFileSize, p_Devfile); 
					}
				}
				M_A_DELETE(fdata);
				fclose(p_Surfile);
			}
			fclose(p_Devfile);
		}


		m_nThreadAnalNum = 0;
		m_nThreadFinshNum = 0;
		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
		{
			_beginthread( ThreadSolidVoxelizationZ, 0, this);
			m_nThreadAnalNum++;
			Sleep(1);
		}

	}
	else if(wParam == 2)
	{
		//ShowTraceTime(L"End Z Voxel", 1);
// 		int nErrorCnt = 0;
// 		for (int v = 0; v < g_vecVoxelZState.size(); v++)
// 		{
// 			if(g_vecVoxelZState[v] != g_vecVoxelZStateCuda[v])
// 			{
// 				nErrorCnt++;
// 				// 				CString strFormat = L"";
// 				// 				strFormat.Format(L"DataIdx : %d\n", v);
// 				// 				TRACE(strFormat);
// 			}
// 		}
// 		CString strTempT = L"";
// 		if(nErrorCnt > 0)
// 		{
// 			strTempT.Format(L" Z : Error Cnt : %d", nErrorCnt);
// 			ShowTraceTime(strTempT , 2);
// 		}
// 		else
// 		{
// 			strTempT.Format(L" Z : GOOD");
// 			ShowTraceTime(strTempT, 2);
// 		}
// 		g_vecVoxelZStateCuda.clear();


		//! 파일 처리
		if(m_nComboProcessSel == dfDATA_PROCESS_FILE)
		{
			// 솔리드 복셀 처리된 파일 합치는 부분!!!	
			CString strFileName = L"";
			strFileName.Format(L"SolidZVoxel.dat");
			CString strSurPath1 = gf_GetModulePath() + strFileName;
			FILE *p_Devfile = _wfopen(strSurPath1, L"wb"); 

			for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
			{
				strFileName.Format(L"SolidZVoxel%02d.dat",n);
				strSurPath1 = gf_GetModulePath() + strFileName;
				FILE *p_Surfile = _wfopen(strSurPath1, L"rb"); 
				bool* fdata = new bool[m_nDivFileSize];

				if(NULL != p_Surfile)
				{ 
					fread(fdata, sizeof(bool), m_nDivFileSize, p_Surfile); 

					if(NULL != p_Devfile)
					{ 
						fwrite(fdata, sizeof(bool), m_nDivFileSize, p_Devfile); 
					}
				}
				M_A_DELETE(fdata);
				fclose(p_Surfile);
			}
			fclose(p_Devfile);
		}

		



		m_nThreadAnalNum = 0;
		m_nThreadFinshNum = 0;
		m_nSolidTotalCnt = 0;



		//ShowTraceTime(L"Start Total Voxel(CPU)");

		for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
		{
			_beginthread( ThreadTotalSolidVoxelization, 0, this);
			m_nThreadAnalNum++;
			Sleep(1);
		}

	}
	else if(wParam == 3)
	{
		ShowTraceTime(L"End Innder Voxel", 1);

		//////////////////////////////////////////////////////////////////////////
		//! 내부 복셀로 데이터를 만들어줌
// 		bool bTRUE = false;
// 		unsigned long long nSeekSize = 0;
// 		ST_DELETE_VOXEL_POS stDeleteVoxelPos;
// 		for(int nZ = 0 ; nZ < m_nZFileVoxCnt ; nZ++)
// 		{
// 			for(int nY = 0 ; nY < m_nYFileVoxCnt ; nY++)
// 			{
// 				for(int nX = 0 ; nX < m_nXFileVoxCnt ; nX++)
// 				{
// 
// 					nSeekSize = nX  + (m_nXFileVoxCnt * nY) + (m_nXFileVoxCnt * m_nYFileVoxCnt * nZ);
// 
// 					//! 메모리 처리
// 					if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
// 					{
// 						bTRUE = g_vecVoxelTotal[nSeekSize];
// 					}
// // 					else //파일 처리
// // 					{
// // 						_fseeki64(p_Devfile,nSeekSize ,SEEK_SET);
// // 						fread(&bTRUE, sizeof(bool), 1, p_Devfile); 
// // 					}
// 
// 					if(bTRUE)
// 					{
// 						stDeleteVoxelPos.nX = nX;
// 						stDeleteVoxelPos.nY = nY;
// 						stDeleteVoxelPos.nZ = nZ;
// 						g_vecCompleteVoxelPosSolid.push_back(stDeleteVoxelPos);
// 					}
// 				}
// 			}
// 
// 		}

		m_editProcessStatus.SetWindowTextW(L"Voxelizaion Complete");

		
		g_bVoxelComplete = TRUE;
		UpdateMainView(FALSE);

		
		if(m_nComboLogicProcess == 0)
		{
			//////////////////////////////////////////////////////////////////////////
			//! 시간 비교를 위함
// 			ShowTraceTime(L"GPU-GPU");
// 			ShowTraceTime(L"Start Inner Voxel");
// 
// 			SetGpuData();
// 
// 
// 			int nSizeT = g_vecVoxelState.size();
// 			memset(m_bDataTmp[dfVOXEL_IDX], NULL, nSizeT);
// 			memset(m_bDataTmp[dfTOTAL_IDX], NULL, nSizeT);
// 			copy(g_vecVoxelState.begin(), g_vecVoxelState.begin() + nSizeT, m_bDataTmp[dfVOXEL_IDX]);
// 			ShowTraceTime(L"External Voxel CPU->GPU COPY" , 1);
// 
// 			ShowTraceTime(L"Start Inner Voxel -> Version 1");
// 
// 			memset(m_bDataTmp[dfTOTAL_IDX], NULL, nSizeT);
// 			m_GPUSolid.CUDA_SolidVoxelizationXYZ(nSizeT, m_bDataTmp[dfVOXEL_IDX], m_bDataTmp[dfTOTAL_IDX]);
// 			std::copy(m_bDataTmp[dfTOTAL_IDX], m_bDataTmp[dfTOTAL_IDX]+nSizeT, g_vecVoxelTotalCuda.begin());
// 
// 			ShowTraceTime(L"End Inner Voxel -> Version 1" , 1);
// 
// 
// 			Sleep(500);
// 
// 			ShowTraceTime(L"Start Inner Voxel -> Version 2");
// 
// 			memset(m_bDataTmp[dfTOTAL_IDX], NULL, nSizeT);
// 			m_GPUSolid.CUDA_SolidVoxelizationXYZBlock(nSizeT, m_bDataTmp[dfVOXEL_IDX], m_bDataTmp[dfTOTAL_IDX]);
// 			std::copy(m_bDataTmp[dfTOTAL_IDX], m_bDataTmp[dfTOTAL_IDX]+nSizeT, g_vecVoxelTotalCuda.begin());
// 
// 			ShowTraceTime(L"End Inner Voxel -> Version 2" , 1);
// 			
// 			int nErrorCnt = 0;
// 			for (int v = 0; v < g_vecVoxelTotalCuda.size(); v++)
// 			{
// 				if(g_vecVoxelTotalCudaTemp[v] != g_vecVoxelTotalCuda[v])
// 				{
// 					nErrorCnt++;
// 				}
// 			}
// 			CString strTempT = L"";
// 			if(nErrorCnt > 0)
// 			{
// 				strTempT.Format(L"Error Cnt : %d\n", nErrorCnt);
// 			}
// 			else
// 			{
// 				strTempT.Format(L"GOOD");
// 			}
// 
// 			ShowTraceTime(strTempT);
// 
// 			ShowTraceTime(L"\n", 2);

		}


		//std::copy(m_bDataTmp[dfTOTAL_IDX], m_bDataTmp[dfTOTAL_IDX]+g_vecVoxelState.size(), g_vecVoxelTotal.begin());


		//CreaetSoildVoxelData();

		//  [9/30/2019 kjky12] 시뮬레이션을 해야되는경우만..
		//PostMessage(WM_SIMULATION_VOXEL, m_nSimulIdx++, m_nSimulObjectIdx);

		OnBnClickedButtonCalcExternalSide();


	}
	else if(wParam == 4)
	{
		SetStoneTypeCntView();
		m_editProcessStatus.SetWindowTextW(L"Calculation Complete");

		g_bVoxelComplete = TRUE;
		UpdateMainView(FALSE);

		OnBnClickedButton5();
	}
	else
	{
		g_bVoxelComplete = TRUE;
		UpdateMainView(FALSE);
		
	}

	return 0;
}



void ThreadTotalSolidVoxelization( void* pArguments )
{
	int nNum = m_nThreadAnalNum-1;

	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;


	CString strProcessStatus;
	strProcessStatus.Format(L"Save Voxels Data");
	pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);


	TRACE("GO! GO! Solid Voxel X\n");

	unsigned int nTotalCnt =0;

	bool bTRUE = false;

	int nDiv = pParent->m_nZFileVoxCnt / dfTHREAD_MAX_CNT;
	int nAddLast = 0;

	if(nDiv <= 0)
		nDiv = 1;

	if(nNum == dfTHREAD_MAX_CNT - 1)
		nAddLast = pParent->m_nZFileVoxCnt - (nDiv * dfTHREAD_MAX_CNT);

	short nSol = 0;
	bool bX = false;
	bool bY = false;
	bool bZ = false;
	if(!((nNum+1) * nDiv > pParent->m_nZFileVoxCnt))
	{
		for (int z=(nNum * nDiv) , ip= ((nNum+1) * nDiv) + nAddLast; z<ip ; z++) 
		{
			for(int y = 0 ; y < pParent->m_nYFileVoxCnt ; y++)	
			{
				for(int x = 0 ; x < pParent->m_nXFileVoxCnt ; x++)
				{
					bX = pParent->GetUsedCellFromSolidFileXYZ(0,x,y,z);
					bY = pParent->GetUsedCellFromSolidFileXYZ(1,x,y,z);
					bZ = pParent->GetUsedCellFromSolidFileXYZ(2,x,y,z);


					if(bX == true && bY == true && bZ == true)
					{	
						pParent->ReplaceFileInfo(x,y,z,4);
						nTotalCnt++;
					}
					else
					{
						nSol = 0;
					}
				}
			}

			strProcessStatus.Format(L"Save Solid Voxel Progress : %.2f %%",float((float)z / (float)pParent->m_nZFileVoxCnt) * 100.0);
			pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);
		}
	}

	m_nSolidTotalCnt += nTotalCnt;
	m_nThreadFinshNum++;
	TRACE1("TRUE %d\n",nNum);

	if(m_nThreadFinshNum == dfTHREAD_MAX_CNT)
	{
		//! 파일처리
		if(pParent->m_nComboProcessSel == dfDATA_PROCESS_FILE)
		{
			// 표면 복셀 처리된 파일 합치는 부분!!!	
			CString strFileName = L"";
			strFileName.Format(L"SolidVoxel.dat");
			CString strSurPath1 = gf_GetModulePath() + strFileName;
			FILE *p_Devfile = _wfopen(strSurPath1, L"wb"); 

			fwrite(&pParent->m_nXFileVoxCnt, sizeof(unsigned long long), 1, p_Devfile); 
			fwrite(&pParent->m_nYFileVoxCnt, sizeof(unsigned long long), 1, p_Devfile); 
			fwrite(&pParent->m_nZFileVoxCnt, sizeof(unsigned long long), 1, p_Devfile); 


			for(int n = 0 ; n < dfTHREAD_MAX_CNT ; n++)
			{
				strFileName.Format(L"SolidVoxel%02d.dat",n);
				strSurPath1 = gf_GetModulePath() + strFileName;
				FILE *p_Surfile = _wfopen(strSurPath1, L"rb"); 
				bool* fdata = new bool[pParent->m_nDivFileSize];

				if(NULL != p_Surfile)
				{ 
					fread(fdata, sizeof(bool), pParent->m_nDivFileSize, p_Surfile); 

					if(NULL != p_Devfile)
					{ 
						fwrite(fdata, sizeof(bool), pParent->m_nDivFileSize, p_Devfile); 
					}
				}
				M_A_DELETE(fdata);
				fclose(p_Surfile);
			}
			fwrite(&m_nSolidTotalCnt, sizeof(unsigned long long), 1, p_Devfile); 

			if(p_Devfile)
				fclose(p_Devfile);

		}


		pParent->SendMessage(WM_FINISH_SOLID_VOXEL_MSG,3,0);	
	}

}


void CModelingWeatheringOfRockDlg::OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 이 기능을 사용하려면 Windows Vista 이상이 있어야 합니다.
	// _WIN32_WINNT 기호는 0x0600보다 크거나 같아야 합니다.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CModelingWeatheringOfRockDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	

	g_nSlideView = m_SliderCtrl3DSlideView.GetPos();

	UpdateMainView(FALSE);

	*pResult = 0;
}


void CModelingWeatheringOfRockDlg::OnBnClickedButtonSolidData()
{
	g_strFilePathName =L"";

// 	CFileDialog FileDlg(TRUE, _T("obj"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("obj File(*.obj)|*.obj||") );
// 	if (FileDlg.DoModal() == IDOK )
// 	{
// 		g_strFilePathName = FileDlg.GetPathName();
// 	}
// 
// 	if(g_strFilePathName.IsEmpty())
// 		return;
// 
// 	m_pDlgOpenglView->m_bComplete = FALSE;
// 	m_pDlgOpenglView->OnPaint();
// 



	int nX = 0; int nY = 0; int nZ = 0;
	CString strTmp;
	m_editXData.GetWindowText(strTmp);
	nX = _wtoi(strTmp);
	m_editYData.GetWindowText(strTmp);
	nY = _wtoi(strTmp);
	m_editZData.GetWindowText(strTmp);
	nZ = _wtoi(strTmp);


	strTmp.Format(L"S: %d,Z: %d,Y: %d,Z: %d ",GetUsedCellFromSolidFile(nX, nY, nZ),GetUsedCellFromSolidFileXYZ(0, nX, nY, nZ),GetUsedCellFromSolidFileXYZ(1, nX, nY, nZ),GetUsedCellFromSolidFileXYZ(2, nX, nY, nZ));
	
	m_editSolidView.SetWindowText(strTmp);
	




	// 	double dDy = 0.1/*double(rand( )%10000)/10000.0*/;
	// 	double dValue1 = dDy * 2.0 - 1.0;
	// 	double dIerf = sqrt(acos((double)-1)) / 2.0 * (dValue1 + acos((double)-1) * pow(dValue1, 3.0) / 12.0 + 7.0 * pow(acos((double)-1), 2.0) * pow(dValue1, 5.0) / 480.0 + 127.0 * pow(acos((double)-1), 3.0) *
	// 		pow(dValue1, 7.0) / 40320.0 + 4369.0 * pow(acos((double)-1), 4.0) * pow(dValue1, 9.0) / 5806080.0 + 34807.0 * pow(acos((double)-1), 5.0) * pow(dValue1, 11.0) / 182476800.0);
	// 
	// 	double dIerf2 = (sqrt(acos((double)-1)) / 2.0) 
	// 		* (
	// 		(dValue1 + (acos((double)-1) * pow(dValue1, 3.0) / 12.0)) 
	// 		+ (7.0 * pow(acos((double)-1), 2.0) * pow(dValue1, 5.0) / 480.0) 
	// 		+ (127.0 * pow(acos((double)-1), 3.0) * pow(dValue1, 7.0) / 40320.0 )
	// 		+ (4369.0 * pow(acos((double)-1), 4.0) * pow(dValue1, 9.0) / 5806080.0 )
	// 		+ (34807.0 * pow(acos((double)-1), 5.0) * pow(dValue1, 11.0) / 182476800.0)
	// 		);

	//dDy = dIerf * sqrt(2.0) * stdDev + mean;

}



bool CModelingWeatheringOfRockDlg::GetUsedCellFromSolidFileXYZ(int nIdx, int nX, int nY, int nZ)
{

	int nZGapSize = m_nZFileVoxCnt/dfTHREAD_MAX_CNT;

	CString strFileName = L"";
	CString strSolPath = L"";
	vector<bool>	*vecBChk = NULL;

	if(nIdx == 0)
	{
		vecBChk = &g_vecVoxelXState;

		strFileName.Format(L"SolidXVoxel.dat");
		strSolPath = gf_GetModulePath() + strFileName;
	}
	else if(nIdx == 1)
	{	
		vecBChk = &g_vecVoxelYState;

		strFileName.Format(L"SolidYVoxel.dat");
		strSolPath = gf_GetModulePath() + strFileName;
	}
	else 
	{	
		vecBChk = &g_vecVoxelZState;

		strFileName.Format(L"SolidZVoxel.dat");
		strSolPath = gf_GetModulePath() + strFileName;
	}
	
	bool bTRUE = false;
	unsigned long long nSeekSize = nX  + (m_nXFileVoxCnt * nY) + (m_nXFileVoxCnt * m_nYFileVoxCnt * nZ);

	//! 메모리 처리
	if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
	{
		bTRUE = vecBChk->at(nSeekSize);
	}
	else //파일처리
	{
		FILE *p_Solfile = _wfopen(strSolPath, L"rb"); 


		if(NULL != p_Solfile)
		{
			_fseeki64(p_Solfile,nSeekSize ,SEEK_SET);
			fread(&bTRUE, sizeof(bool), 1, p_Solfile); 

			fclose(p_Solfile); 
		}
	}
	

	return bTRUE;
}





void CModelingWeatheringOfRockDlg::OnCbnSelchangeCombo1()
{
	g_n3DShowType = m_comboShow3DType.GetCurSel();

	UpdateMainView(FALSE);
}


void CModelingWeatheringOfRockDlg::OnBnClickedButtonSolidData2()
{
// 
// 
// 	// 입상붕괴 도달값 계산을 위한 확률변수 설정
// 	CProbDistribution PDGranularDisintegration(3,0.0,9.0,0.8,0.0,0.0,0.0,0.0,0.0,0.0);
// 	// 공극률 설정을 위한 확률변수 설정
// 	CProbDistribution PDPorosity(4,0.0,0.0,0.5,0.5,0.0,0.0,0.0,0.0,0.0);
// 
// 	for(int n = 0 ; n < 200 ; n++)
// 	{
// 		float fPorosity = PDPorosity.GetDistParam();									// 공극률
// 		TRACE1("%f\n",fPorosity);
// 	}
// 
// 	TRACE("----------------------\n");
// 
// 	for(int n = 0 ; n < 200 ; n++)
// 	{
// 		float fGranularDisintegration = PDGranularDisintegration.GetDistParam();		// 입상붕괴 도달값 
// 		TRACE1("%f\n",fGranularDisintegration);
// 	}





	g_strFilePathName =L"";

	CFileDialog FileDlg(TRUE, _T("obj"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("obj File(*.obj)|*.obj||") );
	if (FileDlg.DoModal() == IDOK )
	{
		g_strFilePathName = FileDlg.GetPathName();
	}

	if(g_strFilePathName.IsEmpty())
		return;

	UpdateMainView(FALSE);

	

}


void CModelingWeatheringOfRockDlg::OnCbnSelchangeCombo2()
{
	g_n3DShowEye = m_combo3DViewEye.GetCurSel();

	m_pDlgOpenglView->m_fScale = 1.0;
	m_pDlgOpenglView->m_XRot = 0.0;
	m_pDlgOpenglView->m_YRot = 0.0;
	m_pDlgOpenglView->m_ZRot = 0.0;

	UpdateMainView(FALSE);

	
}




void CModelingWeatheringOfRockDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

//	pNMUpDown->iDelta = -pNMUpDown->iDelta;

	m_pDlgOpenglView->m_f3DScale += pNMUpDown->iDelta / 10.0;

	CString strTmp = L"";
	strTmp.Format(L"%d",(int)m_pDlgOpenglView->m_f3DScale);
	m_editScaleData.SetWindowTextW(strTmp);
//	m_pDlgOpenglView->m_f3DScale = pNMUpDown->iDelta;
	UpdateMainView(FALSE);

	
	*pResult = 0;
}

void CModelingWeatheringOfRockDlg::SaveStepOutsideData(int nStep)
{

	CString strPath = gf_GetModulePath() + L"Step\\";
	::CreateDirectory(strPath, NULL);

	
	CString strFileName = L"";
	strFileName.Format(L"Setp%09d.dat",nStep);
	CString strNumPath = strPath + strFileName;

	FILE *p_Numfile = _wfopen(strNumPath, L"wb"); 

	if(NULL != p_Numfile )
	{ 

		ST_PARTICLE_POS stParticlePos;
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
		int nTotalCnt = g_MapOutsideData.size();
		fwrite(&nTotalCnt, sizeof(int), 1, p_Numfile); 
		
		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
		{
			stParticlePos = iterOutsideData->second;

			fwrite(&stParticlePos.x, sizeof(int), 1, p_Numfile); 
			fwrite(&stParticlePos.y, sizeof(int), 1, p_Numfile); 
			fwrite(&stParticlePos.z, sizeof(int), 1, p_Numfile); 

			//fwrite(&stParticlePos.bUse, sizeof(bool), 1, p_Numfile); 
			fwrite(&stParticlePos.fPorosity, sizeof(float), 1, p_Numfile); 
			fwrite(&stParticlePos.iExternalSideCnt, sizeof(short), 1, p_Numfile); 
			fwrite(&stParticlePos.fGranularDisintegration, sizeof(float), 1, p_Numfile); 

			fwrite(stParticlePos.abExternalSide, sizeof(bool) * 6, 1, p_Numfile); 


// 			int nSize = stParticlePos.vecExternalSide.size();
// 			fwrite(&nSize, sizeof(int), 1, p_Numfile); 
// 			for(int v = 0 ; v < stParticlePos.vecExternalSide.size() ; v++)
// 				fwrite(&stParticlePos.vecExternalSide[v], sizeof(short), 1, p_Numfile); 

		}
		fclose(p_Numfile); 
	}
}


void ThreadLoadStepOutsideData( void* pArguments )
{
	CModelingWeatheringOfRockDlg* pParent = (CModelingWeatheringOfRockDlg*)pArguments;


	CString strFileName = L"";
	CString strKey= L"";
	CString strPath = gf_GetModulePath() + L"Step\\";
	strFileName.Format(L"Setp%09d.dat",pParent->m_nLoadStep);
	CString strNumPath = strPath + strFileName;

	FILE *p_Numfile = _wfopen(strNumPath, L"rb"); 
	
	CString strProcessStatus;


	g_MapOutsideData.clear();
	if(NULL != p_Numfile )
	{ 
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;

		int nTotalCnt = 0;
		fread(&nTotalCnt, sizeof(int), 1, p_Numfile); 

		for(int n = 0 ; n < nTotalCnt ; n++)
		{
			ST_PARTICLE_POS stParticlePos;

			fread(&stParticlePos.x, sizeof(int), 1, p_Numfile); 
			fread(&stParticlePos.y, sizeof(int), 1, p_Numfile); 
			fread(&stParticlePos.z, sizeof(int), 1, p_Numfile); 

			//fread(&stParticlePos.bUse, sizeof(bool), 1, p_Numfile); 
			fread(&stParticlePos.fPorosity, sizeof(float), 1, p_Numfile); 
			fread(&stParticlePos.iExternalSideCnt, sizeof(short), 1, p_Numfile); 
			fread(&stParticlePos.fGranularDisintegration, sizeof(float), 1, p_Numfile); 

// 			int nSize = 0;
// 			fread(&nSize, sizeof(int), 1, p_Numfile); 
// 			stParticlePos.vecExternalSide.clear();
// 			short nValue = 0;
// 			for(int v = 0 ; v < nSize ; v++)
// 			{
// 				fread(&nValue, sizeof(short), 1, p_Numfile); 
// 				stParticlePos.vecExternalSide.push_back(nValue);
// 			}

			fread(stParticlePos.abExternalSide, sizeof(bool) * 6, 1, p_Numfile); 

			strKey.Format(L"%d-%d-%d",stParticlePos.x,stParticlePos.y,stParticlePos.z);
			g_MapOutsideData.insert(make_pair(strKey,stParticlePos));

			strProcessStatus.Format(L"Loading Progress : %.2f %%",float((float)n / (float)nTotalCnt) * 100.0);
			pParent->m_editProcessStatus.SetWindowTextW(strProcessStatus);
		}
		fclose(p_Numfile); 
	}

	pParent->SendMessage(WM_FINISH_SOLID_VOXEL_MSG,5,0);	
}

void CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling3()
{
	CString strNumPath = gf_GetModulePath() + L"SurfaceNum.dat";
	FILE *p_Numfile = _wfopen(strNumPath, L"rb"); 

	g_MapOutsideData.clear();
	CString strKey = L"";

	if(NULL != p_Numfile )
	{ 
		fread(&m_nXFileVoxCnt, sizeof(int), 1, p_Numfile); 
		fread(&m_nYFileVoxCnt, sizeof(int), 1, p_Numfile); 
		fread(&m_nZFileVoxCnt, sizeof(int), 1, p_Numfile); 

		g_nXVoxCnt = m_nXFileVoxCnt / 2;
		g_nYVoxCnt = m_nYFileVoxCnt / 2;
		g_nZVoxCnt = m_nZFileVoxCnt / 2;

		fclose(p_Numfile); 
	}

	CString strTmp = L"";
	m_editStepCnt.GetWindowTextW(strTmp);

	m_nLoadStep = _wtoi(strTmp);

	_beginthread( ThreadLoadStepOutsideData, 0, this);
		
}


void CModelingWeatheringOfRockDlg::OnBnClickedCheck3()
{
	if(m_checkTranObj.GetCheck())
	{
		m_pDlgOpenglView->m_bTranObj = TRUE;
	}
	else
	{
		m_pDlgOpenglView->m_bTranObj = FALSE;
	}
}


void CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling4()
{
	CString strTmp = L"";
	m_editXRot.GetWindowTextW(strTmp);
	m_pDlgOpenglView->m_XRot = _wtof(strTmp);
	m_editYRot.GetWindowTextW(strTmp);
	m_pDlgOpenglView->m_YRot = _wtof(strTmp);
	m_editZRot.GetWindowTextW(strTmp);
	m_pDlgOpenglView->m_ZRot = _wtof(strTmp);
	
	m_editXTran.GetWindowTextW(strTmp);
	m_pDlgOpenglView->m_XTran = _wtof(strTmp);
	m_editYTran.GetWindowTextW(strTmp);
	m_pDlgOpenglView->m_YTran = _wtof(strTmp);
	m_editZTran.GetWindowTextW(strTmp);
	m_pDlgOpenglView->m_ZTran = _wtof(strTmp);


	m_editScaleData.GetWindowTextW(strTmp);
	m_pDlgOpenglView->m_f3DScale = _wtof(strTmp);

	UpdateMainView(FALSE);
}


LRESULT CModelingWeatheringOfRockDlg::OnChangePosRotMsg(WPARAM wParam, LPARAM lParam)
{

	CString strTmp = L"";
	strTmp.Format(L"%.2f",m_pDlgOpenglView->m_XRot);
	m_editXRot.SetWindowTextW(strTmp);
	
	strTmp.Format(L"%.2f",m_pDlgOpenglView->m_YRot);
	m_editYRot.SetWindowTextW(strTmp);
	
	strTmp.Format(L"%.2f",m_pDlgOpenglView->m_ZRot);
	m_editZRot.SetWindowTextW(strTmp);
	

	strTmp.Format(L"%.2f",m_pDlgOpenglView->m_XTran);
	m_editXTran.SetWindowTextW(strTmp);
	
	strTmp.Format(L"%.2f",m_pDlgOpenglView->m_YTran);
	m_editYTran.SetWindowTextW(strTmp);
	
	strTmp.Format(L"%.2f",m_pDlgOpenglView->m_ZTran);
	m_editZTran.SetWindowTextW(strTmp);
	
	strTmp.Format(L"%.2f",m_pDlgOpenglView->m_f3DScale);
	m_editScaleData.SetWindowTextW(strTmp);
	

	return 0;
}

void CModelingWeatheringOfRockDlg::OnBnClickedCheck4()
{
	if(m_checkViewAxis.GetCheck())
	{
		m_pDlgOpenglView->m_bViewAxis = TRUE;
	}
	else
	{
		m_pDlgOpenglView->m_bViewAxis = FALSE;
	}

	m_pDlgOpenglView->OnPaint();
}


void CModelingWeatheringOfRockDlg::OnCbnSelchangeCombo3()
{
	g_nSelectAxis = m_comboSelectAxis.GetCurSel();
}


void CModelingWeatheringOfRockDlg::OnEnChangeEditCalcTryCnt16()
{
	CString strTmp = L"";
	m_editSaveInterval.GetWindowText(strTmp);
	m_nSaveInterval = _wtoi(strTmp);
}


void CModelingWeatheringOfRockDlg::OnCbnSelchangeComboDataProcess()
{
	m_nComboProcessSel = m_comboDataProcess.GetCurSel();

}



// CTime 값을 초로 변환하는 함수
LONGLONG CModelingWeatheringOfRockDlg::GetSecondFromTime(CTime tmCurrent)
{
	// UTC 표준 시간에 +9이기 때문에 9시부터 시작하는 걸로 해야 0초로 계산됨. ㅋ
	CTime tmStartTime ( 1970, 1, 1, 9, 0, 0 );	
	CTimeSpan tmSpan = tmCurrent - tmStartTime;

	return tmSpan.GetTotalSeconds();
}

double dTimeSec = 0.0;
void CModelingWeatheringOfRockDlg::ShowTraceTime(CString strMemo, int nGapCheck)
{
	CString	strSystemDate;

	CTime tm = CTime::GetTickCount();
	SYSTEMTIME st;
	GetLocalTime(&st);
	strSystemDate.Format(L"%04d-%02d-%02d %02d:%02d:%02d.%03d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond(), st.wMilliseconds);
	TRACE(strSystemDate + strMemo + L"\n");

	
	double dTimeSecTmp = 0.0;
	dTimeSecTmp = (double)GetSecondFromTime(tm);
	dTimeSecTmp += (double)st.wMilliseconds / 1000.0;


	CString strLog = L"";
	if(nGapCheck == 1)
		strLog.Format(L"%s\t%s\t%lf\t%lf", strSystemDate , strMemo, dTimeSecTmp, dTimeSecTmp - dTimeSec);
	else if(nGapCheck == 2)
		strLog.Format(L"%s\t%s", strSystemDate , strMemo);
	else
		strLog.Format(L"%s\t%s\t%lf", strSystemDate , strMemo, dTimeSecTmp);
	write_log_file(strLog);

	dTimeSec = dTimeSecTmp;

}







void CModelingWeatheringOfRockDlg::OnBnClickedButton3()
{
	//CGPUSolid GPUSolid;
	m_GPUSolid.GpuInfoPrint();
	
}


void CModelingWeatheringOfRockDlg::OnBnClickedButton4()
{
	CString strGetDirectory = L"";
	GetDlgItem(IDC_EDIT_TOTAL_DATATEST)->GetWindowText(strGetDirectory);
	CString strPath = gf_GetModulePath() + strGetDirectory + L"\\";



	//  [2/28/2018 kjky12] 파일을 읽고 해본적은 없음..

	//////////////////////////////////////////////////////////////////////////
	// 전체 계산된 데이터를 보고 파일 크기를 읽어옴
	CString strSolPath = strPath + L"SolidVoxel.dat";
	FILE *p_Solfile = NULL; 
	p_Solfile = _wfopen(strSolPath, L"rb"); 
	unsigned long long nXVoxCnt = 0;
	unsigned long long nYVoxCnt = 0;
	unsigned long long nZVoxCnt = 0;
	unsigned long long lTotalsize =  0;
	if(NULL != p_Solfile)
	{
		fread(&nXVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 
		fread(&nYVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 
		fread(&nZVoxCnt, sizeof(unsigned long long), 1, p_Solfile); 	 
		
		unsigned long long lTotalsize = nXVoxCnt * nYVoxCnt * nZVoxCnt;
		bool *bDataTotal = new bool[lTotalsize];

		fread(bDataTotal, lTotalsize * sizeof(bool), 1, p_Solfile); 	 
		copy(bDataTotal, bDataTotal + lTotalsize, g_vecVoxelTotal.begin());


		M_A_DELETE(bDataTotal);

		fclose(p_Solfile);
	}



	//////////////////////////////////////////////////////////////////////////
	// X 데이터를 읽어옴
	CString strFileName = L"";
	strFileName.Format(L"SolidXVoxel.dat");
	p_Solfile = _wfopen(strPath + strFileName, L"rb"); 
	bool *bData = new bool[lTotalsize];
	if(NULL != p_Solfile)
	{
		fread(&bData, lTotalsize * sizeof(bool), 1, p_Solfile); 
		fclose(p_Solfile); 
	}
	M_A_DELETE(bData);
	copy(bData, bData + lTotalsize, g_vecVoxelXState.begin());

	//////////////////////////////////////////////////////////////////////////
	// Y 데이터를 읽어옴
	strFileName.Format(L"SolidYVoxel.dat");
	p_Solfile = _wfopen(strPath + strFileName, L"rb"); 
	bData = new bool[lTotalsize];
	if(NULL != p_Solfile)
	{
		fread(&bData, lTotalsize * sizeof(bool), 1, p_Solfile); 
		fclose(p_Solfile); 
	}
	M_A_DELETE(bData);
	copy(bData, bData + lTotalsize, g_vecVoxelYState.begin());

	//////////////////////////////////////////////////////////////////////////
	// Z 데이터를 읽어옴
	strFileName.Format(L"SolidZVoxel.dat");
	p_Solfile = _wfopen(strPath + strFileName, L"rb"); 
	bData = new bool[lTotalsize];
	if(NULL != p_Solfile)
	{
		fread(&bData, lTotalsize * sizeof(bool), 1, p_Solfile); 
		fclose(p_Solfile); 
	}
	M_A_DELETE(bData);
	copy(bData, bData + lTotalsize, g_vecVoxelZState.begin());


	//////////////////////////////////////////////////////////////////////////
	// GPGPU 계산


	//! 이거랑 비교하면됨.. 아마도..?
	g_vecVoxelTotal;





// 	strFileName.Format(L"SolidZVoxel.dat");
// 	p_Solfile = _wfopen(strPath + strFileName, L"rb"); 
// 	bool *bData = new bool[lTotalsize];
// 	if(NULL != p_Solfile)
// 	{
// 		fread(&bData, lTotalsize * sizeof(bool), 1, p_Solfile); 
// 		fclose(p_Solfile); 
// 	}
// 	M_A_DELETE(bData);
// 	copy(bData, bData + lTotalsize, g_vecVoxelZState.begin());






	L"SolidXVoxel.dat";


}


//! 시뮬레이션
void CModelingWeatheringOfRockDlg::OnBnClickedButton1()
{
	m_nSimulIdx = 0;
	m_bSimulIdx = true;
	m_nSimulObjectIdx = 0;
	PostMessage(WM_SIMULATION_VOXEL, m_nSimulIdx, m_nSimulObjectIdx);
}

LRESULT CModelingWeatheringOfRockDlg::OnSimulationMsg(WPARAM wParam, LPARAM lParam)
{
	if(!m_bSimulIdx)
		return FALSE;
	//g_strFilePathName;

	const int nCntObject = 9;
	const CString strPath[nCntObject] = {
		gf_GetModulePath() + L"Model\\stone1.OBJ",
		gf_GetModulePath() + L"Model\\stone2.OBJ",
		gf_GetModulePath() + L"Model\\stone3.OBJ",
		gf_GetModulePath() + L"Model\\stone4.OBJ",
		gf_GetModulePath() + L"Model\\stone5.OBJ",
		gf_GetModulePath() + L"Model\\spyder.OBJ",
		gf_GetModulePath() + L"Model\\hand.OBJ",
		gf_GetModulePath() + L"Model\\chessPhon.OBJ",
		gf_GetModulePath() + L"Model\\body.OBJ",

	};

	const int nCnt = 7;
	//const int nCnt = 4;
	const int nPixel[nCnt] = {
		10,
		50,
		100,
		150,
		200,
		250,
		300
	};

	if(nCnt <= (int)m_nSimulIdx)
	{
		m_nSimulIdx  = 0;
		m_nSimulObjectIdx++;
		if(m_nSimulObjectIdx >= nCntObject)
		{
		m_bSimulIdx = false;
			m_nSimulObjectIdx = 0;

		return TRUE;
	}

		
		
	}


	CString strValue = L"";
	strValue.Format(L"%d", nPixel[m_nSimulIdx]);
	m_editXAxiDivide.SetWindowTextW(strValue);


	if(m_nSimulIdx == 0)
		m_comboUseCuda.SetCurSel(dfLOGIC_GPU_GPU); //! GPU를 한번 실행하기위해
	else
		m_comboUseCuda.SetCurSel(dfLOGIC_CPU_CPU);
//		m_comboUseCuda.SetCurSel((m_nSimulIdx - 1) % 3);
	
	g_strFilePathName = strPath[m_nSimulObjectIdx];

	OnBnClickedButtonObjFileModeling();

	return TRUE;
}

void CModelingWeatheringOfRockDlg::OnBnClickedButtonViewRateStone()
{
	CDlgPieDemoDlg	DlgPieDemoDlg;

	UpdateData(TRUE);

	float fEditFeldsparPer = _wtof(m_strEditFeldsparPer);
	float fEditQuartzPer = _wtof(m_strEditQuartzPer);
	float fEditPlagioclasePer = _wtof(m_strEditPlagioclasePer);
	float fEditBiotitePer = _wtof(m_strEditBiotitePer);
	float fEditTitanitePer = _wtof(m_strEditTitanitePer);
	float fEditHornblendePer = _wtof(m_strEditHornblendePer);
	float fEditEpidotePer = _wtof(m_strEditEpidotePer);
	float fEditChloritePer = _wtof(m_strEditChloritePer);
	float fEditPorosityPer = _wtof(m_strEditPorosityPer);
	float fRatePercentAll = _wtof(m_strEditFeldsparPer) + _wtof(m_strEditQuartzPer) + _wtof(m_strEditPlagioclasePer) + _wtof(m_strEditBiotitePer) + _wtof(m_strEditTitanitePer) + _wtof(m_strEditHornblendePer) + _wtof(m_strEditEpidotePer) + _wtof(m_strEditChloritePer) + _wtof(m_strEditPorosityPer);

	UpdateData(FALSE);

	DlgPieDemoDlg.AddPice(360 * fEditFeldsparPer / fRatePercentAll, L"장석", RGB(211,165,136));
	DlgPieDemoDlg.AddPice(360 * fEditQuartzPer / fRatePercentAll, L"석영", RGB(192,198,198));
	DlgPieDemoDlg.AddPice(360 * fEditPlagioclasePer / fRatePercentAll, L"시장석", RGB(213,211,217));
	DlgPieDemoDlg.AddPice(360 * fEditBiotitePer / fRatePercentAll, L"흑운모", RGB(37,39,34));
	DlgPieDemoDlg.AddPice(360 * fEditTitanitePer / fRatePercentAll, L"티타나이트", RGB(109,149,43));
	DlgPieDemoDlg.AddPice(360 * fEditHornblendePer / fRatePercentAll, L"각섬석", RGB(32,32,39));
	DlgPieDemoDlg.AddPice(360 * fEditEpidotePer / fRatePercentAll, L"녹렴석", RGB(181,171,90));
	DlgPieDemoDlg.AddPice(360 * fEditChloritePer / fRatePercentAll, L"녹니석", RGB(136,139,106));
	DlgPieDemoDlg.AddPice(360 * fEditPorosityPer / fRatePercentAll, L"공극", RGB(255,255,255));

	DlgPieDemoDlg.DoModal();

}


int CModelingWeatheringOfRockDlg::GetStoneType(ST_PARTICLE_POS* pstParticlePos)
{
	int nStoneType = -1;
	


	float fGranularDisintegration = 0;
	map<int,int>::iterator	iterInitStoneTypeCnt;

	int nFind = 0;
	while (TRUE)
	{
		//srand((unsigned int)time(NULL));
		int nRand = rand();
		int nValue = nRand % dfSTONE_TYPE_CNT;
		nFind++;

		iterInitStoneTypeCnt = m_mapInitStoneTypeCnt.find(nValue);
		if(iterInitStoneTypeCnt != m_mapInitStoneTypeCnt.end())
		{
			if(iterInitStoneTypeCnt->second > 0)
			{
				nStoneType = iterInitStoneTypeCnt->first;

				if(pstParticlePos->bInOut == true && nStoneType == 0)
				{
					nStoneType = -1;
				}
				else
				{
					iterInitStoneTypeCnt->second--;
				}
			}

			if(iterInitStoneTypeCnt->second == 0)
			{
				m_mapInitStoneTypeCnt.erase(iterInitStoneTypeCnt);
			}

			//! 마지막 한개가 공극으로 남아 버리는경우를 위해.. 100번도안 안되면 다른 성분을 추가해줌
			if(nFind >= 100)
			{
				InitStonRate(false);
				nFind = 0;
			}
		}

		if(m_mapInitStoneTypeCnt.size() == 0)
			InitStonRate();

		if(nStoneType >= 0)
		{
			
			break;
		}
		
	}




// 	for (int i = 0; i < dfSTONE_TYPE_CNT ; i++)
// 	{
// 		if (nValue >= fRateHap && nValue <= fRateHap + fStonTypeRate[i])
// 		{
// 			nStoneType = i;
// 
// 			break;
// 		}
// 
// 	}

// 	if(nStoneType == -1 || nStoneType == dfSTONE_TYPE_CNT) //! 해당 값을 찾지 못하는 경우 0으로 입력(공극)
// 		nStoneType = 0;
// 
// 	if(pstParticlePos)
// 	{
// 		//  [2/7/2019 kjky12] 외부의 경우는 공극으로 둘수 없다
// 		while(pstParticlePos->bInOut == true && nStoneType == 0)
// 		{
// 			fRateHap = 0;
// 			fGranularDisintegration = 0;
// 			srand((unsigned int)time(NULL));
// 			nRand = rand();
// 			nValue = nRand % dfSTONE_TYPE_CNT;
// 			for (int i = 0; i < dfSTONE_TYPE_CNT ; i++)
// 			{
// 				if (nValue >= fRateHap && nValue <= fRateHap + fStonTypeRate[i])
// 				{
// 					nStoneType = i;
// 					break;
// 				}
// 
// 				fRateHap += fStonTypeRate[i];
// 			}
// 		}
// 
// 	}
// 
// 	if(pstParticlePos->bInOut == true && nStoneType == 0)
// 	{
// 		int a= 0;
// 	}


	return nStoneType;
}


void CModelingWeatheringOfRockDlg::SetProbDistributionValue()
{
	float fEditFeldsparDis = _wtof(m_strEditFeldsparDis);
	float fEditQuartzDis = _wtof(m_strEditQuartzDis);
	float fEditPlagioclaseDis = _wtof(m_strEditPlagioclaseDis);
	float fEditBiotiteDis = _wtof(m_strEditBiotiteDis);
	float fEditTitaniteDis = _wtof(m_strEditTitaniteDis);
	float fEditHornblendeDis = _wtof(m_strEditHornblendeDis);
	float fEditEpidoteDis = _wtof(m_strEditEpidoteDis);
	float fEditChloriteDis = _wtof(m_strEditChloriteDis);

	float fStonTyDisate[dfSTONE_TYPE_CNT] = {
		0.0, //공극은 안씀..
		fEditFeldsparDis,
		fEditQuartzDis,
		fEditPlagioclaseDis,
		fEditBiotiteDis,
		fEditTitaniteDis,
		fEditHornblendeDis,
		fEditEpidoteDis,
		fEditChloriteDis
	};

	for (int i = 0; i < dfSTONE_TYPE_CNT; i++)
		M_DELETE(m_pPDGranularDisintegration[i]);

	for (int i = 0; i < dfSTONE_TYPE_CNT; i++)
		m_pPDGranularDisintegration[i] = new CProbDistribution(3,0.0,fStonTyDisate[i],0.001,0.0,0.0,0.0,0.0,0.0,0.0);;


}


void CModelingWeatheringOfRockDlg::InitStoneCnt()
{
	m_mapStoneTypeCnt.clear();
	for (int i = 0; i < dfSTONE_TYPE_CNT; i++)
		m_mapStoneTypeCnt.insert(make_pair(i,0));

	m_mapStoneLayerCnt.clear();
	for (int i = 0; i < dfSTONE_DEEP_LAYER + 1; i++)
		m_mapStoneLayerCnt.insert(make_pair(i,0));
	
}

void CModelingWeatheringOfRockDlg::CreaetSoildVoxelData()
{
	bool bTRUE = false;
	unsigned long long nSeekSize = 0;

	ST_DELETE_VOXEL_POS stDeleteVoxelPos;
	for(int nZ = 0 ; nZ < m_nZFileVoxCnt ; nZ++)
	{
		for(int nY = 0 ; nY < m_nYFileVoxCnt ; nY++)
		{
			for(int nX = 0 ; nX < m_nXFileVoxCnt ; nX++)
			{

				nSeekSize = nX  + (m_nXFileVoxCnt * nY) + (m_nXFileVoxCnt * m_nYFileVoxCnt * nZ);

				//! 메모리 처리
				if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
				{
					bTRUE = g_vecVoxelTotal[nSeekSize];
				}
// 				else //파일 처리
// 				{
// 					_fseeki64(p_Devfile,nSeekSize ,SEEK_SET);
// 					fread(&bTRUE, sizeof(bool), 1, p_Devfile); 
// 				}

				if(bTRUE)
				{
					stDeleteVoxelPos.nX = nX;
					stDeleteVoxelPos.nY = nY;
					stDeleteVoxelPos.nZ = nZ;
					//g_vecCompleteVoxelPos.push_back(stDeleteVoxelPos);
					g_vecCompleteVoxelPosSolid.push_back(stDeleteVoxelPos);
				}
			}
		}

	}
	
// 
// 	m_nSolidVoxelCnt = 0;
// 	CString strNumPath = gf_GetModulePath() + L"SurfaceNum.dat";
// 	FILE *p_Numfile = NULL; 
// 
// 	CString strSurPath = gf_GetModulePath() + L"SurfacePos.dat";
// 	FILE *p_Surfile = NULL; 
// 
// 	g_vecNumData.clear();
// 	g_vecSurData.clear();
// 
// 
// 
// 	// 입상붕괴 도달값 계산을 위한 확률변수 설정
// 	CString strGranularDisintegration = L"";
// 	m_editGranularDisintegration.GetWindowText(strGranularDisintegration);
// 	double dGranularDisintegration = _wtof(strGranularDisintegration);
// 	CProbDistribution PDGranularDisintegration(3,0.0,dGranularDisintegration,0.001,0.0,0.0,0.0,0.0,0.0,0.0);
// 	// 공극률 설정을 위한 확률변수 설정
// 	CString strInitData = L"";
// 	m_editInitPorosity.GetWindowText(strInitData);
// 	double dInitPorosity = _wtof(strInitData);
// 	CProbDistribution PDPorosity(4,0.0,0.0,0.0001,dInitPorosity,0.0,0.0,0.0,0.0,0.0);
// 
// 	int nSize = (int)g_vecCompleteVoxelPos.size();		
// 
// 	//! 파일 처리
// 	if(m_nComboProcessSel == dfDATA_PROCESS_FILE)
// 	{
// 		p_Numfile = _wfopen(strNumPath, L"wb"); 
// 		p_Surfile = _wfopen(strSurPath, L"wb");
// 
// 		if(NULL != p_Numfile || NULL != p_Surfile)
// 		{ 
// 			fwrite(&m_nXFileVoxCnt, sizeof(int), 1, p_Numfile); 
// 			fwrite(&m_nYFileVoxCnt, sizeof(int), 1, p_Numfile); 
// 			fwrite(&m_nZFileVoxCnt, sizeof(int), 1, p_Numfile); 
// 			fwrite(&nSize, sizeof(int), 1, p_Numfile); 
// 
// 			fwrite(&m_nXFileVoxCnt, sizeof(int), 1, p_Surfile); 
// 			fwrite(&m_nYFileVoxCnt, sizeof(int), 1, p_Surfile); 
// 			fwrite(&m_nZFileVoxCnt, sizeof(int), 1, p_Surfile); 
// 			fwrite(&nSize, sizeof(int), 1, p_Surfile); 
// 		}
// 	}
// 
// 	int nProcessCnt = 0;
// 	for(int v = 0 ; v <  nSize ; v++)
// 	{
// 		ST_VOXEL_FILE_POS stVoxelFilePos;
// 		stVoxelFilePos.nX = g_vecCompleteVoxelPos[v].nX;
// 		stVoxelFilePos.nY = g_vecCompleteVoxelPos[v].nY;
// 		stVoxelFilePos.nZ = g_vecCompleteVoxelPos[v].nZ;
// 		stVoxelFilePos.fPorosity				= 0;									// 공극률
// 		stVoxelFilePos.fGranularDisintegration = 0;		// 입상붕괴 도달값  ;
// 
// 		if(m_nComboProcessSel == dfDATA_PROCESS_MEMORY)
// 		{
// 			g_vecNumData.push_back(stVoxelFilePos);
// 		}
// 		else
// 		{
// 			if(NULL != p_Numfile || NULL != p_Surfile)
// 				fwrite(&stVoxelFilePos, sizeof(ST_VOXEL_FILE_POS), 1, p_Numfile); 
// 		}
// 
// 
// 		strProcessStatus.Format(L"Save Surface Voxel Progress : %.2f %%",float((float)nProcessCnt++ / (float)nSize)*100.0);
// 		m_editProcessStatus.SetWindowTextW(strProcessStatus);
// 	}
// 	if(NULL != p_Numfile) 
// 		fclose(p_Numfile); 
// 	if(NULL != p_Surfile)
// 		fclose(p_Surfile); 
// 
// 
// 	strProcessStatus.Format(L"Save Complete");
// 	m_editProcessStatus.SetWindowTextW(strProcessStatus);

}


void CModelingWeatheringOfRockDlg::SetStoneTypeCntView()
{
	UpdateData(TRUE);
	m_mapStoneTypeCnt;
	map<int,int>::iterator	iterStoneCnt;
	CString strTmp;
	for (iterStoneCnt = m_mapStoneTypeCnt.begin(); iterStoneCnt != m_mapStoneTypeCnt.end(); iterStoneCnt++)
	{
		strTmp.Format(L"%d", iterStoneCnt->second);
		switch (iterStoneCnt->first)
		{
		case 0:
			m_strEditPorosityCnt = strTmp;
			break;
		case 1:
			m_strEditFeldsparDel = strTmp;
			break;
		case 2:
			m_strEditQuartzDel = strTmp;
			break;
		case 3:
			m_strEditPlagioclaseDel = strTmp;
			break;
		case 4:
			m_strEditBiotiteDel = strTmp;
			break;
		case 5:
			m_strEditTitaniteDel = strTmp;
			break;
		case 6:
			m_strEditHornblendeDel = strTmp;
			break;
		case 7:
			m_strEditEpidoteDel = strTmp;
			break;
		case 8:
			m_strEditChloriteDel = strTmp;
			break;
		default:
			break;
		}
	}
	UpdateData(FALSE);
}




void CModelingWeatheringOfRockDlg::UpdateMainView(bool bComplete)
{
	if(m_checkRenderObj.GetCheck())
	{
		m_pDlgOpenglView->m_bComplete = bComplete;
		m_pDlgOpenglView->OnPaint();
	}
}

void CModelingWeatheringOfRockDlg::OnBnClickedButtonSee()
{
	
	if(m_pDlgOpenglViewerDialog)
	{
		m_pDlgOpenglViewerDialog->ShowWindow(TRUE);

		m_pDlgOpenglViewerDialog->m_pDlgOpenglViewer->m_bComplete = FALSE;
		m_pDlgOpenglViewerDialog->m_pDlgOpenglViewer->OnPaint();
	}

}


void CModelingWeatheringOfRockDlg::OnCbnSelchangeComboUseBlockThread()
{
	m_nComboBlockThreadSel = m_comboBlockThreadSel.GetCurSel();
}


void CModelingWeatheringOfRockDlg::OnCbnSelchangeComboProcessCalcRocking()
{

}


struct extract_second
{
	template <typename T, typename U>
	const U operator() (const std::pair<T,U> &p) const
	{
		return p.second;
	}
};

void CModelingWeatheringOfRockDlg::CalcRockingGpu(int nRepeatCnt)
{
	map<CString,ST_PARTICLE_POS>							mapOutsideDataTemp;	
	mapOutsideDataTemp = g_MapOutsideData;

	ST_PARTICLE_POS_CUDA	*pstPrarticlePos = NULL;
	pstPrarticlePos = new ST_PARTICLE_POS_CUDA[m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt];
	memset(pstPrarticlePos, NULL, sizeof(ST_PARTICLE_POS_CUDA) * m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt);

	SetGpuData();

	//std::transform(g_MapOutsideData.begin(), g_MapOutsideData.end(), pstPrarticlePos, extract_second());


	for (int nZ = 0; nZ < m_nZFileVoxCnt; nZ++)
	{
		for (int nY = 0; nY < m_nYFileVoxCnt; nY++)
		{
			for (int nX = 0; nX < m_nXFileVoxCnt; nX++)
			{
				unsigned long long nSeekSize = nX  + (m_nXFileVoxCnt * nY) + (m_nXFileVoxCnt * m_nYFileVoxCnt * nZ);

				CString strKey = L"";
				strKey.Format(L"%d-%d-%d",nX,nY,nZ);

				map<CString,ST_PARTICLE_POS>::iterator iterOutsideData;	
				iterOutsideData = g_MapOutsideData.find(strKey);
				if(iterOutsideData != g_MapOutsideData.end())
				{
					pstPrarticlePos[nSeekSize].x = nX;
					pstPrarticlePos[nSeekSize].y = nY;
					pstPrarticlePos[nSeekSize].z = nZ;

					pstPrarticlePos[nSeekSize].bCheck = false;

					pstPrarticlePos[nSeekSize].fPorosity = iterOutsideData->second.fPorosity;

					pstPrarticlePos[nSeekSize].abExternalSide[0] = iterOutsideData->second.abExternalSide[0];
					pstPrarticlePos[nSeekSize].abExternalSide[1] = iterOutsideData->second.abExternalSide[1];
					pstPrarticlePos[nSeekSize].abExternalSide[2] = iterOutsideData->second.abExternalSide[2];
					pstPrarticlePos[nSeekSize].abExternalSide[3] = iterOutsideData->second.abExternalSide[3];
					pstPrarticlePos[nSeekSize].abExternalSide[4] = iterOutsideData->second.abExternalSide[4];
					pstPrarticlePos[nSeekSize].abExternalSide[5] = iterOutsideData->second.abExternalSide[5];

					pstPrarticlePos[nSeekSize].fGranularDisintegration = iterOutsideData->second.fGranularDisintegration;
					pstPrarticlePos[nSeekSize].fHaveWater = iterOutsideData->second.fHaveWater;
					pstPrarticlePos[nSeekSize].sStoneType = iterOutsideData->second.sStoneType;
					pstPrarticlePos[nSeekSize].bInOut = iterOutsideData->second.bInOut;
					pstPrarticlePos[nSeekSize].sLayerIdx = iterOutsideData->second.sLayerIdx;
				}
				else
				{
					//! 없는부분!!
					ST_PARTICLE_POS_CUDA	st_particle_pos;
					memset(&st_particle_pos, NULL, sizeof(ST_PARTICLE_POS_CUDA));

					st_particle_pos.x = nX;
					st_particle_pos.y = nY;
					st_particle_pos.z = nZ;
					st_particle_pos.sStoneType = -1;

					pstPrarticlePos[nSeekSize] = st_particle_pos;
				}

			}
		}
	}

	
	
	int nVoxelAllCnt = m_nXFileVoxCnt * m_nYFileVoxCnt * m_nZFileVoxCnt;

	CString strTemp;
	strTemp.Format(L"GPU - CalcRockAging Start(%d)", m_nCalcTryCnt);
	ShowTraceTime(strTemp);

	m_GPUCalcRockAgingInner.SetInnderVoxelData(nRepeatCnt, nVoxelAllCnt, pstPrarticlePos);
	
	//std::copy(pstPrarticlePos, pstPrarticlePos + sizeof(ST_PARTICLE_POS) * g_MapOutsideData.size(), g_MapOutsideData.begin());
	strTemp.Format(L"GPU - Calc Rocking End(%d)", nRepeatCnt + m_nCalcTryCnt);
	ShowTraceTime(strTemp, 1);


	g_MapOutsideData.clear();

	CString strKey;
	m_mapStoneTypeCnt.clear();
	for (int a = 0; a < nVoxelAllCnt; a++)
	{
		ST_PARTICLE_POS_CUDA	stPrarticlePosMask = pstPrarticlePos[a];

		ST_PARTICLE_POS	stParticlePos;
		
		int x = a % m_nXFileVoxCnt;
		int y = ((a / m_nYFileVoxCnt) % m_nYFileVoxCnt);
		int z = ((a / (m_nXFileVoxCnt * m_nYFileVoxCnt)) % (m_nXFileVoxCnt * m_nYFileVoxCnt));

		//! 일단 최종으로는 뺼거기 떄문에 이렇게 해준다.
		stParticlePos.x = stPrarticlePosMask.x;
		stParticlePos.y = stPrarticlePosMask.y;
		stParticlePos.z = stPrarticlePosMask.z;

// 		stParticlePos.x = x;
// 		stParticlePos.y = y;
// 		stParticlePos.z = z;
		

		if(stPrarticlePosMask.bCheck == false)
		{
			int k= 0;
		}



		//stParticlePos.y = a / m_nYFileVoxCnt + (a % m_nYFileVoxCnt);
		//stParticlePos.z = a / (m_nXFileVoxCnt + m_nYFileVoxCnt) + (a % m_nZFileVoxCnt);
		//stParticlePos.bUse = true;
		stParticlePos.fPorosity = stPrarticlePosMask.fPorosity;
		stParticlePos.abExternalSide[0] = stPrarticlePosMask.abExternalSide[0];
		stParticlePos.abExternalSide[1] = stPrarticlePosMask.abExternalSide[1];
		stParticlePos.abExternalSide[2] = stPrarticlePosMask.abExternalSide[2];
		stParticlePos.abExternalSide[3] = stPrarticlePosMask.abExternalSide[3];
		stParticlePos.abExternalSide[4] = stPrarticlePosMask.abExternalSide[4];
		stParticlePos.abExternalSide[5] = stPrarticlePosMask.abExternalSide[5];
		stParticlePos.iExternalSideCnt  = stPrarticlePosMask.abExternalSide[0] + stPrarticlePosMask.abExternalSide[1] + stPrarticlePosMask.abExternalSide[2]
		+ stPrarticlePosMask.abExternalSide[3] + stPrarticlePosMask.abExternalSide[4] + stPrarticlePosMask.abExternalSide[5];
		stParticlePos.fGranularDisintegration = stPrarticlePosMask.fGranularDisintegration;
		stParticlePos.fHaveWater = stPrarticlePosMask.fHaveWater;
		stParticlePos.sStoneType = stPrarticlePosMask.sStoneType;
		stParticlePos.bInOut = stPrarticlePosMask.bInOut;
		stParticlePos.sLayerIdx = stPrarticlePosMask.sLayerIdx;

		if(stParticlePos.sStoneType == -2)
		{
			int a= 0;
		}


		if(stParticlePos.sStoneType != -1)
		{
			strKey.Format(L"%d-%d-%d",stParticlePos.x,stParticlePos.y,stParticlePos.z);
			//g_MapOutsideDataTemp.insert(make_pair(strKey,stParticlePos));		//! 중심 레이어


// 			map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;	// Map <배열 XYZ 위치  / 입자 위치 정보> 
// 			iterOutsideData = g_MapOutsideData.find(strKey);
// 			if(iterOutsideData == g_MapOutsideData.end())
// 				int a= 0;
			g_MapOutsideData.insert(make_pair(strKey,stParticlePos));		//! 중심 레이어


			//////////////////////////////////////////////////////////////////////////
			//! 입자 종류 카운트
			map<int,int>::iterator iterStoneCnt;
			iterStoneCnt = m_mapStoneTypeCnt.find(stParticlePos.sStoneType);
			if(iterStoneCnt != m_mapStoneTypeCnt.end())
				iterStoneCnt->second++;
			else
				m_mapStoneTypeCnt.insert(make_pair(stParticlePos.sStoneType, 1));
			
		}
		else
		{

		}


	}

	

	//SetStoneTypeCntView();

	//g_MapOutsideData.clear();
	map<CString,ST_PARTICLE_POS>::iterator				iterOutsideDataTemp1;	
	map<CString,ST_PARTICLE_POS>::iterator				iterOutsideDataTemp2;		
	for (iterOutsideDataTemp1 = mapOutsideDataTemp.begin(); iterOutsideDataTemp1 != mapOutsideDataTemp.end(); iterOutsideDataTemp1++)
	{
		iterOutsideDataTemp2 = g_MapOutsideData.find(iterOutsideDataTemp1->first);
		if(iterOutsideDataTemp2 != g_MapOutsideData.end())
		{
			int n = memcmp(&iterOutsideDataTemp1->second, &iterOutsideDataTemp2->second, sizeof(ST_PARTICLE_POS));
			if(n == 0)
			{
				int k = 0;
			}
			else
			{
				int a = 0;
			}

			if(iterOutsideDataTemp1->second.abExternalSide[0] == iterOutsideDataTemp2->second.abExternalSide[0])
			{
				int a = 0;
			}


			if(iterOutsideDataTemp1->second.abExternalSide[1] == iterOutsideDataTemp2->second.abExternalSide[1])
			{
				int a = 0;
			}

			if(iterOutsideDataTemp1->second.abExternalSide[2] == iterOutsideDataTemp2->second.abExternalSide[2])
			{
				int a = 0;
			}


			if(iterOutsideDataTemp1->second.abExternalSide[3] == iterOutsideDataTemp2->second.abExternalSide[3])
			{
				int a = 0;
			}

			if(iterOutsideDataTemp1->second.abExternalSide[4] == iterOutsideDataTemp2->second.abExternalSide[4])
			{
				int a = 0;
			}


			if(iterOutsideDataTemp1->second.abExternalSide[5] == iterOutsideDataTemp2->second.abExternalSide[5])
			{
				int a = 0;
			}


		}
		else
		{
			int a =0;
		}
	}

	m_nCalcTotalCnt = g_MapOutsideData.size();
	
	//mapOutsideDataTemp.size() - g_MapOutsideData.size();

	m_nCalcTryCnt += nRepeatCnt;

	m_bStopCalc = TRUE;
	SendMessage(WM_FINISH_SIM_ONE_STEP_MSG,m_bStopCalc,999);


	M_A_DELETE(pstPrarticlePos);


}


void CModelingWeatheringOfRockDlg::OnBnClickedButton5()
{
	CString strPath = gf_GetModulePath() + L"Origin\\";
	::CreateDirectory(strPath, NULL);

	
	int nIdxS = g_strFilePathName.ReverseFind('\\');
	int nIdxE = g_strFilePathName.Find(L".OBJ");
	CString strModel = g_strFilePathName.Mid(nIdxS + 1, nIdxE - (nIdxS + 1));

	CString strFileName = L"";
	strFileName.Format(L"M_%s_P_%d_S_%d.dat", strModel, g_nDivideCnt + 4, m_nCalcTryCnt);
	CString strNumPath = strPath + strFileName;

	FILE *p_Numfile = _wfopen(strNumPath, L"wb"); 

	if(NULL != p_Numfile )
	{ 

		ST_PARTICLE_POS stParticlePos;
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
		int nTotalCnt = g_MapOutsideData.size();
		fwrite(&nTotalCnt, sizeof(int), 1, p_Numfile); 

		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
		{
			stParticlePos = iterOutsideData->second;

			fwrite(&stParticlePos, sizeof(ST_PARTICLE_POS), 1, p_Numfile); 
		}


		fwrite(&m_nXFileVoxCnt, sizeof(int), 1, p_Numfile); 
		fwrite(&m_nYFileVoxCnt, sizeof(int), 1, p_Numfile); 
		fwrite(&m_nZFileVoxCnt, sizeof(int), 1, p_Numfile); 


		int nVoxelSize = g_vecVoxelTotal.size();
		fwrite(&nVoxelSize, sizeof(int), 1, p_Numfile); 

		
		bool* pbImsi = new bool[nVoxelSize];

		copy(g_vecVoxelTotal.begin(), g_vecVoxelTotal.end(), pbImsi);

		fwrite(pbImsi, sizeof(bool) * nVoxelSize, 1, p_Numfile); 

		M_A_DELETE(pbImsi);


		//fwrite(&g_vecVoxelTotal.begin(), sizeof(bool) * g_vecVoxelTotal.size(), 1, p_Numfile); 


		fclose(p_Numfile); 
	}


	//////////////////////////////////////////////////////////////////////////
	//  [10/29/2019 kjky12] 복셀 유무 정보도 저장해야할듯
// 	strFileName = L"";
// 	strFileName.Format(L"M_%s_P_%d_S_%d.dat", strModel, g_nDivideCnt + 4, m_nCalcTryCnt);
// 	strNumPath = strPath + strFileName;
// 
// 	FILE *p_Numfile = _wfopen(strNumPath, L"wb"); 
// 
// 	if(NULL != p_Numfile )
// 	{ 
// 
// 		ST_PARTICLE_POS stParticlePos;
// 		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
// 		int nTotalCnt = g_MapOutsideData.size();
// 		fwrite(&nTotalCnt, sizeof(int), 1, p_Numfile); 
// 
// 		for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
// 		{
// 			stParticlePos = iterOutsideData->second;
// 
// 			fwrite(&stParticlePos, sizeof(ST_PARTICLE_POS), 1, p_Numfile); 
// 
// 
// 
// 		}
// 		fclose(p_Numfile); 
// 	}


}


void CModelingWeatheringOfRockDlg::OnBnClickedButtonObjFileModeling5()
{
		//g_strFilePathName =L"";

	CFileDialog FileDlg(TRUE, _T("dat"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("dat File(*.dat)|*.dat||") );
	if (FileDlg.DoModal() == IDOK )
	{
		//g_strFilePathName = FileDlg.GetPathName();
	}
	else
	{
		return;
	}

// 	if(g_strFilePathName.IsEmpty())
// 		return;


	srand((unsigned int)time(NULL));
	InitStonRate();
	InitEditValue();
	SetProbDistributionValue();
	InitStoneCnt();

	g_vecVoxelTotal.clear();


// 	CString strFileName = L"";
 	CString strKey= L"";
// 	CString strPath = gf_GetModulePath() + L"Origin\\";
// 	strFileName.Format(L"Setp%09d.dat",pParent->m_nLoadStep);
	CString strNumPath = FileDlg.GetPathName();;

	FILE *p_Numfile = _wfopen(strNumPath, L"rb"); 

	CString strProcessStatus;


	g_MapOutsideData.clear();
	int nTotalCnt = 0;
	if(NULL != p_Numfile )
	{ 
		map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;

		fread(&nTotalCnt, sizeof(int), 1, p_Numfile); 

		for(int n = 0 ; n < nTotalCnt ; n++)
		{
			ST_PARTICLE_POS stParticlePos;

			fread(&stParticlePos, sizeof(ST_PARTICLE_POS), 1, p_Numfile); 
		
			strKey.Format(L"%d-%d-%d",stParticlePos.x,stParticlePos.y,stParticlePos.z);
			g_MapOutsideData.insert(make_pair(strKey,stParticlePos));

			strProcessStatus.Format(L"Loading Progress : %.2f %%",float((float)n / (float)nTotalCnt) * 100.0);
			m_editProcessStatus.SetWindowTextW(strProcessStatus);

			map<int,int>::iterator	iterStoneCnt = m_mapStoneTypeCnt.find(stParticlePos.sStoneType);
			if(iterStoneCnt != m_mapStoneTypeCnt.end())
			{
				iterStoneCnt->second++;
			}

			iterStoneCnt = m_mapStoneLayerCnt.find(stParticlePos.sLayerIdx);
			if(iterStoneCnt != m_mapStoneLayerCnt.end())
			{
				iterStoneCnt->second++;
			}



		}

		fread(&m_nXFileVoxCnt, sizeof(int), 1, p_Numfile); 
		fread(&m_nYFileVoxCnt, sizeof(int), 1, p_Numfile); 
		fread(&m_nZFileVoxCnt, sizeof(int), 1, p_Numfile); 


		int nVoxelSize = 0;
		fread(&nVoxelSize, sizeof(int), 1, p_Numfile); 
		g_vecVoxelTotal.resize(nVoxelSize);

		bool* pbImsi = new bool[nVoxelSize];

		fread(pbImsi, sizeof(bool) * nVoxelSize, 1, p_Numfile); 


		copy(pbImsi, pbImsi + nVoxelSize, g_vecVoxelTotal.begin());

		M_A_DELETE(pbImsi);


		fclose(p_Numfile); 
	}

	
	//////////////////////////////////////////////////////////////////////////
	//! 입자 종류 카운트
// 	map<CString,ST_PARTICLE_POS>::iterator		iterOutsideData;
// 	int nTotalCnt = g_MapOutsideData.size();
// 	fwrite(&nTotalCnt, sizeof(int), 1, p_Numfile); 
// 
// 	for(iterOutsideData = g_MapOutsideData.begin() ; iterOutsideData != g_MapOutsideData.end() ; iterOutsideData++)
// 	{
// 		map<int,int>::iterator	iterStoneCnt = m_mapStoneTypeCnt.find(iterOutsideData->second.sStoneType);
// 		if(iterStoneCnt != m_mapStoneTypeCnt.end())
// 			iterStoneCnt->second++;
// 		else
// 			m_mapStoneTypeCnt.insert(make_pair(iterOutsideData->second.sStoneType, 1));
// 
// 		m_nCalcTotalCnt++;
// 		m_nSolidTotalCnt++; //GetTotalCntFromSolidFile();
// 
// 	}
// 
// 	CString strTmp = L"";
// 	strTmp.Format(L"%d",m_nCalcTotalCnt);
// 	m_editTotalSolidCnt.SetWindowTextW(strTmp);

	m_editProcessStatus.SetWindowTextW(L"Load Finsh");

	//////////////////////////////////////////////////////////////////////////
	//  [10/29/2019 kjky12] 초기화
	m_nCalcTotalCnt = nTotalCnt;
	m_setDeleteParticle.clear();
	m_nCalcTryCnt = 1;		// 계산 횟수

	CString strTmp = L"";

	strTmp.Format(L"%d",m_nCalcTotalCnt);
	m_editTotalSolidCnt.SetWindowTextW(strTmp);

	SetStoneTypeCntView();

	//  [10/29/2019 kjky12] 임시...
	OnBnClickedButton6();

	SendMessage(WM_FINISH_SOLID_VOXEL_MSG,5,0);	




}

	vector<bool>											g_vecVoxelTotalT;
	map<CString,ST_PARTICLE_POS>								g_MapOutsideDataT;
	map<int,int>	m_mapStoneTypeCntT;
	map<int,int>	m_mapStoneLayerCntT;

	int m_nXFileVoxCntT;
	int m_nYFileVoxCntT;
	int m_nZFileVoxCntT;
	int m_nCalcTotalCntT;
	set<CString>		m_setDeleteParticleT;
	int m_nCalcTryCntT;



void CModelingWeatheringOfRockDlg::OnBnClickedButton6()
{
// 	srand((unsigned int)time(NULL));
// 	InitStonRate();
// 	InitEditValue();
// 	SetProbDistributionValue();
// 	InitStoneCnt();

	g_vecVoxelTotalT = g_vecVoxelTotal;
	g_MapOutsideDataT	= g_MapOutsideData;
	m_mapStoneTypeCntT	= m_mapStoneTypeCnt;
	m_mapStoneLayerCntT	= m_mapStoneLayerCnt;
	m_nXFileVoxCntT		= m_nXFileVoxCnt;
	m_nYFileVoxCntT		= m_nYFileVoxCnt;
	m_nZFileVoxCntT		= m_nZFileVoxCnt;
	m_nCalcTotalCntT		= m_nCalcTotalCnt;
	

	m_setDeleteParticleT.clear();
	m_nCalcTryCntT = 1;		// 계산 횟수



// 	m_editProcessStatus.SetWindowTextW(L"Load Finsh");
// 
// 
// 	CString strTmp = L"";
// 
// 	strTmp.Format(L"%d",m_nCalcTotalCnt);
// 	m_editTotalSolidCnt.SetWindowTextW(strTmp);
// 
// 	SetStoneTypeCntView();
// 
// 	SendMessage(WM_FINISH_SOLID_VOXEL_MSG,5,0);	
}


void CModelingWeatheringOfRockDlg::OnBnClickedButton7()
{
	srand((unsigned int)time(NULL));
	InitStonRate();
	InitEditValue();
	SetProbDistributionValue();
	InitStoneCnt();

	g_vecVoxelTotal = g_vecVoxelTotalT;
	g_MapOutsideData	= g_MapOutsideDataT;
	m_mapStoneTypeCnt	= m_mapStoneTypeCntT;
	m_mapStoneLayerCnt	= m_mapStoneLayerCntT;
	m_nXFileVoxCnt		= m_nXFileVoxCntT;
	m_nYFileVoxCnt		= m_nYFileVoxCntT;
	m_nZFileVoxCnt		= m_nZFileVoxCntT;
	m_nCalcTotalCnt		= m_nCalcTotalCntT;


	m_setDeleteParticle.clear();
	m_nCalcTryCnt = 1;		// 계산 횟수



	m_editProcessStatus.SetWindowTextW(L"Load Finsh");
	
	
	CString strTmp = L"";
	
	strTmp.Format(L"%d",m_nCalcTotalCnt);
	m_editTotalSolidCnt.SetWindowTextW(strTmp);
	
	SetStoneTypeCntView();
	
	SendMessage(WM_FINISH_SOLID_VOXEL_MSG,5,0);	

}

void CModelingWeatheringOfRockDlg::RefreshMessage()
{
	MSG msg;
	// 	DWORD dwStart;
	// 	dwStart = GetTickCount();

	//	while (GetTickCount() - dwStart < 3/*mseconds*/)
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}



void CModelingWeatheringOfRockDlg::OnBnClickedButton8()
{
	const int nCnt = 7;
	int nSimul[nCnt] = {
		1,
		30,
		60,
		90,
		120,
		150,
		180
	};

	CString strTemp = L"";
	for (int i = 0 ; i < nCnt; i++)
	{
		//! 초기화
		OnBnClickedButton7();

		RefreshMessage();

		strTemp.Format(L"%d", nSimul[i]);
		m_EditStopSimulCnt.SetWindowTextW(strTemp);

		strTemp.Format(L"%d Start", nSimul[i]);
		m_editProcessStatus.SetWindowTextW(strTemp);

		RefreshMessage();

		OnBnClickedButtonCalcRockAging();


		strTemp.Format(L"%d Finish", nSimul[i]);
		m_editProcessStatus.SetWindowTextW(strTemp);

		RefreshMessage();
	}

}
