#include "stdafx.h"
#include <io.h>
#include "light.h"
#include "lightdlg.h"
#include "search.h"
#include "detpurg.h"
#include "detmail.h"
#include "dettoss.h"
#include "det_nl.h"
#include "writmail.h"
#include "filereq.h"
#include "reqsel.h"
#include "cfg_dir.h"
#include "exsearch.h"
#include "floating.h"
#include "MathParser\MathParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CLightApp	FipsApp;
extern detpurg		gPurger;
extern detmail 		gMailer;
extern dettoss 		gTosser;
extern det_nl 		gNLComp;
extern exsearch 	gSearch;
extern floating 	gFloating;
extern _gconfig		gc;
extern CWinThread	*script_thread;
extern int			gSortCriteria;

CString actual_executed_script;
DWORD	xdummy;
int FlagGoto;
CMathParser mp;

int	was_esc_pressed(void);
void move_window(CWnd *pw,int x,int y);
BOOL proccess_param(LPSTR line,LPSTR parm,LPSTR pResult,int *iResult,int ind=1);
char Error[]="ERROR";
char Ok[]="OK";
char Result[]="RESULT";

// ================================================================
// Удаляет переменную
	int DelVarScript(char *Name)
// ================================================================
{ 
	mp.Parameters.DelString(Name);
	return 1;
}

 // ================================================================
// Добавление стринговой переменной
 int AddVarStr(char *Name,char *Var)
// ================================================================
{
int ret;
int *p=&ret;

	DelVarScript(Name); 
	if (ret=mp.Parameters.IndexOf(Name,(void **)&p)>=0)
		mp.Parameters.Replace(Name,Var );
	else 
		mp.Parameters.AddString(Name,Var );

	return ret;
}

// ================================================================
// Значение переменной в виде строки
// Возвращает 1 - строка
	int FindVarStr(char *Name,char *Var)
// ================================================================
{
int ret;
int *p=&ret;

	ret=mp.Parameters.IndexOf(Name,(void **)&p);
	if (ret<0) return 0;
	strcpy(Var,(char*) p); 
	return 1;
}

// ================================================================
	int ParseParamAll(char *str,char *res)
// ================================================================
{
char *p1,*p2;
int	n=0;

	p1=strchr(str,'(');
	p2=strrchr(str,')');

	if (p1 && p2) 
	{
		n=p2-p1-1;
		if (n) 
		{
			strncpy(res,p1+1,n);
			res[n]=0;
		} 
		else 
			strcpy(res,"");
	}
	return n;	
}

// ================================================================
	int ParseString(char *str)
// ================================================================
{
char lbuf[300];
char *p;
int  n=0;

	if (p=strchr(str,'"'))
	{
		strcpy(lbuf,p+1);
		if (p=strchr(lbuf,'"'))
		{
			n=p-lbuf;
			lbuf[n]=0;
			strcpy(str,lbuf);
			return 1;
		}
	}
	return 0;	
}

// ================================================================
	int PoiskMetok(CStrList *pscname,char *str)
// ================================================================
{
char lbuf[300];
char param[300];
int  len,n=0;

	len=pscname->GetCount();
	for (int i=0;i<len;i++) 
	{
		strcpy(lbuf,pscname->GetString(i));
		sscanf(lbuf,"%s",param);
		if (!stricmp(param,str)) 
		{
			n=i;
			break;
		}
	}
	return n;	
}

// ================================================================
	int ParseParam(int Sel,char *str,char *res, char Delim=',')
// ================================================================
{
UINT i;
int len,Kav=0,n=0;
char lbuf[1024],*p;

	strcpy(lbuf,str);   
	p=lbuf-1;
	for (i=0;i<strlen(lbuf);i++)
	{
//Пропуск парных кавычек при подсчете параметров
		if (Kav && lbuf[i] != '"')	continue;
		if (lbuf[i] == '"')	{ Kav=!Kav; continue; }
		if (lbuf[i] == Delim)
		{
			n++;
			if (Sel==1) 
			{
				strncpy(res,lbuf,i);
				res[i]=0;
				trim_all(res);
				return 1;
			}
			if (Sel-1 == n)	p=lbuf+i;
		}
	
		if (n==Sel)
		{
			len=lbuf+i-p-1;
			strncpy(res,p+1,len);
			res[len]=0;
			trim_all(res);
			return 1;
		}
	}	
	
	//Всего один параметр
	if (!n && Sel==1) 
	{
		strcpy(res,lbuf);
		trim_all(res);
		return 1;
	}
        //Последний параметр
	n++;
	if (n==Sel) 
	{
		strcpy(res,p+1);
		trim_all(res);
		return 1;
	}
	return 0;	
}

// ================================================================
	void ParseFileName(char *fname)
