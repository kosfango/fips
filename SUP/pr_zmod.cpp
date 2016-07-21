#include "stdafx.h"
#include <io.h>
#include "supercom.h" 
#include "..\u_crc.h"                    
#include "pr_zmod.h"
#include "pr_xfer.h"                    

#define timer_set(t)     (time(NULL) + (t))
#define timer_running(t) (t != 0L)
#define timer_expired(t) (time(NULL) > (t))
#define timer_reset()    (0L)

extern _gconfig gc;
extern _modemcfg gModemCfg;

/* ------------------------------------------------------------------------- */
/* States of our Zmodem sending protocol                                     */
/* ------------------------------------------------------------------------- */
typedef enum ztxstates {
	ZTX_START,		/* Start sending (init, send "rz\r")         */
	ZTX_RQINIT,		/* Send ZRQINIT frame                        */
	ZTX_RQINITACK,		/* Wait for answer on ZRQINIT frame          */
	ZTX_NEXTFILE,		/* Prepare next file for sending             */
	ZTX_FINFO,		/* Send file's ZFINFO frame with data        */
	ZTX_FINFOACK,		/* Wait for answer on our ZFINFO + ..        */
	ZTX_STARTDATA,		/* Start sending of data (send ZDATA?)       */
	ZTX_DATA,		/* Send new portion of data blocks           */
	ZTX_READCHECK,		/* Check - we got some chars                 */
	ZTX_CRCWACK,		/* Wait for CRCW ack                         */
	ZTX_CRCQACK,		/* Wait for CRCQ ack                         */
	ZTX_EOF,		/* Send ZEOF frame with our file length      */
	ZTX_EOFACK,		/* Wait for answer on our ZEOF               */
	ZTX_FIN,		/* Send ZFIN to finish sending               */
	ZTX_FINACK		/* Wait for answer on sent ZFIN              */
} e_ztxstates;

typedef enum zrxstates {
	ZRX_INIT,		/* Send ZRINIT frame -> goto ZRX_INITACK     */
	ZRX_INITACK,		/* Wait for answer on ZRINIT (for ZRQINIT?)  */
	ZRX_GOTFILE,		/* We got ZFINFO -> check it, open, etc..    */
	ZRX_SENDRPOS,		/* Send position we want receive file from   */
	ZRX_RPOSACK,		/* Wait for answer on (ZRPOS) our postion    */
	ZRX_WAITDATA,		/* And wait for data block with file's data  */
	ZRX_REFUSE,		/* Feee... Refuse this file                  */
	ZRX_SKIP		/* Burn baby.. burn.. skip it!               */
} e_zrxstates;

#define gotoexit(a)	{ rc = a; goto exit; }

void    p_zmod::ZSend_Byte( char current_byte )
    {
      switch( current_byte & 0xff)
      {
		case 0xd:
		case 0xd | 0x80: if (Z_Lastsent!='@') goto sendit;

		case 0x10 :
        case 0x11 :
        case 0x13 :
        case 0x18 :
		case 0x10 | 0x80 :
        case 0x11 | 0x80 :
        case 0x13 | 0x80 :
           com_putbyte (port,ZDLE );
           current_byte ^= 0X40;
sendit:    default : com_putbyte(port,Z_Lastsent=current_byte );
      }
    

    }  

/* ------------------------------------------------------------------------- */
void p_zmod::zm_status (unsigned char xmit)
{ 
long elapsed,bytes,cps;
char tempbuf[200];
char tempbuf2[200];
int dif,t,m,s;

	if (xmit)
	{
		elapsed = time(NULL) - txstart;
		bytes = (txallsize ? txtecsize:0)+Z_Txpos-txoffset;
		show_size(tempbuf2,txfsize);  
		sprintf(tempbuf,"%s %s",txfname,tempbuf2);
		gc.mailer.m_transferred=(txallsize ? txtecsize:0)+Z_Txpos;
		gc.mailer.m_total=(txallsize ? txallsize:txfsize);
		dif=(txallsize ? txallsize-txtecsize:txfsize)-Z_Txpos;
		if (gc.mailer.m_total>0) 
			gc.mailer.m_proz=(gc.mailer.m_transferred*100)/gc.mailer.m_total;
		else 
			gc.mailer.m_proz=0;
		cps = elapsed ? bytes / elapsed:0;
		dif= cps ? dif/cps:0;
		t=(int)(dif/3600);
		dif=dif-(t*3600);
		m=(int)(dif/60);
		s = dif-(m*60);
		sprintf(tempbuf2," (%02d:%02d:%02d) ",t,m,s);
		strcat(tempbuf,tempbuf2);
		gc.mailer.m_cps=cps;
		gc.mailer.m_timeelapsed=elapsed;
		strcpy(gc.mailer.m_filename,tempbuf);
	}
	else
	{
		elapsed = time(NULL) - rxstart;
		bytes = (rxallsize ? rxtecsize:0)+Z_Rxpos-rxoffset;
		show_size(tempbuf2,rxfsize);
		sprintf(tempbuf,"%s %s",rxfname,tempbuf2);
		gc.mailer.m_transferred2=(rxallsize ? rxtecsize:0)+Z_Rxpos;
		gc.mailer.m_total2=(rxallsize ? rxallsize:rxfsize);
		dif=(rxallsize ? rxallsize-rxtecsize:rxfsize)-Z_Rxpos;
		if (gc.mailer.m_total2>0) 
			gc.mailer.m_proz2=(gc.mailer.m_transferred2*100)/gc.mailer.m_total2;
		else 
			gc.mailer.m_proz2=0;
		cps = elapsed ? bytes / elapsed:0;
		dif= cps ? dif/cps:0;
		t=(int)(dif/3600);
		dif=dif-(t*3600);
		m=(int)(dif/60);
		s = dif-(m*60);
		sprintf(tempbuf2," (%02d:%02d:%02d) ",t,m,s);
		strcat(tempbuf,tempbuf2);
		gc.mailer.m_cps2=cps;
		gc.mailer.m_timeelapsed2=elapsed;
		strcpy(gc.mailer.m_filename2,tempbuf);
	}
	update_charging();
	modem_update();
}/*zm_status()*/

/*---------------------------------------------------------------------------*/
	int p_zmod::zdlread(void)
/*---------------------------------------------------------------------------*/
{
	int c;
	c = noxrd7();
	if (c==ZDLE) { c = noxrd7();c^= 0X40;}
	return c;
}

/* ------------------------------------------------------------------------- */
/* Read a character from the modem line with timeout.                        */
/* Eat parity, XON and XOFF characters.                                      */
/* ------------------------------------------------------------------------- */
	int p_zmod::noxrd7(void)
/* ------------------------------------------------------------------------- */
{
	int c,n;
	n=com_getbyte (port, 0,(char*) &c );
    c&=0xFF; 
	return(c);
}


/* ------------------------------------------------------------------------- */
/* Send ZMODEM binary header hdr of type $type                               */
/* ------------------------------------------------------------------------- */
	int p_zmod::zsbhdr(int type, char *hdr)
