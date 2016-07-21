/****************************************************/
/*		                WinIO			                */
/*                                                  */
/* Windows 32                                       */
/* Copyright (c) 1991,94  ADONTEC Ltd.              */
/****************************************************/

#if defined (_cplusplus_)||(__cplusplus)
extern "C" {
#endif


#define IDM_TXFILE  101
#define IDM_RXFILE  102
#define IDM_ABOUT   103
#define IDM_QUIT  24340

#define ID_PROT_ZMODEM        (1000+5)
#define ID_PROT_YMODEMBATCH   (1000+4)
#define ID_PROT_YMODEM        (1000+3)
#define ID_PROT_XMODEMCRC     (1000+2)
#define ID_PROT_XMODEM        (1000+1)
#define ID_PROT_ASCII         (1000)

#define IDC_FILENAME 100
#define IDC_PATHNAME 101
#define IDC_FILES    102
#define IDC_DIRS     103

#define SCREENLEFT     0
#define SCREENRIGHT   79
#define SCREENTOP      0
#define SCREENBOTTOM  24


typedef BOOL (*BFPROC) (VOID);


typedef struct {
	  BFPROC Init;
	  BFPROC Run;
	  BFPROC Done;
	  BFPROC RXFile;
	  BFPROC TXFile;
	  BFPROC Break;
	} PGM;



BOOL InitApplication(HINSTANCE hInstance);	     /* current instance	     */

BOOL InitInstance(HINSTANCE hInstance,	/* Current instance identifier.	      */
		  int	 nCmdShow);	/* Param for first ShowWindow() call. */

LRESULT CALLBACK MainWndProc(HWND hWnd,	       /* window handle		     */
				 UINT message,  /* type of message		     */
				 WPARAM wParam,       /* additional information	     */
				 LPARAM lParam);	/* additional information	     */

BOOL CALLBACK AboutProc(HWND hDlg,	/* window handle of the dialog box */
				UINT message,	/* type of message		   */
				WPARAM wParam,	/* message-specific information    */
				LPARAM lParam);

BOOL CALLBACK GetFileNameProc(HWND hDlg, /* window handle of the dialog box */
				UINT message,	   /* type of message		   */
				WPARAM wParam,	   /* message-specific information    */
				LPARAM lParam);

BYTE wio_wherex(void);
BYTE wio_wherey(void);
void wio_curon(void);
void wio_gotoxy(BYTE x, BYTE y);
void wio_clrscr(void);
void wio_clreol(void);
void wio_write(LPSTR s, WORD len);
void wio_writech(char c);
BOOL wio_kbdhit(void);
char wio_getch(void);
void wio_updoff(void);
void wio_updon(BYTE nFromY, BYTE nToY);
void wio_updonex(BYTE nFromX, BYTE nToX, BYTE nFromY, BYTE nToY);
void wio_message(LPSTR strMsg, LPSTR strTitle, UINT buttons);
WORD wio_getfilename(LPSTR Resource, LPSTR FileName);

void wio_printf(char *fmt, ...);

LRESULT wio_Global_Yield(HWND hWnd);

#if defined (_cplusplus_)||(__cplusplus)
}
#endif

