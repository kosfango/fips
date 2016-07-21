/***********************************************/
/*              Terminal ANSI                  */
/* COMMON                                      */
/* Copyright (c) 1991,95 by ADONTEC (R) Ltd.   */
/***********************************************/


#include <stdafx.h>
#include "supercom.h"

PANSIDATA pfANSIData[MAX_COM];

SHORT _GetVal1(LPSTRING Msg);    /*##16.6.1995*/
SHORT _GetVal2(LPSTRING Msg);
SHORT ANSI_GetVal1(LPSTRING Msg);
SHORT ANSI_GetVal2(LPSTRING Msg);

// ==========================================================
	SHORT _GetVal1(LPSTRING Msg)     /*##16.6.1995*/
// ==========================================================
{
SHORT Res=0;
char str[3];
BYTE i=0;

	str[0]=0;
	while (Msg[i] && (Msg[i]!=';') && (i<2))
	{
		str[i]=Msg[i];
		i++;
	}
	str[i]=0;

	if (str[0]) Res=(SHORT)atoi(str);
		return(Res);
}

// ==========================================================
	SHORT _GetVal2(LPSTRING Msg)     /*##16.6.1995*/
// ==========================================================
{
SHORT Res=0;
char str[3];
BYTE i=0;
BYTE j=0;

	str[0]=0;       /* locate  ';' */
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j]==';')
	{
	 j++;
	 while (Msg[j+i] && (i<2))
	 {
		str[i]=Msg[j+i];
		i++;
	 }
	 str[i]=0;
	}
	if (str[0]) Res=(SHORT)atoi(str);

	return(Res);
}


// ==========================================================
	int semicount(LPSTRING Msg)
// ==========================================================
{

	int retval=0;
	int j=0;

	while (Msg[j]) 
	{
		if (Msg[j] == ';')
			retval++;
		j++; 
	}

	return retval;

}


// ==========================================================
	SHORT _GetVal3(LPSTRING Msg)     /*##16.6.1995*/
// ==========================================================
{
SHORT Res=0;
char str[3];
BYTE i=0;
BYTE j=0;

	str[0]=0;       /* locate  ';' */
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j] != 0)
		j++;

	// und nochmal
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j]==';')
	{
	 j++;
	 while (Msg[j+i] && (i<2))
	 {
		str[i]=Msg[j+i];
		i++;
	 }
	 str[i]=0;
	}
	if (str[0]) Res=(SHORT)atoi(str);

	return(Res);
}


// ==========================================================
	SHORT _GetVal4(LPSTRING Msg)     /*##16.6.1995*/
// ==========================================================
{
SHORT Res=0;
char str[3];
BYTE i=0;
BYTE j=0;

	str[0]=0;       /* locate  ';' */
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j] != 0)
		j++;

	// und nochmal
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j] != 0)
		j++;

	// und nochmal
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j]==';')
	{
	 j++;
	
	 while (Msg[j+i] && (i<2))
	 {
		str[i]=Msg[j+i];
		i++;
	 }
	 str[i]=0;
	}
	if (str[0]) Res=(SHORT)atoi(str);

	return(Res);
}

// ==========================================================
	SHORT _GetVal5(LPSTRING Msg)     /*##16.6.1995*/
// ==========================================================
{
SHORT Res=0;
char str[3];
BYTE i=0;
BYTE j=0;

	str[0]=0;       /* locate  ';' */
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j] != 0)
		j++;

	// und nochmal
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j] != 0)
		j++;

	// und nochmal
	while (Msg[j] && (Msg[j]!=';')) { j++; }
	
	if (Msg[j] != 0)
		j++;

	// und nochmal
	while (Msg[j] && (Msg[j]!=';')) { j++; }

	if (Msg[j]==';')
	{
	 j++;
	 while (Msg[j+i] && (i<2))
	 {
		str[i]=Msg[j+i];
		i++;
	 }
	 str[i]=0;
	}
	if (str[0]) Res=(SHORT)atoi(str);

	return(Res);
}

// ==========================================================
	SHORT ANSI_GetVal1(LPSTRING Msg)
// ==========================================================
{
SHORT Res=0;
	Res=_GetVal1(Msg);
	if (Res==0) Res=1;
	return(Res);
}

// ==========================================================
	SHORT ANSI_GetVal2(LPSTRING Msg)
// ==========================================================
{
SHORT Res=0;
	Res=_GetVal2(Msg);
	if (Res==0) Res=1;
	return(Res);
}


// ==========================================================
	BOOLEAN COMMAPI RS_ANSIInit(BYTE Com, PANSIDATA TermData)
