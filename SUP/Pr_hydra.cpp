#include "stdafx.h"
#include "supercom.h" 
#include "..\structs.h"
#include "..\u_crc.h"                    
#include "pr_hydra.h"                    
#include "pr_xfer.h"                    


/* HYDRA's protocol -------------------------------------------------------- */

extern _gconfig gc;
extern _modemcfg gModemCfg;

static struct _h_flags h_flags[] = {
        { "XON", HOPT_XONXOFF },
        { "TLN", HOPT_TELENET },
        { "CTL", HOPT_CTLCHRS },
        { "HIC", HOPT_HIGHCTL },
        { "HI8", HOPT_HIGHBIT },
        { "BRK", HOPT_CANBRK  },
        { "ASC", HOPT_CANASC  },
        { "UUE", HOPT_CANUUE  },
        { "C32", HOPT_CRC32   },
        { "DEV", HOPT_DEVICE  },
        { "FPT", HOPT_FPT     },
        { NULL , 0x0L         }
};

#define gotoexit(a)	{ rc = a; goto exit; }


/*==========================================================================*/
	void p_hydra::hydra_status(boolean xmit)
/*==========================================================================*/
{ 
long elapsed,bytes,cps;
char tmp[200];
char tmp2[200];
int	 dif,t,m,s;

	if (xmit)
	{
		elapsed = time(NULL) - txstart;
		bytes = (txallsize ? txtecsize:0)+txpos-txoffset;
		show_size(tmp2,txfsize);  
		sprintf(tmp,"%s %s",txfname,tmp2);
		gc.mailer.m_transferred=(txallsize ? txtecsize:0)+txpos;
		gc.mailer.m_total=(txallsize ? txallsize:txfsize);
		dif=(txallsize ? txallsize-txtecsize:txfsize)-txpos;
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
		sprintf(tmp2," (%02d:%02d:%02d) ",t,m,s);
		strcat(tmp,tmp2);
		gc.mailer.m_cps=cps;
		gc.mailer.m_timeelapsed=elapsed;
		strcpy(gc.mailer.m_filename,tmp);
	}
	else
	{
		elapsed = time(NULL) - rxstart;
		bytes = (rxallsize ? rxtecsize:0)+rxpos-rxoffset;
		show_size(tmp2,rxfsize);
		sprintf(tmp,"%s %s",rxfname,tmp2);
		gc.mailer.m_transferred2=(rxallsize ? rxtecsize:0)+rxpos;
		gc.mailer.m_total2=(rxallsize ? rxallsize:rxfsize);
		dif=(rxallsize ? rxallsize-rxtecsize:rxfsize)-rxpos;
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
		sprintf(tmp2," (%02d:%02d:%02d) ",t,m,s);
		strcat(tmp,tmp2);
		gc.mailer.m_cps2=cps;
		gc.mailer.m_timeelapsed2=elapsed;
		strcpy(gc.mailer.m_filename2,tmp);
	}
	update_charging();
	modem_update();
}/*hydra_status()*/

/*==========================================================================*/
	char *p_hydra::h_revdate(long revstamp)
/*==========================================================================*/
{
static char buf[12];
struct tm   *t;

	t = localtime((time_t *)&revstamp);
	sprintf(buf,"%02d.%02d.%d",t->tm_mday,t->tm_mon+1,t->tm_year + 1900);
	return buf;
}

/*---------------------------------------------------------------------------*/
	void p_hydra::put_flags (char *buf, struct _h_flags flags[], long val)
/*---------------------------------------------------------------------------*/
{
char *p;
int  i;

	p = buf;
	for (i = 0; flags[i].val; i++) 
	{
		if (val & flags[i].val) 
		{
		   if (p > buf) 
			   *p++ = ',';
		   strcpy(p,flags[i].str);
		   p += H_FLAGLEN;
		}
	}
	*p = '\0';
}/*put_flags()*/

/*---------------------------------------------------------------------------*/
	dword p_hydra::get_flags(char *buf, struct _h_flags flags[])
/*---------------------------------------------------------------------------*/
{
dword val;
char  *p;
int   i;

	val = 0x0L;
	for (p=strtok(buf,","); p; p = strtok(NULL,",")) 
	{
		for (i=0; flags[i].val; i++) 
		{
			if (!strcmp(p,flags[i].str)) 
			{
			   val |= flags[i].val;
			   break;
			}
		}
	}
	return val;
}/*get_flags()*/

/*---------------------------------------------------------------------------*/
	byte *p_hydra::put_binbyte (byte *p, byte c)
/*---------------------------------------------------------------------------*/
{
byte n;

	n = c;
	if (txoptions & HOPT_HIGHCTL)
	   n &= 0x7f;

	if (n == H_DLE ||
		((txoptions & HOPT_XONXOFF) && (n == XON || n == XOFF)) ||
		((txoptions & HOPT_TELENET) && n == '\r' && txlastc == '@') ||
		((txoptions & HOPT_CTLCHRS) && (n < 32 || n == 127))) 
	{
		*p++ = H_DLE;
		c ^= 0x40;
	}

	*p++ = c;
	txlastc = n;

	return p;
}/*put_binbyte()*/

