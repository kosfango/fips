// Diese ist unser 'Verbindungsinclude-File

#ifndef CRITICAL_SECTON
  #include <windows.h>
#endif


typedef void             *LPVOID;
typedef int 			ConnectionID;
typedef int 			(*CAPICALLBACK)(int);  	// funcptr is synonym for "pointer
                           						//    to function returning int"


typedef int 			(*USERBREAKCALLBACK)(int);  	// funcptr is synonym for "pointer
                           						//    to function returning int"


extern CRITICAL_SECTION GlobalCAPIAccess;

extern char capioutbound[65000];
extern int  capioutcount;


extern USERBREAKCALLBACK	UserBreakCallBackFunc;


// CAPI RingBuff Stuff
int capi_append_to_ringbuf			(char	*buffer, int len);
int capi_get_from_ringbuf			(int	toget,	 char *buffer,int *reallygot);
int capi_init_ringbuf_stuff			(void );
int capi_free_ringbuf_stuff			(void );
int capi_get_ringbufcount			(void);
int errout							(char *text);
int init_capi_stuff					(void);
int free_capi_stuff					(void);
int capi_clear_ringbuf				(void);
int capi_dial_system				(char *telnr,char *mynumber,int *abort_from_user);
int capi_disconnect_system			(void);
int capi_send_data					(char *data,int len);
int capi_getconnectstate			(void);


int  StartCapiWorkerThread			(void);
int  StopCapiWorkerThread			(void);
int  CapiWorkerThread				(void *p);

int 	capi_hangup					();


int	 load_capi_lib					(void);
int	 free_capi_lib					(void);
int	 capi_set_callbackfunc			(CAPICALLBACK funcptr);
int  capi_abort						(int value);
int	 userbreak_set_callbackfunc		(USERBREAKCALLBACK funcptr);


void Handle_CAPI_Msg				(void);  


// ein paar Macros ...
#define ISDNPORT  250		// !!!! BEI AENDERUNG KOPIE IN STRUCTS.H NICHT VERGERSSEN !!!!

#define B1PROTOCOL	    0
#define B2PROTOCOL	    0
#define B3PROTOCOL	    0
#define B3CONFIGURATION     NULL
#define INVALID_SLOT	-1
#define maxSlots		2	/*----- this demo program handles max. -----*/

#define ISDN_CONNNECT_STATE_INVALID	 	-1
#define ISDN_CONNNECT_STATE_BUSY	 	2
#define ISDN_CONNNECT_STATE_CONNECTED	1
#define ISDN_CONNNECT_STATE_	 -1


extern int    stop_capi_worker_thread;	     // Auf 1 setzten um den CAPI Worker Thread 
									 		 // zu beenden ...
extern int    senddatablock;				 // Ging der Versand eines Packets gut ?
extern ConnectionID	Slot[maxSlots];

int		get_free_outbuf							(void);
char*	put_in_outbuf							(char * data, int datalength, unsigned short handle);
int		free_handle_in_outbuf					(unsigned short handle);
int		check_for_old_blocks					(void);
int		get_outbound_buffer_access				(void);
int		free_outbound_buffer_access				(void);
int		free_critical_section_in_outbound		(void);
void	capi_saubermann							(void);