/* ------------------------------------------------------------------------- */
{
int n;
unsigned short crc;//,c;
unsigned long crc32;

	message("zsbhdr: %d %x (CRC%s)",type,rclhdr(hdr),Z_Txfcs32?"32":"16");
	    
	if( type == ZDATA )
		for( n=Z_Txnulls; --n >= 0; ) 
			com_putbyte(port,'\0');
		
	com_putbyte (port,ZPAD);
    com_putbyte (port,ZDLE);

	Z_Txcrc32 = Z_Txfcs32;
	
	if( Z_Txcrc32 )
	{
		/* Use CRC32 */
		crc32 = 0xFFFFFFFFL;
       		com_putbyte (port,ZBIN32);
       		com_putbyte (port,type);
		crc32 = updcrc32(type, crc32);
		for( n = 4; --n >= 0; ++hdr )
		{ ZSend_Byte(*hdr);crc32 = updcrc32((*hdr)&0xFF, crc32);}
		crc32 = ~crc32;
		for( n = 4; --n >= 0; )
		{ZSend_Byte((char)(crc32 & 0xFF));crc32 >>= 8;}
	}
	else /* CRC-16 */
	{
		crc = 0;
       		com_putbyte (port,ZBIN);
       		com_putbyte (port,type);
		crc = updcrc16(type, crc);
		for( n = 4; --n >= 0; ++hdr )
		{ ZSend_Byte (*hdr);crc = updcrc16((*hdr)&0xFF, crc);}
		crc = updcrc16(0,updcrc16(0,crc));
        ZSend_Byte  ((crc>>8)&0xFF);ZSend_Byte  (crc&0xFF);
 		
	}
	
	
	return(0);
}

/* ------------------------------------------------------------------------- */
/* Send ZMODEM HEX header hdr of type $type                                  */
/* ------------------------------------------------------------------------- */
int p_zmod::zshhdr(int type, char *hdr)
{
	int n;
	unsigned short crc;

	message("zshhdr: %d %lx (CRC16)",type, rclhdr(hdr));

	Z_Txcrc32 = 0;

        com_putbyte (port,ZPAD);com_putbyte (port,ZPAD);
        com_putbyte (port,ZDLE);com_putbyte (port,ZHEX);
	zputhex(type);

	crc = updcrc16(type, 0);
	for( n = 4; --n >= 0; ++hdr )
	{zputhex((*hdr));crc = updcrc16((*hdr)&0xFF, crc);}
		
	crc = updcrc16(0,updcrc16(0,crc));
	zputhex((crc>>8)&0xFF);zputhex(crc&0xff);

	/* Make it printable on remote machine */
         com_putbyte (port,'\r');com_putbyte (port,'\n');

	/* Uncork the remote in case a fake XOFF has stopped data flow */
	if( type != ZFIN && type != ZACK )  com_putbyte (port,XON);
	return(0);
}

/* ------------------------------------------------------------------------- */
/* Send binary array buf of length length, with ending ZDLE sequence frameend*/
/* $pos must point to current offset of sending file (for logging only)      */
/* ------------------------------------------------------------------------- */

int p_zmod::zsdata(const char *buf, int length, int frameend, long pos)
{
	int n;
	unsigned short crc,len;
	unsigned long crc32;

	len=length;
	if( Z_Txcrc32 )
	{
		crc32 = getcrc32ccitt((unsigned char *)buf,length);
		
		for( ; --length >= 0; ++buf ) ZSend_Byte(*buf);
                com_putbyte (port,ZDLE);com_putbyte (port,frameend);
		crc32 = updcrc32(frameend, crc32);
		crc32 = ~crc32;

	        message("zsdata: Len-%d Frame-%X (CRC32-%X) from %X pos", len,frameend,crc32, pos);
		for( n = 4; --n >= 0; )
		{ZSend_Byte((char)(crc32 & 0xFF));crc32 >>= 8;}
	}
	else /* CRC-16 */
	{
		crc = getcrc16((unsigned char *)buf,length);
		for( ; --length >= 0; ++buf ) ZSend_Byte(*buf);
        	com_putbyte (port,ZDLE);com_putbyte (port,frameend);
		crc = updcrc16(frameend, crc);
		crc = updcrc16(0, updcrc16(0, crc));
                message("zsdata: Len-%d Frame-%X (CRC-%X) from %X pos", len,frameend,crc, pos);
        	ZSend_Byte((crc>>8) & 0xFF);ZSend_Byte(crc & 0xFF); 
	}
	
	
	if( frameend == ZCRCW ) com_putbyte (port,XON);
	
	
	return(0);
}

/* ------------------------------------------------------------------------- */
/* Receive array buf of max length+1 with ending ZDLE sequence               */
/* and CRC.  Returns the ending character or error code.                     */
/* $pos must contatain current offset of receiving file (for logging only!)  */
/* ------------------------------------------------------------------------- */
int p_zmod::zrdata(char *buf, int length, long pos)
{
	return (Z_Rxframeind == ZBIN32) ? zrdat32(buf, length, pos)
	                                : zrdat16(buf, length, pos);
}

int p_zmod::zrdat16(char *buf, int length, long pos)
{
	
	unsigned short crc,c;
	char *end,*in;
	int d,n;
	int gotdle;

	Z_Rxcount = 0;
	
	gotdle=0;
	crc=0;
	in=buf;
	
	end = buf + length;
	
	while( buf <= end )
	{
		
switch_again:
	   if (gc.mailer.keyboard_break==1)  return (ZEXIT);
       if (!RS_Carrier(port)) return (ZHANGUP);
     
			switch( c=noxrd7()) 
			{
			case ZDLE:
				gotdle++;
				if (gotdle==5) return ZCAN;
				goto switch_again;
			case ZCRCE:
			case ZCRCG:
			case ZCRCQ:
			case ZCRCW:
			if (gotdle==0) break;
				d = c;
				Z_Rxcount = buf - in;
				crc=getcrc16((unsigned char *)in,Z_Rxcount);
				crc = updcrc16(c, crc);
					
                for( n = 2; --n >= 0; )
				{c=zdlread();
				crc = updcrc16(c, crc);}
					
				
				
				if( crc & 0xFFFF )
				{
					message("zrdat16: %d %d : Bad CRC",Z_Rxcount, d);
					return ZCRCERR;
				}
				message("zrdat16: %d %d (CRC16) at %d pos",Z_Rxcount, d, pos);
				return d;
				
				
			}

		if (gotdle) {c^=0x40;gotdle=0;}
		*buf++ = (char)c;

}
	
	message("zrdat16: data packet too long");
	
	return ZERROR;
}