/*==========================================================================*/
//   Процедура передачи пакета на сом порт
/*==========================================================================*/
	void p_hydra::txpkt (word len, int type)
{
boolean crc32 = false;
byte	*in, *out;
word	c, n;
byte    format;
static char hexdigit[] = "0123456789abcdef";

	txbufin[len++] = type;

	//Определение формата отправляемого пакета
	switch (type) 
	{
		case HPKT_START:
		case HPKT_INIT:
		case HPKT_INITACK:
		case HPKT_END:
		case HPKT_IDLE:
			format = HCHR_HEXPKT;
			break;
		default:
			if (txoptions & HOPT_HIGHBIT) 
			{
				if ((txoptions & HOPT_CTLCHRS) && (txoptions & HOPT_CANUUE))
					format = HCHR_UUEPKT;
				else if (txoptions & HOPT_CANASC)
					format = HCHR_ASCPKT;
				else
					format = HCHR_HEXPKT;
			}
			else
				format = HCHR_BINPKT;

			break;
	}

	if (format != HCHR_HEXPKT && (txoptions & HOPT_CRC32)) 
		crc32 = true;
	pkttxdebug(format,type,crc32,len);

	//Расчет CRC для отправляемых данных
	if (crc32) 
	{
		dword crc = ~getcrc32ccitt(txbufin,len);
		txbufin[len++] = crc;
		txbufin[len++] = crc >> 8;
		txbufin[len++] = crc >> 16;
		txbufin[len++] = crc >> 24;
	}
	else 
	{
		word crc = ~getcrc16ccitt(txbufin,len);
		txbufin[len++] = (byte)crc;
		txbufin[len++] = crc >> 8;
	}

	in = txbufin;
	out = txbuf;
	txlastc = 0;
	*out++ = H_DLE;
	*out++ = format;

	//Кодирование данных в зависимости от их формата
	switch (format) 
	{
		case HCHR_HEXPKT:
			for (; len > 0; len--, in++)
			{
				if (*in & 0x80) 
				{
					*out++ = '\\';
					*out++ = hexdigit[((*in) >> 4) & 0x0f];
					*out++ = hexdigit[(*in) & 0x0f];
				}
				else if (*in < 32 || *in == 127) 
				{
					*out++ = H_DLE;
					*out++ = (*in) ^ 0x40;
				}
				else if (*in == '\\') 
				{
					*out++ = '\\';
					*out++ = '\\';
				}
				else
					*out++ = *in;
			}
			break;
		case HCHR_BINPKT:
			for (; len > 0; len--)
				out = put_binbyte(out,*in++);
			break;
		case HCHR_ASCPKT:
			for (n = c = 0; len > 0; len--) 
			{
				c |= ((*in++) << n);
				out = put_binbyte(out,c & 0x7f);
				c >>= 7;
				if (++n >= 7) 
				{
					out = put_binbyte(out,c & 0x7f);
					n = c = 0;
				}
			}
			if (n > 0)
				out = put_binbyte(out,c & 0x7f);
			break;
		case HCHR_UUEPKT:
			for ( ; len >= 3; in += 3, len -= 3) 
			{
				*out++ = h_uuenc(in[0] >> 2);
				*out++ = h_uuenc(((in[0] << 4) & 0x30) | ((in[1] >> 4) & 0x0f));
				*out++ = h_uuenc(((in[1] << 2) & 0x3c) | ((in[2] >> 6) & 0x03));
				*out++ = h_uuenc(in[2] & 0x3f);
			}
			if (len > 0) 
			{
				*out++ = h_uuenc(in[0] >> 2);
				*out++ = h_uuenc(((in[0] << 4) & 0x30) | ((in[1] >> 4) & 0x0f));
				if (len == 2)
					*out++ = h_uuenc((in[1] << 2) & 0x3c);
			}
			break;
	}

	*out++ = H_DLE;
	*out++ = HCHR_PKTEND;

    if (type != HPKT_DATA && format != HCHR_BINPKT) 
    {
		*out++ = '\r';
		*out++ = '\n';
    }

    for (in = (byte *) txpktprefix; *in; in++) 
	{
		switch (*in) 
		{
			case 221: /* transmit break signal for one second */
				break;
			case 222: 
				{ 
					h_timer t = h_timer_set(2);
					while (!h_timer_expired(t));
				}
				break;
			case 223: 
				com_putbyte(port,0);
				break;
			default:  
				com_putbyte(port,*in);
				break;
		}
    }
    com_putblock(port,txbuf,(word) (out - txbuf));
}/*txpkt()*/