// ================================================================
{
char lbuf[MAX_PATH],*p;
int n;
	
	while (p=strstr(fname,"%INBOUND%"))
    {
		n=p-fname;
		memmove(lbuf,fname,n);
		lbuf[n]=0;
		strcat(lbuf,gc.InboundPath);
		strcat(lbuf,p+9);
		strcpy(fname,lbuf);
	}
	while (p=strstr(fname,"%OUTBOUND%"))
    {
		n=p-fname;
		memmove(lbuf,fname,n);
		lbuf[n]=0;
		strcat(lbuf,gc.OutboundPath);
		strcat(lbuf,p+10);
		strcpy(fname,lbuf);
	}
	while (p=strstr(fname,"%BASEDIR%"))
    {
		n=p-fname;
		memmove(lbuf,fname,n);
		lbuf[n]=0;
		strcat(lbuf,gc.BasePath);
		strcat(lbuf,p+9);
		strcpy(fname,lbuf);
	}
	while (p=strstr(fname,"%IMPORT%"))
    {
		n=p-fname;
		memmove(lbuf,fname,n);
		lbuf[n]=0;
		strcat(lbuf,gc.BasePath);
		strcat(lbuf,"\\IMPORT");
		strcat(lbuf,p+8);
		strcpy(fname,lbuf);
	}
	while (p=strstr(fname,"%NODELIST%"))
    {
		n=p-fname;
		memmove(lbuf,fname,n);
		lbuf[n]=0;
		strcat(lbuf,gc.NodelistPath);
		strcat(lbuf,p+10);
		strcpy(fname,lbuf);
	}
	while (p=strstr(fname,"%MULTIMEDIA%"))
    {
		n=p-fname;
		memmove(lbuf,fname,n);
		lbuf[n]=0;
		strcat(lbuf,gc.MultimedPath);
		strcat(lbuf,p+12);
		strcpy(fname,lbuf);
	}
	while (p=strstr(fname,"%UTIL%"))
    {
		n=p-fname;
		memmove(lbuf,fname,n);
		lbuf[n]=0;
		strcat(lbuf,gc.UtilPath);
		strcat(lbuf,p+6);
		strcpy(fname,lbuf);
	}
}

 // ================================================================
// Вычисляет выражение
 // Возвращает 1-разбор, 0 - ошибка
	int CLightDlg::eval(CStrList *pscname,int *SelLine,LPSTR str,LPSTR Name)
// ================================================================
{
int i,ret,iResult;
char EvalStr[1024],lbuf[1024];
double res,*tresult;

        //Проверка на команду-функцию
	//Возвращает 1- число 2 - строка
	ret=execute_a_single_line(str,&iResult,(char*) &EvalStr,pscname,SelLine);
    DelVarScript(Result);

    if (ret==1)  
		{//Проверяем сушествует ли уже эта переменная
		res=iResult;
		if (mp.Parameters.IndexOf(Name,(void **)&tresult)>=0)
		mp.Parameters.Replace(Name,&res );
		else
		mp.Parameters.AddString(Name,&res );
		return 1; };

		if (ret==2) 
		{ AddVarStr(Name,(char *)&EvalStr);  return 1;}

	// По умолчанию результат числовой
       // Делаем разбор матем. парсером
	
	char *ErrMsg = mp.Parse( str, &res );
	if ( ErrMsg == NULL )
	{//Проверяем сушествует ли уже эта переменная
     if (mp.Parameters.IndexOf(Name,(void **)&tresult)>=0)
     mp.Parameters.Replace(Name,&res );
	 else
	 mp.Parameters.AddString(Name,&res );
	 return 1; };

	// Значит результат должен быть текстовым
        // Разбираем результат сами

	strcpy(EvalStr,"");
	i=1;
	for (;;i++)
	{
	if (!ParseParam(i,str,lbuf, '+')) break;
	//Проверка на стринговую переменную
	if (FindVarStr(lbuf,lbuf))
	   {  strcat(EvalStr,lbuf); continue;  }
	//Проверка на стринговую константу
	if (!ParseString(lbuf)) return 0;
	strcat(EvalStr,lbuf);
	}
	
	AddVarStr(Name,(char *)&EvalStr);
	  
   return 1;
}

// ====================================================
// Функция по обработке цикла while
	int CLightDlg::dowhile(CStrList *pscname,int *SelLine)
// ====================================================
{
double *pres;
char lbuf[300],dummy[100],line[300],pResult[1024];
char len,status,docommand;
int  i,ret,iResult;

	status=1;
	docommand=1;

	while (status)
	{
		strcpy(lbuf,pscname->GetString(*SelLine));
   	ret=sscanf(lbuf,"%s",dummy); 
		len=strlen(dummy)+1;
		iResult=0;
		
		eval(pscname,SelLine,lbuf+len,Result);
		ret=mp.Parameters.IndexOf(Result,(void**)&pres);
		if (ret<0) return 0;
		iResult= (int) *pres;
	
		if (iResult)
		{
			i=*SelLine+1;
			while (1)
			{
				strcpy(lbuf,pscname->GetString(i));
				if (!stricmp(lbuf,"ENDDO")) break;;
				if (!stricmp(lbuf,"CONTINUE")) break;
				if (!stricmp(lbuf,"BREAK")) {docommand=0;break;};
				if (docommand) 
				{//Вложение структурных комманд 
					sscanf(lbuf,"%s",line); 
					if (!stricmp(line,"WHILE")) ret=dowhile(pscname,&i);
					else if (!stricmp(line,"IF")) ret=ifelse(pscname,&i);
					else if (!stricmp(line,"GOTO")) {dogoto(pscname,&i);FlagGoto=1;*SelLine=i;return 0;}
					else ret=execute_a_single_line(lbuf,&iResult,pResult,pscname,SelLine);
					if (FlagGoto) *SelLine=i;
					if (ret==0 || ret==99) return ret;
				}
				i++;
	    }
    } else status=0;
  }
  *SelLine=i;
	return 1;
}

