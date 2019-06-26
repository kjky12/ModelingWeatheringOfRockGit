
// ModelingWeatheringOfRockDlg.h : 헤더 파일
//

#pragma once


#include "DlgOpenglView.h"
#include "DlgOpenglViewerDialog.h"


#include "DlgColorChart.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "ObjLoader.h"

#include "GPUSolid.cuh"
#include "GPUSurface.cuh"
#include "GPUCalcRockAgingInner.cuh"


#include "ColourPicker.h"
#include "PieChartCtrl.h"

#include "ProbDistribution.h"

#define dfSTONE_DEEP_LAYER	10 //레이어 깊이를 0 ~ dfSTONE_DEEP_LAYER 번호까지 있음
#define dfSTONE_TYPE_CNT	9


// CModelingWeatheringOfRockDlg 대화 상자
class CModelingWeatheringOfRockDlg : public CDialogEx
{
// 생성입니다.
public:
	CModelingWeatheringOfRockDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MODELINGWEATHERINGOFROCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCreateModelData();
	CEdit m_editXSize;
	CEdit m_editYSize;
	CEdit m_editZSize;
	CEdit m_editParticleSize;

	unsigned int m_nCalcTotalCnt;	// 계산 입자 개수
	int m_nCalcTryCnt;		// 계산 횟수

	BOOL	m_bStopCalc;	//0:계속, 1:쭉 수행
	BOOL	m_bTriangle;

	int m_nSolidVoxelCnt;

	CDlgOpenglView*		m_pDlgOpenglView;
	CDlgColorChart*		m_pDlgColorChart;

	//! 가시를 위한 
	CDlgOpenglViewerDialog*		m_pDlgOpenglViewerDialog;

	//CDlgOpenglView*		m_pDlgOpenglViewer;
	
// 	float m_fMaxX;
// 	float m_fMaxY;
// 	float m_fMaxZ;
// 	float m_fMinX;
// 	float m_fMinY;
// 	float m_fMinZ;


	unsigned long long m_nXFileVoxCnt;
	unsigned long long m_nYFileVoxCnt;
	unsigned long long m_nZFileVoxCnt;
	unsigned long long m_nDivFileSize;


	void ReplaceFileInfo(unsigned int nX,unsigned int nY,unsigned int nZ,short iType = 0);

	map<CString,ST_PARTICLE_POS> m_mapInsertParticle;
	set<CString>		m_setDeleteParticle;

	CObjLoader m_obj1;

	

	// 솔리드 파일에서 내부 셀 사용 여부 적용 [2/9/2014 ART]
	void DeleteCellToSolidFile(int nX, int nY, int nZ);
	// 솔리드 파일에서 내부 셀 사용 여부 판단 [2/9/2014 ART]
	bool GetUsedCellFromSolidFile(int nX, int nY, int nZ);
	int m_nXVoxCnt;
	int m_nYVoxCnt;
	int m_nZVoxCnt;

	
	void CalcExternalSide(int x,int y,int z,int nXVoxCnt, int nYVoxCnt, int nZVoxCnt, ST_PARTICLE_POS& stParticlePos);
	// 입자 제거후 주변 입자의 외부 노출 갯수 재계산
	void ReCalcExternalSide(int x,int y,int z, map<CString,ST_PARTICLE_POS>	&mapOutsideData);

	
	
	afx_msg LRESULT OnCalcRockAgingMsg(WPARAM wParam, LPARAM lParam);		
	afx_msg LRESULT OnFinshVoxelMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFinshSolidVoxelMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangePosRotMsg(WPARAM wParam, LPARAM lParam);
	
	

//	CProgressCtrl m_ProgressCtrl;
	afx_msg void OnDestroy();
	//! 뷰 프레임 크기
	CStatic m_frameView;
	
	afx_msg void OnBnClickedButtonCreateModelData2();
	
	
	afx_msg void OnBnClickedOk();
	CStatic m_staticColorChart;
	afx_msg void OnBnClickedButtonObjFileModeling();


	static bool	VexelXDataSort(ST_VOXEL_3D_POS &s, ST_VOXEL_3D_POS &t);
	static bool	VexelYDataSort(ST_VOXEL_3D_POS &s, ST_VOXEL_3D_POS &t);
	static bool	VexelZDataSort(ST_VOXEL_3D_POS &s, ST_VOXEL_3D_POS &t);
	afx_msg void OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_SliderCtrl3DSlideView;
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_editXData;
	CEdit m_editYData;
	CEdit m_editZData;
	afx_msg void OnBnClickedButtonSolidData();


