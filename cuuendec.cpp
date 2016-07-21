/*
	File: UUENDEC.CPP
	Description:
		UUENCODE/UUDECODE classes.
	Copyright 1995, David G. Roberts
*/

#include "stdafx.h"	// Needed for sscanf().
#include "cuuendec.h"
#include <io.h>

extern void restore_base_path(void);
extern _gconfig gc;
extern char uufname[];
void parse_uutitle(LPCSTR line);

#define SPACE			((UCHAR)32)
// Remember, 077 = octal 77 = 0x3F = binary 00111111.
#define DECODE_CHAR(c)	((((UCHAR)c) - SPACE) & 0x3F)
#define ENCODE_BYTE(b)	(((UCHAR)(b) & 0x3F) == 0 ? '`' : (((UCHAR)(b) & 0x3F) + SPACE))
#define CHECK_CHAR(c)	(c<0x21 || c>0x60)
/*********************************************************************
	UUDECODE
*********************************************************************/

/*
	Function: CUUDEC::CUUDEC
	Description:
		Constructor for CUUDEC class.  Puts the state machine into
		the AWAIT_BEGIN state and initializes the OPENFILENAME
		structure used for the save file common dialog.
*/
CUUDEC::CUUDEC(HWND hwnd) : m_state(AWAIT_BEGIN), m_hfile(NULL)
{
char *p;

	strcpy(m_Filter,L("S_84"));
	p=m_Filter;
	while (p=strchr(p+1,'|'))	*p=0;
	strcpy(m_Title,L("S_459"));		// uu-decoding
	m_ofn.lStructSize		= sizeof(OPENFILENAME);
	m_ofn.hwndOwner			= hwnd;
	m_ofn.hInstance			= NULL;		// Not using templates
										// so NULL is OK.
	m_ofn.lpstrFilter		= m_Filter;
	m_ofn.lpstrCustomFilter	= NULL;
	m_ofn.nMaxCustFilter	= 0;
	m_ofn.nFilterIndex		= 1;
	m_ofn.lpstrFile			= m_szFullName;
	m_ofn.nMaxFile			= sizeof(m_szFullName);
	m_ofn.lpstrFileTitle	= m_szFileName;
	m_ofn.nMaxFileTitle		= sizeof(m_szFileName);
	m_ofn.lpstrInitialDir	= NULL;
	m_ofn.lpstrTitle		= m_Title;
	m_ofn.Flags				= OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT;
	m_ofn.nFileOffset		= 0;		// Set by dialog.
	m_ofn.nFileExtension	= 0;		// Set by dialog.
	m_ofn.lpstrDefExt		= NULL;		// User chooses total name.
	m_ofn.lCustData			= 0;		// Not using hook.
	m_ofn.lpfnHook			= NULL;
	m_ofn.lpTemplateName	= NULL;		// Not using custom template.
}

/*
	Function: CUUDEC::~CUUDEC
	Description:
		Destructor for CUUDEC class.  Simply make sure the file we
		were working on is closed if this gets called while we're
		in the middle of something.
*/
CUUDEC::~CUUDEC()
{
	if (m_hfile != NULL)
		_lclose(m_hfile);
}

/*
	Function: CUUDEC::DecodeLine
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
int CUUDEC::DecodeLine(char * pszLine)
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
	Function: CUUDEC::AwaitBegin
	Description:
		Waits for a line starting with the characters "begin "
		and containing a file mode and file name.  When this is
		found, it asks the user to confirm the file name and select
		a destination directory for the decode.  The file is then
        opened and the state changed to DECODING.
*/
	int CUUDEC::AwaitBegin(char *pszLine)
{
// Look for a line starting with "begin "
	if (sscanf(pszLine, "begin %o %s", &m_iMode,m_szFullName) != 2)
	{
		parse_uutitle(pszLine);
		return RESULT_OK;	// skip line
	}

	if (*uufname) 
		strcpy(m_ofn.lpstrFile,uufname);	// filename from title
	OemToChar(m_szFullName,m_szFullName);

	BOOL bResult = GetSaveFileName(&m_ofn);

	restore_base_path();

	if (!bResult) // user cancel
	{
		m_state = DONE;
		return RESULT_CANCEL;
	}
	m_hfile = _lcreat(m_szFullName, 0);
	if (m_hfile == HFILE_ERROR) 
	{
		m_state = DONE;
		return RESULT_ERROR;
	}
	m_state = DECODING;
	return RESULT_OK;
}

