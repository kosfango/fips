#include "pr_common.h"

/* Send/Recv file status values */
#define FSTAT_PROCESS  1
#define FSTAT_WAITACK  2
#define FSTAT_SUCCESS  3
#define FSTAT_SKIPPED  4
#define FSTAT_REFUSED  5

#define TRUE  1
#define FALSE 0
#ifndef XON
#define XON          ('Q' - '@')        /* Ctrl-Q (^Q) xmit-on character     */
#define XOFF         ('S' - '@')        /* Ctrl-S (^S) xmit-off character    */
#endif
/* ------------------------------------------------------------------------- */
/* Z-Modem chracteristics definitions                                        */
/* ------------------------------------------------------------------------- */
#define ZDEADTIMER	180	/* If expired think remote side is dead      */
#define ZRXTIMEOUT	120	/* Time to wait for receiving whole block    */
#define ZWAITTIME	30	/* Time to wait for character                */
#define ZMAXBLOCKLEN	8192	/* Maximal zmodem RX/TX block size           */
#define ZMAXTRIES	10	/* Maximal number of tries to do somthing    */
#define ZRXSKIPFIN	2	/* Number of ZFINs to ignore at start up     */
#define ZMAXFNAME	4096	/* Max. file name, to avoid txbuf overflow   */


/* ------------------------------------------------------------------------- */
/* Z-Modem constants definition                                              */
/* ------------------------------------------------------------------------- */
#define ZPAD		'*'	/* 052 Padding character begins frames       */
#define ZDLE		030	/* Ctrl-X Zmodem escape - `ala BISYNC DLE    */
#define ZDLEE (ZDLE^0100)	/* Escaped ZDLE as transmitted */
#define ZBIN		'A'	/* Binary frame indicator                    */
#define ZHEX		'B'	/* HEX frame indicator                       */
#define ZBIN32		'C'	/* Binary frame with 32 bit FCS              */

/* ------------------------------------------------------------------------- */
/* Frame types (see array "Frametypes" in zm.c)                              */
/* ------------------------------------------------------------------------- */
#define ZCRCERR		-5	/* CRC error                                 */
#define ZERROR		-4	/* Miscelaneouse errors                      */
#define ZEXIT		-3	/* Critical errors                           */
#define ZHANGUP		-2	/* No Carrier                                */
#define ZTIMER		-1	/* Time out waiting for smth.                */
#define ZOK		 0	/* All right                                 */

#define ZRQINIT		0	/* Request receive init                      */
#define ZRINIT		1	/* Receive init                              */
#define ZSINIT		2	/* Send init sequence (optional)             */
#define ZACK		3	/* ACK to above                              */
#define ZFILE		4	/* File name from sender                     */
#define ZSKIP		5	/* To sender: skip this file                 */
#define ZNAK		6	/* Last packet was garbled                   */
#define ZABORT		7	/* Abort batch transfers                     */
#define ZFIN		8	/* Finish session                            */
#define ZRPOS		9	/* Resume data trans at this position        */
#define ZDATA		10	/* Data packet(s) follow                     */
#define ZEOF		11	/* End of file                               */
#define ZFERR		12	/* Fatal Read or Write error Detected        */
#define ZCRC		13	/* Request for file CRC and response         */
#define ZCHALLENGE	14	/* Receiver's Challenge                      */
#define ZCOMPL		15	/* Request is complete                       */
#define ZCAN		16	/* Other end canned session with CAN*5       */
#define ZFREECNT	17	/* Request for free bytes on filesystem      */
#define ZCOMMAND	18	/* Command from sending program              */
#define ZSTDERR		19	/* Output to standard error, data follows    */