/*==========================================================================*/
//   Процедура считывания пакета из сом порта
/*==========================================================================*/
	int p_hydra::rxpkt (void)
{
byte	*p, *q;
short	int c,n,i;
byte	St_Pkt;
char	cm;

	if (gc.mailer.keyboard_break==1)
		return (H_SYSABORT);

	if (!RS_Carrier(port))
		return (H_CARRIER);

	p = rxbufptr;
	rxpktformat=NULL;
	St_Pkt=true;
	rxdle = 0;

    //// Цикл поступающих символов с компорта
    while (com_getbyte(port,St_Pkt,&cm)) 
    {
		c = cm;
		n = c;

		if (rxdle || c == H_DLE) 
		{
            switch (c) 
			{
				case H_DLE:
					if (++rxdle >= 5)
						return (H_CANCEL);
					break;
                case HCHR_PKTEND:
                   	if (rxpktformat==NULL) 
					return (H_NOPKT);
                     rxbufptr = p;
                    message(" <- <PKTEND> (pktformat='%c'", rxpktformat);

				
					///Начало обработки пакета 
                    switch (rxpktformat) 
					{
                        case HCHR_BINPKT:
                             q = rxbufptr;
                             break;
                        case HCHR_HEXPKT:
                             for (p = q = rxbuf; p < rxbufptr; p++) 
                             {
                                 if (*p == '\\' && *++p != '\\') 
                                 {
                                    i = *p;n = *++p;
                                    if ((i -= '0') > 9) i -= ('a' - ':');
                                    if ((n -= '0') > 9) n -= ('a' - ':');
                                    if ((i & ~0x0f) || (n & ~0x0f)) 
                                    { 
                                       i = H_NOPKT;
                                       break;
                                    }
                                    *q++ = (i << 4) | n;
                                 }
                                 else
                                    *q++ = *p;
                             }
                             if (p > rxbufptr)
                                c = H_NOPKT;
                             break;
                        case HCHR_ASCPKT:
                             n = i = 0;
                             for (p = q = rxbuf; p < rxbufptr; p++) 
                             {
                                 i |= ((*p & 0x7f) << n);
                                 if ((n += 7) >= 8) 
                                 { 
                                   *q++ = (byte) (i & 0xff);
                                   i >>= 8;n -= 8;
                                 }
                             }
                             break;

                        case HCHR_UUEPKT:
                             n = (int) (rxbufptr - rxbuf);
                             for (p = q = rxbuf; n >= 4; n -= 4, p += 4) 
                             {
                                 if (p[0] <= ' ' || p[0] >= 'a' ||
                                     p[1] <= ' ' || p[1] >= 'a' ||
                                     p[2] <= ' ' || p[2] >= 'a') {c=H_NOPKT;break;}
                                 *q++ = (byte) ((h_uudec(p[0]) << 2) | (h_uudec(p[1]) >> 4));
                                 *q++ = (byte) ((h_uudec(p[1]) << 4) | (h_uudec(p[2]) >> 2));
                                 *q++ = (byte) ((h_uudec(p[2]) << 6) | h_uudec(p[3]));
                             }

                             if (n >= 2) 
                             {
                                if (p[0] <= ' ' || p[0] >= 'a') {c = H_NOPKT;break;}
                                *q++ = (byte) ((h_uudec(p[0]) << 2) | (h_uudec(p[1]) >> 4));
                                if (n == 3) 
                                {  if (p[0] <= ' ' || p[0] >= 'a') {c = H_NOPKT;break;}
                                   *q++ = (byte) ((h_uudec(p[1]) << 4) | (h_uudec(p[2]) >> 2));
                                }
                             }
                             break;

                        default:  
                        c = H_NOPKT;
                        
                        break;
                     }
                     ///Конец обработки пакета 

                     rxbufptr = NULL;

                     if (c == H_NOPKT)
                        break;

                     rxpktlen = (word) (q - rxbuf);

                     if (rxpktformat != HCHR_HEXPKT && (rxoptions & HOPT_CRC32)) 
                     {
                        if (rxpktlen < 5) {c = H_NOPKT; break;}
                        n = CRC32TEST(getcrc32ccitt(rxbuf,rxpktlen));
                        /* remove CRC-32 */
                        rxpktlen -= (int) sizeof (long);  
                     }
                     else 
                     {
                        if (rxpktlen < 3) {c = H_NOPKT;break;}
                        n = CRC16TEST(getcrc16ccitt(rxbuf,rxpktlen));
                        /* remove CRC-16 */
                        rxpktlen -= (int) sizeof (word);  
                     }

                     /* remove type  */
                     rxpktlen--;           

                     if (n) 
                     { pktrxdebug();
                     // Если все нормально то тут выходим из процедуры
                       return ((int) rxbuf[rxpktlen]);
                     }

                     message(" Bad CRC (format='%c'  type='%c'  len=%d)", rxpktformat, (int) rxbuf[rxpktlen], rxpktlen);
                     break;

                case HCHR_BINPKT: 
                case HCHR_HEXPKT: 
                case HCHR_ASCPKT: 
                case HCHR_UUEPKT:
                     message(" <- <PKTSTART> (pktformat='%c')",c);
                     rxpktformat = (byte)c;
                     p = rxbufptr = rxbuf;
                     rxdle = 0;
                                                 St_Pkt=true;
                     break;

                default:
                     if (p) 
                     {
                        if (p < rxbufmax)
                           *p++ = (byte) (c ^ 0x40);
                        else 
                        {
                           message(" <- Pkt too long - discarded");
                           p = NULL;
                        }
                     }
                     rxdle = 0;
                     break;
             }
          }
          else if (p) 
          {
             if (p < rxbufmax)
                *p++ = (byte) c;
             else {
                message(" <- Pkt too long - discarded");
                p = NULL;
                  }
          }
    }
       //Конец цикла обработки поступающих символов
        rxbufptr = p;

        if (h_timer_running(braindead) && h_timer_expired(braindead)) 
        {
           message(" <- BrainDead (timer=%08lx  time=%08lx)",braindead,time(NULL));
           return (H_BRAINTIME);
        }

        if (h_timer_running(txtimer) && h_timer_expired(txtimer)) 
        {
           message(" <- TxTimer (timer=%08lx  time=%08lx)",txtimer,time(NULL));
           return (H_TXTIME);
        }

        
        return (H_NOPKT);
}/*rxpkt()*/

/*---------------------------------------------------------------------------*/
	void p_hydra::hydra_init (void)
/*---------------------------------------------------------------------------*/
{

	batchesdone = 0;
	options = ((HOPT_XONXOFF|HOPT_TELENET) & HCAN_OPTIONS) & ~HUNN_OPTIONS;
	timeout =  (H_MINTIMER+H_MAXTIMER)/2;
	txmaxblklen = H_MAXBLKLEN;
	rxblklen =  512;
	txblklen =  512;
	txgoodbytes  = 0;
	txgoodneeded = 1024;
}/*hydra_init()*/

/*---------------------------------------------------------------------------*/
	void p_hydra::hydra_deinit (void)
/*---------------------------------------------------------------------------*/
{
	clear_status(true);
	clear_status(false);
}/*hydra_deinit()*/

/*---------------------------------------------------------------------------*/
	int p_hydra::hydra(char *sendlist)