/*
	Function: CUUDEC::Decode
	Description:
		Decode a line of uuencoded bytes and write them to the
		destination file.  Move to AWAIT_END if the end of the text
		is found.
*/
int CUUDEC::Decode(char *pszLine)
{
// The first character of the line holds the number of bytes that are encoded on this line.
	if (!*pszLine || isspace(pszLine[0]))
		return RESULT_OK;	//	skip lines begining with whitespace chars
	int nBytes = DECODE_CHAR(pszLine[0]);
	if (nBytes==0) // If there are no bytes on this line, we're at the end.
	{
		if (_lclose(m_hfile) == HFILE_ERROR) 
		{
			m_hfile = NULL;
			m_state = DONE;
            return RESULT_ERROR;
		}
		else 
		{
			m_hfile = NULL;
			m_state = AWAIT_END;
			return RESULT_OK;
		}
	}
	int iByteIndex = 0;
	int iCharIndex = 1;
	while (iByteIndex < nBytes) 
	{
		// Note that nBytes may not be a multiple of 3, but the
		// number of characters on the line should be a multiple of 4.
		// If the number of encoded bytes is not a multiple of 3 then
		// extra pad characters should have been added to the text
		// by the encoder to make the character count a multiple of 4.
// Decode the line in 3-byte (=4-character) jumps.
		if (CHECK_CHAR(pszLine[iCharIndex]) || CHECK_CHAR(pszLine[iCharIndex+1]) ||
			(CHECK_CHAR(pszLine[iCharIndex+2]) && (nBytes-iByteIndex)>1) ||
			(CHECK_CHAR(pszLine[iCharIndex+3]) && (nBytes-iByteIndex)>2))
				return RESULT_OK;	// skip garbage line
		m_uchDecode[iByteIndex]		=
			(UCHAR)((DECODE_CHAR(pszLine[iCharIndex]) << 2) |
			(DECODE_CHAR(pszLine[iCharIndex + 1]) >> 4));
		m_uchDecode[iByteIndex + 1]	=
			(UCHAR)((DECODE_CHAR(pszLine[iCharIndex + 1]) << 4) |
			(DECODE_CHAR(pszLine[iCharIndex + 2]) >> 2));
		m_uchDecode[iByteIndex + 2]	=
			(UCHAR)((DECODE_CHAR(pszLine[iCharIndex + 2]) << 6) |
			DECODE_CHAR(pszLine[iCharIndex + 3]));
		iByteIndex += 3;
		iCharIndex += 4;
	}
	if (_lwrite(m_hfile, (LPCSTR)m_uchDecode, nBytes) != (UINT)nBytes) 
	{
		_lclose(m_hfile);
		m_hfile = NULL;
		m_state = DONE;
		return RESULT_ERROR;
	}
	return RESULT_OK;
}

/*
	Function: CUUDEC::AwaitEnd
	Description:
		According to the uuencode format, there should be a
		line with just "end" on it following the text.
*/
int CUUDEC::AwaitEnd(char * pszLine)
{
	// See if the line is "end"
	if (strncmp(pszLine, "end",3) == 0) {
		// Yep.  Whew!  Now we're really done.
		m_state = DONE;
		return RESULT_DONE;
	}
	else {
		// Nope.  Signal an error.  The data didn't follow the
		// standard uuencode format.  The decoded file on disk
		// may or may not be corrupted.  We have no way to tell.
		m_state = DONE;
		return RESULT_ERROR;
	}
}

/*
	Function: CUUDEC::Reset
	Description:
		Resets the decoder state machine.
*/
void CUUDEC::Reset(void)
{
	if (m_hfile != NULL)
		_lclose(m_hfile);
	m_state = AWAIT_BEGIN;
}

/*********************************************************************
	UUENCODE
*********************************************************************/