	unsigned int GetTotalCntFromSolidFile();

	bool GetUsedCellFromSolidFileXYZ(int nIdx, int nX, int nY, int nZ);
	CEdit m_editSolidView;
	CEdit m_editXAxiDivide;
	CComboBox m_comboShow3DType;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButtonSolidData2();
	CComboBox m_combo3DViewEye;
	afx_msg void OnCbnSelchangeCombo2();
	
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	CSpinButtonCtrl m_SpinCtrl;
	CEdit m_editScaleData;
	CEdit m_editProcessStatus;
	
	CButton m_checkRenderObj;

	void SaveStepOutsideData(int nStep);
	void LoadStepOutsideData(int nStep);
	afx_msg void OnBnClickedButtonObjFileModeling3();
	CEdit m_editStepCnt;
	CButton m_checkTranObj;
	afx_msg void OnBnClickedCheck3();
	
	//! X축 회전 컨트롤
	CEdit m_editXRot;
	CEdit m_editYRot;
	CEdit m_editZRot;
	//! X 축 이동 컨트롤
	CEdit m_editXTran;
	CEdit m_editYTran;
	CEdit m_editZTran;

	// 위치 설정 버튼 
	afx_msg void OnBnClickedButtonObjFileModeling4();

	int m_nLoadStep;
	CButton m_checkViewAxis;
	afx_msg void OnBnClickedCheck4();
	CEdit m_editVoxelAxisCnt;

	CComboBox m_comboSelectAxis;
	
	
	float m_fTopRate;
	float m_fSideRate;
	float m_fBottomRate;




	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnEnChangeEditCalcTryCnt16();
	//////////////////////////////////////////////////////////////////////////
	//! 시뮬레이션 관련 함수 및 변수
	//! 외부가속 계수
	CEdit m_editCalcMulti;
	// 수분흡수율
	CEdit m_editCalcWaterInnerAbsorption;
	//! 수분 부피 팽찰률
	CEdit m_editCalcWaterChange;
	// 레이어별 수분부피차감률
	CEdit m_editCalcLayerWaterAborption;
	//! 초기 공극률
	CEdit m_editInitPorosity;
	//! 입상붕괴 임계치
	CEdit m_editGranularDisintegration;
	//! 공극률 개수
	CEdit m_editCalcPorosity;
	//! 상단 비율
	CEdit m_editTopRate;
	//! 측면 비율
	CEdit m_editSideRate;
	//! 하단 비율
	CEdit m_editBottomRate;

	//! 중지 주기
	CEdit m_EditStopSimulCnt;
	//! 삭제된 복셀수 컨트롤
	CEdit m_editGrDisCnt;
	//! 계산중인 복셀수 컨트롤
	CEdit m_editGrTotalCnt;
	//! 시뮬레이션 횟수 컨트롤
	CEdit m_editCalcTryCnt;
	//! 총 복셀 수 
	CEdit m_editTotalSolidCnt;
	//! 시뮬레이션 결과 저장 체크 박스
	CButton m_checkSaveStep;

	//! 저장 간격 컨트롤
	CEdit m_editSaveInterval;
	//! 저장 간격 변수
	int m_nSaveInterval;

	//! 외부 노출 단면 계산 버튼 클릭
	afx_msg void OnBnClickedButtonCalcExternalSide();
	//! 시뮬레이션 시작 버튼
	afx_msg void OnBnClickedButtonCalcRockAging();
	//! 시뮬레이션 중지 버튼
	afx_msg void OnBnClickedButtonCalcRockAgingCancel();
	//! 재수행 버튼
	afx_msg void OnBnClickedButtonCalcRockAging2();
	//! 한번 수행 버튼
	afx_msg void OnBnClickedButtonCalcRockAging3();
	//! 시뮬레이션 데이터 세팅
	bool CalcRockAgingSetData();


	
	//! 프로그램 성능 관련 //////////////////////////////////////////////////////////////////////////
	CComboBox	m_comboDataProcess;
	int			m_nComboProcessSel; //0: 메모리처리, 1: 파일처리
	afx_msg void OnCbnSelchangeComboDataProcess();
	//////////////////////////////////////////////////////////////////////////

	void ShowTraceTime(CString strMemo, int nGapCheck = 0);
	//! 시뮬레이션 클릭
	bool m_bSimulIdx;
	int m_nSimulIdx;
	int m_nSimulObjectIdx;
	afx_msg void OnBnClickedButton1();

