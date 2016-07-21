/* File transfer protocols return codes */
#define PRC_NOERROR         0       /* No comments :) */
#define PRC_ERROR           1       /* I/O error occured while snd./rcv. */
#define PRC_REMOTEABORTED   2       /* "ABORT" initiated by remote */
#define PRC_LOCALABORTED    3       /* We got SIGINT/SIGTERM? */
#define PRC_CPSTOOLOW       4       /* Cps was so low.. :( */
#define PRC_STOPTIME        5       /* Aborted due to the time limits */


void show_size (char *buf, long val);
void clear_status (unsigned char xmit);
void  com_putblock (int port, unsigned char *s, unsigned short len);
short com_getbyte (int port, unsigned char St_Pkt,char *p );
void  com_putbyte (int port, unsigned char s);
void  message (char *fmt,...);
int resdata(char *emsibuf);
int senddata(char *sendlist);
void blog_read(unsigned char c);
void blog_readstr(unsigned char *p,int len);
void blog_write(char c);
//int pmatch(char *pattern,char *string);


