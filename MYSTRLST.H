#ifndef MYSTRLSTINCLUDED
  #define MYSTRLSTINCLUDED 1

class CStrList : public CStringList
{

// Construction
public:
    CStrList();
    ~CStrList();
	void operator=				(CListBox &lst);
	void operator=				(CListCtrl &lst);
	LPCSTR GetString			(int i);
	int  GetString				(int i,CString &text);
	int  SaveToFile  			(LPCSTR fname,int conv=0);
	int  LoadFromFile			(LPCSTR fname,int conv=0);
	void SaveToDelimText		(LPCSTR fname,int conv,char delim);
	void SaveToDelimText		(LPCSTR fname,int conv);
	int  LoadFromDelimText		(LPCSTR fname,int conv,char delim);
    int	 LoadFromDelimText		(LPCSTR fname,int conv);
	void SaveToDelimString		(CString &str,char delim=',');
    void LoadFromDelimString	(LPCSTR str,char delim=',',BOOL trim=FALSE);
	void UpdateListBox			(CListBox *lb);
	void UpdateListBox			(CListCtrl &lb);
	void Insert					(int i,LPCSTR p);	// Insert after
	void InsertB				(int i,LPCSTR p);	// Insert before
	void Replace				(int i,LPCSTR p);
	void Remove					(int i);
	int  FillWithDirs			(LPCSTR basedir);
	int  FillWithFiles			(LPCSTR basedir);
	int  FillAsEdit				(LPCSTR filename);
	int  SaveAsEdit				(LPCSTR filename);
	void DeleteEmptySubDirs		(LPCSTR basedir);
	int  SplitWords				(LPCSTR text);
	int  SplitTabs				(LPSTR text,char splitchar='\t');
    void BuildAsTabbedString    (CStrList &stringslist,char splitchar='\t');
	LPSTR GetFromTable	   	    (int strnbr,int tabnr,char splitchar='\t');
	void SplitPath				(LPSTR tosplit);
	int  convert_to_newer_format(LPSTR filename);
	int  set_date_check			(int *pinstall_date,int * pinstall_checksum);
	void AddMenu				(CString en,CString ge,CString ru,CString fr,int resid,int flags=MF_STRING | MF_ENABLED);
	void DoMenu					(POINT pt,CWnd *wndptr);
	void Sort					(int sortindex=0,int sort_numeric=0);
	int	 BuildFromMemoryString  (LPSTR text,LPSTR split="\r\n");
	int	 AppendToString			(int i,LPSTR apptext,LPSTR appafter=NULL);
	int  EnumRegistryKeys		(HKEY whichtree,LPSTR treepath);
	int  EnumStringKeyValues	(HKEY whichtree,LPSTR treepath);
	int	 FindString				(LPCSTR tofind,CString &result,int ind=0,int ign=1);
	int  AppendFileInfo			(LPSTR path);
	int  HandleAsciiExport		(LPCSTR fname,int fromload=0);
	int  defaultindex;
	int  defaultindexnew;
	int  install_date;				 // Das Installationsdatum ...
	int  install_checksum;			 // Die Checksum ...
	int  reserved3;
	int  reserved4;
	int  reserved5;
};

#endif


