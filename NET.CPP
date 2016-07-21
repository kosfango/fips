#include "stdafx.h"

extern _gconfig gc;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include <winsock.h>
	typedef int		(_stdcall *WSAStartupPROC	)	(WORD wVersionRequired, LPWSADATA lpWSAData);
	typedef int		(_stdcall *WSACleanupPROC	)	(void);
	typedef SOCKET	(_stdcall *socketPROC		)	(int af, int type, int protocol);
	typedef int		(_stdcall *setsockoptPROC	)	(SOCKET s, int level, int optname,const char* optval, int optlen);
	typedef int		(_stdcall *closesocketPROC	)	(SOCKET s);
	typedef int		(_stdcall *bindPROC			)	(SOCKET s, const struct sockaddr *addr, int namelen);
	typedef int		(_stdcall *sendtoPROC		)	(SOCKET s, const char * buf, int len, int flags,const struct sockaddr *to, int tolen);
	typedef int		(_stdcall *recvfromPROC		)	(SOCKET s, char * buf, int len, int flags,struct sockaddr FAR *from, int * fromlen);

	WSAStartupPROC	WSAStartupProc	 ;
	WSACleanupPROC	WSACleanupProc	 ;
	socketPROC		socketProc		 ;
	setsockoptPROC	setsockoptProc	 ;
	closesocketPROC	closesocketProc	 ;
	bindPROC		bindProc		 ;
	sendtoPROC		sendtoProc		 ;
	recvfromPROC	recvfromProc	 ;

	#define				PORT					37963
	#define				NO_FLAGS_SET 			0
	SOCKET 				destSocket				=INVALID_SOCKET;
	SOCKADDR_IN			destSockAddr;
	SOCKADDR_IN			recvSockAddr;
	SOCKET				recvSocket				=INVALID_SOCKET;
	int					tcpip_was_used			=0;
	int					tcpip_dynamically_loaded=0;
	WSADATA				Data;
	struct 				_udp_packet receivepacket;
    extern              CStrList remote;
	CRITICAL_SECTION	UdpSend;
	unsigned short		myhons(int port);

HMODULE htcpLib=0;

// ===========================================
   int init_tcpip_stuff(void)
// ===========================================
{
#ifdef DEBUDP
	char dbuf[300];
#endif
char    dummy[100],dummy1[100];
int		ret,status,enable=1;

	memset(dummy,0,sizeof(dummy));
	memset(dummy1,0,sizeof(dummy1));

	gc.tcpipLoadError=0;
	if (!remote.defaultindex || tcpip_was_used)	return 1;
	if (!dynamic_load_tcpip())	return 0;

	memset(&Data,0,sizeof(Data));
    if ((*WSAStartupProc) (0x0101,&Data))
	{
		gc.tcpipLoadError=1;
		return 0;
 	}
 	// specify the IP address
	memset(&destSockAddr, 0, sizeof(destSockAddr));
	destSockAddr.sin_addr.s_addr=INADDR_BROADCAST;
	destSockAddr.sin_port		=myhons(PORT);
	destSockAddr.sin_family		=AF_INET;

	// create a socket
	destSocket=(*socketProc)(AF_INET, SOCK_DGRAM, 0);
	if (destSocket == INVALID_SOCKET)
	{
		gc.tcpipLoadError=2;
		return 0;
 	}

    // permit broadcasting on the socket
    ret=(*setsockoptProc)(destSocket,SOL_SOCKET,SO_BROADCAST,(char *)&enable,4);
	if (ret!=0)	   		// setsockopt
	{
		#ifdef DEBUDP
			sprintf(dbuf,"UDP: setsockopt failed!!\n"); OutputDebugString(dbuf);
		#endif
		(*closesocketProc)(destSocket);
		destSocket=INVALID_SOCKET;
		gc.tcpipLoadError=3;
		return 0;
	}

	// Initialisiere das receive Zeug ...
	// Init TCP/IP Stuff
	memset(&recvSockAddr, 0, sizeof(recvSockAddr));
	recvSockAddr.sin_port			=myhons(PORT+1);
	recvSockAddr.sin_family			=AF_INET;
	recvSockAddr.sin_addr.s_addr	=INADDR_ANY;

	recvSocket=(*socketProc)(AF_INET, SOCK_DGRAM, 0);
	if (recvSocket == INVALID_SOCKET)
	{
		gc.tcpipLoadError=2;
		return 0;
 	}

	status=(*bindProc)(recvSocket,(LPSOCKADDR) &recvSockAddr,sizeof(recvSockAddr));
	if (status == SOCKET_ERROR)
	{
		(*closesocketProc)(recvSocket);
		gc.tcpipLoadError=4;
		return 0;
	}

	InitializeCriticalSection(&UdpSend);
	#ifdef DEBUDP
		sprintf(dbuf,"UDP: init_tcpip_stuff OK\n"); OutputDebugString(dbuf);
	#endif
	tcpip_was_used=1;
	return 1;
}

// ===============================================================
	int	send_datagram(char *tmp)
// ===============================================================
{
struct _udp_packet sp;

	if (!tcpip_was_used) return 0;	// Falls wir gar nicht initialisiert sind ...

	memset(&sp,0,sizeof(struct _udp_packet));
	sp.command=UDP_MODEM_LISTBOX;
	if (strlen(tmp)>99)
		tmp[99]=0;
	strcpy(sp.data,tmp);
	send_tcp_request(&sp);
	return 1;
}

// ===============================================================
   int send_tcp_request(_udp_packet *sendpacket)