/*
	Function: CUUENC::CUUENC
	Description:
		Constructor for CUUENC class.  Puts the state machine into
		the BEGIN state and initializes the OPENFILENAME
		structure used for the save file common dialog.
*/
CUUENC::CUUENC(HWND hwnd) : m_state(BEGIN), m_hfile(NULL)
{
char *p;

	strcpy(m_Filter,L("S_84"));
	p=m_Filter;
	while (p=strchr(p+1,'|'))	*p=0;
	strcpy(m_Title,L("S_460"));		// uu-encoding
	m_ofn.lStructSize		= sizeof(OPENFILENAME);
	m_ofn.hwndOwner			= hwnd;
	m_ofn.hInstance			= NULL;		// Not using templates
	m_ofn.lpstrFilter		= m_Filter;
	m_ofn.lpstrCustomFilter	= NULL;
	m_ofn.nMaxCustFilter	= 0;
	m_ofn.nFilterIndex		= 1;
	m_ofn.lpstrFile			= m_szFullName;
	m_ofn.nMaxFile			= sizeof(m_szFullName);
	m_ofn.lpstrFileTitle	= m_szFileName;
	m_ofn.nMaxFileTitle		= sizeof(m_szFileName);
	m_ofn.lpstrInitialDir	= NULL;
	m_ofn.lpstrTitle		= m_Title;
	m_ofn.Flags				= OFN_HIDEREADONLY;
	m_ofn.nFileOffset		= 0;		// Set by dialog.
	m_ofn.nFileExtension	= 0;		// Set by dialog.
	m_ofn.lpstrDefExt		= NULL;		// User chooses total name.
	m_ofn.lCustData			= 0;		// Not using hook.
	m_ofn.lpfnHook			= NULL;
	m_ofn.lpTemplateName	= NULL;		// Not using custom template.
	m_MaxLines=gc.MaxUULines;
}

/*
	Function: CUUENC::~CUUENC
	Description:
		Destructor for CUUENC class.  Simply make sure the file we
		were working on is closed if this gets called while we're
		in the middle of something.
*/
CUUENC::~CUUENC()
{
	if (m_hfile != NULL)
		_lclose(m_hfile);
}
/*
	Function: CUUENC::EncodeLine
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
		The user can then destroy the object or call CUUENC::Reset
		to reinitialize it.  If an error occurs during processing,
		RESULT_ERROR is returned.  If the user cancels out of the
		file selection dialog, RESULT_CANCEL is returned.  If the
		supplied buffer is too small to hold the data that needs to
		be returned, RESULT_SMALLBUFFER is returned.  To avoid this,
        it's best to supply 80 characters minimum.
*/
int CUUENC::EncodeLine(char * pszLine, int nLength)
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
	Function: CUUENC::Begin
	Description:
		Get the file to encode from the user using a standard file
		dialog, open it, and return the "begin..." line.  Returns
		RESULT_CANCEL if the user cancels out of the file dialog.
*/
int CUUENC::Begin(char *pszLine, int nLength)
{
FILE *fp;
int	 lf;
char title[1000],shortpath[MAX_PATH],shortname[MAX_PATH],buf1[50],buf2[50],buf3[50];
 
    lstrcpy(m_szFullName, "");
	BOOL bResult = GetOpenFileName(&m_ofn);
	restore_base_path();
	if (!bResult)	// cancel pressed
	{
		m_state = DONE;
		return STAT_END;
	}
	fp=fopen(m_szFullName,"rb"); if (!fp) return STAT_ERROR;
	lf=_filelength(fileno(fp));
	GetShortPathName(m_szFullName,shortpath,MAX_PATH);
	get_filename(m_szFullName,shortname);
	get_filename(shortpath,gc.UUfilename);
	if (lf>gc.MaxUULines*45)
	{
		gc.isLongUUstuf=1;
		if (gc.LongUUBasePointer)	free(gc.LongUUBasePointer);
// allocate buffer
		gc.LongUUBasePointer=(char *)malloc(lf*2);
		if (!gc.LongUUBasePointer)
		{
			err_out("E_MEM_OUT");
			m_state = DONE;
			return STAT_END;
		}
		gc.UUBasePointertmp=gc.LongUUBasePointer;
		gc.LongUUBasePointer[0]=0;
	}
    fclose(fp);

	if (nLength>400) 
	{
		// Open the file.  Handle any errors.
		m_hfile = _lopen(m_szFullName, OF_READ);
		if (m_hfile == HFILE_ERROR) 
		{
			m_state = DONE;
			return RESULT_ERROR;
		}
		// Create title
		BY_HANDLE_FILE_INFORMATION fi;
		GetFileInformationByHandle((HANDLE)m_hfile,&fi);
		int		nlenkb=floor(fi.nFileSizeLow,1024);
		CTime	ct(fi.ftCreationTime);
		gc.NumberOfSections=floor(fi.nFileSizeLow,gc.MaxUULines*45);	// number of sections
		build_fido_time(buf2);
		sprintf(title,
			"            source file name : %s\r\n"
			"               original size : %d (%d kb)\r\n"
			"                  created on : %s\r\n"
			"                  encoded on : %s\r\n"
			"    approximate encoded size : %d kb\r\n"
			"          number of sections : %d\r\n"
			"           lines per section : %d\r\n\r\n"
			"section 1 of %d of file %s   -=< FIPS-uuencoder >=-\r\n\r\n"
			"begin 644 %s",shortname/*gc.UUfilename*/,fi.nFileSizeLow,nlenkb,
				langdate((time_t)ct.GetTime(),buf1,TRUE,TRUE),langtime(buf2,buf3),nlenkb*4/3,
				gc.NumberOfSections,gc.MaxUULines,gc.NumberOfSections,
				gc.UUfilename,gc.UUfilename);
		lstrcpy(pszLine, title);
		m_state = ENCODING;	// Now we're ready to encoding
		return RESULT_OK;
	}
	else
	{
    	// The buffer was too small.
		m_state = DONE;
		return RESULT_SMALLBUFFER;
	}
}