/*---------------------------------------------------------------------------*/
{
int   rc;
int   pkttype;
char *p, *q;
int   i;
int LenFinfo;
char sbuf[255];
char tempbuf2[255];
int Index=1;

           txstate   = HTX_START;
           txoptions = HTXI_OPTIONS;
           txpktprefix[0] = '\0';
           rxstate   = HRX_INIT;
           rxoptions = HRXI_OPTIONS;
           rxfd      = NULL;
           rxdle     = 0;
           rxbufptr  = NULL;
           rxtecsize = 0;
           txtecsize = 0;
           rxtimer   = h_timer_reset();
           braindead = h_timer_set(H_BRAINDEAD);
           txtimer   = h_timer_reset();
           txretries = 0;
           rxstart   = 0;
           txstart   = 0;
           txtecsize = 0;
           rxtecsize = 0;
           strcpy(message_error,"OK");           
	   rc = PRC_NOERROR;
	  
	       txidle    = 0;
        /*-------------------------------------------------------------------*/
         while ((txstate != HTX_DONE) || (rxstate != HRX_DONE)) 
         {
          /*----------------------------------------------------------------*/
           switch (txstate) {
                  /*---------------------------------------------------------*/
                  case HTX_START:
						com_putblock(port,(byte *) autostr,(int) strlen(autostr));
						txpkt(0,HPKT_START);
						txtimer = h_timer_set(H_START);
						txidle  = h_timer_set(H_IDLE);
						txstate = HTX_SWAIT;
						break;

                  /*---------------------------------------------------------*/
                  case HTX_INIT:
                       p = (char *) txbufin;
                       sprintf(p,"%08lx%s,%s %s",H_REVSTAMP,PRGNAME,VERSION,HC_OS);
                       p += ((int) strlen(p)) + 1;
                       put_flags(p,h_flags,HCAN_OPTIONS);
                       p += ((int) strlen(p)) + 1;
                       put_flags(p,h_flags,options);
                       p += ((int) strlen(p)) + 1;
                       sprintf(p,"%08lx%08lx",hydra_txwindow,hydra_rxwindow);
                       p += ((int) strlen(p)) + 1;
                       strcpy(p,pktprefix);
                       p += ((int) strlen(p)) + 1;
                       txoptions = HTXI_OPTIONS;
                       txpkt((word) (((byte *) p) - txbufin), HPKT_INIT);
                       txoptions = rxoptions;
                       txtimer = h_timer_set(timeout / 2);
                       txstate = HTX_INITACK;
                       break;

                  /*---------------------------------------------------------*/
                  case HTX_FINFO:
                  next_txpathname(sendlist,Index);
                   if (txfd > 0) 
                       {
                          if (!txretries) 
                          {
                             message(txfname);
                             strlwr(txfname);
                          }
                             sprintf((char *) txbufin,"%08lx%08lx%08lx%08lx%08lx%s",txftime, txfsize, 0L, 0L, 0L,txfname);
                             LenFinfo=strlen((char *) txbufin)+1;
                             strupr(txfname);
                             show_size(tempbuf2,txfsize);
                             modem_add_listbox(L("S_129",txfname,tempbuf2));	// >> File: %s %s

                             strcpy((char *) &txbufin[LenFinfo],txfname);
                             LenFinfo=LenFinfo+strlen((char *) txfname);
                       }
                       else 
                       {
                          if (!txretries) 
                          {
                             message("End of batch");
                             message("+HSEND: End of batch");
                          }
                          strcpy((char *) txbufin,txfname);
                          LenFinfo=strlen((char *) txbufin);
                       }
                       txpkt(LenFinfo + 1,HPKT_FINFO);
                       txtimer = h_timer_set(txretries ? timeout / 2 : timeout);
                       txstate = HTX_FINFOACK;
                       if (!txallsize) txstart = NULL;

                       break;

                  /*---------------------------------------------------------*/
                  case HTX_XDATA:
                       if (txpos < 0L) 
						   i = -1;
                       else 
                       {
                          h_long1(txbufin) = txpos;
                          if ((i = fread(txbufin+sizeof(long),1,txblklen,txfd)) < 0) 
                          {
                             message("!HSEND: File read error");
                             fclose(txfd);
                             txfd = NULL;
                             txpos = -2L;
                          }
                       }

                       if (i > 0) 
                       {
                          txpos += i;
                          txpkt(((int) sizeof (long)) + i, HPKT_DATA);
                          if (txblklen < txmaxblklen && (txgoodbytes += i) >= txgoodneeded) 
                          {
                             txblklen <<= 1;
                             if (txblklen >= txmaxblklen) 
                             {
                                txblklen = txmaxblklen;
                                txgoodneeded = 0;
                             }
                             txgoodbytes = 0;
                          }

                          
                          if (!txstart)  txstart = time(NULL);
                          hydra_status(true);
                          break;
                       }


                  /*---------------------------------------------------------*/
                  case HTX_EOF:
                       h_long1(txbufin) = txpos;
                       txpkt((int) sizeof (long),HPKT_EOF);
                       txtimer = h_timer_set(txretries ? timeout / 2 : timeout);
                       txstate = HTX_EOFACK;
                       break;
                  case HTX_END:
					txpkt(0,HPKT_END);
					txpkt(0,HPKT_END);
					txtimer = h_timer_set(timeout / 2);
					txidle = h_timer_set(H_IDLE);
					txstate = HTX_ENDACK;
					break;
                  default:
                       break;

                  /*---------------------------------------------------------*/
           }

            
                 /*----------------------------------------------------------*/
                 switch (pkttype=rxpkt()) 
                 {
                        /*---------------------------------------------------*/
                        case H_CARRIER:
                        case H_CANCEL:
                        case H_SYSABORT:
                        case H_BRAINTIME:
                            switch (pkttype) 
                            {
                                case H_CARRIER:   
				gotoexit(PRC_ERROR);
				break;
                                case H_CANCEL:    
				gotoexit(PRC_REMOTEABORTED);
				break;
                                case H_SYSABORT:  
				gotoexit(PRC_LOCALABORTED);
				break;
                                case H_BRAINTIME: 
				gotoexit(PRC_STOPTIME);
				break;
                            }

                            txstate = HTX_DONE;
                            rxstate = HRX_DONE;
                            
                            break;

                        /*---------------------------------------------------*/
                        case H_TXTIME:
                            if (txstate == HTX_XWAIT || txstate == HTX_REND) 
                            {
                                txpkt(0,HPKT_IDLE);
                                txtimer = h_timer_set(H_IDLE);
                                break;
                            }

                            if (++txretries > H_RETRIES) 
                            {
                                message("-HSEND: Too many errors");
                                txstate = HTX_DONE;
                                gotoexit(PRC_ERROR);
                                break;
                            }

                            message("-HSEND: Timeout - Retry %u",txretries);
                            txtimer = h_timer_reset();
                            switch (txstate) 
                            {
                                case HTX_SWAIT:    txstate = HTX_START; break;
                                case HTX_INITACK:  txstate = HTX_INIT;  break;
                                case HTX_FINFOACK: txstate = HTX_FINFO; break;
                                case HTX_DATAACK:  txstate = HTX_XDATA; break;
                                case HTX_EOFACK:   txstate = HTX_EOF;   break;
                                case HTX_ENDACK:   txstate = HTX_END;   break;
                            }
                            break;
                        case HPKT_START:
                            if (txstate == HTX_START || txstate == HTX_SWAIT) 
                            {
                                txtimer = h_timer_reset();
                                txretries = 0;
                                txstate = HTX_INIT;
                                braindead = h_timer_set(H_BRAINDEAD);
                            }
                            break;
                        case HPKT_INIT:
                             if (rxstate == HRX_INIT) 
                             {
                                p = (char *) rxbuf;
                                p += ((int) strlen(p)) + 1;
                                q = p + ((int) strlen(p)) + 1;
                                rxoptions  = options | HUNN_OPTIONS;
                                rxoptions |= get_flags(q,h_flags);
                                rxoptions &= get_flags(p,h_flags);
                                rxoptions &= HCAN_OPTIONS;
                                if (rxoptions < (options & HNEC_OPTIONS)) 
                                {
                                   message("!HYDRA: Incompatible on this link");
                                   txstate = HTX_DONE;
                                   gotoexit(PRC_ERROR);
                                   break;
                                }
                                p = q + ((int) strlen(q)) + 1;
                                rxwindow = txwindow = 0L;
                                sscanf(p,"%08lx%08lx", &rxwindow,&txwindow);
                                if (rxwindow < 0L) rxwindow = 0L;
                                if (hydra_rxwindow && (!rxwindow || hydra_rxwindow < rxwindow))
                                   rxwindow = hydra_rxwindow;
                                if (txwindow < 0L) txwindow = 0L;
                                if (hydra_txwindow && (!txwindow || hydra_txwindow < txwindow))
                                   txwindow = hydra_txwindow;
                                p += ((int) strlen(p)) + 1;
                                strncpy(txpktprefix,p,H_PKTPREFIX);
                                txpktprefix[H_PKTPREFIX] = '\0';

                                if (!batchesdone) 
                                { time_t revstamp;
                                   p = (char *) rxbuf;
                                   sscanf(p,"%08lx",&revstamp);
                                   message("*HYDRA: Other's HydraRev=%s", h_revdate(revstamp));
                                   p += 8;
                                   if ((q = strchr(p,',')) != NULL) *q = ' ';
                                   if ((q = strchr(p,',')) != NULL) *q = '/';
                                   message("*HYDRA: Other's App.Info '%s'",p);
                                   put_flags((char *) rxbuf,h_flags,rxoptions);
                                   message("*HYDRA: Using link options '%s'",rxbuf);
                                }
                                txoptions = rxoptions;
                                rxstate = HRX_FINFO;
                             }

                             txpkt(0,HPKT_INITACK);
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_INITACK:
                             if (txstate == HTX_INIT || txstate == HTX_INITACK) 
                             {
                                braindead = h_timer_set(H_BRAINDEAD);
                                txtimer = h_timer_reset();
                                txretries = 0;
                                txstate = HTX_RINIT;
                             }
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_FINFO:
                                   
                             if (rxstate == HRX_FINFO) 
                             {
                                braindead = h_timer_set(H_BRAINDEAD);
                                if (!rxbuf[0]) 
                                {
                                   message("End of batch");
                                   message("*HRECV: End of batch");
                                   rxpos = 0L;
                                   rxstate = HRX_DONE;
                                   batchesdone++;
                                }
                                else 
                                {

                                   rxfsize = rxftime = 0L;
                                   rxfname[0] = '\0';
                                   sscanf((char *) rxbuf,"%08lx%08lx%*08lx%*08lx%*08lx%s",&rxftime, &rxfsize, rxfname);
                                   strupr(rxfname);show_size(tempbuf2,rxfsize);
                                   modem_add_listbox(L("S_595",rxfname,tempbuf2));	// << File: %s %s

                                   switch(xfer_init(rxfname,rxfsize,rxftime))
                                   {
                                       case 1:
                                                    rxpos = -1L;
                                                    break;
                                       case 2:
                                                    rxpos = -2L;
                                                    break;
                                       default:
                                      if ( existfile(xfer_pathname)) 
                                      { 
                                         if ((rxfd = fopen(xfer_pathname,"a+b")) <= 0) 
                                         {
                                            message("!HRECV: Unable to re-open %s",xfer_pathname);
                                            rxpos = -2L;
                                         }
                                      }
                                      else if ((rxfd = fopen(xfer_pathname,"wb")) <= 0) 
                                      {
                                         message("!HRECV: Unable to create %s",xfer_pathname);
                                         rxpos = -2L;
                                      }
                                      break;
								   };

                                      
									message("%s ",xfer_pathname);

								   if (rxfd > 0) 
                                      {
                                         message("+HRECV: %s (%ldb)",rxfname, rxfsize);
                                         if (fseek(rxfd,0L,SEEK_END)) 
                                         {
                                            message("!HRECV: File seek error");
                                            pr_badxfer();
                                            rxpos = -2L;
                                         }
                                         else {
                                            rxoffset = rxpos = ftell(rxfd);
                                            if (rxpos < 0L) 
                                            {
                                               message("!HRECV: File tell error");
                                               pr_badxfer();
                                               rxpos = -2L;
                                            }
                                            else 
                                            {
                              
                                               rxtimer = h_timer_reset();
                                               rxretries = 0;
                                               rxlastsync = 0L;
                                               rxsyncid = 0L;
                                               hydra_status(false);
                                               if (rxpos > 0L) 
                                               {
                                                  message("%ld/%ld",rxpos,rxfsize);
                                                  message("+HRECV: Resuming from offset %ld", rxpos);
                                               }
                                               rxstate = HRX_DATA;
                                            }
                                         }
                                      }
                                   }
                                
                             }
                             else if (rxstate == HRX_DONE)
                                rxpos = (!rxbuf[0]) ? 0L : -2L;

							if (rxallsize && (rxallsize<rxtecsize+rxfsize)) rxallsize=0;	// !!!
							if (!rxallsize) rxstart = NULL;
                             h_long1(txbufin) = rxpos;
                             txpkt((int) sizeof (long),HPKT_FINFOACK);
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_FINFOACK:
                             if (txstate == HTX_FINFO || txstate == HTX_FINFOACK) 
                             {
                                braindead = h_timer_set(H_BRAINDEAD);
                                txretries = 0;
                                if (!txfname[0]) 
                                {
                                   txtimer = h_timer_set(H_IDLE);
                                   txstate = HTX_REND;
                                }
                                else {
                                   txtimer = h_timer_reset();
                                   txpos = h_long1(rxbuf);
                                   if (txpos >= 0L) {
                                      txoffset = txpos;
                                      txlastack = txpos;
                                      hydra_status(true);
                                      if (txpos > 0L) {
                                         message("+HSEND: Transmitting from offset %ld",txpos);
                                         if (fseek(txfd,txpos,SEEK_SET) < 0L) {
                                            message("!HSEND: File seek error");
                                            fclose(txfd);
                                            txfd = NULL;
                                            txpos = -2L;
                                            txstate = HTX_EOF;
                                            break;
                                         }
                                      }
                                      txstate = HTX_XDATA;
                                   }
                                   else 
                                   {
                                      fclose(txfd);
                                      if (txpos == -1L) 
                                      {
                                         message("They already have file");
                                         message("+HSEND: They already have %s",txfname);
                                         //Стираем файл если он из OUTBOUND
                                         
										make_path(sbuf,gc.OutboundPath);
										if (strstr(txpathname,sbuf))
                                         DeleteFile(txpathname);
                                      }
                                      else 
                                      {  /* (txpos < -1L) file NOT sent */
                                         message("Skipping");
                                         message("+HSEND: Skipping %s",txfname);
                                      };
                                      txstate = HTX_FINFO;
                                   }
                                }
                             }
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_DATA:
							if (txidle && h_timer_expired(txidle))  
							{
								txpkt(0,HPKT_IDLE);
								txidle  = h_timer_set(H_IDLE);
							}

                             if (rxstate == HRX_DATA) 
                             {
                                if (h_long1(rxbuf) != rxpos || h_long1(rxbuf) < 0L) 
                                {
                                   if (h_long1(rxbuf) <= rxlastsync) 
                                   {
                                      rxtimer = h_timer_reset();
                                      rxretries = 0;
                                   }
                                   rxlastsync = h_long1(rxbuf);

                                   if (!h_timer_running(rxtimer) || h_timer_expired(rxtimer)) 
                                   {
                                      if (++rxretries > H_RETRIES) 
                                      {
                                         message("-HRECV: Too many errors");
                                         txstate = HTX_DONE;
                                         rc = PRC_ERROR;
                                         break;
                                      }
                                      if (rxretries == 1)
                                         rxsyncid++;

                                      rxblklen /= 2;
                                      i = rxblklen;
                                      if      (i <=  64) i =   64;
                                      else if (i <= 128) i =  128;
                                      else if (i <= 256) i =  256;
                                      else if (i <= 512) i =  512;
                                      else               i = 1024;
                                      message("-HRECV: Bad pkt at %ld - Retry %u (newblklen=%u)", rxpos,rxretries,i);
                                      h_long1(txbufin) = rxpos;
                                      h_long2(txbufin) = (long) i;
                                      h_long3(txbufin) = rxsyncid;
                                      txpkt(3 * ((int) sizeof(long)),HPKT_RPOS);
                                      rxtimer = h_timer_set(timeout);
                                   }
                                }
                                else 
                                {
                                   braindead = h_timer_set(H_BRAINDEAD);
                                   rxpktlen -= (int) (sizeof (long));
                                   rxblklen = rxpktlen;
                                   if (fwrite(rxbuf+sizeof (long),rxpktlen,1,rxfd) != 1)
                                   {
                                      message("!HRECV: File write error");
                                      pr_badxfer();
                                      rxpos = -2L;
                                      rxretries = 1;
                                      rxsyncid++;
                                      h_long1(txbufin) = rxpos;
                                      h_long2(txbufin) = 0L;
                                      h_long3(txbufin) = rxsyncid;
                                      txpkt(3 * ((int) sizeof(long)),HPKT_RPOS);
                                      rxtimer = h_timer_set(timeout);
                                      break;
                                   }
                                   rxretries = 0;
                                   rxtimer = h_timer_reset();
                                   rxlastsync = rxpos;
                                   rxpos += rxpktlen;

                                   //Сбросим указатель на конец для пропуска файла
                                   if (gc.mailer.skip_file)
                                   {
									    rxpos= (gc.mailer.skip_file==-1) ? -1 : -2;
  									    h_long1(txbufin) = rxpos;
										h_long2(txbufin) = 1024;
										h_long3(txbufin) = 0;
										txpkt(3 * ((int) sizeof(long)),HPKT_RPOS);
										rxtimer = h_timer_set(timeout);
                                        message("Пропуск файла %s пользователем",rxfname);
                                   }

                                   if (rxwindow) 
                                   {
                                      h_long1(txbufin) = rxpos;
                                      txpkt((int) sizeof(long),HPKT_DATAACK);
                                   }
                                   if (!rxstart)
                                   rxstart = time(NULL);
                                   hydra_status(false);
                                }/*badpkt*/
                             }/*rxstate==HRX_DATA*/
							break;
                        /*---------------------------------------------------*/
                        case HPKT_DATAACK:
                             if (txstate == HTX_XDATA || txstate == HTX_DATAACK ||
                                 txstate == HTX_XWAIT ||
                                 txstate == HTX_EOF || txstate == HTX_EOFACK) 
                            {
                                if (txwindow && h_long1(rxbuf) > txlastack) 
                                {
                                   txlastack = h_long1(rxbuf);
                                   if (txstate == HTX_DATAACK && (txpos < (txlastack + txwindow))) 
                                   {
                                      txstate = HTX_XDATA;
                                      txretries = 0;
                                      txtimer = h_timer_reset();
                                   }
                                }
                             }
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_RPOS:
                             if (txstate == HTX_XDATA || txstate == HTX_DATAACK ||
                                 txstate == HTX_XWAIT ||
                                 txstate == HTX_EOF || txstate == HTX_EOFACK) 
                             {
                                if (h_long3(rxbuf) != txsyncid) 
                                {
                                   txsyncid = h_long3(rxbuf);
                                   txretries = 1;
                                   txtimer = h_timer_reset();
                                   txpos = h_long1(rxbuf);
                                   if (txpos < 0L) 
                                   {
                                      if (txfd > 0) 
                                      {
                                         message("+HSEND: Skipping %s",txfname);
                                         fclose(txfd);
                                         txfd = NULL;
                                         txstate = HTX_EOF;
                                      }
                                      txpos = -2L;
                                      break;
                                   }

                                   if (txblklen > h_long2(rxbuf))
                                      txblklen = (word) h_long2(rxbuf);
                                   else
                                      txblklen >>= 1;
                                   if      (txblklen <=  64) txblklen =   64;
                                   else if (txblklen <= 128) txblklen =  128;
                                   else if (txblklen <= 256) txblklen =  256;
                                   else if (txblklen <= 512) txblklen =  512;
                                   else                      txblklen = 1024;
                                   txgoodbytes = 0;
                                   txgoodneeded += 1024;
                                   if (txgoodneeded > 8192)
                                      txgoodneeded = 8192;

                                   hydra_status(true);
                                   message("+HSEND: Resending from offset %ld (newblklen=%u)",txpos,txblklen);
                                   if (fseek(txfd,txpos,SEEK_SET) < 0L) 
                                   {
                                      message("!HSEND: File seek error");
                                      fclose(txfd);
                                      txfd = NULL;
                                      txpos = -2L;
                                      txstate = HTX_EOF;
                                      break;
                                   }

                                   if (txstate != HTX_XWAIT)
                                      txstate = HTX_XDATA;
                                }
                                else {
                                   if (++txretries > H_RETRIES) 
                                   {
                                      message("-HSEND: Too many errors");
                                      txstate = HTX_DONE;
                                      gotoexit(PRC_ERROR);
                                      break;
                                   }
                                }
                             }
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_EOF:
                             if (rxstate == HRX_DATA) 
                             {
                               if (h_long1(rxbuf) < 0L) 
                               {
                                   pr_badxfer();
                                   message("+HRECV: Skipping %s",rxfname);
                                   rxstate = HRX_FINFO;
                                   braindead = h_timer_set(H_BRAINDEAD);
                                }
                                else if (h_long1(rxbuf) != rxpos) 
                                {
                                   if (h_long1(rxbuf) <= rxlastsync) 
                                   {
                                      rxtimer = h_timer_reset();
                                      rxretries = 0;
                                   }
                                   rxlastsync = h_long1(rxbuf);

                                   if (!h_timer_running(rxtimer) || h_timer_expired(rxtimer)) 
                                   {
                                      if (++rxretries > H_RETRIES) 
                                      {
                                         message("-HRECV: Too many errors");
                                         txstate = HTX_DONE;
                                         gotoexit(PRC_ERROR);
                                         break;
                                      }
                                      if (rxretries == 1)
                                         rxsyncid++;

                                      rxblklen /= 2;
                                      i = rxblklen;
                                      if      (i <=  64) i =   64;
                                      else if (i <= 128) i =  128;
                                      else if (i <= 256) i =  256;
                                      else if (i <= 512) i =  512;
                                      else               i = 1024;
                                      message("-HRECV: Bad EOF at %ld - Retry %u (newblklen=%u)",rxpos,rxretries,i);
                                      h_long1(txbufin) = rxpos;
                                      h_long2(txbufin) = (long) i;
                                      h_long3(txbufin) = rxsyncid;
                                      txpkt(3 * ((int) sizeof(long)),HPKT_RPOS);
                                      rxtimer = h_timer_set(timeout);
                                   }
                                }
                                else 
                                {
                                   rxfsize = rxpos;
                                   fclose(rxfd);
                                   rxfd = NULL;
                                   

                                   p = xfer_okay();
                                   if (p) 
                                   {
                                      message("  -> %s",p);
                                      message("+HRECV: Dup file renamed: %s",p);
                                   }

                                   hydra_status(false);
                                   message("+Rcvd-H %s",p ? p : rxfname);
                                   rxstate = HRX_FINFO;
                                   braindead = h_timer_set(H_BRAINDEAD);
									clear_status(false);
                                }/*skip/badeof/eof*/
                             }/*rxstate==HRX_DATA*/

                             rxtecsize += rxfsize;
							if (rxstate == HRX_FINFO)	txpkt(0,HPKT_EOFACK);
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_EOFACK:
                             if (txstate == HTX_EOF || txstate == HTX_EOFACK)
                             {
                                braindead = h_timer_set(H_BRAINDEAD);
                                if (txfd > 0)
                                {
                                   txfsize = txpos;
                                   fclose(txfd);
                                   txfd=NULL;
                                   clear_status(true);
                                   //Стираем файл если он из OUTBOUND
 				   make_path(sbuf,gc.OutboundPath);
                                   //Проверка на реквест
				DelReqFile(txfname);
                                   //Проверка на атач
				DelXferFile(txfname);

				   if (strstr(txpathname,sbuf) && !pmatch(txfname,"*.[Rr][Ee][Qq]"))
                                   DeleteFile(txpathname);
                                 }
                                 Index++;
                                 txstate=HTX_FINFO;
                             }
                                 txtecsize += txfsize;
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_IDLE:
                             if (txstate == HTX_XWAIT) 
                             {
                                txtimer = h_timer_reset();
                                txretries = 0;
                                txstate = HTX_XDATA;
                             }
                             else if (txstate >= HTX_FINFO && txstate < HTX_REND)
                                braindead = h_timer_set(H_BRAINDEAD);
                             break;

                        /*---------------------------------------------------*/
                        case HPKT_END:
                             if (txstate == HTX_END || txstate == HTX_ENDACK) 
                             {
                                txpkt(0,HPKT_END);
                                txpkt(0,HPKT_END);
                                txpkt(0,HPKT_END);
                                message("<< HYDRA: Completed");
                                txstate = HTX_DONE;
				}
                           break;

                        /*---------------------------------------------------*/
                        default:  /* unknown packet types: IGNORE, no error! */
                             break;

                        /*---------------------------------------------------*/
                 }/*switch(pkttype)*/

                 /*----------------------------------------------------------*/
                 switch (txstate) {
                        /*---------------------------------------------------*/
                        case HTX_START:
                        case HTX_SWAIT:
                             if (rxstate == HRX_FINFO) 
                             {
                                txtimer = h_timer_reset();
                                txretries = 0;
                                txstate = HTX_INIT;
                             }
                             break;

                        /*---------------------------------------------------*/
                        case HTX_RINIT:
                             if (rxstate == HRX_FINFO) 
                             {
                                txtimer = h_timer_reset();
                                txretries = 0;
                                txstate = HTX_FINFO;
                             }
                             break;

                        /*---------------------------------------------------*/
                        case HTX_XWAIT:
                             if (!rxstate) 
                             {
                                txtimer = h_timer_reset();
                                txretries = 0;
                                txstate = HTX_XDATA;
                             }
                             break;

                        /*---------------------------------------------------*/
                        case HTX_REND:
                             if (!rxstate) 
                             {
                                txtimer = h_timer_reset();
                                txretries = 0;
                                txstate = HTX_END;
                             }
                             break;

                        /*---------------------------------------------------*/
                 }/*switch(txstate)*/
        } ;

exit:
        if (txfd > 0) fclose(txfd);
        if (rxfd > 0) fclose(rxfd);

        if (rc != PRC_NOERROR)	
        {pr_badxfer();
         com_putblock(port,(byte *) abortstr,(int) strlen(abortstr));}
           
        return (rc);
}/*hydra()*/
/*---------------------------------------------------------------------------*/