// ===============================================================
{
#ifdef DEBUDP
	char dbuf[300];
#endif
int ret;

	if (!tcpip_was_used) return 0;

	EnterCriticalSection(&UdpSend);
	#ifdef DEBUDP
		sprintf(dbuf,"UDP: send_tcp_request\n"); OutputDebugString(dbuf);
	#endif
	ret=(*sendtoProc)(destSocket,(char *)sendpacket,sizeof(_udp_packet),NO_FLAGS_SET,(LPSOCKADDR)&destSockAddr,sizeof(destSockAddr));
	LeaveCriticalSection(&UdpSend);
	return 1;
}


// ===============================================================
  int free_tcpip_stuff (void)
// ===============================================================
{
#ifdef DEBUDP
	char dbuf[300];
#endif
	if (tcpip_was_used)
	{
		gc.DisableRemote=1;
		if (recvSocket!=INVALID_SOCKET)
		{
			(*closesocketProc)(recvSocket);
			recvSocket=INVALID_SOCKET;
		}

		if (destSocket!=INVALID_SOCKET)
		{
			(*closesocketProc)(destSocket);
			destSocket=INVALID_SOCKET;
		}
		(*WSACleanupProc)();
		DeleteCriticalSection(&UdpSend);
		tcpip_was_used=0;
		#ifdef DEBUDP
			sprintf(dbuf,"UDP: free_tcpip_stuff OK\n"); OutputDebugString(dbuf);
		#endif
	}
	return 1;
}


// ===============================================================
	int get_UDP_request(struct _udp_packet *receivepacket)
// ===============================================================
{
#ifdef DEBUDP
	char dbuf[300];
#endif
int numrcv;

	if (!tcpip_was_used) return 0;	// Falls wir gar nicht initialisiert sind ...

	memset(receivepacket,0,sizeof(receivepacket));

	__try
	{
		numrcv=(*recvfromProc)(recvSocket,(char *)receivepacket,sizeof(struct _udp_packet),NO_FLAGS_SET,NULL,NULL);
	} __except(1, EXCEPTION_EXECUTE_HANDLER )
	{
		numrcv=SOCKET_ERROR;
	}


	#ifdef DEBUDP
		sprintf(dbuf,"UDP: get_UDP_request <%d>\n",numrcv); OutputDebugString(dbuf);
	#endif
	return numrcv;
}


// ===============================================================
	int dynamic_load_tcpip(void)
// ===============================================================
{
#ifdef DEBUDP
	char dbuf[300];
#endif
    // Falls wir es bereits geladen haben ...
	if (tcpip_dynamically_loaded)
		return 1;

    /* Get a handle to the DLL module. */
    htcpLib = LoadLibrary("wsock32.dll");
	if (!htcpLib)
	{
		gc.tcpipLoadError=5;
		return 0;
 	}

	WSAStartupProc	=(WSAStartupPROC	)GetProcAddress(htcpLib,"WSAStartup");
	WSACleanupProc	=(WSACleanupPROC	)GetProcAddress(htcpLib,"WSACleanup");
	socketProc		=(socketPROC		)GetProcAddress(htcpLib,"socket");
	setsockoptProc	=(setsockoptPROC	)GetProcAddress(htcpLib,"setsockopt");
	closesocketProc	=(closesocketPROC	)GetProcAddress(htcpLib,"closesocket");
	bindProc		=(bindPROC			)GetProcAddress(htcpLib,"bind");
	sendtoProc		=(sendtoPROC		)GetProcAddress(htcpLib,"sendto");
	recvfromProc	=(recvfromPROC		)GetProcAddress(htcpLib,"recvfrom");

	if (
		(!WSAStartupProc)			||
		(!WSACleanupProc)			||
		(!socketProc	)			||
		(!setsockoptProc)			||
		(!closesocketProc)			||
		(!bindProc		)			||
		(!sendtoProc	)			||
		(!recvfromProc))

	{
		gc.tcpipLoadError=6;
		return 0;
 	}
	tcpip_dynamically_loaded=1;
	#ifdef DEBUDP
		sprintf(dbuf,"UDP: dynamic_load_tcpip OK\n"); OutputDebugString(dbuf);
	#endif

	return 1;
}


// ===============================================================
	int dynamic_unload_tcpip(void)
// ===============================================================
{
#ifdef DEBUDP
	char dbuf[300];
#endif
    // Falls gar nicht geladen haben ...
	if (!tcpip_dynamically_loaded)
		return 1;
	if (htcpLib)
	{
		FreeLibrary(htcpLib);
		htcpLib=0;
	}
	#ifdef DEBUDP
		sprintf(dbuf,"UDP: dynamic_unload_tcpip OK\n"); OutputDebugString(dbuf);
	#endif
	tcpip_dynamically_loaded=0;
	gc.tcpipLoadError=0;
	return 1;
}

// ===============================================================
	unsigned short myhons(int port)
// ===============================================================
{
unsigned short help;
unsigned short erg=0;
unsigned char  *p;
unsigned char  *pe;
unsigned char  x;

	help=(unsigned short) port;
	p= (unsigned char *)&help;
	pe=(unsigned char *)&erg;
	pe++;
	x=*p;
	*pe=x;
	pe--;
	p++;
	x=*p;
	*pe=x;
	return erg;
}


// ===============================================================
	int	net_debug(char *tmp)
// ===============================================================
{
char buf[300];

	sprintf(buf,"ND:%s",tmp);
	send_datagram(buf);
	return 1;
}
