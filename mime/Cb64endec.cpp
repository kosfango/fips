/*
	File: B64ENDEC.CPP
Description:
		B64ENCODE/B64DECODE classes.

        25/03/1999
        Written by David Barton (davebarton@bigfoot.com) - pascal unit
        Function B64Encode(const S: string): string;
        Function B64Decode(const S: string): string;
 
        14/06/2001
        Modifed by Denis Korablev (FidoNet 2:5015/52.51) 
        pascal unit -> Class C++,optimization code
*/

#include "stdafx.h"
#include "cb64endec.h"

extern void restore_base_path	(void);


typedef unsigned char uchar;


/*********************************************************************
	B64DECODE
*********************************************************************/
char CB64DEC::DECODE_CHAR(char c)	
{
    if (c==43) return 62; 
    if (c>47 && c< 58)  return c+4;
    if (c>64 && c< 91)  return c-65;
    if (c>96 && c< 123) return c-71;
    return 63;
}

int CB64DEC::IsB64Char(char *pszLine)	
{
  int i;
  char *p;
  char tmp[2];
  
  memset(&tmp[0],'\0',sizeof(tmp));
  for(p=pszLine,i=0;*p != '\0';p++) 
  {
     if (*p != '=')
     {tmp[0]=*p;
     if (!strstr(B64Table,tmp)) break;} 
  };
  return (*p =='\0') ? 1:0 ;  
}



/*
	Function: CB64DEC::CB64DEC
	Description:
		Constructor for CB64DEC class.Puts the state machine into
		the AWAIT_BEGIN state and initializes the OPENFILENAME
		structure used for the save file common dialog.
*/
CB64DEC::CB64DEC(HWND hwnd) : m_state(AWAIT_BEGIN), m_hfile(NULL)
{
	static char *szFilter[] = { "All Files (*.*)", "*.*", "", "" };

	m_ofn.lStructSize		= sizeof(OPENFILENAME);
	m_ofn.hwndOwner			= hwnd;
	m_ofn.hInstance			= NULL;		// Not using templates
										// so NULL is OK.
	m_ofn.lpstrFilter	= szFilter[0];
	m_ofn.lpstrCustomFilter	= NULL;
	m_ofn.nMaxCustFilter	= 0;
	m_ofn.nFilterIndex	= 1;
	m_ofn.lpstrFile		= m_szFullName;
	m_ofn.nMaxFile		= sizeof(m_szFullName);
	m_ofn.lpstrFileTitle	= m_szFileName;
	m_ofn.nMaxFileTitle	= sizeof(m_szFileName);
	m_ofn.lpstrInitialDir	= NULL;
	m_ofn.lpstrTitle	= "B64DECODE To";
	m_ofn.Flags		= OFN_HIDEREADONLY | OFN_NOREADONLYRETURN |  OFN_OVERWRITEPROMPT;
	m_ofn.nFileOffset	= 0;		// Set by dialog.
	m_ofn.nFileExtension	= 0;		// Set by dialog.
	m_ofn.lpstrDefExt	= NULL;		// User chooses total name.
	m_ofn.lCustData		= 0;		// Not using hook.
	m_ofn.lpfnHook		= NULL;
	m_ofn.lpTemplateName	= NULL;		// Not using custom template.
}

/*
	Function: CB64DEC::~CB64DEC
	Description:
		Destructor for CB64DEC class.  Simply make sure the file we
		were working on is closed if this gets called while we're
		in the middle of something.
*/
CB64DEC::~CB64DEC()
{
	if (m_hfile != NULL) {
		_lclose(m_hfile);
	}
}

/*
	Function: CB64DEC::DecodeLine
	Description:
		Dispatch function for the state machine.  The user of the
		object calls DecodeLine with every line of interest in the
		file, buffer, or other data source.  This is repeated until
		DecodeLine returns a result value other than RESULT_OK.
		RESULT_DONE signals that the end of a uuencoded file has
		been reached and everything was successful.  RESULT_ERROR
		results if a problem cropped up during decoding.
		RESULT_CANCEL is returned if the user hits the cancel button
		in the save file common dialog.
*/
int CB64DEC::DecodeLine(char * pszLine)
{
	ASSERT(pszLine != NULL);

	switch (m_state) {
		case AWAIT_BEGIN:
			return AwaitBegin(pszLine);
		case DECODING:
			return Decode(pszLine);
		case AWAIT_END:
			return AwaitEnd(pszLine);
		case DONE:
        	return RESULT_DONE;
	}
    return RESULT_ERROR;
}