/* ------------------------------------------------------------------------- */
/* ZDLE sequences                                                            */
/* ------------------------------------------------------------------------- */
#define ZCRCE		'h'	/* CRC next, frame ends, header packet follows */
#define ZCRCG		'i'	/* CRC next, frame continues nonstop         */
#define ZCRCQ		'j'	/* CRC next, frame continues, ZACK expected  */
#define ZCRCW		'k'	/* CRC next, ZACK expected, end of frame     */
#define ZRUB0		'l'	/* Translate to rubout 0177                  */
#define ZRUB1		'm'	/* Translate to rubout 0377                  */

/* ------------------------------------------------------------------------- */
/* $zdlread return values (internal)                                         */
/* -1 is general error, -2 is timeout                                        */
/* ------------------------------------------------------------------------- */
#define GOTOR		0400
#define GOTCRCE		(ZCRCE|GOTOR)	/* ZDLE-ZCRCE received               */
#define GOTCRCG		(ZCRCG|GOTOR)	/* ZDLE-ZCRCG received               */
#define GOTCRCQ		(ZCRCQ|GOTOR)	/* ZDLE-ZCRCQ received               */
#define GOTCRCW		(ZCRCW|GOTOR)	/* ZDLE-ZCRCW received               */
#define GOTCAN		(GOTOR|030)	/* CAN*5 seen                        */

/* ------------------------------------------------------------------------- */
/* Byte positions within header array                                        */
/* ------------------------------------------------------------------------- */
#define ZF0		3	/* First flags byte                          */
#define ZF1		2
#define ZF2		1
#define ZF3		0
#define ZP0		0	/* Low order 8 bits of position              */
#define ZP1		1
#define ZP2		2
#define ZP3		3	/* High order 8 bits of file position        */

/* ------------------------------------------------------------------------- */
/* Bit Masks for ZRINIT flags byte ZF0                                       */
/* ------------------------------------------------------------------------- */
#define CANFDX		0x01	/* Rx can send and receive true FDX          */
#define CANOVIO		0x02	/* Rx can receive data during disk I/O       */
#define CANBRK		0x04	/* Rx can send a break signal                */
#define CANCRY		0x08	/* Receiver can decrypt                      */
#define CANLZW		0x10	/* Receiver can uncompress                   */
#define CANFC32		0x20	/* Receiver can use 32 bit Frame Check       */
#define ESCCTL		0x40	/* Receiver expects ctl chars to be escaped  */
#define ESC8		0x80	/* Receiver expects 8th bit to be escaped    */

/* ------------------------------------------------------------------------- */
/* Bit Masks for ZRINIT flags byze ZF1                                       */
/* ------------------------------------------------------------------------- */
#define ZF1_CANVHDR	0x01	/* Variable headers OK                       */
#define ZF1_TIMESYNC	0x02	/* nonstandard, Receiver request timesync    */

/* ------------------------------------------------------------------------- */
/* Parameters for ZSINIT frame                                               */
/* ------------------------------------------------------------------------- */
#define ZATTNLEN	32	/* Max length of attention string            */

/* ------------------------------------------------------------------------- */
/* Bit Masks for ZSINIT flags byte ZF0                                       */
/* ------------------------------------------------------------------------- */
#define TESCCTL		0100    /* Transmitter expects ctl chars to be escaped */
#define TESC8		0200    /* Transmitter expects 8th bit to be escaped */

/* ------------------------------------------------------------------------- */
/* Parameters for ZFILE frame                                                */
/* Conversion options one of these in ZF0                                    */
/* ------------------------------------------------------------------------- */
#define ZCBIN		1	/* Binary transfer - inhibit conversion      */
#define ZCNL		2	/* Convert NL to local end of line convention */
#define ZCRESUM		3	/* Resume interrupted file transfer          */

/* ------------------------------------------------------------------------- */
/* Management include options, one of these ored in ZF1                      */
/* ------------------------------------------------------------------------- */
#define ZF1_ZMSKNOLOC   0x80	/* Skip file if not present at rx */