int p_zmod::zrdat32(char *buf, int length, long pos)
{
	int n,d;
	unsigned long crc,c;
	char *end,*in;
	int gotdle;

	Z_Rxcount = 0;
	
	end = buf + length;
	gotdle=0;
	in=buf;
	
	while( buf <= end )
	{
switch_again:
		if (gc.mailer.keyboard_break==1)  return (ZEXIT);
        if (!RS_Carrier(port)) return (ZHANGUP);

     			switch( c = noxrd7() )
			{
			case ZDLE:
				gotdle++;
				if (gotdle==5) return ZCAN;
				goto switch_again;
			case ZCRCE:
			case ZCRCG:
			case ZCRCQ:
			case ZCRCW:
				if (gotdle==0) break;
				d = c;
                Z_Rxcount = buf - in;
				crc=getcrc32ccitt((unsigned char *) in ,Z_Rxcount);
				crc = updcrc32(c,crc);
				
                for( n = 4; --n >= 0; )
				{c = zdlread();crc = updcrc32(c, crc);}
				
				if( crc != 0xDEBB20E3 )
				{
				message("zrdat32: %d %d : Bad CRC",Z_Rxcount, d);
				return ZCRCERR;
				}

				message("zrdat32: %d %d (CRC32) at %d pos",Z_Rxcount, d, pos);
				return d;
				
			}
		if (gotdle)	
		{gotdle=0; c^=0x40;};
		
		*buf++ = (char)c;
	
		
	}
	
	message("zrdat32: data packet too long");
	
	return ZERROR;
}

/* ------------------------------------------------------------------------- */
/* Read a ZMODEM header to hdr, either binary or hex.                        */
/* On success, set Z_Rxpos and return type of header.                        */
/* Otherwise return negative on error.                                       */
/* Return ZERROR instantly if ZCRCW sequence, for fast error recovery.       */
/* ------------------------------------------------------------------------- */
int p_zmod::zgethdr(char *hdr)
{
	int c, n, cancount;

	n        = 8192;    /* Maximum number of garbage chars */
	cancount = 5;       /* CANcel on this can number */

	Z_Rxframeind = 0;
	Z_Rxtype     = 0;
	
again:
	if (gc.mailer.keyboard_break==1)  return (ZEXIT);
        if (!RS_Carrier(port)) return (ZHANGUP);

	switch(c =  noxrd7()) 
	{
	case CAN:
gotcan:
		if( --cancount == 0 ) {	c = ZCAN; goto fifi;}
		switch( c = noxrd7() ) 
		{
		case ZCRCW:
			c = ZERROR;
		case CAN:
			if( --cancount == 0 ) {c = ZCAN; goto fifi;}
			goto again;
		default:
			break;
		}
	case ZPAD|0200:		/* This is what we want. */
	case ZPAD:		/* This is what we want. */
		break;
	default:
		if( --n == 0 ) 	{c = ZERROR; goto fifi;}
		cancount = 5;
		goto again;
	}
	cancount = 5;
	
splat:
	switch( c = noxrd7() ) 
	{
	case ZPAD:
		goto splat;
	case ZDLE:		/* This is what we want. */
		break;
	default:
		if( --n == 0 )
		{
			message("zgethdr: garbage count exceeded");
			c = ZERROR; goto fifi;
		}
		goto again;
	}

	switch( c=noxrd7() ) {
	case ZBIN:
		/* receive binary header with crc16 */
		Z_Rxcrc32    = 0;
		Z_Rxframeind = ZBIN;
		c = zrbhdr16(hdr);
		break;
	case ZBIN32:
		/* receive binary header with crc32 */
		Z_Rxcrc32    = 1;
		Z_Rxframeind = ZBIN32;
		c = zrbhdr32(hdr);
		break;
	case ZHEX:
		/* receive hex header (all hex headers have crc16) */
		Z_Rxcrc32    = 0;
		Z_Rxframeind = ZHEX;
		c = zrhhdr(hdr);
		break;
	case CAN:
		goto gotcan;
	default:
		if( --n == 0 )
		{
			message("zgethdr: garbage count exceeded");
			c = ZERROR; goto fifi;
		}
		goto again;
	}
	
	if( c==ZRPOS)
	{
		Z_Rxpos = hdr[ZP3] & 0xFF ;
		Z_Rxpos = (Z_Rxpos<<8) + (hdr[ZP2] &0xFF);
		Z_Rxpos = (Z_Rxpos<<8) + (hdr[ZP1] &0xFF);
		Z_Rxpos = (Z_Rxpos<<8) + (hdr[ZP0] &0xFF);
	}

fifi:
    	if( c == GOTCAN ) c = ZCAN;
	message("zgethdr: Type-%d", c);
	return(c);
}

/* ------------------------------------------------------------------------- */
/* Receive a binary style header (type and position)                         */
/* ------------------------------------------------------------------------- */
int p_zmod::zrbhdr16(char *hdr)
{
	int c, n;
	unsigned short crc;

	c = zdlread();
	Z_Rxtype = c;
	crc = updcrc16(c, 0);

	for( n = 4; --n >= 0; ++hdr )
	{
		c = zdlread();
		crc = updcrc16(c, crc);
		*hdr = c;
	}
	
	c = zdlread();
	crc = updcrc16(c, crc);
	c = zdlread();
	crc = updcrc16(c, crc);
	
	if( crc & 0xFF )
	{
		message("zrbhdr: Bad CRC");
		return(ZCRCERR);
	}
	
	return(Z_Rxtype);
}

/* ------------------------------------------------------------------------- */
/* Receive a binary style header (type and position) with 32 bit FCS         */
/* ------------------------------------------------------------------------- */
int p_zmod::zrbhdr32(char *hdr)
{
	int n;
	unsigned long crc,c;

	Z_Rxtype = c= zdlread();
	crc = 0xFFFFFFFFL;
	crc = updcrc32(c, crc);

	for( n = 4; --n >= 0; ++hdr )
	{
		c = zdlread();
		crc = updcrc32(c, crc);
		*hdr = (char)c;
	}
	
	for( n = 4; --n >= 0; )
	{
		c = zdlread();
		crc = updcrc32(c, crc);
	}
	
	if( crc != 0xDEBB20E3 )
	{
		message("zrbhdr32: Bad CRC");
		return(ZCRCERR);
	}
	
	return(Z_Rxtype);
}


/* ------------------------------------------------------------------------- */
/* Receive a hex style header (type and position)                            */
/* ------------------------------------------------------------------------- */
int p_zmod::zrhhdr(char *hdr)
{
	unsigned int c;
	unsigned short crc;
	int n;
	
	c = zgethex();
        
	Z_Rxtype = c; crc = updcrc16(c, 0);

	for( n = 4; n > 0;n-- )
	{
		c = zgethex();
		crc = updcrc16(c, crc);
		*hdr++ = c;
	}

	n = zgethex();
    crc = updcrc16(n, crc);
	
	c = zgethex();
    crc = updcrc16(c, crc);

	if( crc & 0xFF)
	{
		message("zrhhdr: Bad CRC");
		return(ZCRCERR);
	}
	
	return(Z_Rxtype);
}

/* ------------------------------------------------------------------------- */
/* Write a byte as two hex digits                                            */
/* ------------------------------------------------------------------------- */
int p_zmod::zputhex(int c)
{
	static char digits[] = "0123456789abcdef";
        ZSend_Byte(digits[(c&0xF0)>>4]);
        ZSend_Byte(digits[(c&0x0F)]);
	return 2;
	
}


/* ------------------------------------------------------------------------- */
/* Decode two lower case hex digits into an 8 bit byte value                 */
/* ------------------------------------------------------------------------- */
unsigned int p_zmod::zgethex(void)
{
	unsigned int c, n;
    	n = zdlread() ;
	if( (n >='0') && (n<='9') ) n -= '0';
	if( (n >='a') && (n<='f') ) n -= 'a'-10;
	if( n & ~0xF ) return(ZERROR);

	c = zdlread() ;
	if( (c >='0') && (c<='9') ) c -= '0';
	if( (c >='a') && (c<='f') ) c -= 'a'-10;
	if( c & ~0xF ) return(ZERROR);
	
	c += (n<<4);
	
	return(c);
}