/*
	Function: CB64DEC::AwaitBegin
	Description:
*/
int CB64DEC::AwaitBegin(char * pszLine)
{
   char tmp_buf[500];
   int iScaned;
   int p;

   // Проверка версии формата MIME 1.0
	   if (!strcmp(szMime,pszLine))  return RESULT_OK;
   	
   	// Проверка типа информации
           iScaned=sscanf(pszLine,szType,&tmp_buf);
	   if (iScaned==1)   return RESULT_OK;
        
        // Проверка протокола передачи
           if (!strcmp(szTran,pszLine))  return RESULT_OK;

        // Проверка имени передаваемого файла
           iScaned=sscanf(pszLine,szDisp, m_szFullName);
           if (iScaned==1)   {
               if (*m_szFullName == '"') m_szFullName[0]=' ';
               p= strlen(m_szFullName);
               if (p>0)
               if (m_szFullName[p-1] == '"') m_szFullName[p-1]='\0';
               
               BOOL bResult = GetSaveFileName(&m_ofn);
               restore_base_path();
               if (!bResult) {m_state = DONE;	return RESULT_CANCEL;};
               m_hfile = _lcreat(m_szFullName, 0);
               if (m_hfile == HFILE_ERROR) {m_state = DONE; return RESULT_ERROR;};
	                     };
	//Проверка на начало запуска декодирования
	  if  (m_hfile!=NULL)	m_state = DECODING;

	return RESULT_OK;
}

/*
	Function: CB64DEC::Decode
	Description:
		Decode a line of  encoded bytes and write them to the
		destination file. Move to AWAIT_END if the end of the text
		is found.
*/
int CB64DEC::Decode(char * pszLine)
{
  int i;
  int iEqual=0;
  unsigned char InBuf[4];
  unsigned char OutBuf[3];

    //Если длина декодируемой строки 0 то нет символа окончания "="
  if (strlen(pszLine) == 0)
  {   m_state = DONE; return RESULT_DONE;};
  
  if (!IsB64Char(pszLine)) return RESULT_OK;
  
  //Длина декодируемой строки должна быть кратна 4
  if ((strlen(pszLine) % 4)!= 0)
  {   m_state = DONE; return RESULT_ERROR;};

  
  int iByteIndex = 0;
  int iCharIndex = 0;
  int nBytes=((strlen(pszLine) / 4))*3;
  while (iByteIndex<nBytes) {
    memmove(&InBuf,&pszLine[iCharIndex],4);
    for (i=0;i<4;i++) InBuf[i]=DECODE_CHAR(InBuf[i]);
    OutBuf[0]= (InBuf[0] << 2) | ((InBuf[1] >> 4) & 0x03);
    OutBuf[1]= (InBuf[1] << 4) | ((InBuf[2] >> 2) & 0x0F);
    OutBuf[2]= (InBuf[2] << 6) | (InBuf[3] & 0x3F);
    memmove(&m_uchDecode[iByteIndex],&OutBuf,3);

    //Добавляемые символы окончания "=" есть ?
    for(iEqual=0;iEqual<4;iEqual++) 
		if (pszLine[iCharIndex+iEqual]=='=') break;
		

	if (iEqual!=4) m_state = DONE;
	iByteIndex += 3;
	iCharIndex += 4;               } ;

	//Коректировка на сиволы окончания
	switch (iEqual)
	 {
	 case 2:iByteIndex-=2; break;

     case 3:iByteIndex-=1; break; 
     };
	// When all the bytes have been decoded, write them out to the
	// file.  Handle any write error.
	if (_lwrite(m_hfile, (LPCSTR)m_uchDecode, iByteIndex) != 
		(UINT)iByteIndex) {
		// Close up the file.
		_lclose(m_hfile);
		m_hfile = NULL;
		m_state = DONE;
		return RESULT_ERROR;
	};


    return RESULT_OK;
}

/*
	Function: CB64DEC::AwaitEnd
	Description:
		According to the base64 format, there should be a
		line with just "/0" on it following the text.
*/
int CB64DEC::AwaitEnd(char * pszLine)
{
      m_state = DONE;
      if (strlen(pszLine) == 0) return RESULT_DONE;
      return RESULT_ERROR;
}

/*
	Function: CB64DEC::Reset
	Description:
		Resets the decoder state machine.
*/
void CB64DEC::Reset(void)
{
	if (m_hfile != NULL) {
		_lclose(m_hfile);
	}
	m_state = AWAIT_BEGIN;
}

/*********************************************************************
	B64ENCODE
*********************************************************************/

/*
	Function: CB64ENC::CB64ENC
	Description:
		Constructor for CB64ENC class.  Puts the state machine into
		the BEGIN state and initializes the OPENFILENAME
		structure used for the save file common dialog.
*/
CB64ENC::CB64ENC(HWND hwnd) : m_state(BEGIN), m_hfile(NULL)
{
	static char *szFilter[] = { "All Files (*.*)", "*.*", "", "" };

	m_ofn.lStructSize	= sizeof(OPENFILENAME);
	m_ofn.hwndOwner		= hwnd;
	m_ofn.hInstance		= NULL;		// Not using templates
										// so NULL is OK.
	m_ofn.lpstrFilter	= "All Files (*.*)\0*.*\0\0";
	m_ofn.lpstrCustomFilter	= NULL;
	m_ofn.nMaxCustFilter	= 0;
	m_ofn.nFilterIndex	= 1;
	m_ofn.lpstrFile		= m_szFullName;
	m_ofn.nMaxFile		= sizeof(m_szFullName);
	m_ofn.lpstrFileTitle	= m_szFileName;
	m_ofn.nMaxFileTitle	= sizeof(m_szFileName);
	m_ofn.lpstrInitialDir	= NULL;
	m_ofn.lpstrTitle	= "B64ENCODE From";
	m_ofn.Flags		= OFN_HIDEREADONLY;
	m_ofn.nFileOffset	= 0;		// Set by dialog.
	m_ofn.nFileExtension	= 0;		// Set by dialog.
	m_ofn.lpstrDefExt	= NULL;		// User chooses total name.
	m_ofn.lCustData		= 0;		// Not using hook.
	m_ofn.lpfnHook		= NULL;
	m_ofn.lpTemplateName	= NULL;		// Not using custom template.
}