/*==========================================================================*/
// Сессия протокола гидры
	int p_hydra::hydra_batch (char *sendlist,char *emsibuf)
/*==========================================================================*/
{  
int result;
char sbuf[255];

  port=gModemCfg.m_comport-1;
	mailer_stat(L("S_392"));	// Session Hydra
	update_charging();
	modem_update();
	sprintf(sbuf,L("S_627"));	// == Start Hydra session
	modem_add_listbox(sbuf);
	hydra_init();
	rxallsize=resdata(emsibuf);
	txallsize=senddata(sendlist);
	message("Traffic: recieve-%d send-%d",rxallsize,txallsize);
	result=hydra(sendlist);
	rxallsize=0;
	txallsize=0;

	if (result==PRC_NOERROR)  result=hydra(NULL);

	errcode(result,message_error);
	modem_add_listbox(L("S_70",message_error));	// == Xfer status: [%s]
	modem_add_listbox(L("S_626"));	// == End Hydra session
	hydra_deinit();
    
	return result;
}

/*==========================================================================*/
//   Отладочная процедуры вывода принятого пакета
/*==========================================================================*/
void p_hydra::pktrxdebug(void)
{
   char *s1, *s2, *s3, *s4;

   message(" <- PKT (format='%c'  type='%c'  len=%d)",  rxpktformat, (int) rxbuf[rxpktlen], rxpktlen);
   switch (rxbuf[rxpktlen]) {
          case HPKT_START:    message("    START");
                              break;
          case HPKT_INIT:     s1 = ((char *) rxbuf) + ((int) strlen((char *) rxbuf)) + 1;
                              s2 = s1 + ((int) strlen(s1)) + 1;
                              s3 = s2 + ((int) strlen(s2)) + 1;
                              s4 = s3 + ((int) strlen(s3)) + 1;
                              message("    INIT (appinfo='%s'  can='%s'  want='%s'  options='%s'  pktprefix='%s')", (char *) rxbuf, s1, s2, s3, s4);
                              break;
          case HPKT_INITACK:  message("    INITACK");
                              break;
          case HPKT_FINFO:    message("    FINFO ('%s'  rxstate=%d)",rxbuf,rxstate);
                              break;
          case HPKT_FINFOACK: message("    FINFOACK (pos=%ld  txstate=%d  txfd=%d)", h_long1(rxbuf), txstate, txfd);
                              break;
          case HPKT_DATA:     message("    DATA (rxstate=%d  pos=%ld  len=%u)", rxstate, h_long1(rxbuf),(word) (rxpktlen - ((int) sizeof (long))));
                              break;
          case HPKT_DATAACK:  message("    DATAACK (rxstate=%d  pos=%ld)", rxstate, h_long1(rxbuf));
                              break;
          case HPKT_RPOS:     message("    RPOS (pos=%ld%s  blklen=%u->%ld  syncid=%ld%s  txstate=%d  txfd=%d)", h_long1(rxbuf), h_long1(rxbuf) < 0L ? " SKIP" : "", txblklen, h_long2(rxbuf), h_long3(rxbuf), h_long3(rxbuf) == rxsyncid ? " DUP" : "", txstate, txfd);
                              break;
          case HPKT_EOF:      message("    EOF (rxstate=%d  pos=%ld%s)", rxstate, h_long1(rxbuf), h_long1(rxbuf) < 0L ? " SKIP" : "");
                              break;
          case HPKT_EOFACK:   message("    EOFACK (txstate=%d)", txstate);
                              break;
          case HPKT_IDLE:     message("    IDLE");
                              break;
          case HPKT_END:      message("    END");
                              break;
          default:            message("    Unkown pkttype %d (txstate=%d  rxstate=%d)", (int) rxbuf[rxpktlen], txstate, rxstate);
                              break;
   }
}


