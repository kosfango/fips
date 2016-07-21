/*
	File: UUENDEC.H
	Description:
		UUENCODE/UUDECODE classes.
	Copyright 1995, David G. Roberts
*/

#ifndef _UUENDEC_H
#define _UUENDEC_H

#define STAT_END	0x99992222
#define STAT_ERROR	0x99994444
#define STAT_LONG	0x99993333

#include <windows.h>
#include <commdlg.h>

class CUUDEC {
private:
	enum STATE { AWAIT_BEGIN, DECODING, AWAIT_END, DONE };
	STATE			m_state;
	HFILE			m_hfile;
	int				m_iMode;
	char			m_szFullName[MAX_PATH];
	char			m_szFileName[MAX_PATH];
	char			m_Filter[100];
	char			m_Title[100];
	unsigned char	m_uchDecode[64];
	OPENFILENAME	m_ofn;

	// State functions.
	int AwaitBegin(char *);
	int Decode(char *);
    int AwaitEnd(char *);
public:
	CUUDEC(HWND hwnd);
	virtual ~CUUDEC();

    enum { RESULT_OK, RESULT_ERROR, RESULT_DONE, RESULT_CANCEL };
	int DecodeLine(char * pszLine);
	void Reset(void);
};

class CUUENC {
private:
	enum STATE { BEGIN, ENCODING, END, DONE };
	enum { MAXBYTESPERLINE = 45 };
	STATE			m_state;
	HFILE			m_hfile;
	int				m_MaxLines;
	char			m_szFullName[MAX_PATH];
	char			m_szFileName[MAX_PATH];
	char			m_Filter[100];
	char			m_Title[100];
	unsigned char	m_uchBuffer[MAXBYTESPERLINE];
	OPENFILENAME	m_ofn;

	// State functions.
	int Begin(char *, int);
	int Encode(char *, int);
	int End(char *, int);
public:
	CUUENC(HWND hwnd);
	virtual ~CUUENC();

	enum { RESULT_OK, RESULT_ERROR, RESULT_DONE, RESULT_CANCEL,
		RESULT_SMALLBUFFER };
	int EncodeLine(char * pszLine, int nLength);
	void Reset(void);
};
#endif

