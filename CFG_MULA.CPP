// cfg_mula.cpp : implementation file
// IDD_CFG_AREA_MULTIPLE

#include "stdafx.h"
#include "light.h"
#include "cfg_mula.h"
#include "structs.h"
#include "mystrlst.h"
#include "cfg_area.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern cfg_area *gcfg_area;
static char DlgName[]="IDD_CFG_AREA_MULTIPLE";

// ================================================================
	cfg_mula::cfg_mula(CWnd* pParent ) 	: CDialog(cfg_mula::IDD, pParent)
// ================================================================
{
	//{{AFX_DATA_INIT(cfg_mula)
	//}}AFX_DATA_INIT
}

// ================================================================
	void cfg_mula::DoDataExchange(CDataExchange* pDX)
// ================================================================
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_mula)
	DDX_Control(pDX, IDC_U_UPLINK, m_u_uplink);
	DDX_Control(pDX, IDC_U_ENABLE_NR_OF_MAILS, m_u_enable_nr_of_mails);
	DDX_Control(pDX, IDC_U_EDIT_NR_OF_MAILS, m_u_edit_nr_of_mails);
	DDX_Control(pDX, IDC_U_EDIT_AGE_OF_RECIPTION, m_u_edit_age_of_reciption);
	DDX_Control(pDX, IDC_U_EDIT_AGE_OF_CREATION, m_u_edit_age_of_creation);
	DDX_Control(pDX, IDC_U_EDIT_ADDITIONAL_DAYS, m_u_edit_additional_days);
	DDX_Control(pDX, IDC_U_AKA, m_u_aka);
	DDX_Control(pDX, IDC_U_AGE_OF_REACIPTION, m_u_age_of_reciption);
	DDX_Control(pDX, IDC_U_AGE_OF_CREATION, m_u_age_of_creation);
	DDX_Control(pDX, IDC_COMBO1, m_uplink);
	DDX_Control(pDX, IDC_NRMAILS, m_nrmails);
	DDX_Control(pDX, IDC_AKA, m_aka);
	DDX_Control(pDX, IDC_AGE_RECIPT, m_age_recipt);
	DDX_Control(pDX, IDC_AGE_CREATE, m_age_create);
	DDX_Control(pDX, IDC_EDIT_MAXNR, m_edit_maxnr);
	DDX_Control(pDX, IDC_EDIT_AGE_RECIPT, m_edit_age_recipt);
	DDX_Control(pDX, IDC_EDIT_AGE_CREATE, m_edit_age_create);
	DDX_Control(pDX, IDC_EDIT_ADDITIONAL_DAYS, m_edit_additional_days);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cfg_mula, CDialog)
	//{{AFX_MSG_MAP(cfg_mula)
	ON_BN_CLICKED(IDC_U_AGE_OF_CREATION, OnUAgeOfCreation)
	ON_BN_CLICKED(IDC_U_AGE_OF_REACIPTION, OnUAgeOfReaciption)
	ON_BN_CLICKED(IDC_U_AKA, OnUAka)
	ON_BN_CLICKED(IDC_U_EDIT_ADDITIONAL_DAYS, OnUEditAdditionalDays)
	ON_BN_CLICKED(IDC_U_EDIT_AGE_OF_CREATION, OnUEditAgeOfCreation)
	ON_BN_CLICKED(IDC_U_EDIT_AGE_OF_RECIPTION, OnUEditAgeOfReciption)
	ON_BN_CLICKED(IDC_U_EDIT_NR_OF_MAILS, OnUEditNrOfMails)
	ON_BN_CLICKED(IDC_U_ENABLE_NR_OF_MAILS, OnUEnableNrOfMails)
	ON_BN_CLICKED(IDC_U_UPLINK, OnUUplink)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ================================================================
	BOOL cfg_mula::OnInitDialog()