/* ------------------------------------------------------------------------- */
/* Store long integer pos in hdr                                             */
/* ------------------------------------------------------------------------- */
void p_zmod::stohdr(char *hdr, long pos)
{
	hdr[ZP0] = (char)(pos & 0xFF   ) ;
	hdr[ZP1] = (char)(pos>>8 & 0xFF) ;
	hdr[ZP2] = (char)(pos>>16 & 0xFF) ;
	hdr[ZP3] = (char)(pos>>24 & 0xFF) ;
}

/* ------------------------------------------------------------------------- */
/* Recover a long integer from a header                                      */
/* ------------------------------------------------------------------------- */
long p_zmod::rclhdr(char *hdr)
{
	long l;

	l = (hdr[ZP3] & 0xFF);
	l = (l << 8) | (hdr[ZP2] & 0xFF);
	l = (l << 8) | (hdr[ZP1] & 0xFF);
	l = (l << 8) | (hdr[ZP0] & 0xFF);
	return(l);
}




/* ------------------------------------------------------------------------- */
/* Receive files with Z-Modem protocol                                       */
/* Return:                                                                   */
/*  PRC_SUCCESSFUL: All files received or transfer was finished cleanly      */
/*  PRC_ERROR: Error at sending something(include carrier lost)              */
/*  PRC_ERROR: Error at receiving something(include carrier lost)            */
/*  PRC_MERROR: Misc. errors (i.e. incompatibilies, failures, etc)           */
/*  PRC_TRIESOUT: Very many tries doing something without result             */
/*  PRC_REMOTEDEAD: For a long time no real information transfered           */
/*  PRC_REMOTEABORT: Transfer was aborted by remote (i.e. five CAN, etc..)   */
/*  PRC_LOCALBORT: Transfer was aborted by local (i.e. CPS too low, etc..)   */
/* ------------------------------------------------------------------------- */
	int p_zmod::rx_zmodem(void)
