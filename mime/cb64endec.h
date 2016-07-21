/*
	File: B64ENDEC.H
	Description:
		B64ENCODE/B64DECODE classes.

        25/03/1999
        Written by David Barton (davebarton@bigfoot.com) - pascal unit
        Function B64Encode(const S: string): string;
        Function B64Decode(const S: string): string;
 
        14/06/2001
        Modifed by Denis Korablev (FidoNet 2:5015/52.51) 
        pascal unit -> Class C++
*/

#ifndef _B64ENDEC_H
#define _B64ENDEC_H

#include <windows.h>
#include <commdlg.h>

     const char szMime[]="MIME-Version: 1.0";
     const char szType[]="Content-Type: application/octet-stream; name=%s";
     const char szTran[]="Content-Transfer-Encoding: base64";
     const char szDisp[]="Content-Disposition: attachment; filename=%s";
     const char B64Table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


class CB64DEC {
private:
	enum STATE { AWAIT_BEGIN, DECODING, AWAIT_END, DONE };
	enum { MAXBYTESPERLINE = 255 };
	STATE			m_state;
	HFILE			m_hfile;
	int				m_iMode;
	char			m_szFullName[260];
	char			m_szFileName[255];
	unsigned char	m_uchDecode[MAXBYTESPERLINE];
	OPENFILENAME	m_ofn;

	// State functions.
        char DECODE_CHAR(char c);	
        int IsB64Char(char *pszLine);	
	int AwaitBegin(char *);
	int Decode(char *);
        int AwaitEnd(char *);
public:
	CB64DEC(HWND hwnd);
	virtual ~CB64DEC();

    enum { RESULT_OK, RESULT_ERROR, RESULT_DONE, RESULT_CANCEL };
	int DecodeLine(char * pszLine);
	void Reset(void);
};

class CB64ENC {
private:
	enum STATE { BEGIN, ENCODING, END, DONE };
	enum { MAXBYTESPERLINE = 255 };
	STATE			m_state;
	HFILE			m_hfile;
	char			m_szFullName[260];
	char			m_szFileName[255];
	unsigned char	m_uchBuffer[MAXBYTESPERLINE];
	OPENFILENAME	m_ofn;

	// State functions.
	int Begin(char *, int);
	int Encode(char *, int);
	int End(char *, int);
public:
	CB64ENC(HWND hwnd);
	virtual ~CB64ENC();

	enum { RESULT_OK, RESULT_ERROR, RESULT_DONE, RESULT_CANCEL,
		RESULT_SMALLBUFFER };
	int EncodeLine(char * pszLine, int nLength);
	void Reset(void);
};
#endif