// ==========================================================
{
PANSIDATA ANSI;

	if (Com != LOCALCOM)
		if (!ComValid(Com)) return(FALSE);

	ANSI=pfANSIData[Com]=TermData;
	ANSI->Msg[0]=0;
	ANSI->ScanState=scanESC;

	if (ANSI->ANSIProc)
	 {
	  //ANSI->ANSIProc(Com, cfANSIGETXY, 0, (POINTER)&(ANSI->SaveScrPos));
     ANSI->RemScrPos.X=1;
     ANSI->RemScrPos.Y=1;
     ANSI->RXRemXY=FALSE;
	  return(TRUE);
	 }
	else
	  return(FALSE);
}


// ==========================================================
	BOOLEAN COMMAPI RS_ANSIRead(BYTE Com, LPCHAR RCh)
// ==========================================================
{
char Ch;
BYTE i;
char valstr[20];
TSCRPOS ScrPos;
PANSIDATA ANSI;

  if (pfANSIData[Com]==(PANSIDATA)NULL) return(FALSE);


  while (ComRead(Com,&(*RCh)))
  {
    Ch=*RCh;
	 ANSI=pfANSIData[Com];

	 switch (ANSI->ScanState)
	 {
		case scanESC: if (Ch==ESC)
		      ANSI->ScanState=scanESC2;
		    else
		      return(TRUE);
						  break;
		case scanESC2:if (Ch=='[')
		      ANSI->ScanState=scanDATA;
		    else
		      {
			ANSI->ScanState=scanESC;
			return(TRUE);
		      }
						  break;
		case scanDATA:
		   {
		    BYTE Count;
		    LPSTRING str;

		    str=(LPSTRING)&(ANSI->Msg);
			 Count=(BYTE)strlen(str);

		    if (Count<(BYTE)(ANSISTRING-1))
						  {
							 if (isdigit(Ch) || Ch==';')
								{
								 str[Count]=Ch;
								 Count++;
								 str[Count]=0;
								}
							 else /* else #2 */
								{
							    ANSI->ANSIProc(Com, cfANSIGETXY, 0, (POINTER)&ScrPos);

								 switch (Ch)
								 {
								  case 'A': /* Up */
										ScrPos.Y=(SHORT)(ScrPos.Y-ANSI_GetVal1((LPSTRING)str));
										break;
								  case 'B':/* Down */
										ScrPos.Y=(SHORT)(ScrPos.Y+ANSI_GetVal1((LPSTRING)str));
										break;
								  case 'C':/* Move Right */
										ScrPos.X=(SHORT)(ScrPos.X+ANSI_GetVal1((LPSTRING)str));
										break;
								  case 'D':/* Move Left */
										ScrPos.X=(SHORT)(ScrPos.X-ANSI_GetVal1((LPSTRING)str));
										break;
								  case 'R':/* Store remote X, Y */
										ANSI->RemScrPos.Y=ANSI_GetVal1((LPSTRING)str);
										ANSI->RemScrPos.X=ANSI_GetVal2((LPSTRING)str);
			      ANSI->RXRemXY=TRUE;
										break;
								  case 'H':/* gotoxy */
								  case 'f':/* gotoxy */
										ScrPos.Y=ANSI_GetVal1((LPSTRING)str);
										ScrPos.X=ANSI_GetVal2((LPSTRING)str);
										break;
								  case 'J':/* clear screen and home */
										ANSI->ANSIProc(Com, cfANSICLEARSCR, 0, (POINTER)NULL);
			      ANSI->ANSIProc(Com, cfANSIGETXY, 0, (POINTER)&ScrPos);
										break;
								  case 'K':/* clear end of line */
				   ANSI->ANSIProc(Com, cfANSICLEAREOL, 0, (POINTER)NULL);
										break;
								  case 'm':/* set video attribute */ /*##16.6.1995*/
				   ANSI->ANSIProc(Com, cfANSISETVIDEO, _GetVal1((LPSTRING)str), (POINTER)NULL);
										break;
								  case 'n':ComWrite(Com,ESC);
											  ComWrite(Com,'[');
											  itoa(ScrPos.Y,valstr,10);
											  i=0;
											  while (valstr[i])
											  { ComWrite(Com,valstr[i]); i++; }
											  ComWrite(Com,';');
											  itoa(ScrPos.X,valstr,10);
											  i=0;
											  while (valstr[i])
											  { ComWrite(Com,valstr[i]); i++; }
											  ComWrite(Com,'R');
										break;
								  case 's':ANSI->ANSIProc(Com, cfANSIGETXY, 0, (POINTER)&(ANSI->SaveScrPos));
										break;
								  case 'u':ScrPos=ANSI->SaveScrPos;
										break;
								  default:
										{ /* uknown code, show user */
				 str[Count]=Ch;
									 Count++;
									 str[Count]=0;
							  ANSI->ANSIProc(Com, cfANSIUSER, 0, (POINTER)str);
										}
										break;
								 }  /* end switch */

							    ANSI->ANSIProc(Com, cfANSICHECKXY, 0, (POINTER)&ScrPos);
							    ANSI->ANSIProc(Com, cfANSIGOTOXY, 0, (POINTER)&ScrPos);
								 str[0]=0;
								 ANSI->ScanState=scanESC;
								} /* end else #2 */
						  }/* end if Count */
						  else
						  { /* error, show user */
							str[Count]=Ch;
							Count++;
							str[Count]=0;
							 ANSI->ANSIProc(Com, cfANSIUSER, 0, (POINTER)str);
							 str[0]=0;
							 ANSI->ScanState=scanESC;
						  }
						 } /* end of scanDATA */
		   break;
	 }
  }

  return(FALSE);
}