/*==========================================================================*/
//   Отладочная процедуры вывода отправлямого пакета
/*==========================================================================*/
void p_hydra::pkttxdebug(char format, char type, int crc32, int len)
{
   char *s1, *s2, *s3, *s4;

   message(" -> Send PKT (format='%c'  type='%c'  crc=%d  len=%d)", format, type, crc32 ? 32 : 16, len - 1);

   switch (type) {
          case HPKT_START:    message("    <autostr>START");
                              break;
          case HPKT_INIT:     s1 = ((char *) txbufin) + ((int) strlen((char *) txbufin)) + 1;
                              s2 = s1 + ((int) strlen(s1)) + 1;
                              s3 = s2 + ((int) strlen(s2)) + 1;
                              s4 = s3 + ((int) strlen(s3)) + 1;
                              message("    Send INIT (appinfo='%s'  can='%s'  want='%s'  options='%s'  pktprefix='%s')", (char *) txbufin, s1, s2, s3, s4);
                              break;
          case HPKT_INITACK:  message("    Send INITACK");
                              break;
          case HPKT_FINFO:    message("    Send FINFO (%s)",txbufin);
                              break;
          case HPKT_FINFOACK: if (rxfd > 0) 
                              {
                                 if (rxpos > 0L) s1 = "RES";
                                 else            s1 = "BOF";
                              }
                              else if (rxpos == -1L) s1 = "HAVE";
                              else if (rxpos == -2L) s1 = "SKIP";
                              else                   s1 = "EOB";
                              message("    Send FINFOACK (pos=%ld %s  rxstate=%d  rxfd=%d)", rxpos,s1,rxstate,rxfd);
                              break;
          case HPKT_DATA:     message("    Send DATA (ofs=%ld  len=%d)",h_long1(txbufin), len - 5);
                              break;
          case HPKT_DATAACK:  message("    Send DATAACK (ofs=%ld)",h_long1(txbufin));
                              break;
          case HPKT_RPOS:     message("    Send RPOS (pos=%ld%s  blklen=%ld  syncid=%ld)",rxpos, rxpos < 0L ? " SKIP" : "", h_long2(txbufin), rxsyncid);
                              break;
          case HPKT_EOF:      message("    Send EOF (ofs=%ld%s)",txpos, txpos < 0L ? " SKIP" : "");
                              break;
          case HPKT_EOFACK:   message("    Send EOFACK");
                              break;
          case HPKT_IDLE:     message("    Send IDLE");
                              break;
          case HPKT_END:      message("    Send END");
                              break;
          default:    
                              break;
   }
}

p_hydra::p_hydra(void)
{  
  strcpy(abortstr,"\0x18\0x18\0x18\0x18\0x18\0x18\0x18\0x18\0x10\0x10\0x10\0x10\0x10\0x10\0x10\0x10\0x10\0x10");
  strcpy(autostr,"hydra\r");
  txbuf= (byte *)malloc(H_BUFLEN+1);
  rxbuf= (byte *)malloc(H_BUFLEN+1);
  if (!txbuf || !rxbuf)  
  {
	  message("!HYDRA: Can't allocate buffers!");
  }

  txbufin  = txbuf + ((H_MAXBLKLEN + H_OVERHEAD + 5) * 2);
  rxbufmax = rxbuf + H_MAXPKTLEN;

}
p_hydra::~p_hydra()
{  
	free(txbuf);
	free(rxbuf);
 }