// ====================================================
// Функция по обработке условия if else endif
	int CLightDlg::ifelse(CStrList *pscname,int *SelLine)
// ====================================================
{
char lbuf[300],dummy[100],pResult[1024],line[100],len,docommand;
int  i,ret,iResult;
double *pres;

	strcpy(lbuf,pscname->GetString(*SelLine));
   	ret=sscanf(lbuf,"%s",dummy); 
	len=strlen(dummy)+1;
	iResult=0;
	
	docommand=0;
	eval(pscname,SelLine, lbuf+len,Result);
	ret=mp.Parameters.IndexOf(Result,(void**)&pres);
	if (ret<0) {return 0;};
	iResult= (int) *pres;
	if (iResult) docommand=1;
	
    i=*SelLine+1;
	while (1)
	{
		strcpy(lbuf,pscname->GetString(i));
		if (!strcmp(lbuf,"ELSE")) {docommand=!docommand;goto inc;}
		if (!strcmp(lbuf,"ENDIF")) break;
		if (docommand) 
		{
			//Вложение структурных комманд 
			sscanf(lbuf,"%s",line); 
			if (!stricmp(line,"WHILE")) ret=dowhile(pscname,&i);
			else if (!stricmp(line,"IF")) ret=ifelse(pscname,&i);
			else if (!stricmp(line,"GOTO")) {dogoto(pscname,&i);FlagGoto=1;*SelLine=i;return 0;}
			else ret=execute_a_single_line(lbuf,&iResult,pResult,pscname,SelLine);
			if (FlagGoto) *SelLine=i;
			if (ret==0 || ret==99) return ret;
		};
inc:	i++;
	 }
	
    *SelLine=i;
	return 1;

}

// ====================================================
// Функция по обработке перехода goto
	int CLightDlg::dogoto(CStrList *pscname,int *SelLine)
// ====================================================
{
int  i;
char lbuf[300],dummy[100],param1[100];
	
	strcpy(lbuf,pscname->GetString(*SelLine));
   	sscanf(lbuf,"%s %s",dummy,param1);
	i=atoi(param1);
	if (i==0) 
	{ 
		strcat(param1,":");
		i=PoiskMetok(pscname,param1);
	} 
	else 
		i--;
	if (i>=0) 
	{
		*SelLine=i;	
		return 1;
	}
	return 0;
}

// ====================================================
	int CLightDlg::handle_autoexec(void)
// ====================================================
{
	if (gc.detscript.running)
	   return 0;

	if (!access(AUTOEXEC_FPS,0))
	{
		strcpy(gc.detscript.scriptname,AUTOEXEC_FPS);
		gc.detscript.running=1;
		script_thread=AfxBeginThread(ScriptWorkerThread,&xdummy);
	}
	return 1;
}

// ====================================================
	int CLightDlg::handle_startup_script (char *name)
// ====================================================
{
	if (gc.detscript.running)
       return 0;

	strcpy(gc.detscript.scriptname,name);
	gc.detscript.running=1;
	script_thread=AfxBeginThread(ScriptWorkerThread,&xdummy);
	return 1;
}

// =======================================================
	int CLightDlg::execute_script(LPCSTR name)
// =======================================================
{
CStrList scname;
int  ret,lines;
char lbuf[300],line[300];

int iResult;		//Возврат значения целочисленной функцией
char sResult[1024];	//Возврат значения строковой функцией

	 	 
	 ASSERT(name);
	 gc.block_status=0;
	 scname.RemoveAll();
	 scname.FillAsEdit(name);
	 lines=scname.GetCount();
	 if (lines<1)	return 0;

	 if (actual_executed_script==name)
		ERR_MSG_RET0("E_NORECURSIVE");

	 actual_executed_script=name;
	 was_esc_pressed();
	 for (int i=0;i<lines;i++)
	 {
		 
begin:
		 FlagGoto=0;
		 if (gc.detscript.abort)
		 {
		     gc.detscript.abort=0;
			 if (!gc.block_status)	m_statusbar.SetText(L("S_589"),0,0);	// script aborted
			 return 1;
		 }

		 strcpy(lbuf,scname.GetString(i));
		 sscanf(lbuf,"%s",line);	
		 
		 if (!stricmp(line,"WHILE")) 
			 ret=dowhile(&scname,&i);
		 else if (!stricmp(line,"IF")) 
			 ret=ifelse(&scname,&i);
		 else if (!stricmp(line,"GOTO")) 
		 {
			 ret=dogoto(&scname,&i);
			 FlagGoto=1;
		 }
         else 	
			ret=execute_a_single_line(lbuf,&iResult,sResult,&scname,&i);

		 if (ret==99)						// User abort ...
		 {
			MessageBeep(0);
			goto enit;
		 }
// check GOTO
		 if (FlagGoto) goto begin;
		 if (!ret)							// User abort ...
		 {
			 actual_executed_script.Empty();
			 err_out("E_SCRIPTFAILED",i+1,line); return 0;
		 }
	 }
enit:
	 actual_executed_script.Empty();
	 if (!gc.block_status)	m_statusbar.SetText(L("S_191"),0,0);	// end of script
	 return 1;
}

// =====================================================================================
	int CLightDlg::execute_a_single_line(LPSTR line,int *iResult,LPSTR pResult,CStrList *pscname,int *SelLine)