// ================================================================
{
CStrList  allgroups;
CString group;
CString	help;
char	buf[MAX_BOSSLEN];
int     i,n,found;
int		lang_patches[]={
						IDOK,
						IDCANCEL,
						IDC_NRMAILS,
						IDC_AGE_CREATE,
						IDC_AGE_RECIPT,
						IDC_STATIC3,
						IDC_STATIC5,
						IDC_STATIC66,
						IDC_STATIC77,
						IDC_STATIC100,
						IDHELP
					};

	CDialog::OnInitDialog();
    set_dlg_language(this,DlgName,lang_patches,sizeof lang_patches/sizeof(int));

// at first disable all Windows
	m_nrmails.EnableWindow(0);
	m_aka.EnableWindow(0);
	m_age_recipt.EnableWindow(0);
	m_age_create.EnableWindow(0);
	m_edit_maxnr.EnableWindow(0);
	m_edit_age_recipt.EnableWindow(0);
	m_edit_age_create.EnableWindow(0);
	m_edit_additional_days.EnableWindow(0);
	m_uplink.EnableWindow(0);

	((CComboBox *)GetDlgItem(IDC_COMBO1))->ResetContent();

	i=0;
	while (db_get_boss_by_index(i++,buf))
	{
		get_token(buf,BOSSADDR,help);
		((CComboBox *)GetDlgItem(IDC_COMBO1))->AddString(help);
	}
	((CComboBox *)GetDlgItem(IDC_COMBO1))->SetCurSel(0);

	allgroups.RemoveAll();
	for (i=0;i<(gcfg_area->m_listctl).GetItemCount();i++)
	{
		group=(gcfg_area->m_listctl).GetItemText(i,4);
		found=0;
		for (n=0;n<allgroups.GetCount();n++)
		{
		   if (!stricmp(group,allgroups.GetString(n)))
		   {
		      found=1;
			  break;
		   }
		}
		if (!found)
		   allgroups.AddTail(group);
	}
	((CComboBox *)GetDlgItem(IDC_GROUPNAME))->ResetContent();
	for (n=0;n<allgroups.GetCount();n++)
		((CComboBox *)GetDlgItem(IDC_GROUPNAME))->AddString(allgroups.GetString(n));

	((CComboBox *)GetDlgItem(IDC_GROUPNAME))->AddString("ALL");
	((CComboBox *)GetDlgItem(IDC_GROUPNAME))->SetCurSel(0);

	return TRUE;
}

// ================================================================
	void cfg_mula::OnCancel()
// ================================================================
{
	CDialog::OnCancel();
}

// ================================================================
	void cfg_mula::OnOK()