//=============================================================================
	BOOLEAN COMMAPI RS_ANSICheck(BYTE Com, char * RCh,int init)
//=============================================================================
//
// check char and return TRUE if char should be printed, otherwise FALSE
//
{
char		Ch;
BYTE		i;
char		valstr[20];
TSCRPOS		ScrPos;
static		PANSIDATA ANSI;


  if (pfANSIData[Com]==(PANSIDATA)NULL) return(FALSE);


  //while (ComRead(Com,&(*RCh)))
  {
    Ch=*RCh;

	if (init)
	{
		ANSI=pfANSIData[Com];
		return 0;
	}

	 switch (ANSI->ScanState)
	 {
		case scanESC: if (Ch==ESC)
		      ANSI->ScanState=scanESC2;
		    else
		      return(TRUE);
						  break;
		case scanESC2:if (Ch=='[')
		      ANSI->ScanState=scanDATA;
		    else
		      {
			ANSI->ScanState=scanESC;
			return(TRUE);
		      }
						  break;
		case scanDATA:
		   {
		    BYTE Count;
		    LPSTRING str;

		    str=(LPSTRING)&(ANSI->Msg);
			 Count=(BYTE)strlen(str);

		    if (Count<(BYTE)(ANSISTRING-1))
			{
				if (isdigit(Ch) || Ch==';')
				{
					 str[Count]=Ch;
					 Count++;
					 str[Count]=0;
				}
				else /* else #2 */
				{
					ANSI->ANSIProc(Com, cfANSIGETXY, 0, (POINTER)&ScrPos);
					 switch (Ch)
					 {
					  case 'A': /* Up */
							ScrPos.Y=(SHORT)(ScrPos.Y-ANSI_GetVal1((LPSTRING)str));
							break;
					  case 'B':/* Down */
							ScrPos.Y=(SHORT)(ScrPos.Y+ANSI_GetVal1((LPSTRING)str));
							break;
					  case 'C':/* Move Right */
							ScrPos.X=(SHORT)(ScrPos.X+ANSI_GetVal1((LPSTRING)str));
							break;
					  case 'D':/* Move Left */
							ScrPos.X=(SHORT)(ScrPos.X-ANSI_GetVal1((LPSTRING)str));
							break;
					  case 'R':/* Store remote X, Y */
							ANSI->RemScrPos.Y=ANSI_GetVal1((LPSTRING)str);
							ANSI->RemScrPos.X=ANSI_GetVal2((LPSTRING)str);
							ANSI->RXRemXY=TRUE;
							break;
					  case 'H':/* gotoxy */
					  case 'f':/* gotoxy */
							ScrPos.Y=ANSI_GetVal1((LPSTRING)str);
							ScrPos.X=ANSI_GetVal2((LPSTRING)str);
							break;
					  case 'J':/* clear screen and home */
							ANSI->ANSIProc(Com, cfANSICLEARSCR, 0, (POINTER)NULL);
							ANSI->ANSIProc(Com, cfANSIGETXY, 0, (POINTER)&ScrPos);
							break;
					  case 'K':/* clear end of line */
							ANSI->ANSIProc(Com, cfANSICLEAREOL, 0, (POINTER)NULL);
							break;
					  case 'm':/* set video attribute */ /*##16.6.1995*/
						   ANSI->ANSIProc(Com, cfANSISETVIDEO, _GetVal1((LPSTRING)str), (POINTER)NULL);
						   if (semicount((LPSTRING)str) >0)
						   ANSI->ANSIProc(Com, cfANSISETVIDEO, _GetVal2((LPSTRING)str), (POINTER)NULL);
						   if (semicount((LPSTRING)str) >1)
						   ANSI->ANSIProc(Com, cfANSISETVIDEO, _GetVal3((LPSTRING)str), (POINTER)NULL);
						   if (semicount((LPSTRING)str) >2)
						   ANSI->ANSIProc(Com, cfANSISETVIDEO, _GetVal4((LPSTRING)str), (POINTER)NULL);
						   if (semicount((LPSTRING)str) >3)
						   ANSI->ANSIProc(Com, cfANSISETVIDEO, _GetVal5((LPSTRING)str), (POINTER)NULL);

							break;
					  case 'n':		
						  if (Com != LOCALCOM)
							{
								ComWrite(Com,ESC);
								ComWrite(Com,'[');
							}
							itoa(ScrPos.Y,valstr,10);
							i=0;
							while (valstr[i])
							{		
							if (Com != LOCALCOM)
							{
								ComWrite(Com,valstr[i]); i++; }
								ComWrite(Com,';');
							}
							itoa(ScrPos.X,valstr,10);
							i=0;
							while (valstr[i])
							{ 
							if (Com != LOCALCOM)
							{
								ComWrite(Com,valstr[i]); i++; }
								ComWrite(Com,'R');
							}
							break;
					  case 's':ANSI->ANSIProc(Com, cfANSIGETXY, 0, (POINTER)&(ANSI->SaveScrPos));
							break;
					  case 'u':ScrPos=ANSI->SaveScrPos;
							break;
					  default:
							{ /* uknown code, show user */
					 str[Count]=Ch;
					 Count++;
					 str[Count]=0;
					ANSI->ANSIProc(Com, cfANSIUSER, 0, (POINTER)str);
						}
						break;
					}  /* end switch */

					ANSI->ANSIProc(Com, cfANSICHECKXY, 0, (POINTER)&ScrPos);
					ANSI->ANSIProc(Com, cfANSIGOTOXY, 0, (POINTER)&ScrPos);
					str[0]=0;
					ANSI->ScanState=scanESC;
					} /* end else #2 */
				}/* end if Count */
				else
				{ /* error, show user */
				str[Count]=Ch;
				Count++;
				str[Count]=0;
				 ANSI->ANSIProc(Com, cfANSIUSER, 0, (POINTER)str);
				 str[0]=0;
				 ANSI->ScanState=scanESC;
				}
				} /* end of scanDATA */
				break;
	 }
  }

  return(FALSE);
}



