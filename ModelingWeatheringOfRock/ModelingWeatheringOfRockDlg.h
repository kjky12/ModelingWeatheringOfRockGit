
// ModelingWeatheringOfRockDlg.h : ��� ����
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

#define dfSTONE_DEEP_LAYER	10 //���̾� ���̸� 0 ~ dfSTONE_DEEP_LAYER ��ȣ���� ����
#define dfSTONE_TYPE_CNT	9


// CModelingWeatheringOfRockDlg ��ȭ ����
class CModelingWeatheringOfRockDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CModelingWeatheringOfRockDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MODELINGWEATHERINGOFROCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


	


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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

	unsigned int m_nCalcTotalCnt;	// ��� ���� ����
	int m_nCalcTryCnt;		// ��� Ƚ��

	BOOL	m_bStopCalc;	//0:���, 1:�� ����
	BOOL	m_bTriangle;

	int m_nSolidVoxelCnt;

	CDlgOpenglView*		m_pDlgOpenglView;
	CDlgColorChart*		m_pDlgColorChart;

	//! ���ø� ���� 
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

	

	// �ָ��� ���Ͽ��� ���� �� ��� ���� ���� [2/9/2014 ART]
	void DeleteCellToSolidFile(int nX, int nY, int nZ);
	// �ָ��� ���Ͽ��� ���� �� ��� ���� �Ǵ� [2/9/2014 ART]
	bool GetUsedCellFromSolidFile(int nX, int nY, int nZ);
	int m_nXVoxCnt;
	int m_nYVoxCnt;
	int m_nZVoxCnt;

	
	void CalcExternalSide(int x,int y,int z,int nXVoxCnt, int nYVoxCnt, int nZVoxCnt, ST_PARTICLE_POS& stParticlePos);
	// ���� ������ �ֺ� ������ �ܺ� ���� ���� ����
	void ReCalcExternalSide(int x,int y,int z, map<CString,ST_PARTICLE_POS>	&mapOutsideData);

	
	
	afx_msg LRESULT OnCalcRockAgingMsg(WPARAM wParam, LPARAM lParam);		
	afx_msg LRESULT OnFinshVoxelMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFinshSolidVoxelMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangePosRotMsg(WPARAM wParam, LPARAM lParam);
	
	