/* ------------------------------------------------------------------------- */
{
char  zconv;        /* ZMODEM file conversion request              */
char  zmanag;       /* ZMODEM file management request              */
char  ztrans;       /* ZMODEM file transport request               */
char  zexten;       /* ZMODEM file extended options                */
char *rxbuf;        /* Buffer with ZMAXBLOCKLEN size               */
int   zrqinitcnt;   /* Count received ZRQINITs                     */
int   zfincnt;      /* Count received ZFINs (in state ZTX_INITACK) */
int   inithdrtype;  /* Send this header at ZRX_INIT state          */
int   txtries = 0;
int   skipbypos = 0;
int   ftype = 0;
enum zrxstates rxstate;
time_t deadtimer;
int   rc = PRC_NOERROR;
int   n, c;
char *p;
	
	
	rxstart = 0;
	rxbuf          = (char *)malloc(ZMAXBLOCKLEN+1);
	zrqinitcnt     = 0;
	zfincnt        = 0;
	inithdrtype    = ZRINIT;
	rxstate        = ZRX_INIT;
	rxtecsize	=0;		
	Z_Lastsent=0;

	xfer_logged=FALSE;
	deadtimer=timer_set(ZDEADTIMER);

	while(1)
	{
		if( timer_expired(deadtimer) && 0)
		{
			message("rx_zmodem: deadtimer = %ld", (long)deadtimer);
			message("rx_zmodem: rxstate = %d", (int)rxstate);
			message("rx_zmodem: rc = %d", rc);
			message("rx_zmodem: txtries = %d", txtries);
			message("brain dead! (abort)");
			gotoexit(PRC_LOCALABORTED);
		}
		
		if( rxstate == ZRX_INIT || rxstate == ZRX_SENDRPOS )
		{
			if( ++txtries > ZMAXTRIES )
			{
				message("tries reached miximal count");
				gotoexit(PRC_LOCALABORTED);
			}
		}
		
		switch(rxstate) {
		case ZRX_INIT:
			message("rx_zmodem: entering state ZRX_INIT");
			stohdr(Z_Txhdr, 0L);
			
			Z_Txhdr[ZF0] =  CANFC32 | CANOVIO | CANFDX;
			zshhdr(inithdrtype, Z_Txhdr);
			if( inithdrtype == ZSKIP ) inithdrtype = ZRINIT;
			rxstate = ZRX_INITACK;
			break;
			
		case ZRX_GOTFILE:
			message("rx_zmodem: entering state ZRX_GOTFILE");
			switch(zmod_file(rxbuf, Z_Rxcount)) {
			case 0:
				txtries = 0;
				rxstate = ZRX_SENDRPOS;
				break;
			case 2:
				rxstate = ZRX_SKIP;
				break;
			default:
				rxstate = ZRX_REFUSE;
				break;
			}
			break;
			
		case ZRX_SENDRPOS:
			message("rx_zmodem: entering state ZRX_SENDRPOS");
			stohdr(Z_Txhdr,Z_Rxpos);
			if( zshhdr(ZRPOS, Z_Txhdr)<0) gotoexit(PRC_ERROR);
			rxstate = ZRX_RPOSACK;
			break;
			
		case ZRX_WAITDATA:
			if( rxstart == 0 )	rxstart = time(NULL);
			message("rx_zmodem: entering state ZRX_WAITDATA");
			
			//Пропускаем файл
			if (gc.mailer.skip_file)
			{
				com_putblock (port,(PUCHAR) &Z_Attn[0],ZATTNLEN);
				txtries = 0;
				Z_Rxpos= ((gc.mailer.skip_file==-1) ? -1:-2);
				//Возможно тут нужна другая строка
				// rxstate =ZRX_SENDRPOS;
				rxstate=(gc.mailer.skip_file==-1) ? ZRX_SKIP : ZRX_REFUSE; 
				break;		
			}
			
			switch(c = zrdata(rxbuf, ZMAXBLOCKLEN, Z_Rxpos)) {
			case ZHANGUP:
				gotoexit(PRC_ERROR);
				break;
			case ZEXIT:
			gotoexit(PRC_LOCALABORTED);
				break;
			case ZCAN:
				gotoexit(PRC_REMOTEABORTED);
				break;
			case ZTIMER:
				message("time out waiting for data");
				txtries = 0;
				rxstate = ZRX_SENDRPOS;
				break;
			case ZERROR:
			case ZCRCERR:	/* CRC error */
				com_putblock (port, (unsigned char *) &Z_Attn[0],ZATTNLEN);
				txtries = 0;
				rxstate = ZRX_SENDRPOS;
				break;
			case ZCRCW:
			case ZCRCQ:
			case ZCRCG:
			case ZCRCE:
				deadtimer=timer_set(ZDEADTIMER);
				if( (n = fwrite(rxbuf, 1,Z_Rxcount,rxfd)) <= 0 )
				{
					rxstate = ( n == -2 ) ? ZRX_SKIP : ZRX_REFUSE;
					break;
				}
				Z_Rxpos = ftell(rxfd);
		                zm_status(FALSE);
				switch(c) {
				case ZCRCW:
					stohdr(Z_Txhdr, Z_Rxpos);
					zshhdr(ZACK, Z_Txhdr);
					rxstate = ZRX_RPOSACK;
					break;
				case ZCRCQ:
					stohdr(Z_Txhdr, Z_Rxpos);
					zshhdr(ZACK, Z_Txhdr);
					break;
				case ZCRCG:
					break;
				case ZCRCE:
					txtries = 0;
					rxstate = ZRX_RPOSACK;
					break;
				}
				break;
			}
		        
			break;

			
		case ZRX_REFUSE:
			message("rx_zmodem: entering state ZRX_REFUSE");
			txtries = 0;
                	rxtecsize+= rxfsize;
			inithdrtype = ZFERR;
			rxstate = ZRX_INIT;
			break;
			
		case ZRX_SKIP:
			message("rx_zmodem: entering state ZRX_SKIP");
			txtries = 0;
			rxtecsize+= rxfsize;
			inithdrtype = ZSKIP;
			rxstate = ZRX_INIT;
			break;
			
		default:; /* Ignore */
		} /* end of switch(rxstate) */   
		
		
		if( rxstate != ZRX_INIT     && rxstate != ZRX_GOTFILE
		 && rxstate != ZRX_SENDRPOS && rxstate != ZRX_WAITDATA
		 && rxstate != ZRX_SKIP     && rxstate != ZRX_REFUSE )
		{
			switch( ftype = zgethdr(Z_Rxhdr) ) {
			case ZCAN:
				gotoexit(PRC_REMOTEABORTED);
				break;
				
			case ZHANGUP:
			case ZEXIT:
				gotoexit(PRC_ERROR);
				break;
			
			case ZTIMER:
				message("time out");
				switch(rxstate) {
				case ZRX_INITACK: rxstate = ZRX_INIT;     break;
				case ZRX_RPOSACK: rxstate = ZRX_SENDRPOS; break;
				default:;
				}
				break;
				
			case ZERROR:
			case ZCRCERR:
				/* NAK them all! (TODO: think a little) */
				stohdr(Z_Txhdr, 0L);
				zshhdr(ZNAK, Z_Txhdr);
				break;
				
			case ZRINIT:
			case ZRQINIT:
				if( rxstate == ZRX_INITACK )
				{
					zrqinitcnt++;
					rxstate = ZRX_INIT;
				}
				break;
				
			case ZFILE:
				if( rxstate == ZRX_INITACK || rxstate == ZRX_RPOSACK )
				{
					zconv  = Z_Rxhdr[ZF0];
					zmanag = Z_Rxhdr[ZF1];
					ztrans = Z_Rxhdr[ZF2];
					zexten = Z_Rxhdr[ZF3];
			
					/* default to "binary" mode */
					if( !zconv ) zconv = ZCBIN;
			
					inithdrtype = ZRINIT;
					
					switch(zrdata(rxbuf, ZMAXBLOCKLEN, 0)) 
					{
					case ZCAN:
						gotoexit(PRC_REMOTEABORTED);
						break;
					case ZHANGUP:
					case ZEXIT:
						gotoexit(PRC_ERROR);
						break;
					case ZTIMER:
						txtries = 0;
						rxstate = ZRX_INIT;
						break;
					case ZCRCW:
						rxstate = ZRX_GOTFILE;
						break;
					default:
						stohdr(Z_Txhdr, 0L);
						zshhdr(ZNAK, Z_Txhdr);
						rxstate = ZRX_INITACK;
						break;
					};
				} ;
				break;
				
			case ZSINIT:
				if( rxstate == ZRX_INITACK )
				{
					Z_Ctlesc  = TESCCTL & Z_Rxhdr[ZF0];
					switch( zrdata(Z_Attn, ZATTNLEN, 0) ) {
					case ZHANGUP:
					case ZEXIT:
						gotoexit(PRC_ERROR);
						break;
					case ZTIMER:
						txtries = 0;
						rxstate = ZRX_INIT;
						break;
					case ZCRCW:
						stohdr(Z_Txhdr, 1L);
						zshhdr(ZACK, Z_Txhdr);
						rxstate = ZRX_INIT;
						break;
					default:
						stohdr(Z_Txhdr, 0L);
						zshhdr(ZNAK, Z_Txhdr);
						break;
					}
				}
				break;

			case ZNAK:
				switch(rxstate) {
				case ZRX_INITACK: rxstate = ZRX_INIT;      break;
				case ZRX_RPOSACK: rxstate = ZRX_SENDRPOS;  break;
				;
				}
				break;
				
			case ZEOF:
				if( rxstate == ZRX_RPOSACK )
				{
				if( !fclose(rxfd) )
				{rxstate = ZRX_INIT;inithdrtype = ZRINIT;}
				else rxstate = ZRX_REFUSE;
				}
				p = xfer_okay();
                if (p) 
                {message("  -> %s",p);
                 message("+HRECV: Dup file renamed: %s",p);};

				rxtecsize += rxfsize;
				break;
			
			case ZSKIP:
				if( rxstate == ZRX_RPOSACK )
				{
					message("remote side skipped file");
					rxstate = ZRX_INIT;
				}
				break;
				
			case ZDATA:
				if( rxstate == ZRX_RPOSACK )
				{
					if( rclhdr(Z_Rxhdr) == Z_Rxpos )
					{
						rxstate = ZRX_WAITDATA;
					}
					else
					{
						message("out of sync");
						com_putblock (port, (PUCHAR) &Z_Attn[0],ZATTNLEN);
						txtries = 0;
						rxstate = ZRX_SENDRPOS;
					}
				}
				break;
				
			case ZFREECNT:
				if( rxstate == ZRX_INITACK )
				{
					stohdr(Z_Txhdr, ~0L);
					zshhdr(ZACK, Z_Txhdr);
				}
				break;
				
			case ZCOMMAND:
				if( rxstate == ZRX_INITACK )
				{
					inithdrtype = ZRINIT;
					
					switch( zrdata(rxbuf, ZMAXBLOCKLEN, 0) ) {
					case ZHANGUP:
					case ZEXIT:
						gotoexit(PRC_ERROR);
						break;
					case ZTIMER:
						txtries = 0;
						rxstate = ZRX_INIT;
						break;
					case ZCRCW:
						message("command: \"%s\"", rxbuf);
						stohdr(Z_Txhdr, 0L);
						zshhdr(ZCOMPL, Z_Txhdr);
						gotoexit(PRC_LOCALABORTED);
						break;
					default:
						stohdr(Z_Txhdr, 0L);
						zshhdr(ZNAK, Z_Txhdr) ;
						txtries = 0;
						rxstate = ZRX_INIT;
						break;
					}
				}
				break;
				
			case ZFIN:
				if( zrqinitcnt )
				{
					stohdr(Z_Txhdr, 0L);
					zshhdr(ZFIN, Z_Txhdr);
					gotoexit(PRC_NOERROR);
				}
				else if( rxstate == ZRX_INITACK )
				{
					/*
					 * Don't believe first ZFIN on outgoing calls
					 */
					if( ++zfincnt > ZRXSKIPFIN || !RS_Carrier(port) )
					{
						stohdr(Z_Txhdr, 0L);
						zshhdr(ZFIN, Z_Txhdr);
						gotoexit(PRC_NOERROR);
					}
					rxstate = ZRX_INIT;
				}
				break;
				
			default:
				message("got unexpected frame %d", ftype);
				break;
			}
		}
	} /* end of while(1) */

exit:
	if (rc!=PRC_NOERROR)
        pr_badxfer();
	message("rx_zmodem: RECV exit = %d", rc);
	if( rxfd )	fclose(rxfd);
	if( rxbuf ) free(rxbuf);
	clear_status(FALSE);
	return(rc);
}