/*
	Function: CB64ENC::~CB64ENC
	Description:
		Destructor for CB64ENC class.  Simply make sure the file we
		were working on is closed if this gets called while we're
		in the middle of something.
*/
CB64ENC::~CB64ENC()
{
	if (m_hfile != NULL) {
		_lclose(m_hfile);
	}
}
/*
	Function: CB64ENC::EncodeLine
	Description:
		Dispatch function for the state machine.  A user of this
		object calls EncodeLine repeatedly with a data buffer.
		EncodeLine dispatches the call to the right routine based
		on the state machine.  On each call, the state machine returns
		some data to the caller in the provided buffer and a result
		code.  If the code is RESULT_OK, the caller should store
		the returned data somewhere (usually a file or buffer) and
		call EncodeLine again.  Eventually, the end of the input file
		will be reached and EncodeLine will return RESULT_DONE.
		The user can then destroy the object or call CB64ENC::Reset
		to reinitialize it.  If an error occurs during processing,
		RESULT_ERROR is returned.  If the user cancels out of the
		file selection dialog, RESULT_CANCEL is returned.  If the
		supplied buffer is too small to hold the data that needs to
		be returned, RESULT_SMALLBUFFER is returned.  To avoid this,
        it's best to supply 80 characters minimum.
*/
int CB64ENC::EncodeLine(char * pszLine, int nLength)
{
	ASSERT(pszLine != NULL);
	ASSERT(nLength > 0);

	switch (m_state) {
		case BEGIN:
			return Begin(pszLine, nLength);
		case ENCODING:
			return Encode(pszLine, nLength);
		case END:
			return End(pszLine, nLength);
		case DONE:
        	return 999;
	}
    return RESULT_ERROR;
}

/*
	Function: CB64ENC::Begin
*/
int CB64ENC::Begin(char *pszLine, int nLength)
{
FILE *fp;

        lstrcpy(m_szFullName, "");
	BOOL bResult = GetOpenFileName(&m_ofn);
	restore_base_path();

	if (!bResult) {
		m_state = DONE;
		return 0x99992222;
	}

	// Проверка наличия файла
	fp=fopen(m_szFullName,"rb");
	if (!fp)
	   return (0x99994444);         // File Open error
        fclose(fp);

		// Создание линий заголовка.
		lstrcpy(pszLine, szMime);
		sprintf(pszLine, szType,m_szFileName);
		lstrcpy(pszLine, szTran);
	
		sprintf(pszLine, szDisp,m_szFileName);
		m_state = ENCODING;
		return RESULT_OK;
}

/*
	Function: CB64ENC::Encode
*/
int CB64ENC::Encode(char * pszLine, int nLength)
{
  unsigned int i;
  unsigned char InBuf[2];
  unsigned char OutBuf[3];
  
  int iByteIndex = 0;
  int iCharIndex = 0;
  int nBytes=strlen(pszLine) % 3;
  for(i=1;i<strlen(pszLine);i=+3) {
    memmove(&pszLine[iByteIndex],&InBuf,3);
    OutBuf[0]= B64Table[((InBuf[0] & 0xFC) > 2) + 1];
    OutBuf[1]= B64Table[(((InBuf[0] & 0x03) < 4) | ((InBuf[1] & 0xF0) > 4)) + 1];
    OutBuf[2]= B64Table[(((InBuf[1] & 0x0F) < 2) | ((InBuf[2] & 0xC0) > 6)) + 1];
    OutBuf[3]= B64Table[(InBuf[2] & 0x3F) + 1];
    memmove(&OutBuf,&m_uchBuffer[iCharIndex],4);
    iByteIndex += 3;
    iCharIndex += 4;               } ;
    memset(&m_uchBuffer[iCharIndex],'=',3-nBytes);
	return RESULT_OK;
}

/*
	Function: CB64ENC::End
	Description:
		Returns the "end" line to the caller.
*/
int CB64ENC::End(char * pszLine, int nLength)
{
	const char szEndText[] = "=";
	if (lstrlen(szEndText) + 1 <= nLength) {
		lstrcpy(pszLine, szEndText);
		m_state = DONE;
		return RESULT_DONE;
	}
	else {
		m_state = DONE;
		return RESULT_SMALLBUFFER;
	}
}

/*
	Function: CB64ENC::Reset
	Description:
		Resets the encoder state machine.
*/
void CB64ENC::Reset(void)
{
	if (m_hfile != NULL) {
		_lclose(m_hfile);
	}
	m_state = BEGIN;
}



