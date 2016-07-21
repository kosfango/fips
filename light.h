// light.h : main header file for the LIGHT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLightApp:
// See light.cpp for the implementation of this class
//

class CLightApp : public CWinApp
{
public:

	CLightApp();
	void 	StartEventWorkerThread	(void); 
	void	StartIncomeWorkerThread	(void); 
	void	StartUDPWorkerThread		(void);
	void	OnPurgeOne							(int ind);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
//	virtual BOOL OnCmdMsg		(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLightApp)
	afx_msg void OnScale10();
	afx_msg void OnScale20();
	afx_msg void OnScale30();
	afx_msg void OnScale40();
	afx_msg void OnScale50();
	afx_msg void OnScale60();
	afx_msg void OnScale70();
	afx_msg void OnScale80();
	afx_msg void OnScale90();
	afx_msg void OnScale100();
	afx_msg void OnCfgStation();
	afx_msg void OnCfgBosses();
	afx_msg void OnMenuEditor();
	afx_msg void OnMenuAreas();
	afx_msg void OnMenuPacker();
	afx_msg void OnStartmailer();
	afx_msg void OnStarttosser();
	afx_msg void OnStartpurger();
	afx_msg void OnStartnlcompile();
	afx_msg void OnSetupSystemsounds();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectall();
	afx_msg void OnEditUndo();
	afx_msg void OnMailCopy();
	afx_msg void OnSetupNewmail();
	afx_msg void OnMenuModem();
	afx_msg void OnFilerequest();
	afx_msg void OnMenuNodelist();
	afx_msg void OnReadercolor();
	afx_msg void OnSetupScripteditor();
	afx_msg void OnFileExit();
	afx_msg void OnActionChangecurrentmail();
	afx_msg void OnSetupDialingtimeouts();
	afx_msg void OnSetupCarboncopystrings();
	afx_msg void OnActionEcholistmanagement();
	afx_msg void OnInfoAreatrafficinfo();
	afx_msg void OnHelpMe();
	afx_msg void OnFIDONET();
	afx_msg void OnFIDOLANG();
	afx_msg void OnDICTIONARY();
	afx_msg void OnConfigOtherconfigs();
	afx_msg void OnHelpAbout();
	afx_msg void OnInfoAddressinfo();
	afx_msg void OnForward();
	afx_msg void OnPrintMail();
	afx_msg void OnSaveSelToFile();
	afx_msg void OnSaveMailToFile();
	afx_msg void OnInfoViewlogfiles();
	afx_msg void OnSetupLogfilesettings();
	afx_msg void OnSetupAdditionalakas();
	afx_msg void OnFilterDisplay();
	afx_msg void OnMarkAllMailsAsRead();
	afx_msg void OnSetSubjectFont();
	afx_msg void OnSetMailtextFontANSI();
	afx_msg void OnSetMailtextFontOEM();
	afx_msg void OnActionAbortscript();
	afx_msg void OnNeverDelete();
	afx_msg void OnMailsAnswerasnetmail();
	afx_msg void OnRescanmail();
	afx_msg void OnConvertSpecialChars();
	afx_msg void OnCrosspost();
	afx_msg void OnViewOutbound();
	afx_msg void OnHelp();
	afx_msg void OnSetupTrashcanfilter();
	afx_msg void OnAutosave();
	afx_msg void OnQuoteToArea();
	afx_msg void OnQuote();
	afx_msg void OnSwitchLeftIcons();
	afx_msg void OnSwitchMailer();
	afx_msg void OnSwitchTosser();
	afx_msg void OnSwitchPurger();
	afx_msg void OnSwitchNodelist();
	afx_msg void OnSwitchExtSearch();
	afx_msg void OnSimpleSearch();
	afx_msg void OnActionsStartscript1();
	afx_msg void OnActionsStartscript2();
	afx_msg void OnActionsStartscript3();
	afx_msg void OnActionsStartscript4();
	afx_msg void OnActionsStartscript5();
	afx_msg void OnFreqFromMail();
	afx_msg void OnMailCreateNewMail();
	afx_msg void OnDeleteAll();
	afx_msg void OnActionsUudecodemail();
	afx_msg void OnConfigEvents();
	afx_msg void OnMenuModem2();
	afx_msg void OnSetupEditortypereplacment();
	afx_msg void OnSetupTextimport();
	afx_msg void OnActionsAbortcurrentevent();
	afx_msg void OnActionsExecuteanyscript();
	afx_msg void OnMulAsPrint();
	afx_msg void OnMulAsRead();
	afx_msg void OnSaveMultipleToFile();
	afx_msg void OnMulAutosave();
	afx_msg void OnMulDelete();
	afx_msg void OnMulNeverDelete();
	afx_msg void OnToggleRot13();
	afx_msg void OnDupeCheck();
	afx_msg void OnDupeCheckAll();
	afx_msg void OnConfigTimesAndDevices();
	afx_msg void OnIncommingcallsConfigNrofrings();
	afx_msg void OnMultimediaDisplayoptions();
	afx_msg void OnMultimediaAccessibleelements();
	afx_msg void OnCaptureRecipientAddr();
	afx_msg void OnCaptureSenderAddr();
	afx_msg void OnSubjListAdjustment();
	afx_msg void OnSetupRequestPathes();
	afx_msg void OnSetupMagics();
	afx_msg void OnIncomingcallsRemotemonitor();
	afx_msg void OnMarkMailFreeze();
	afx_msg void OnSetupTicechosetup();
	afx_msg void OnHatchFile();
	afx_msg void OnBeginMultiSelection();
	afx_msg void OnEndMultiSelection();
	afx_msg void OnUUFromSelected();
	afx_msg void OnStartWWWBrowser();
	afx_msg void OnUnmarkAllMessages();
	afx_msg void OnInfoCostcalculation();
	afx_msg void OnInfoCostanalyze();
	afx_msg void OnFindOriginalCopy();
	afx_msg void OnCallsHistory();
	afx_msg void OnFaxSetup();
	afx_msg void OnLocallogin();
	afx_msg void OnChooseFontEditor();
	afx_msg void OnFontMainLabels();
	afx_msg void OnFontMainFields();
	afx_msg void OnAutosaveToArea();
	afx_msg void OnPollSystem();
	afx_msg void OnSaveMarkedToSpecificArea();
	afx_msg void OnBbsusergroupsetup();
	afx_msg void OnBbsmainconfig();
	afx_msg void OnBbsuserconfig();
	afx_msg void OnBbsareaconfig();
	afx_msg void OnOrderEchoes();
	afx_msg void OnAllareasread();
	afx_msg void OnSortRecieve();
	afx_msg void OnSortCreate();
	afx_msg void OnSortFrom();
	afx_msg void OnSortTo();
	afx_msg void OnSortSubject();
	afx_msg void OnSortStatus();
	afx_msg void OnSortOrder();
	afx_msg void OnHsfloating();
	afx_msg void OnResetdialogboxpos();
	afx_msg void OnCfgFloating();
	afx_msg void OnSetupAdressbook();
	afx_msg void OnIncomingModem1();
	afx_msg void OnIncomingModem2();
	afx_msg void OnMarkPresentAgain();
	afx_msg void OnInfoPresentagain();
	afx_msg void OnDeleteMail();
	afx_msg void OnCfgKludges();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////


