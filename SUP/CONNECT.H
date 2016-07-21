/*--------------------------------------------------------------------------*\

    CONNECT.H	Version 1.1					    1995 AVM

    Header file for CONNECT.C

\*--------------------------------------------------------------------------*/
#ifndef _connect_h_
#define _connect_h_


#include "id.h"

/*--------------------------------------------------------------------------*\
 * Listen: send a LISTEN_REQ
 * parameters: CIPmask (which services shall be accepted) (see CAPI 2.0 spec.)
 * Listen will be sent to the number of controllers specified in InitISDN.
 * Listen with CIPmask = 0 results in getting no incoming calls signaled
 * by CAPI.
\*--------------------------------------------------------------------------*/
#define     ALL_SERVICES    0x1FFFFFFF
#define     SPEECH	    0x00000001
#define     DATA_TRANSFER   0x00000002
#define     AUDIO3_1KHZ     0x00000004
#define     TELEPHONY	    0x00010000
#define     FAX_GROUP2_3    0x00020000

unsigned Listen(unsigned long CIPmask);

/*--------------------------------------------------------------------------*\
 * Connect: try's to connect to 'CalledPartyNumber'
 * the return value of CAPI_PUT_CMSG is the same as CAPI_PUT_MESSAGE
 * (defined in CAPI 2.0 spec. error class 0x11xx )
 * If CallingPartyNumber is not needed, set to NULL.
 * CallingPartyNumber & CalledPartyNumber have to be zero terminated strings.
 * For datatransmission set the protocols to zero, B3Configuration to NULL
\*--------------------------------------------------------------------------*/
unsigned Connect(ConnectionID	     *Connection,
		 char		     *CalledPartyNumber,
		 char		     *CallingPartyNumber,
		 unsigned long	     Service,
		 unsigned short      B1Protocol,
		 unsigned short      B2Protocol,
		 unsigned short      B3Protocol,
		 unsigned char __far *B3Configuration);

/*--------------------------------------------------------------------------*\
 * Disconnect: disconnects one channel
 * The ConnectionID must be valid
\*--------------------------------------------------------------------------*/
unsigned Disconnect(ConnectionID Connection);

/*--------------------------------------------------------------------------*\
 * IncomingCall: signals an incoming call
 * This function will be executed if a CONNECT_INDication appears to
 * inform the user. This function has to be implemented in the main program
\*--------------------------------------------------------------------------*/
void IncomingCall(ConnectionID	Connection,
		  char		*CallingPartyNumber);

/*--------------------------------------------------------------------------*\
 * AnswerCall: answers incoming call with the specified reject-value
 *	       (for more see CAPI 2.0 spec.)
\*--------------------------------------------------------------------------*/
typedef enum _RejectValue {
	ACCEPT,
	CAPIIGNORE,
	REJECT
} RejectValue;

unsigned AnswerCall(ConnectionID	Connection,
		    RejectValue 	Reject,
		    unsigned short	B1Protocol,
		    unsigned short	B2Protocol,
		    unsigned short	B3Protocol,
		    unsigned char __far *B3Configuration);

/*--------------------------------------------------------------------------*\
 * StateChange: signals a state change on both B-channels (connected, disconnected)
 * Whenever a channel changes his state this function is called
 * This function has to be implemented in the main program
\*--------------------------------------------------------------------------*/
void StateChange(ConnectionID	  Connection,
		 ConnectionState  State);


#endif	/*----- _connect_h_ -----*/