/* ------------------------------------------------------------------------- */
/* Process incoming file information header                                  */
/* ------------------------------------------------------------------------- */
int p_zmod::zmod_file(char *blkptr, size_t blklen)
{
char  *fileiptr = NULL;
int rc;
char temp[255];

	rxfsize = 0L;
	rxftime = 0L;
	blkptr[blklen] = '\0';
	fileiptr = blkptr + strlen(blkptr) + 1;
	if( rxfd )	fclose(rxfd);
	
	if( fileiptr >= (blkptr + blklen) ||
	    sscanf(fileiptr, "%d%lo", &rxfsize, &rxftime) < 1 )
	{
		message("zmodem: got invalid ZFILE packet");
		return 1;
	}
	strcpy(rxfname,blkptr);Z_Rxpos=0;
	message("Recieve: %s size=%ld,time=%lo",rxfname,(long)rxfsize, (long)rxftime);
        rc=xfer_init(rxfname,rxfsize,rxftime);
	if (rxallsize && (rxallsize<rxtecsize+rxfsize)) rxallsize=0;

        if (!access(xfer_pathname,0)) 
        { 
           if ((rxfd = fopen(xfer_pathname,"a+b")) <= 0) 
           {
			   message("!HRECV: Unable to re-open %s",xfer_pathname);
			   Z_Rxpos=-2;
		   }
         }
         else if ((rxfd = fopen(xfer_pathname,"wb")) <= 0) 
         { 
			 message("!HRECV: Unable to create %s",xfer_pathname);
			 Z_Rxpos=-2;
		 }
        if (rxfd) 
        {
			fseek(rxfd,0,SEEK_END);
			Z_Rxpos=ftell(rxfd); 
			show_size(temp,rxfsize);
			modem_add_listbox(L("S_595",rxfname,temp));	// << File: %s %s
		}

	return rc;
}

