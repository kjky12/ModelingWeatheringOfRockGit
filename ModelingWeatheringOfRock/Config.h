//Config.h

#ifndef _CONFIG_H_
#define _CONFIG_H_


//File Path-----------------------------------------------------------------------------------------------------------------------
#define dfINI_POSITION_FILE_NAME													L"Config\\ConfigCNMsPos.ini"
#define dfINI_TEXT_FILE_NAME														L"Config\\ConfigText.ini"
#define dfINI_CONFIG_SETUP_NAME														L"Config\\ConfigSetup.ini"
#define dfFILE_PATH_SKINS															L"Image\\"






//RGB ------------------------------------------------------------------------------------------------------------------------
#define dfCOLOR_DIALOG_TITLE														RGB(255, 255, 255)	//다이얼로그 타이틀 텍스트 색
#define dfCOLOR_BACK_DIALOG															RGB(238, 238, 238)	//다이얼로그 배경 색
#define dfCOLOR_BACK_POPUP_DIALOG													RGB(244, 248, 252)	//팝업다이얼로그 배경색

#define dfCOLOR_BACK_SUB_DIALOG														RGB(198, 212, 221)	//서브다이얼로그

#define dfCOLOR_MENU																RGB(0, 0, 70)		//메뉴 텍스트 색
#define dfCOLOR_TOOLBAR																RGB(255, 255, 255)	//툴바텍스트 색

#define dfCOLOR_TAB_ENABLE															RGB(0, 0, 70)		//활성 탭 버튼 텍스트 색
#define dfCOLOR_TAB_DISABLE															RGB(120, 120, 120)	//비활성 탭 버튼 텍스트 색

#define dfCOLOR_SMALL_DLG_HEADER													RGB(98,146,193)		//SMALL 다이얼로그 타이틀 배경색
#define dfCOLOR_SMALL_DLG_TITLE														RGB(154,199,225)	//SMALL 다이얼로그 타이틀 텍스트 색

#define dfCOLOR_TEXT_BOX_FONT														RGB(0, 0, 0)

#define dfCOLOR_CHECKBOX_BACK														RGB(238, 238, 238)	//체크박슥 배경색
#define dfCOLOR_CHECKBOX_TEXT														RGB(0, 0, 70)		//체크박스 텍스트 색

#define dfCOLOR_TEXT																RGB(0, 0, 70)		//텍스트 색
#define dfCOLOR_GROUPBOX_TEXT														RGB(0, 0, 0)		//텍스트 색

#define dfCOLOR_GROUPBOX_TITLE_BACK													RGB(170, 192, 210)	//텍스트 배경색
#define dfCOLOR_GROUPBOX_TITLE_LINE													RGB(142, 172, 194)	//텍스트 테두리

#define dfCOLOR_GROUPBOX_BACK														RGB(202,213,222)	//Group Box 배경색
#define dfCOLOR_GROUPBOX_LINE														RGB(171,188,200)	//Group Box 배경색

#define dfCOLOR_TEXT_BACK															RGB(220, 228, 234)
#define dfCOLOR_TEXT_LINE															RGB(189, 201, 210)	//텍스트 테두리 라인 색


#define dfCOLOR_REDIT_DISABLE														RGB(202,213,222)
#define dfCOLOR_REDIT_ENABLE														RGB(255,255,255)

#define dfFREQ_TRAN_STATUS_UNKNOWN_COLOR											RGB(255,255,255)
#define dfFREQ_TRAN_STATUS_COLOR_OK													RGB(0,255,0)
#define dfFREQ_TRAN_STATUS_COLOR_NG													RGB(255,0,0)

#define dfCOLOR_MAIN_TAB_ENABLE														RGB(36, 112, 193)	//Main Tab Btn
#define dfCOLOR_MAIN_TAB_DISABLE													RGB(135, 135, 135)	//Main Tab Btn
//-----------------------------------------------------------------------------------------------------------------------




//Menu-----------------------------------------------------------------------------------------------------------------------
#define dfMENU_ITEM_TEXT_RECT														CRect(10, 0, 135, 26)
#define dfMENU_ITEM_TEXT_ENABLE_COLOR												RGB(0, 0, 0)
#define dfMENU_ITEM_TEXT_DISABLE_COLOR												RGB (220, 220, 220)
//-----------------------------------------------------------------------------------------------------------------------




//Size ---------------------------------------------------------------------------------------------------------------------
#define dfSIZE_TEXT_BOX_MAX															15
#define dfSIZE_TEXT_BOX_MID															5

#define dfTAB_BTN_HEIGHT															27		//탭버튼 높이
#define dfTAB_BTN_WIDTH																95		//탭버튼 폭
#define dfTAB_DLG_HEADER_HEIGHT														39		//다이얼로그 헤더 높이

#define dfPOPUP_DLG_BOTTOM_HEIGHT													42
#define dfTEXT_GROUPBOX_ROUND														5
#define dfTEXT_ROUND																0
#define dfLIST_SHOW_CHECKBOX_SIZE													10
#define dfLIST_ITEM_CNT																30

#define dfWIDTH_BTN_COMMON															82
#define dfHEIGHT_BTN_COMMON															27

//Rect
#define dfDIALOG_SUB_TITLE_RECT														CRect(11,11,150,25)
#define dfDIALOG_TITLE_RECT															CRect(38,6,187,25)
#define dfDIALOG_TITLE_RECT_FIRE_COMMOND_POP										CRect(30,6,187,25)
#define dfDIALOG_TITLE_RECT_SMALL													CRect(38,6,127,25)
#define dfDLG_POS_LEFT																13
#define dfDLG_POS_RIGHT																1579
#define dfDLG_POS_UP																208
#define dfDLG_POS_DOWN																1113
#define dfDLG_HEIGHT																39
#define dfDLG_TAB_BTN_HEIGHT														27








#endif