	//! 자동 시뮬레이션
	afx_msg LRESULT OnSimulationMsg(WPARAM wParam, LPARAM lParam);


	//! 시간을 초로 바꿈..
	LONGLONG GetSecondFromTime(CTime tmCurrent);


	
	afx_msg void OnBnClickedButton3();
	CButton m_cudaCheck;
	afx_msg void OnBnClickedButton4();


	CGPUSolid		m_GPUSolid;
	CGPUSurface		m_GPUSurface;
	CGPUCalcRockAgingInner		m_GPUCalcRockAgingInner;

	//! 로직 처리 관련 //////////////////////////////////////////////////////////////////////////
	CComboBox	m_comboUseCuda;
	int			m_nComboLogicProcess; //0: CPU-CPU,  1:GPU-GPU

	CComboBox m_comboBlockThreadSel;
	int			m_nComboBlockThreadSel; //0: Blcok 3개,  1: Thread 3개


	//! 메모리 new 시간을 없애서 사용하기위해..
	bool* m_bDataTmp[dfTOTAL_COUNT];

	//////////////////////////////////////////////////////////////////////////
	//! 암석 성분에 대한 함수 및 변수
	afx_msg void OnBnClickedButtonViewRateStone();
	
	//! 입자비율
	CString	m_strEditFeldsparPer;		//장석
	CString	m_strEditQuartzPer;			//석영
	CString	m_strEditPlagioclasePer;	//사장석
	CString	m_strEditBiotitePer;		//흑운모
	CString	m_strEditTitanitePer;		//티타나이트
	CString	m_strEditHornblendePer;		//각섬석
	CString	m_strEditEpidotePer;		//녹렴석
	CString	m_strEditChloritePer;		//녹니석
	CString	m_strEditPorosityPer;		//공극비율

	//! 입자별 임계치
	CString	m_strEditFeldsparDis;
	CString	m_strEditQuartzDis;
	CString	m_strEditPlagioclaseDis;
	CString	m_strEditBiotiteDis;
	CString	m_strEditTitaniteDis;
	CString	m_strEditHornblendeDis;
	CString	m_strEditEpidoteDis;
	CString	m_strEditChloriteDis;

	//! 삭제 복셀
	CString	m_strEditFeldsparDel;
	CString	m_strEditQuartzDel;
	CString	m_strEditPlagioclaseDel;
	CString	m_strEditBiotiteDel;
	CString	m_strEditTitaniteDel;
	CString	m_strEditHornblendeDel;
	CString	m_strEditEpidoteDel;
	CString	m_strEditChloriteDel;
	//! 공극 개수
	CString	m_strEditPorosityCnt;


	void InitEditValue();

	int GetStoneType(ST_PARTICLE_POS* pstParticlePos = NULL);

	//! 스톤 개수를 카운트한다.
	void SetStoneTypeCntView();

	CProbDistribution* m_pPDGranularDisintegration[dfSTONE_TYPE_CNT];
	//! 입상붕괴 도달값 변수 초기화
	void SetProbDistributionValue();
	
	//! 입자별 개수 카운트 초기화
	void InitStoneCnt();

	//! 입자별 스톤 타입 초기화
	void InitStonRate(bool bClear = true);
	map<int,int>	m_mapInitStoneTypeCnt;

	//! 입자별 개수 카운트(초기화에서)
	map<int,int>	m_mapStoneTypeCnt;
	//! 레이어당 입자 개수
	map<int,int>	m_mapStoneLayerCnt;



	void CreaetSoildVoxelData();
	

	set<CString> CalcDeleteParticleInside(CString strPos);
	void CalcDeleteParticleInsideDetail(CString strPos, set<CString>		&setDelDataParticleInside);

	//! 오픈지엘 업데이트
	void UpdateMainView(bool bComplete = false);
	
	//! 가시화 화면 열기
	afx_msg void OnBnClickedButtonSee();
	//! 가시화 데이터 초기화 방향 - 0:상, 1:하, 2:좌, 3:우, 4:앞, 5:뒤
	void InitShowData(int nShow);
	void UpdateShowData(int nShow);

	int SameStartIdxFind(int nStartIdx, ST_VOXEL_3D_POS_VIEWER	&stVoxel3dPosViewer, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA> &mapShowData, vector<ST_VOXEL_3D_POS_VIEWER>&	vecVoxel3DPosViewer, set<ST_VOXEL_3D_POS_VIEWER>		&setVoxel3dPosViewer);
	afx_msg void OnCbnSelchangeComboUseBlockThread();
};