/* ------------------------------------------------------------------------- */
/* Send files with Z-Modem protocol                                          */
/* Files to transfer stored in *sendlist char                                */
/* Return:                                                                   */
/*  PRC_SUCCESSFUL: All files received or transfer was finished cleanly      */
/*  PRC_ERROR: Error at sending something(include carrier lost)              */
/*  PRC_ERROR: Error at receiving something(include carrier lost)            */
/*  PRC_MERROR: Misc. errors (i.e. incompatibilies, failures, etc)           */
/*  PRC_TRIESOUT: Very many tries doing something without result             */
/*  PRC_REMOTEDEAD: For a long time no real information transfered           */
/*  PRC_REMOTEABORT: Transfer was aborted by remote (i.e. five CAN, etc..)   */
/*  PRC_LOCALBORT: Transfer was aborted by local (i.e. CPS too low, etc..)   */
/* ------------------------------------------------------------------------- */
int p_zmod::tx_zmodem(char *sendlist)
{
	int   startblk  = 64;  /* Initial Zmodem block size                  */
	int   minblk    = 64;  /* Minimal Z-protocol block size              */
	int   maxblk    = 1024;/* Maximal Z-protocol block size              */
	int   blocklen  = 0;   /* Length of transmitted blocks               */
	int   goodblk   = 0;   /* How many blocks we sent w/o ZRPOS'tion :)  */
	int   txwindow  = 0;   /* Tranmitter window size (0 means streaming) */
	int   newcnt    = 0;   /* Count free bytes in receiver's buffer      */
	int   rxbuflen  = 0;   /* Receiver's max buffer length               */
	int   rxlastpos = 0;   /* Receiver's last reported offset            */
	int   beenhere  = 0;   /* How many times we've been ZRPOS'd same place */
	char  zconv     = 0;   /* Local ZMODEM file conversion request       */
	char  zmanag    = 0;   /* Local ZMODEM file management request       */
	char  ztrans    = 0;   /* Local ZMODEM file translation request      */
	char  zexten    = 0;   /* Local ZMODEM file extended options         */
	char *txbuf     = NULL;/* Buffer with ZMAXBLOCKLEN size              */
	int   zrinitcnt = 0;   /* Count received ZRINITs                     */
	int   rxflags1  = 0;
	int   rxflags2  = 0;
	int   txtries   = 0;
	int   junkcnt   = 0;
	int   initacked = 0;   /* TRUE when at least one ZRQINIT was sent    */
	                       /* after first ZRINIT was received            */
	int   rc        = 0;   /* Our return code                            */
	int   dtype, n;
	int   ftype;
	char  c, *p;
	long unsigned crc32;
	enum  ztxstates txstate;
	time_t deadtimer;

        int index;
					
	/* Set time transfer started at */
	if( txstart == 0 ) txstart = time(NULL);
	
	index=0;
	Z_Lastsent=0;
	txbuf      = (char *)malloc(ZMAXBLOCKLEN+1);
	zconv      = ZCBIN;
	maxblk     = 8192;
	txtecsize	=0;
	
	/* Set initial block size (default is 128b) */
	startblk = 512;
	
	blocklen  = startblk;
	txwindow  = 0;
	txstate   = ZTX_START;

	deadtimer=timer_set(ZDEADTIMER);
	char tempbuf2[255],sbuf[255];
	
/*
 * At zmodem batches send empty netmail packet
 * if no real outgoing traffic available
 */
	while(1)
	{
		if( timer_expired(deadtimer) && 0)
		{
			message("brain dead! (abort)");
			gotoexit(PRC_LOCALABORTED);
		}
		
		if( txstate == ZTX_RQINIT || txstate == ZTX_FINFO
		 || txstate == ZTX_EOF    || txstate == ZTX_FIN )
		{
			if( txtries ) message("tx_zmodem: try #%d", txtries);
			if( ++txtries > ZMAXTRIES )
			{
				message("out of tries");
				gotoexit(PRC_LOCALABORTED);
			}
		}
		
		switch(txstate) {
		case ZTX_START:
			message("tx_zmodem: entering state ZTX_START");
		        com_putblock (port,(PUCHAR) autostr ,4);
			txtries = 0;
			txstate = ZTX_RQINIT;
			break;
			
		case ZTX_RQINIT:
			message("tx_zmodem: entering state ZTX_RQINIT");
			stohdr(Z_Txhdr, 0L);
			zshhdr(ZRQINIT, Z_Txhdr);
			txstate = ZTX_RQINITACK;
			break;
			
		case ZTX_NEXTFILE:
			message("tx_zmodem: entering state ZTX_NEXTFILE");
			index++;
            next_txpathname(sendlist,index);
			txtries = 0;
			txstate = txfd ?  ZTX_FINFO:ZTX_FIN;
			break;

		case ZTX_FINFO:
			message("tx_zmodem: entering state ZTX_FINFO");
			zrinitcnt = 0;
            strupr(txfname);
			strncpy(txbuf, txfname, ZMAXFNAME);
            show_size(tempbuf2,txfsize);
            modem_add_listbox(L("S_129",txfname,tempbuf2));	// >> File: %s %s

			
			p = txbuf + strlen(txbuf) + 1; 
			sprintf(p, "%ld %lo %lo 0 %ld %ld",(long) txfsize, (long) txftime,(long) 0, (long) 0,(long) 0);
			
			Z_Txhdr[ZF0] = zconv;	/* file conversion request */
			Z_Txhdr[ZF1] = zmanag;  /* file management request */
			Z_Txhdr[ZF2] = ztrans;  /* file transport request  */
			Z_Txhdr[ZF3] = zexten;
			
			zsbhdr(ZFILE,Z_Txhdr);
			zsdata(txbuf,(p-txbuf)+strlen(p),ZCRCW, 0);
			txstate = ZTX_FINFOACK;
			break;
			
		case ZTX_STARTDATA:
			message("tx_zmodem: entering state ZTX_STARTDATA");
			newcnt   = rxbuflen;
			junkcnt  = 0;
			stohdr(Z_Txhdr,Z_Txpos);
			zsbhdr(ZDATA, Z_Txhdr);
			txstate = ZTX_DATA;
			break;
			
		case ZTX_DATA:
			message("tx_zmodem: entering state ZTX_DATA");
			deadtimer=timer_set(ZDEADTIMER);
			if( (n = fread(txbuf, 1,blocklen, txfd)) < 0 )
			{
				/* error occured, remote wait for DATA */
				/* so send null ZCRCE data subpacket   */
				if( zsdata(txbuf,0,ZCRCE,0)<0) gotoexit(PRC_ERROR);
				txstate = ZTX_NEXTFILE;
				break;
			}
			if(n!=blocklen )
				dtype = ZCRCE;
			else if( junkcnt > 6 )
				dtype = ZCRCW;
			else if( rxbuflen && (newcnt -= n) <= 0 )
				dtype = ZCRCW;
			else
				dtype = ZCRCG;

			
			zsdata(txbuf,n,dtype,Z_Txpos);
			
			if( ++goodblk > 5 && blocklen*2 <= maxblk )
			{
				goodblk = 0;
				blocklen *= 2;
				message("tx_zmodem: new blocklen = %ld byte(s)", blocklen);
			}
			
				Z_Txpos = ftell( txfd);
		        zm_status(TRUE);
			
			if( dtype == ZCRCW )
			{
				junkcnt = 0;
				txstate = ZTX_CRCWACK;
				break;
			}
			else if( dtype == ZCRCQ )
			{
				junkcnt = 0;
				txstate = ZTX_CRCQACK;
				break;
			}
			else if( dtype == ZCRCE )
			{
				txtries = 0;
				txstate = ZTX_EOF;
				break;
			}
			
			break;
		
		case ZTX_EOF:
			message("tx_zmodem: entering state ZTX_EOF");
			stohdr(Z_Txhdr, Z_Txpos);
			zsbhdr(ZEOF, Z_Txhdr);
			txstate = ZTX_EOFACK;
			break;
			
		case ZTX_FIN:
			message("tx_zmodem: entering state ZTX_FIN");
			stohdr(Z_Txhdr, 0L);
			zshhdr(ZFIN, Z_Txhdr);
			txstate = ZTX_FINACK;
			break;
			
		default:
			/* Ignore them all */
			break;
		} /* end of switch(txstate) */
	
		if( txstate != ZTX_START  && txstate != ZTX_RQINIT
		 && txstate != ZTX_FINFO  && txstate != ZTX_DATA
		 && txstate != ZTX_EOF    && txstate != ZTX_FIN )
		{
			switch( ftype = zgethdr(Z_Rxhdr) ) {
			case ZCAN:
				gotoexit(PRC_REMOTEABORTED);
				break;
				
			case ZHANGUP:
			case ZEXIT:
				gotoexit(PRC_ERROR);
				break;
				
			case ZTIMER:
				message("time out");
				if(txstate == ZTX_READCHECK) zsdata(txbuf,0,ZCRCE,0);
				switch(txstate) {
				case ZTX_RQINITACK: txstate = ZTX_RQINIT;    break;
				case ZTX_FINFOACK:  txstate = ZTX_FINFO;     break;
				case ZTX_READCHECK: txstate = ZTX_STARTDATA; break;
				case ZTX_CRCWACK:   txstate = ZTX_STARTDATA; break;
				case ZTX_CRCQACK:   txstate = ZTX_STARTDATA; break;
				case ZTX_EOFACK:    txstate = ZTX_EOF;       break;
				case ZTX_FINACK:    txstate = ZTX_FIN;       break;
				default:;
				}
				break;
				
			case ZERROR:
			case ZCRCERR:
				/* NAK them all! */
				stohdr(Z_Txhdr, 0L);
				zshhdr(ZNAK, Z_Txhdr);
				break;
				
			case ZRQINIT:
				if( txstate == ZTX_RQINITACK )
				{
				 if( Z_Rxhdr[0] == ZCOMMAND ) break;
				 stohdr(Z_Txhdr, 0L);
				 zshhdr(ZNAK,Z_Txhdr);
				 txstate = ZTX_RQINIT;
				}
				else if( txstate == ZTX_FINFOACK )
				{
					/* remote is sender - abort */
					message("zmodem: remote is sender");
					gotoexit(PRC_LOCALABORTED);
				}
				break;
				
			case ZRINIT:
				if( txstate == ZTX_RQINITACK )
				{
					if( initacked == 0 )
					{
					/* Be sure ack first ZRINIT */
					stohdr(Z_Txhdr, 0L);
					zshhdr(ZRQINIT,Z_Txhdr);
					initacked = 1;
					}
					
					/* Get receiver's options */
					rxflags1  = ( Z_Rxhdr[ZF0]);
					rxflags2  = ( Z_Rxhdr[ZF1]);
					Z_Txfcs32 = (rxflags1 & CANFC32);
					Z_Ctlesc |= (rxflags1 & TESCCTL);
					rxbuflen  = ( Z_Rxhdr[ZP0]);
					rxbuflen += ( Z_Rxhdr[ZP1]<<8);
					
					/* No ZCRCQ if remote doesn't indicate */
					/* FDX ability                         */ 
					txwindow = 0;
					message("tx_zmodem: Z_Txfcs32 = %d Z_Ctlesc = %d",Z_Txfcs32, Z_Ctlesc);
					message("tx_zmodem: rxbuflen = %d blocklen = %d",rxbuflen, blocklen);
				
					txstate   = ZTX_NEXTFILE;
				}
				else if( txstate == ZTX_FINFOACK )
				{
					/* Possible they didn't see */
					/* our file information     */
					if( ++zrinitcnt > 2 )
						txstate = ZTX_FINFO;
				}
				else if( txstate == ZTX_READCHECK
				      || txstate == ZTX_CRCQACK
				      || txstate == ZTX_CRCWACK )
				{
					if( txstate == ZTX_READCHECK
					 || txstate == ZTX_CRCQACK )
						zsdata(txbuf, 0, ZCRCE, 0);
					
					/* Assume file normaly sent ? */
					message("assume file normaly sent");
					
					txstate = ZTX_NEXTFILE;
				}
				else if( txstate == ZTX_EOFACK )
				{
					/* ok, send next */
					txstate = ZTX_NEXTFILE;
                                      if (txfd > 0)
                                	{
                                   	if (txfd) fclose(txfd);
                                   	txfd=NULL;
                                   	clear_status(true);
                                   	//Стираем файл если он из OUTBOUND
                                        make_path(sbuf,gc.OutboundPath);
				        //Проверка на реквест
					DelReqFile(txfname);
                                   	//Проверка на атач
					DelXferFile(txfname);

                                        if (strstr(txpathname,sbuf))
                                        DeleteFile(txpathname);
                                 	}
                                         txtecsize += txfsize;
 				}
				else if( txstate == ZTX_FINACK )
				{
					/* Possible we should ignore  */
					/* first ZRINIT. Because they */
					/* didn't see our first ZFIN  */
					/* But I'm soo lazy .. :))    */
					txstate = ZTX_FIN;
				}
				break;
				
			case ZACK:
				if( txstate == ZTX_CRCWACK )
				{
						txstate = ZTX_STARTDATA;
				}
				else if( txstate == ZTX_READCHECK
				      || txstate == ZTX_CRCQACK )
				{
					txstate   = ZTX_DATA;
				}
				break;
				
			case ZSKIP:
				if( txstate == ZTX_FINFOACK
				 || txstate == ZTX_READCHECK
				 || txstate == ZTX_CRCQACK
				 || txstate == ZTX_CRCWACK
				 || txstate == ZTX_EOFACK )
				{
					if( txstate == ZTX_READCHECK
					 || txstate == ZTX_CRCQACK )
						zsdata(txbuf, 0, ZCRCE, 0);
					
					
					message("remote side skipped file");
					
					txstate = ZTX_NEXTFILE;
				}
				break;
				
			case ZFIN:
				/* BUG!BUG!BUG!BUG!BUG!BUG!BUG!BUG!BUG! */
				/* BUG!BUG!BUG!BUG!BUG!BUG!BUG!BUG!BUG! */
				/* BUG!BUG!BUG!BUG!BUG!BUG!BUG!BUG!BUG! */
				if( txstate == ZTX_FINACK )
				{
				        com_putblock (port, (unsigned char *) &"OO",2);
					gotoexit(PRC_NOERROR);
				}
				break;
				
			case ZRPOS:
				if( txstate == ZTX_FINFOACK
				 || txstate == ZTX_READCHECK
				 || txstate == ZTX_CRCQACK
				 || txstate == ZTX_CRCWACK
				 || txstate == ZTX_EOFACK )
				{
					
				if( txstate == ZTX_READCHECK || txstate == ZTX_CRCQACK )
				zsdata(txbuf, 0, ZCRCE, 0);
				/* Check pos */
                 fseek(txfd, Z_Rxpos, 0);                 
 				 Z_Txpos=ftell(txfd);
		 		 txstate = ZTX_STARTDATA;
				}
				break;
				
			case ZNAK:
				switch(txstate) {
				case ZTX_RQINITACK: txstate = ZTX_RQINIT; break;
				case ZTX_FINFOACK:  txstate = ZTX_FINFO;  break;
				case ZTX_EOFACK:    txstate = ZTX_EOF;    break;
				case ZTX_FINACK:    txstate = ZTX_FIN;    break;
				default: ;
				}
				break;
				
			case ZCRC:
				if( txstate == ZTX_FINFOACK )
				{
					/* Send file's CRC-32 */
					crc32 = 0xFFFFFFFFL;
					
					while( ((c = getc(txfd)) != EOF) && --Z_Rxpos )
						crc32 = updcrc32(c, crc32);
					crc32 = ~crc32;
					fseek(txfd, 0L, 0);
					stohdr(Z_Txhdr, crc32);
					zsbhdr(ZCRC, Z_Txhdr);
				}
				break;
				
			case ZCHALLENGE:
				if( txstate == ZTX_RQINITACK )
				{
					/* Echo receiver's challenge number */
					stohdr(Z_Txhdr, Z_Rxpos);
					zshhdr(ZACK, Z_Txhdr);
					txstate = ZTX_RQINIT;
				}
				break;
				
			case ZCOMMAND:
				if( txstate == ZTX_RQINITACK ) txstate = ZTX_RQINIT;
				break;

			case ZABORT:
				message("remote requested for session abort");
				stohdr(Z_Txhdr, 0L);
				zshhdr(ZFIN, Z_Txhdr);
				gotoexit(PRC_REMOTEABORTED);
				break;
				
			case ZFERR:
				if( txstate == ZTX_FINFOACK
				 || txstate == ZTX_READCHECK
				 || txstate == ZTX_CRCWACK
				 || txstate == ZTX_CRCQACK
				 || txstate == ZTX_EOFACK )
				{
					if( txstate == ZTX_READCHECK || txstate == ZTX_CRCQACK )
					zsdata(txbuf,0,ZCRCE,0);
					message("remote side refused file");
					txstate = ZTX_NEXTFILE;
				}
				break;
				
			default:
				message("got unexpected frame %d", ftype);
				break;
			} /* end of switch(hdr) */
		} /* end of if */
	} /* end of while */
	
exit:
	message("tx_zmodem: SEND exit = %d", rc);
	
	if( txfd ) fclose(txfd);
	if( txbuf ) free(txbuf);
	clear_status(TRUE);
	
	return(rc);
}