/* ------------------------------------------------------------------------- */
/* Management options, one of these ored in ZF1                              */
/* ------------------------------------------------------------------------- */
#define ZF1_ZMMASK	0x1f	/* Mask for the choices below                */
#define ZF1_ZMNEWL	1	/* Transfer if source newer or longer        */
#define ZF1_ZMCRC	2	/* Transfer if different file CRC or length  */
#define ZF1_ZMAPND	3	/* Append contents to existing file (if any) */
#define ZF1_ZMCLOB	4	/* Replace existing file                     */
#define ZF1_ZMNEW	5	/* Transfer if source newer                  */
				/* Number 5 is alive ...                     */
#define ZF1_ZMDIFF	6	/* Transfer if dates or lengths different    */
#define ZF1_ZMPROT	7	/* Protect destination file                  */
#define ZF1_ZMCHNG	8	/* Change filename if destination exists     */

/* ------------------------------------------------------------------------- */
/* Transport options, one of these in ZF2                                    */
/* ------------------------------------------------------------------------- */
#define ZTLZW		1	/* Lempel-Ziv compression                    */
#define ZTCRYPT		2	/* Encryption                                */
#define ZTRLE		3	/* Run Length encoding                       */

/* ------------------------------------------------------------------------- */
/* Extended options for ZF3, bit encoded                                     */
/* ------------------------------------------------------------------------- */
#define ZXSPARS		64	/* Encoding for sparse file operations       */

/* ------------------------------------------------------------------------- */
/* Parameters for ZCOMMAND frame ZF0 (otherwise 0)                           */
/* ------------------------------------------------------------------------- */
#define ZCACK1		1	/* Acknowledge, then do command              */



/* HYDRA Prototypes */
class p_zmod: public p_common
{ public:
  int  Z_Rxframeind;	/* ZBIN ZBIN32, or ZHEX type of frame received */
  int  Z_Rxtype;		/* Type of header received                   */
  int  Z_Rxcount;		/* Count of data bytes received              */
  int  Z_Txnulls;		/* Number of nulls to send at beginning of ZDATA hdr */
  char Z_Rxhdr[4];	/* Received header                           */
  char Z_Txhdr[4];	/* Transmitted header                        */
  long Z_Rxpos;		/* Received file position                    */
  long Z_Txpos;		/* Send file position	                     */
  int  Z_Txfcs32;		/* TRUE means send binary frames with 32 bit FCS */
  int  Z_Txcrc32;		/* Display flag indicating 32 bit CRC being sent */
  int  Z_Rxcrc32;		/* Display flag indicating 32 bit CRC being received */
  char Z_Attn[ZATTNLEN+1];/* Attention string rx sends to tx on err     */
  int  Z_Ctlesc;		/* Encode control characters                 */
  int  Z_Lastsent;	/* Character we sent last                    */
  //int Z_Dirzap;		/* Direct ZedZap (don't escape XON/XOFF)     */

  p_zmod(void);
  virtual ~p_zmod();
  int zmod_batch (char *sendlist,char *emsibuf);
  int tx_zmodem(char *sendlist);
  int rx_zmodem(void);
  int zmod_file(char *blkptr, size_t blklen);

  private:
/* ------------------------------------------------------------------------- */
/* Z-Modem primitives 				                             */
/* ------------------------------------------------------------------------- */
void ZSend_Byte( char current_byte );
void zm_status (unsigned char xmit);

int zsbhdr(int type, char *hdr);
int zshhdr(int type, char *hdr);
int zsdata(const char *buf, int length, int frameend, long pos);
int zrdata(char *buf, int length, long pos);
int zgethdr(char *hdr);
void stohdr(char *hdr, long pos);
long rclhdr(char *hdr);
int  zrbhdr16(char *hdr);
int  zrbhdr32(char *hdr);
int  zrhhdr(char *hdr);
int  zrdat32(char *buf, int length, long pos);
int  zrdat16(char *buf, int length, long pos);
unsigned int  zgethex(void);
int  noxrd7(void);
int  zdlread(void);
int  zputhex(int c);

};