//	CProgressCtrl m_ProgressCtrl;
	afx_msg void OnDestroy();
	//! �� ������ ũ��
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
	
	//! X�� ȸ�� ��Ʈ��
	CEdit m_editXRot;
	CEdit m_editYRot;
	CEdit m_editZRot;
	//! X �� �̵� ��Ʈ��
	CEdit m_editXTran;
	CEdit m_editYTran;
	CEdit m_editZTran;

	// ��ġ ���� ��ư 
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
	//! �ùķ��̼� ���� �Լ� �� ����
	//! �ܺΰ��� ���
	CEdit m_editCalcMulti;
	// ���������
	CEdit m_editCalcWaterInnerAbsorption;
	//! ���� ���� ������
	CEdit m_editCalcWaterChange;
	// ���̾ ���к���������
	CEdit m_editCalcLayerWaterAborption;
	//! �ʱ� ���ط�
	CEdit m_editInitPorosity;
	//! �Ի�ر� �Ӱ�ġ
	CEdit m_editGranularDisintegration;
	//! ���ط� ����
	CEdit m_editCalcPorosity;
	//! ��� ����
	CEdit m_editTopRate;
	//! ���� ����
	CEdit m_editSideRate;
	//! �ϴ� ����
	CEdit m_editBottomRate;

	//! ���� �ֱ�
	CEdit m_EditStopSimulCnt;
	//! ������ ������ ��Ʈ��
	CEdit m_editGrDisCnt;
	//! ������� ������ ��Ʈ��
	CEdit m_editGrTotalCnt;
	//! �ùķ��̼� Ƚ�� ��Ʈ��
	CEdit m_editCalcTryCnt;
	//! �� ���� �� 
	CEdit m_editTotalSolidCnt;
	//! �ùķ��̼� ��� ���� üũ �ڽ�
	CButton m_checkSaveStep;

	//! ���� ���� ��Ʈ��
	CEdit m_editSaveInterval;
	//! ���� ���� ����
	int m_nSaveInterval;

	//! �ܺ� ���� �ܸ� ��� ��ư Ŭ��
	afx_msg void OnBnClickedButtonCalcExternalSide();
	//! �ùķ��̼� ���� ��ư
	afx_msg void OnBnClickedButtonCalcRockAging();
	//! �ùķ��̼� ���� ��ư
	afx_msg void OnBnClickedButtonCalcRockAgingCancel();
	//! ����� ��ư
	afx_msg void OnBnClickedButtonCalcRockAging2();
	//! �ѹ� ���� ��ư
	afx_msg void OnBnClickedButtonCalcRockAging3();
	//! �ùķ��̼� ������ ����
	bool CalcRockAgingSetData();


	
	//! ���α׷� ���� ���� //////////////////////////////////////////////////////////////////////////
	CComboBox	m_comboDataProcess;
	int			m_nComboProcessSel; //0: �޸�ó��, 1: ����ó��
	afx_msg void OnCbnSelchangeComboDataProcess();
	//////////////////////////////////////////////////////////////////////////

	void ShowTraceTime(CString strMemo, int nGapCheck = 0);
	//! �ùķ��̼� Ŭ��
	bool m_bSimulIdx;
	int m_nSimulIdx;
	int m_nSimulObjectIdx;
	afx_msg void OnBnClickedButton1();

	//! �ڵ� �ùķ��̼�
	afx_msg LRESULT OnSimulationMsg(WPARAM wParam, LPARAM lParam);


	//! �ð��� �ʷ� �ٲ�..
	LONGLONG GetSecondFromTime(CTime tmCurrent);


	
	afx_msg void OnBnClickedButton3();
	CButton m_cudaCheck;
	afx_msg void OnBnClickedButton4();


	CGPUSolid		m_GPUSolid;
	CGPUSurface		m_GPUSurface;
	CGPUCalcRockAgingInner		m_GPUCalcRockAgingInner;

	//! ���� ó�� ���� //////////////////////////////////////////////////////////////////////////
	CComboBox	m_comboUseCuda;
	int			m_nComboLogicProcess; //0: CPU-CPU,  1:GPU-GPU

	CComboBox m_comboBlockThreadSel;
	int			m_nComboBlockThreadSel; //0: Blcok 3��,  1: Thread 3��


	//! �޸� new �ð��� ���ּ� ����ϱ�����..
	bool* m_bDataTmp[dfTOTAL_COUNT];

	//////////////////////////////////////////////////////////////////////////
	//! �ϼ� ���п� ���� �Լ� �� ����
	afx_msg void OnBnClickedButtonViewRateStone();
	
	//! ���ں���
	CString	m_strEditFeldsparPer;		//�弮
	CString	m_strEditQuartzPer;			//����
	CString	m_strEditPlagioclasePer;	//���弮
	CString	m_strEditBiotitePer;		//����
	CString	m_strEditTitanitePer;		//ƼŸ����Ʈ
	CString	m_strEditHornblendePer;		//������
	CString	m_strEditEpidotePer;		//��ż�
	CString	m_strEditChloritePer;		//��ϼ�
	CString	m_strEditPorosityPer;		//���غ���

	//! ���ں� �Ӱ�ġ
	CString	m_strEditFeldsparDis;
	CString	m_strEditQuartzDis;
	CString	m_strEditPlagioclaseDis;
	CString	m_strEditBiotiteDis;
	CString	m_strEditTitaniteDis;
	CString	m_strEditHornblendeDis;
	CString	m_strEditEpidoteDis;
	CString	m_strEditChloriteDis;

	//! ���� ����
	CString	m_strEditFeldsparDel;
	CString	m_strEditQuartzDel;
	CString	m_strEditPlagioclaseDel;
	CString	m_strEditBiotiteDel;
	CString	m_strEditTitaniteDel;
	CString	m_strEditHornblendeDel;
	CString	m_strEditEpidoteDel;
	CString	m_strEditChloriteDel;
	//! ���� ����
	CString	m_strEditPorosityCnt;


	void InitEditValue();

	int GetStoneType(ST_PARTICLE_POS* pstParticlePos = NULL);

	//! ���� ������ ī��Ʈ�Ѵ�.
	void SetStoneTypeCntView();

	CProbDistribution* m_pPDGranularDisintegration[dfSTONE_TYPE_CNT];
	//! �Ի�ر� ���ް� ���� �ʱ�ȭ
	void SetProbDistributionValue();
	
	//! ���ں� ���� ī��Ʈ �ʱ�ȭ
	void InitStoneCnt();

	//! ���ں� ���� Ÿ�� �ʱ�ȭ
	void InitStonRate(bool bClear = true);
	map<int,int>	m_mapInitStoneTypeCnt;

	//! ���ں� ���� ī��Ʈ(�ʱ�ȭ����)
	map<int,int>	m_mapStoneTypeCnt;
	//! ���̾�� ���� ����
	map<int,int>	m_mapStoneLayerCnt;



	void CreaetSoildVoxelData();
	

	set<CString> CalcDeleteParticleInside(CString strPos);
	void CalcDeleteParticleInsideDetail(CString strPos, set<CString>		&setDelDataParticleInside);

	//! �������� ������Ʈ
	void UpdateMainView(bool bComplete = false);
	
	//! ����ȭ ȭ�� ����
	afx_msg void OnBnClickedButtonSee();
	//! ����ȭ ������ �ʱ�ȭ ���� - 0:��, 1:��, 2:��, 3:��, 4:��, 5:��
	void InitShowData(int nShow);
	void UpdateShowData(int nShow);

	int SameStartIdxFind(int nStartIdx, ST_VOXEL_3D_POS_VIEWER	&stVoxel3dPosViewer, map<ST_VOXEL_3D_POS_VIEWER, ST_SHOW_DATA> &mapShowData, vector<ST_VOXEL_3D_POS_VIEWER>&	vecVoxel3DPosViewer, set<ST_VOXEL_3D_POS_VIEWER>		&setVoxel3dPosViewer);
	afx_msg void OnCbnSelchangeComboUseBlockThread();
};