/*==========================================================================*/
//   Сессия протокола зетмдем
/*==========================================================================*/
int p_zmod::zmod_batch (char *sendlist,char *emsibuf)
{  
int result;
char sbuf[255];

	port=gModemCfg.m_comport-1;
	mailer_stat(langstr("Session ZModem","Сессия ZModem"));
	update_charging();
	modem_update();
	sprintf(sbuf,langstr("== Start ZModem session","== Начало сессии ZModem"));
	modem_add_listbox(sbuf);
	rxallsize=resdata(emsibuf);
	txallsize=senddata(sendlist);
	message("Traffic: recieve-%d send-%d",rxallsize,txallsize);
	modem_add_listbox(langstr("== Sending ZModem","== Передача ZModem"));
	result=tx_zmodem(sendlist);
	errcode(result,message_error);
	sprintf(sbuf,langstr("== Xfer status: [%s]","== Результат: [%s]"),message_error);
	txallsize=0;
	modem_add_listbox(langstr("== Recieving ZModem","== Прием ZModem"));
	result=rx_zmodem();
	errcode(result,message_error);
	sprintf(sbuf,langstr("== Xfer status: [%s]","== Результат: [%s]"),message_error);
	modem_add_listbox(sbuf);
	modem_add_listbox(langstr("== End ZModem session","== Конец сессии ZModem"));
	return result;
}


p_zmod::p_zmod(void)
{
    strcpy(autostr,"rz\r");
}
p_zmod::~p_zmod()
{
}