// =====================================================================================
{
LVFINDINFO lvf;
tm	 *mt;
time_t xx;
int  ret,x,y,value=0,waittime=100,counter=0,ord;
char buf[300],param1[300],param2[300],param3[300],dummy[300];
char *pmet,*pmet2;

	*iResult=0;
	strcpy(pResult,Ok);
	DelVarScript(Result);
	
	if (sscanf(line,"%s",buf)<1)	return 1;	// An empty line

// Проверка на метки
	if (pmet=strchr(buf,':'))
	{
		pmet2=strchr(buf,'(');
//Символ после скобки не метка 
		if (pmet2 && pmet2<pmet) goto nomet;
		strcpy(line,line+strlen(buf));
		ltrim(line);
		ret=sscanf(line,"%s",buf);	
	}

nomet:
	if (*buf==';' || strncmp(buf,"//",2)==0)   return 1;	// комментарий
	
// Проверка на оператор присваивания
    
	if (pmet=strchr(buf,'='))
	{
		pmet2=strchr(buf,'(');

		if (pmet2 && pmet2<pmet) goto not_eq;	//Символ после скобки не присваивание 
		if (*(pmet-1)=='>') goto not_eq;		//Проверка на больше или равно в выражении
		
		x=pmet-buf;
		if (x>12) x=12;
		buf[x]=0;
		pmet=strchr(line,'=');
		strcpy(line,pmet+1);
		ltrim(line);
		
		ret=eval(pscname,SelLine,line,buf);
		if (ret==2) ret=1;
		return ret;
	}

not_eq:
	
	if (gc.detscript.abort)	 return 0;
	if (!gc.block_status)	m_statusbar.SetText(L("S_136",line),0,0);	// Command: %s

// #######################################################################
	if (!strnicmp(buf,"SHOW(",5))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		if (!strnicmp(param1,"S",1)) { gSearch.ShowWindow(SW_SHOWNORMAL);return 1;}
		if (!strnicmp(param1,"M",1)) { gMailer.ShowWindow(SW_SHOWNORMAL); gMailer.GraphicalStatusDisplay(1); return 1;}
		if (!strnicmp(param1,"T",1)) { gTosser.ShowWindow(SW_SHOWNORMAL); 	return 1;}
		if (!strnicmp(param1,"P",1)) { gPurger.ShowWindow(SW_SHOWNORMAL); return 1;}
		if (!strnicmp(param1,"N",1)) { gNLComp.ShowWindow(SW_SHOWNORMAL); 	return 1;}
		if (!strnicmp(param1,"Q",1)) { gMailer.show_events=0; gMailer.OnShowQueue(); return 1;}
		if (!strnicmp(param1,"E",1)) { gMailer.show_events=1; gMailer.OnShowQueue(); return 1;}
		if (!strnicmp(param1,"F",1)) { gFloating.ShowWindow(SW_SHOWNORMAL);	return 1; }
		if (!strnicmp(param1,"R",1)) { OnFilerequest(); return 1; }
		strcpy(pResult,Error);
		return 0;
	}
	if (!strnicmp(buf,"HIDE(",5))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		if (!strnicmp(param1,"S",1)) { gSearch.ShowWindow(SW_HIDE);	return 1; }
		if (!strnicmp(param1,"M",1)) { gMailer.ShowWindow(SW_HIDE);	return 1; }
		if (!strnicmp(param1,"T",1)) { gTosser.ShowWindow(SW_HIDE);	return 1; }
		if (!strnicmp(param1,"P",1)) { gPurger.ShowWindow(SW_HIDE);	return 1; }
		if (!strnicmp(param1,"N",1)) { gNLComp.ShowWindow(SW_HIDE); 		return 1; }
		if (!strnicmp(param1,"F",1)) { gFloating.ShowWindow(SW_HIDE);	return 1; }
		strcpy(pResult,Error);
		return 0;
	}
	if (!strnicmp(buf,"FLASH(",6))		// Flash window
	{
		if(!proccess_param(line,param1,pResult,iResult,1))	return 0;
		if(!proccess_param(line,param2,pResult,iResult,2))	return 0;
		waittime=atoi(param1);
  	show_msg(param2,waittime-400);
		return 1;
	}

	if (!strnicmp(buf,"APPENDTOFILE(",13))	// Save to file
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		AppendMailTofile(param1);
		*iResult=1;
		sprintf(pResult,"%d",*iResult);
		return 1;
	}

	if (!stricmp(buf,"CHECKNODELISTS"))
		{ if (!check_for_nodediffs())	return 1; nl_compile();	return 1; }

	if (!strnicmp(buf,"MOVE(",5))				 // Move Window
	{
		if (!proccess_param(line,param1,pResult,iResult,1))	return 0;
		if (!proccess_param(line,param2,pResult,iResult,2))	return 0;
		if (!proccess_param(line,param3,pResult,iResult,3))	return 0;
		x=atoi(param2);
		y=atoi(param3);
		if (!strnicmp(param1,"M",1)) { move_window(&gMailer,x,y);	return 1; }
		if (!strnicmp(param1,"T",1)) { move_window(&gTosser,x,y);	return 1; }
		if (!strnicmp(param1,"P",1)) { move_window(&gPurger,x,y);	return 1; }
		if (!strnicmp(param1,"N",1)) { move_window(&gNLComp,x,y);	return 1; }
		if (!strnicmp(param1,"S",1)) { move_window(&gSearch,x,y);return 1; }
		if (!strnicmp(param1,"F",1)) { move_window(&gFloating,x,y);	return 1; }
		strcpy(pResult,Error);
		return 0;
	}

	if (!strnicmp(buf,"SYSTEM(",6))
	{
		if (!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		*iResult=system(param1);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"WINEXEC(",8))
	{
		if (!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		*iResult=WinExec(param1,SW_SHOWNORMAL);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"PLAYWAVE(",9))
	{
		if (!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		*iResult=play_sound_file(param1);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}

	// #######################################################################
	if (!strnicmp(buf,"MSGBOX(",7))				 // The Message COMMAND
	{
		if (!proccess_param(line,param1,pResult,iResult))	return 0;
		eval(pscname,SelLine,param1,Result);
		ret=FindVarStr(Result,param1);
		MessageBox(param1,L("S_396"));	// script message
		*iResult=0;
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"STATUS(",7))		// output message in statusbar
	{
		if (!proccess_param(line,param1,pResult,iResult))	return 0;
		m_statusbar.SetText(param1,0,0);
		gc.block_status= *param1 != 0;	// block output if message is not empty
		return 1;
	}