// ==========================================================
	BOOLEAN COMMAPI RS_TranslateToANSI(TANSITERMCMD Cmd, PSCRPOS ScrPos, LPSTRING ANSIString)
// ==========================================================
{
char str[20];
BOOLEAN Res=TRUE;

  ANSIString[0]=0;

  switch (Cmd)
  {
    case tcANSICURSORUP    :strcpy(ANSIString,"\x1B[1A");    /* Up */
	    break;
    case tcANSICURSORDOWN  :strcpy(ANSIString,"\x1B[1B");    /* Down */
	    break;
    case tcANSICURSORLEFT  :strcpy(ANSIString,"\x1B[1D");    /* Left */
	    break;
    case tcANSICURSORRIGHT :strcpy(ANSIString,"\x1B[1C");    /* Right */
	    break;
    case tcANSICURSORHOME  :strcpy(ANSIString,"\x1B[1;1H");  /* Home */
	    break;
    case tcANSICLEARSCREEN :strcpy(ANSIString,"\x1B[2J");    /* ClrScr */
	    break;
    case tcANSICLEAREOL    :strcpy(ANSIString,"\x1B[K");     /* ClrEol */
	    break;
    case tcANSIGETXY       :strcpy(ANSIString,"\x1B[6n");    /* cmd to get trm x,y */
	    break;
    case tcANSISAVEXY      :strcpy(ANSIString,"\x1B[s");     /* cmd to trm to save the x,y */
	    break;
    case tcANSIGOTOSAVEDXY :strcpy(ANSIString,"\x1B[u");     /* cmd to trm to pos cursor to */
			    break;                              /* the pos saved with tcSAVEXY */
    case tcANSIGOTOXY      :strcpy(ANSIString,"\x1B[");
			    strcat(ANSIString,itoa(ScrPos->X,str,10));
			    strcat(ANSIString,";");
			    strcat(ANSIString,itoa(ScrPos->Y,str,10));
			    strcat(ANSIString,"H");
			    break;

    default: Res=FALSE;
  }
  return(Res);
}

// ==========================================================
	BOOLEAN COMMAPI RS_ANSIGetXY(BYTE Com, PSCRPOS ScrPos)
// ==========================================================
{
	PANSIDATA ANSI;

	if (pfANSIData[Com]==(PANSIDATA)NULL)
		return(FALSE);

	ANSI=pfANSIData[Com];

	if (ANSI->RXRemXY)
	{
		ANSI->RXRemXY=FALSE;
		ScrPos->X=ANSI->RemScrPos.X;
		ScrPos->Y=ANSI->RemScrPos.Y;
		return(TRUE);
	}
	else
		return(FALSE);
}