/*
	Function: CUUENC::Encode
	Description:
		Reads a text line's-worth of bytes (typically 45) into a
		buffer, encodes them, and returns them to the user.
*/
int CUUENC::Encode(char *pszLine,int nLength)
{
	// Zero out the buffer.
/*	for (int i = 0; i < sizeof(m_uchBuffer); i++)
		m_uchBuffer[i] = 0;*/
	memset(m_uchBuffer,0, sizeof(m_uchBuffer));
	// Read a line's-worth of bytes from the file into the buffer.
	int nBytes = _lread(m_hfile, m_uchBuffer,sizeof(m_uchBuffer));
	// Handle the potential error.
	if (nBytes == HFILE_ERROR) 
	{
		_lclose(m_hfile);
		m_hfile = NULL;
		m_state = DONE;
		return RESULT_ERROR;
	}
// Make sure the text line buffer supplied by the user will be large enough
// to hold all the coded text.  If not, inform the user.
// Line length is length byte + coded characters + string terminator.
	int iLineLength = 1+(((nBytes / 3) + (nBytes % 3) ? 1 : 0) * 4) + 1;
	if (iLineLength > nLength) {
		if (_lclose(m_hfile) == HFILE_ERROR) 
		{
			m_hfile = NULL;
			m_state = DONE;
            return RESULT_ERROR;
		}
		else 
		{
			m_hfile = NULL;
			m_state = DONE;
			return RESULT_SMALLBUFFER;
		}
	}
// Encode the number of bytes in this line and store as first character in the line.
	pszLine[0] = (char) ENCODE_BYTE(nBytes);
// Encode all the bytes in this line.
	int iByteIndex=0,iCharIndex=1;
	while (iByteIndex < nBytes)
	{
		pszLine[iCharIndex] =
			(char)(ENCODE_BYTE(m_uchBuffer[iByteIndex] >> 2));
		pszLine[iCharIndex + 1] =
			(char)(ENCODE_BYTE((m_uchBuffer[iByteIndex] << 4) |
				(m_uchBuffer[iByteIndex + 1] >> 4)));
		pszLine[iCharIndex + 2] =
			(char)(ENCODE_BYTE((m_uchBuffer[iByteIndex + 1] << 2) |
				(m_uchBuffer[iByteIndex + 2] >> 6)));
		pszLine[iCharIndex + 3] =
			(char)(ENCODE_BYTE(m_uchBuffer[iByteIndex + 2]));
		iByteIndex += 3;
        iCharIndex += 4;
	}
    // Add the line terminator.
	pszLine[iCharIndex] = 0;
	// See if we hit the end of the file.
	if (nBytes == 0) 
	{
		// Yup, so close it up, handling any error as appropriate.
		if (_lclose(m_hfile) == HFILE_ERROR) 
		{
			m_hfile = NULL;
			m_state = DONE;
            return RESULT_ERROR;
		}
		else 
		{
			m_hfile = NULL;
			m_state = END;
			return RESULT_OK;
		}
	}
	else
		return RESULT_OK;
}

/*
	Function: CUUENC::End
	Description:
		Returns the "end" line to the caller.
*/
int CUUENC::End(char * pszLine, int nLength)
{
	const char szEndText[] = "end";
	if (lstrlen(szEndText) + 1 <= nLength) 
	{
		lstrcpy(pszLine, szEndText);
		m_state = DONE;
		return RESULT_DONE;
	}
	else 
	{
		m_state = DONE;
		return RESULT_SMALLBUFFER;
	}
}

/*
	Function: CUUENC::Reset
	Description:
		Resets the encoder state machine.
*/
void CUUENC::Reset(void)
{
	if (m_hfile != NULL)
		_lclose(m_hfile);
	m_state = BEGIN;
}

	void parse_uutitle(LPCSTR line)
{
const char *p;

	p=strstr(line,"source file name :");
	if (p)
	{
		strncpy(uufname,p+18,MAX_PATH);
		uufname[MAX_PATH-1]=0;
		trim_all(uufname);
	}
}