// sorting
	if (!strnicmp(buf,"SORT(",5))
	{
		int crit=0;
		if (!proccess_param(line,param1,pResult,iResult,1))	return 0;
		if (!proccess_param(line,param2,pResult,iResult,2))	return 0;
		if (!stricmp(param2,"DEC")) crit = SORT_ORDER; 
		if (!strnicmp(param1,"FR",2))	crit = (crit & SORT_ORDER) | SORT_FROM;
		else if (!strnicmp(param1,"TO",2))	crit = (crit & SORT_ORDER) | SORT_TO;
		else if (!strnicmp(param1,"SU",2))	crit = (crit & SORT_ORDER) | SORT_SUBJECT;
		else if (!strnicmp(param1,"CR",2))	crit = (crit & SORT_ORDER) | SORT_CREATE;
		else if (!strnicmp(param1,"RE",2))	crit = (crit & SORT_ORDER) | SORT_RECIEVE;
		else if (!strnicmp(param1,"ST",2))	crit = (crit & SORT_ORDER) | SORT_STATUS;
		if (crit) { gc.SortCriteria=crit; MailSort(1);	return 1; }	else return 0;
	}
// check dupes
	if (!strnicmp(buf,"CHECKDUPES(",11))
	{
		if (!proccess_param(line,param1,pResult,iResult,1))	return 0;
		if (!strnicmp(param1,"CUR",3))		{ OnDupeCheck();	return 1; }
		else if (!stricmp(param1,"ALL"))	{ OnDupeCheckAll();	return 1; }
		else return 0;
	}
	if (!strnicmp(buf,"ACCEPTCALL(",11))
	{
		if (!proccess_param(line,param1,pResult,iResult,1))	return 0;
		if (!stricmp(param1,"1"))			{ OnIncomingModem1(); return 1; }
		else if (!stricmp(param1,"2"))		{ OnIncomingModem2(); return 1; }
		else return 0;
	}
	if (!strnicmp(buf,"SET(",4))
	{
		if (!proccess_param(line,param1,pResult,iResult,1))	return 0;
		if (!proccess_param(line,param2,pResult,iResult,2))	return 0;
		if (!stricmp(param2,"OFF")) ord=FALSE; else if (!stricmp(param2,"ON")) ord=TRUE;
		else return 0;
		if (!stricmp(param1,"TT")){ gc.NoTooltips=!ord;	return 1; }
		if (!stricmp(param1,"UD")){ m_subjlist.SetRedraw(ord);
			m_mailview.SetRedraw(1); gc.detscript.disableupdate=1;	return 1; }
		if (!stricmp(param1,"EV")){ gc.eventsenabled=1; return 1; }
		if (!stricmp(param1,"TG")){ gc.show_tags=!ord; show_mail(&gustat); return 1;}
		if (!stricmp(param1,"SB")){ gc.show_seenby=!ord; show_mail(&gustat); return 1;}
		if (!stricmp(param1,"EM")){ gc.hide_leading_blank=ord; show_mail(&gustat); return 1;}
	}
	if (!stricmp(buf,"TOGGLEUSERMARKFORMAIL"))	{ OnToggleUsermark(); return 1; }
	if (!stricmp(buf,"REMOVEUSERMARKFORALL"))	{ unmark_all_mails(); return 1; }
	if (!stricmp(buf,"MARKALLASREAD"))			{ set_attributes(0,TRUE); return 1; }
	if (!stricmp(buf,"MARKALLAREASASREAD"))		{ MarkAllAreasAsRead(); return 1;	}
	if (!stricmp(buf,"MARKALLASDELETED"))		{ set_attributes(DB_DELETED,TRUE); return 1; }
	if (!stricmp(buf,"AUTOSAVE"))				{ OnAsave(); return 1; }
	if (!stricmp(buf,"FLUSHFILES"))				{ db_flush_area(); return 1; }
	if (!stricmp(buf,"RESETEMERGENCY"))			{ reset_Notbremse(); return 1; }
	if (!stricmp(buf,"RESTARTEVENTS"))			{ restart_events();	return 1; }
	if (!stricmp(buf,"CHECKINBOUND"))			{ check_inbound();	return 1; }
	if (!stricmp(buf,"VIEWOUTBOUND"))			{ FipsApp.OnViewOutbound();	return 1; }
 	if (!stricmp(buf,"BEEP"))					{ MessageBeep(0); return 1; }
	if (!stricmp(buf,"UUDECODE"))				{ ScriptUudecode(); return 1; }
	if (!stricmp(buf,"FINDORIGINAL"))			{ FindOriginalMail(); return 1; }
	if (!stricmp(buf,"UPDATEDISPLAY"))			{ OnUpdateScreenDisplay(); return 1; }
	if (!stricmp(buf,"PRINT"))					{ FipsApp.OnPrintMail(); return 1; }
	if (!strnicmp(buf,"WAIT(",5))
	{
		int whour=0,wmin=0,wsec=0,hour=0,min=0,sec=0;
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		ret=sscanf(param1,"%d:%d:%d",&whour,&wmin,&wsec);
		while (1)
		{
			if (gc.detscript.abort) return 1;
			time(&xx);
			mt=localtime(&xx);
			hour=mt->tm_hour;
			min	=mt->tm_min;
			sec	=mt->tm_sec;

			switch (ret)
			{
				case 2:					// Test hour only
					if (whour==hour)	return 1;
					break;
				case 3:					// Test hour and minute
					if (whour==hour && wmin==min)	return 1;
					break;
				case 4:					// Test hour minute and second
					if (whour==hour && wmin==min && wsec==sec)	return 1;
					break;
				default:
					return 1;
			}
            if (was_esc_pressed() && (GetKeyState(VK_SHIFT) & 0x80000000))
			{
				show_msg(L("S_589"),500);
				return 99;
			}
			Sleep(300);
		}
		return 1;
	}

 	if (!strnicmp(buf,"SLEEP(",6))
	{
		if(!proccess_param(line,param1,pResult,iResult,1))	return 0;
		Sleep(atoi(param1));
		return 1;
	}

 	if (!strnicmp(buf,"CALL(",5))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		return execute_script(param1);
	}

 	if (!stricmp(buf,"RESCAN"))
	{
		gc.WeComeFromEventScript=1;
		gMailer.OnRescan();
		gc.WeComeFromEventScript=0;
		return 1;
	}

	if (!strnicmp(buf,"GO(",3))
	{
		int cnt;
		if (!proccess_param(line,param1,pResult,iResult,1))	return 0;
		if (!proccess_param(line,param2,pResult,iResult,2))	return 0;
		if (!stricmp(param2,"FIRST")) ord=1; 
		else if (!stricmp(param2,"LAST")) ord=2; 
		else if (!stricmp(param2,"PREV")) ord=3; 
		else if (!stricmp(param2,"NEXT")) ord=4;
		else return 0;
		if (!stricmp(param1,"AREA"))
		{
			switch(ord) {
			case 1:
				gustat.cur_area_handle=0;
				select_area(TRUE,FALSE);
				break;
			case 2:
				db_get_count_of_areas(&cnt);
				gustat.cur_area_handle=cnt-1;
				select_area(TRUE,FALSE);
				break;
			case 3:
				select_prevarea();	
				break;
			case 4:
				select_nextarea();
				break;
			}
			return 1;
		}
		else if (!stricmp(param1,"MAIL"))
		{
			switch(ord) {
			case 1:
				m_subjlist.SetFocus();
                keybd_event(VK_CONTROL,MapVirtualKey(VK_CONTROL,0),0,0);
                keybd_event(VK_HOME,MapVirtualKey(VK_HOME,0),0,0);
                keybd_event(VK_HOME,MapVirtualKey(VK_HOME,0),KEYEVENTF_KEYUP,0);
                keybd_event(VK_CONTROL,MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
					break;
			case 2:
				m_subjlist.SetFocus();
                keybd_event(VK_CONTROL,MapVirtualKey(VK_CONTROL,0),0,0);
                keybd_event(VK_END,MapVirtualKey(VK_END,0),0,0);
                keybd_event(VK_END,MapVirtualKey(VK_END,0),KEYEVENTF_KEYUP,0);
                keybd_event(VK_CONTROL,MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
				break;
			case 3:
				SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,
					gc.golded_compatible ? VK_LEFT : VK_UP,0);
				break;
			case 4:
				SendDlgItemMessage(IDC_SUBJLIST,WM_KEYDOWN,
					gc.golded_compatible ? VK_RIGHT : VK_DOWN,0);
				break;
			}
			return 1;
		}
		return 0;
	}

 	if (!strnicmp(buf,"STARTMAILER",11))		// The Start Mailer Command
	{
		int dm[10],k;
		for (k=0;k<10;k++) 
		{
			if(!proccess_param(line,param1,pResult,iResult,k+1))	break;
			if(*param1==0)	break;
			dm[k]=atoi(param1);
		}
//		if (k==0)	{strcpy(pResult,Error);	return 0;}	// no selected queue elements
		
		gMailer.m_queue.SetSel(-1,FALSE);	// remove all selection
		for (int t=0;t<k;t++)
			gMailer.m_queue.SetSel(dm[t]-1);

		gMailer.OnStart();
		Sleep(1000);   					// Let the MAILER thread execute
		if (!gc.mailer.running) {strcpy(pResult,Error); return 0;} // Failed to start thread

		while (1)
		{
            if (was_esc_pressed() && (GetKeyState(VK_SHIFT) & 0x80000000))
			{
				show_msg(L("S_589"),500);
				return 99;
			}
		    Sleep(300);
			if (!gc.mailer.running)
			   return 1;
		}
	}

 	if (!stricmp(buf,"STARTTOSSER"))		      // The Start Tosser Command
	{
		gTosser.OnStart();
		while (1)
		{
            if (was_esc_pressed() && (GetKeyState(VK_SHIFT) & 0x80000000))
			{
				show_msg(L("S_589"),500);
				return 99;
			}
		    Sleep(300);
			if (!gc.tosser.running)
			   return 1;
		}
	}

 	if (!stricmp(buf,"STARTPURGER"))
	{
		gPurger.PostMessage(WM_COMMAND,MAKEWPARAM(IDC_PURGEALL,BN_CLICKED));
		while (1)
		{
            if (was_esc_pressed() && GetKeyState(VK_SHIFT) & 0x80000000)
			{
				show_msg(L("S_589"),500);	// aborted
				return 99;
			}
		    Sleep(300);
			if (!gc.purger.running)	break;
		}
		return 1;
	}
 	if (!stricmp(buf,"STARTNLCOMP"))
	{
		gNLComp.OnStart();
		while (1)
		{
            if (was_esc_pressed() && GetKeyState(VK_SHIFT) & 0x80000000)
			{
				show_msg(L("S_589"),500);
				return 99;
			}
		    Sleep(300);
			if (!gc.detnl.running)	break;
		}
		return 1;
	}

	if (!stricmp(buf,"EXIT"))	
	{ gc.ExitFips=1; gc.ExitEventThread=1; return 1; }
	if (!stricmp(buf,"ESSHOWFIRST"))	
	{ if (gSearch.GetSafeHwnd()) gSearch.DisplayFirstResult(); return 1; }
	if (!stricmp(buf,"ESSHOWNEXT"))	
	{ if (gSearch.GetSafeHwnd()) gSearch.DisplayNextResult(); return 1; }
	if (!stricmp(buf,"ESCLEARQUERY"))	
	{ if (gSearch.GetSafeHwnd()) gSearch.OnClearQueryList(); return 1; }
	if (!stricmp(buf,"ESCLEARRESULTS"))	
	{ if (gSearch.GetSafeHwnd()) gSearch.OnClearSearch(); return 1; }
 	if (!strnicmp(buf,"ESLOADQUERY(",12))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		if (gSearch.GetSafeHwnd()) gSearch.OnLoadQuery2(param1);
		return 1;
	}
 	if (!strnicmp(buf,"ESLOADRESULTS(",14))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		if (gSearch.GetSafeHwnd()) gSearch.OnLoadResults2(param1);
		return 1;
	}
 	if (!strnicmp(buf,"ESSAVERESULTS(",14))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		ParseFileName(param1);
		if (gSearch.GetSafeHwnd()) gSearch.OnSaveResults2(param1);
		return 1;
	}
 	if (!stricmp(buf,"ESSTART"))
	{
		if (gSearch.GetSafeHwnd())
		{
			gSearch.OnSearch();
			while (1)
			{
				Sleep(300);
				if (!gc.SearchThreadRunning)	return 1;
			}
		}
		return 1;
	}
	if (!strnicmp(buf,"ESSELAREA(",10))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		if (gSearch.GetSafeHwnd())
		{
			if (stricmp(param1,"ALL"))
			{
				memset(&lvf,0,sizeof(lvf));
				lvf.flags=LVFI_STRING;
				lvf.psz=param1;
				int i=gSearch.m_listarea.FindItem(&lvf);;
				if (i>=0) SelectRow(gSearch.m_listarea,i);
			}
			else
				gSearch.OnSelAll();
		}
		return 1;
	}
	if (!strnicmp(buf,"ESDESELAREA(",12))
	{
		if(!proccess_param(line,param1,pResult,iResult))	return 0;
		if (gSearch.GetSafeHwnd())
		{
			if (stricmp(param1,"ALL"))
			{
				memset(&lvf,0,sizeof(lvf));
				lvf.flags=LVFI_STRING;
				lvf.psz=param1;
				int i=gSearch.m_listarea.FindItem(&lvf);;
				if (i>=0) DeselectRow(gSearch.m_listarea,i);
			}
			else
				gSearch.OnClearAll();
		}
		return 1;
	}
	if (!strnicmp(buf,"FILEIMPORT(",11))		 // Импорт письма
	{
		long hfile;
		_finddata_t se;
		
		proccess_param(line,param1,pResult,iResult);
		ParseFileName(param1);
		hfile = _findfirst(param1,&se);
		if (hfile != -1L)
		{
			do	*iResult=parse_import(se.name);
			while (_findnext(hfile,&se)==0);
			_findclose(hfile);
		}
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"FILECREATE(",11))		 // создание файла флага
	{
		FILE* pFile;
		proccess_param(line,param1,pResult,iResult);
		ParseFileName(param1);
		pFile=fopen(param1,"wt");
		
		ret=ParseParam(2,dummy,param2);
		ParseString(param2);
		if (ret!=0) fwrite(param2,strlen(param2),1,pFile);
		fclose(pFile);
		return 1;
	}
	if (!strnicmp(buf,"FILEDELETE(",11))	 // удаление файла-флага
	{
		proccess_param(line,param1,pResult,iResult);
		ParseFileName(param1);
		*iResult=DeleteFile(param1);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"FILEEXIST(",10))		 // проверка файла-флага
	{
		proccess_param(line,param1,pResult,iResult);
		ParseFileName(param1);
		*iResult=existfile(param1);
		sprintf(pResult,"%d",(int) *iResult);
		return 1;
	}
	if (!strnicmp(buf,"FILEEXISTDEL(",13))	 // проверка файла-флага и удаление
	{
		proccess_param(line,param1,pResult,iResult);
		ParseFileName(param1);
		*iResult=existfile(param1);
		if (*iResult) DeleteFile(param1);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"FILECOPY(",9))		 // Копирование файла-флага
	{
		ParseParamAll(line,dummy);
		ret=ParseParam(1,dummy,param1);
		ParseString(param1);
		ret=ParseParam(2,dummy,param2);
		ParseString(param2);
		if (ret==0)	{*iResult=0;strcpy(pResult,Error);return 0;};
		ParseFileName(param1);
		ParseFileName(param2);
		*iResult=CopyFile(param1,param2,NULL);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"ITOA(",5))		 // Преобразовать в строку
	{
		double *pres;
		ParseParamAll(line,dummy);
		ret=ParseParam(1,dummy,param1);
		eval(pscname,SelLine, param1,Result);
		
		ret=mp.Parameters.IndexOf(Result,(void**)&pres);
		if (ret<0)	{*iResult=0;strcpy(pResult,Error);return 0;};
		itoa((int) *pres,pResult,10);
		return 2;
	}
	if (!strnicmp(buf,"GCVT(",5))
	{
		double *pres;
		ParseParamAll(line,dummy);
		ret=ParseParam(1,dummy,param1);
		eval(pscname,SelLine, param1,Result);
		ret=mp.Parameters.IndexOf(Result,(void**)&pres);
		if (ret<0)	{*iResult=0;strcpy(pResult,Error);return 0;};
		gcvt(*pres,10,pResult);
		ret=strlen(pResult)-1;
		if ((ret)&&(*(pResult+ret)=='.')) *(pResult+ret)=0;
		return 2;
	}
	// 4 параметр ини файл 1 параметр - имя секции
	// 2 параметр имя ключа 3 -параметр значение по умолчанию
	if (!strnicmp(buf,"READINIINT(",11))		 // Чтение значения из ини файла
	{
		char param3[300];
		char param4[300];
		
		ParseParamAll(line,dummy);
		ret=ParseParam(1,dummy,param1);
		ParseString(param1);
		ret=ParseParam(2,dummy,param2);
		ParseString(param2);
		ret=ParseParam(3,dummy,param3);
		ParseString(param3);
		ret=ParseParam(4,dummy,param4);
		ParseString(param4);
		if (ret==0)	{*iResult=0;strcpy(pResult,Error);return 0;}
		ParseFileName(param1);
		*iResult=GetPrivateProfileInt(param1,param2,atoi(param3),param4);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	if (!strnicmp(buf,"READINISTR(",11))		 // Чтение значения из ини файла
	{
		char param3[300];
		char param4[300];

		ParseParamAll(line,dummy);
		ret=ParseParam(1,dummy,param1);
		ParseString(param1);
		ret=ParseParam(2,dummy,param2);
		ParseString(param2);
		ret=ParseParam(3,dummy,param3);
		ParseString(param3);
		ret=ParseParam(4,dummy,param4);
		ParseString(param4);
		if (ret==0)	{*iResult=0;strcpy(pResult,Error);return 0;};
		ParseFileName(param1);
		GetPrivateProfileString(param1,param2,param3,pResult,100,param4);
		
		return 2;
	}
	if (!strnicmp(buf,"WRITEINISTR(",11))		 // Запись значения из ини файла
	{
		char param3[300];
		char param4[300];
		
		ParseParamAll(line,dummy);
		ret=ParseParam(1,dummy,param1);
		ParseString(param1);
		ret=ParseParam(2,dummy,param2);
		ParseString(param2);
		ret=ParseParam(3,dummy,param3);
		ParseString(param3);
		ret=ParseParam(4,dummy,param4);
		ParseString(param4);
		if (ret==0)	{*iResult=0;strcpy(pResult,Error);return 0;};
		ParseFileName(param1);
		*iResult=WritePrivateProfileString(param1,param2,param3,param4);
		sprintf(pResult,"%d",*iResult);
		return 1;
	}
	
	strcpy(pResult,Error);
	return 0;
}

// ====================================================
   int was_esc_pressed(void)
// ====================================================
{
MSG  msg;

   if (GetAsyncKeyState ((int)VK_ESCAPE))
   {
      PeekMessage (&msg,NULL,WM_KEYFIRST,WM_KEYLAST,PM_REMOVE);
      return 1;
   }
   return 0;
}

// ====================================================
	void move_window(CWnd *pw,int x,int y)
// ====================================================
{
int diff;
RECT re;

	pw->GetWindowRect(&re);
	diff=re.left-x;	
	re.left-=diff; 
	re.right-=diff;	
	diff=re.top-y;	
	re.top-=diff;  
	re.bottom-=diff;
	pw->MoveWindow(&re);
}

// ====================================================
	BOOL proccess_param(LPSTR line,LPSTR parm,LPSTR pResult,int *iResult,int ind)
// ====================================================
{
char dm[300];

	ParseParamAll(line,dm);
	if (ParseParam(ind,dm,parm)==0)	
	{
		*iResult=0;
		strcpy(pResult,Error);
		return FALSE;
	}
	ParseString(parm);
	return TRUE;
}