// ================================================================
{
CString uplink;
CString group;
int     sel;
CStrList yytmp;
CString  help;
CString  text;
int      i;
struct   areadef pa;

	// So, hier geht's zu Sache ...
	sel=((CComboBox *)GetDlgItem(IDC_GROUPNAME))->GetCurSel();
	((CComboBox *)GetDlgItem(IDC_GROUPNAME))->GetLBText(sel,group);

	sel=((CComboBox *)GetDlgItem(IDC_COMBO1))->GetCurSel();
	((CComboBox *)GetDlgItem(IDC_COMBO1))->GetLBText(sel,uplink);

	// So, nun rennen wir durch die gesamte AreaListe
	yytmp.RemoveAll();
	for (i=0;i<(gcfg_area->m_listctl).GetItemCount();i++)
	{
		 GetRow(gcfg_area->m_listctl,i,help);
		 memset(&pa,0,sizeof (struct areadef));
		 gcfg_area->convert(PS help,&pa);

	     // Wenn die Gruppe passt oder halt alle
		 if (!stricmp(group,pa.group) || !stricmp(group,"ALL"))
		 {
			 // BOOL NrOfMail
			 if (m_u_enable_nr_of_mails.GetCheck())
				  pa.purge_flag_nrmails=m_nrmails.GetCheck();

			 // BOOL Create
	         if (m_u_age_of_creation.GetCheck())
				  pa.purge_flag_create=m_age_create.GetCheck();

			 // BOOL Recipt
	         if (m_u_age_of_reciption.GetCheck())
				  pa.purge_flag_age_recipt=m_age_recipt.GetCheck();

			 // Ne neue AKA
			 if (m_u_aka.GetCheck())
			    m_aka.GetWindowText(pa.aka,20);

			 // EDIT Additional days
			 if (m_u_edit_additional_days.GetCheck())
			 {
				m_edit_additional_days.GetWindowText(text);
				if (text=="?") text="10";
				sscanf(PS text,"%d",&pa.additional_days);
			 }

			 // EDIT AGE of creation
			 if (m_u_edit_age_of_creation.GetCheck())
			 {
				m_edit_age_create.GetWindowText(text);
				if (text=="?") text="100";
				sscanf(PS text,"%d",&pa.purge_age_create);
			 }

			 // EDIT AGE of recipt
			 if (m_u_edit_age_of_reciption.GetCheck())
			 {
				m_edit_age_recipt.GetWindowText(text);
				if (text=="?") text="100";
				sscanf(PS text,"%d",&pa.purge_age_recipt);
			 }

			 // EDIT Nr of Mail
			 if (m_u_edit_nr_of_mails.GetCheck())
			 {
				m_edit_maxnr.GetWindowText(text);
				if (text=="?") text="1000";
				sscanf(PS text,"%d",&pa.purge_max_num);
			 }

			 // Ein neuer Uplink
			 if (m_u_uplink.GetCheck())
				strcpy(pa.uplink,PS uplink);

			 // Nun konvertieren wir das Teilwieder zurueck in einen CString
			 // BUGBUG: Aber in einen Buffer, nicht direkt in eine CString ...
			 help.Format("%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t\t\t\t\t\t",
				 pa.echotag,
				 pa.description,
				 pa.aka,
				 pa.uplink,
				 pa.group,
				 pa.purge_age_create,
				 pa.purge_age_recipt,
				 pa.purge_max_num,
				 pa.purge_flag_create,
				 pa.purge_flag_age_recipt,
				 pa.purge_flag_nrmails,
				 pa.localmail,
				 pa.additional_days
				 );
		 }
		 yytmp.AddTail(help);
	}

	gcfg_area->m_listctl.DeleteAllItems();
	for (i=0;i<yytmp.GetCount();i++)
		AddRow(gcfg_area->m_listctl,yytmp.GetString(i));

	CDialog::OnOK();
}

// ================================================================
	void cfg_mula::OnUAgeOfCreation()
// ================================================================
{
	m_age_create.EnableWindow(m_u_age_of_creation.GetCheck());
}

// ================================================================
	void cfg_mula::OnUAgeOfReaciption()
// ================================================================
{
	m_age_recipt.EnableWindow(m_u_age_of_reciption.GetCheck());
}

// ================================================================
	void cfg_mula::OnUAka()
// ================================================================
{
	m_aka.EnableWindow(m_u_aka.GetCheck());
}

// ================================================================
	void cfg_mula::OnUEditAdditionalDays()
// ================================================================
{
	m_edit_additional_days.EnableWindow(m_u_edit_additional_days.GetCheck());
}

// ================================================================
	void cfg_mula::OnUEditAgeOfCreation()
// ================================================================
{
	m_edit_age_create.EnableWindow(m_u_edit_age_of_creation.GetCheck());
}

// ================================================================
	void cfg_mula::OnUEditAgeOfReciption()
// ================================================================
{
	m_edit_age_recipt.EnableWindow(m_u_edit_age_of_reciption.GetCheck());
}

// ================================================================
	void cfg_mula::OnUEditNrOfMails()
// ================================================================
{
	m_edit_maxnr.EnableWindow(m_u_edit_nr_of_mails.GetCheck());
}

// ================================================================
	void cfg_mula::OnUEnableNrOfMails()
// ================================================================
{
	m_nrmails.EnableWindow(m_u_enable_nr_of_mails.GetCheck());
}

// ================================================================
	void cfg_mula::OnUUplink()
// ================================================================
{
	m_uplink.EnableWindow(m_u_uplink.GetCheck());
}

// ================================================================
	void cfg_mula::OnHelp()
// ================================================================
{
	WinHelp(VHELP_CFG_AREA_MULTIPLE);
}

BOOL cfg_mula::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
