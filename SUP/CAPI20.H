/*--------------------------------------------------------------------------*\

    CAPI20.H	Version 1.1					    1995 AVM

    Header file of CAPI20.LIB
    type definitions, CAPI functions, parameter definitions
    see CAPI 2.0 spec. february 1994

\*--------------------------------------------------------------------------*/
#ifndef _capi20_h_
#define _capi20_h_

#ifdef __cplusplus
extern "C" {
#endif

#define CPROT 0
#define DEBUG 0
#define TARGET_32BIT

#define __far
#define _far


// Unsere Prototypes ...


/*----- basic-type definitions -----*/

typedef unsigned char __far *CAPI_MESSAGE;
typedef unsigned char  _cbyte;
typedef unsigned short _cword;
typedef unsigned long  _cdword;
typedef CAPI_MESSAGE   _cstruct;
typedef enum {
    CAPI_COMPOSE,
    CAPI_DEFAULT
} _cmstruct;

/*----- errornumbers -----*/

typedef enum {
    CapiToManyAppls		     =0x1001,
    CapiLogBlkSizeToSmall	     =0x1002,
    CapiBuffExeceeds64k 	     =0x1003,
    CapiMsgBufSizeToSmall	     =0x1004,
    CapiAnzLogConnNotSupported	     =0x1005,
    CapiRegReserved		     =0x1006,
    CapiRegBusy 		     =0x1007,
    CapiRegOSResourceErr	     =0x1008,
    CapiRegNotInstalled 	     =0x1009,
    CapiRegCtrlerNotSupportExtEquip  =0x100a,
    CapiRegCtrlerOnlySupportExtEquip =0x100b
} CAPI_REGISTER_ERROR;

typedef enum {
    CapiIllAppNr		     =0x1101,
    CapiIllCmdOrSubcmdOrMsgToSmall   =0x1102,
    CapiSendQueueFull		     =0x1103,
    CapiReceiveQueueEmpty	     =0x1104,
    CapiReceiveOverflow 	     =0x1105,
    CapiUnknownNotPar		     =0x1106,
    CapiMsgBusy 		     =0x1107,
    CapiMsgOSResourceErr	     =0x1108,
    CapiMsgNotInstalled 	     =0x1109,
    CapiMsgCtrlerNotSupportExtEquip  =0x110a,
    CapiMsgCtrlerOnlySupportExtEquip =0x110b
} MESSAGE_EXCHANGE_ERROR;

/*----- CAPI commands -----*/

#define CAPI_ALERT 0x01
#define CAPI_CONNECT 0x02
#define CAPI_CONNECT_ACTIVE 0x03
#define CAPI_CONNECT_B3_ACTIVE 0x83
#define CAPI_CONNECT_B3 0x82
#define CAPI_CONNECT_B3_T90_ACTIVE 0x88
#define CAPI_DATA_B3 0x86
#define CAPI_DISCONNECT_B3 0x84
#define CAPI_DISCONNECT 0x04
#define CAPI_FACILITY 0x80
#define CAPI_INFO 0x08
#define CAPI_LISTEN 0x05
#define CAPI_MANUFACTURER 0xff
#define CAPI_RESET_B3 0x87
#define CAPI_SELECT_B_PROTOCOL 0x41

/*----- CAPI subcommands -----*/

#define CAPI_REQ  0x80
#define CAPI_CONF 0x81
#define CAPI_IND  0x82
#define CAPI_RESP 0x83

/*--------------------------------------------------------------------------*\
    The _cmsg structure contains all possible CAPI 2.0 parameter.
    All parameters are stored here first. The function CAPI_CMSG_2_MESSAGE
    assembles the parameter and builds CAPI2.0 conform messages.
    CAPI_MESSAGE_2_CMSG disassembles CAPI 2.0 messages and stores the
    parameter in the _cmsg structure
\*--------------------------------------------------------------------------*/

typedef struct {
    /* Header */
    _cword ApplId;
    _cbyte Command;
    _cbyte Subcommand;
    _cword Messagenumber;

    /* Parameter */
    union {
	_cdword adrController;
	_cdword adrPLCI;
	_cdword adrNCCI;
    } adr;

    _cmstruct AdditionalInfo;
    _cstruct B1configuration;
    _cword B1protocol;
    _cstruct B2configuration;
    _cword B2protocol;
    _cstruct B3configuration;
    _cword B3protocol;
    _cstruct BC;
    _cstruct BChannelinformation;
    _cmstruct BProtocol;
    _cstruct CalledPartyNumber;
    _cstruct CalledPartySubaddress;
    _cstruct CallingPartyNumber;
    _cstruct CallingPartySubaddress;
    _cdword CIPmask;
    _cdword CIPmask2;
    _cword CIPValue;
    _cdword Class;
    _cstruct ConnectedNumber;
    _cstruct ConnectedSubaddress;
    _cdword Data;
    _cword DataHandle;
    _cword DataLength;
    _cstruct FacilityConfirmationParameter;
    _cstruct Facilitydataarray;
    _cstruct FacilityIndicationParameter;
    _cstruct FacilityRequestParameter;
    _cword FacilitySelector;
    _cword Flags;
    _cdword Function;
    _cstruct HLC;
    _cword Info;
    _cstruct InfoElement;
    _cdword InfoMask;
    _cword InfoNumber;
    _cstruct Keypadfacility;
    _cstruct LLC;
    _cstruct ManuData;
    _cdword ManuID;
    _cstruct NCPI;
    _cword Reason;
    _cword Reason_B3;
    _cword Reject;
    _cstruct Useruserdata;

    /* intern */
    unsigned l,p;
    unsigned char *par;
    CAPI_MESSAGE m;
} _cmsg;

/*--------------------------------------------------------------------------*\
    CAPI_CMSG_2_MESSAGE assembles the parameter from _cmsg to a CAPI 2.0
    conform message
\*--------------------------------------------------------------------------*/
unsigned __far CAPI_CMSG_2_MESSAGE (_cmsg __far *cmsg, CAPI_MESSAGE msg);

/*--------------------------------------------------------------------------*\
    CAPI_MESSAGE_2_CMSG disassembles a CAPI message an writes the parameter
    into _cmsg for easy access
\*--------------------------------------------------------------------------*/
unsigned __far CAPI_MESSAGE_2_CMSG (_cmsg __far *cmsg, CAPI_MESSAGE msg);

/*--------------------------------------------------------------------------*\
    CAPI_PUT_CMSG works like CAPI_PUT_MESSAGE but it converts the _cmsg
    first with CAPI_CMSG_2_MESSAGE. Possible errors from CAPI_PUT_MESSAGE
    will be returned.
\*--------------------------------------------------------------------------*/
unsigned __far CAPI_PUT_CMSG (_cmsg __far *cmsg);

/*--------------------------------------------------------------------------*\
    CAPI_GET_CMSG works like CAPI_GET_MESSAGE and converts the CAPI message
    to a _cmsg with CAPI_MESSAGE_2_CMSG. Possible errors from CAPI_GET_MESSAGE
    will be returned.
\*--------------------------------------------------------------------------*/
unsigned __far CAPI_GET_CMSG (_cmsg __far *cmsg, unsigned applid);

/*--------------------------------------------------------------------------*\
    CAPI_CMSG_HEADER fills the _cmsg structure with default values, so only
    parameter with non default values must be changed before sending the
    message.
\*--------------------------------------------------------------------------*/
unsigned __far CAPI_CMSG_HEADER (_cmsg __far *cmsg, _cword _ApplId, _cbyte _Command, _cbyte _Subcommand, _cword _Messagenumber, _cdword _Controller);

/*--------------------------------------------------------------------------*\
    CAPI_CMSG_ANSWER is used to answer indications. It changes the header
    of an indication to a response, and leaves all other parameters the same
\*--------------------------------------------------------------------------*/
unsigned __far CAPI_CMSG_ANSWER (_cmsg __far *cmsg);

/*----- if the symbol CPROT is defined, protocol functions are included -----*/

#if defined (CPROT)
typedef enum {
    CAPI_PROTOCOL_HEADER,   /*----- occurs only once when CAPI_PROTOCOL_INIT is executed -----*/
    CAPI_PROTOCOL_MSG,	    /*----- the protocol-output is a CAPI Message -----*/
    CAPI_PROTOCOL_TXT,	    /*----- the output is text caused by CAPI_PROTOCOL_TEXT -----*/
} CAPI_PROTOCOL_TYP;

#define CAPI_PROTOCOL_INIT_BUF_SIZE  (9*1024)

/*--------------------------------------------------------------------------*\
    CAPI_PROTOCOL_INIT wants as first parameter a buffer with the size
    CAPI_PROTOCOL_INIT_BUF_SIZE. The second parameter is the address of
    a callback function with 3 parameter which is called whenever there is
    protocoltext to give out. The callback functions parameters are a pointer
    to the (ASCII) message, the type of the message and a pointer to the CAPI
    Message if the type is CAPI_PROTOCOL_MSG.
\*--------------------------------------------------------------------------*/
void __far CAPI_PROTOCOL_INIT (char *_buf, void (*_signal)(char *, CAPI_PROTOCOL_TYP, CAPI_MESSAGE));

/*--------------------------------------------------------------------------*\
    CAPI_PROTOCOL_MESSAGE prints the specified CAPI message on the protocol
\*--------------------------------------------------------------------------*/
void __far CAPI_PROTOCOL_MESSAGE (CAPI_MESSAGE msg);

/*--------------------------------------------------------------------------*\
    CAPI_PROTOCOL_TEXT prints the specified text on the protocol.
    It has the same syntax as printf
\*--------------------------------------------------------------------------*/
void __far CAPI_PROTOCOL_TEXT (char *fmt, ...);
#endif

/*----- defines to access specific parameter -----*/

#define ALERT_REQ_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define ALERT_REQ_ADDITIONALINFO(x) ((x)->AdditionalInfo)
		 /* Additional info elements */
#define ALERT_REQ_BCHANNELINFORMATION(x) ((x)->BChannelinformation)
#define ALERT_REQ_KEYPADFACILITY(x) ((x)->Keypadfacility)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define ALERT_REQ_USERUSERDATA(x) ((x)->Useruserdata)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define ALERT_REQ_FACILITYDATAARRAY(x) ((x)->Facilitydataarray)
		 /* which is used to transfer additional parameters coded */
		 /* according to ETS 300 102-1 / Q.931 starting from octet 1.  */
		 /* This field is used to transport one or more complete facility  */
		 /* data information elements. */
#define ALERT_CONF_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define ALERT_CONF_INFO(x) ((x)->Info)
		 /* 0: alert initiated */
		 /* 0x0003: alert already sent by another application */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal PLCI */
		 /* 0x2007: illegal message parameter coding */
#define CONNECT_REQ_CONTROLLER(x) ((x)->adr.adrController)
#define CONNECT_REQ_CIPVALUE(x) ((x)->CIPValue)
		 /* Compatibility Information Profile */
#define CONNECT_REQ_CALLEDPARTYNUMBER(x) ((x)->CalledPartyNumber)
		 /* Called party number */
#define CONNECT_REQ_CALLINGPARTYNUMBER(x) ((x)->CallingPartyNumber)
		 /* Calling party number */
#define CONNECT_REQ_CALLEDPARTYSUBADDRESS(x) ((x)->CalledPartySubaddress)
		 /* Called party subaddress */
#define CONNECT_REQ_CALLINGPARTYSUBADDRESS(x) ((x)->CallingPartySubaddress)
		 /* Calling party subaddress */
#define CONNECT_REQ_BPROTOCOL(x) ((x)->BProtocol)
		 /* B protocol to be used */
#define CONNECT_REQ_B1PROTOCOL(x) ((x)->B1protocol)
		 /* Physical layer and framing */
#define CONNECT_REQ_B2PROTOCOL(x) ((x)->B2protocol)
		 /* Data link layer */
#define CONNECT_REQ_B3PROTOCOL(x) ((x)->B3protocol)
		 /* Network layer */
#define CONNECT_REQ_B1CONFIGURATION(x) ((x)->B1configuration)
		 /* Physical layer and framing parameter */
#define CONNECT_REQ_B2CONFIGURATION(x) ((x)->B2configuration)
		 /* Data link layer parameter */
#define CONNECT_REQ_B3CONFIGURATION(x) ((x)->B3configuration)
		 /* Network layer parameter */
#define CONNECT_REQ_BC(x) ((x)->BC)
		 /* Bearer Capability */
#define CONNECT_REQ_LLC(x) ((x)->LLC)
		 /* Low Layer Compatibility */
#define CONNECT_REQ_HLC(x) ((x)->HLC)
		 /* High Layer Compatibility */
#define CONNECT_REQ_ADDITIONALINFO(x) ((x)->AdditionalInfo)
		 /* Additional information elements */
#define CONNECT_REQ_BCHANNELINFORMATION(x) ((x)->BChannelinformation)
#define CONNECT_REQ_KEYPADFACILITY(x) ((x)->Keypadfacility)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define CONNECT_REQ_USERUSERDATA(x) ((x)->Useruserdata)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define CONNECT_REQ_FACILITYDATAARRAY(x) ((x)->Facilitydataarray)
		 /* which is used to transfer additional parameters coded */
		 /* according to ETS 300 102-1 / Q.931 starting from octet 1.  */
		 /* This field is used to transport one or more complete facility  */
		 /* data information elements. */
#define CONNECT_CONF_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define CONNECT_CONF_INFO(x) ((x)->Info)
		 /* 0: connect initiated */
		 /* 0x2002: illegal controller */
		 /* 0x2003: out of PLCI */
		 /* 0x2007: illegal message parameter coding */
		 /* 0x3001: B1 protocol not supported */
		 /* 0x3002: B2 protocol not supported */
		 /* 0x3003: B3 protocol not supported */
		 /* 0x3004: B1 protocol parameter not supported */
		 /* 0x3005: B2 protocol parameter not supported */
		 /* 0x3006: B3 protocol parameter not supported */
		 /* 0x3007: B protocol combination not supported */
		 /* 0x300A: CIP Value unknown */
#define CONNECT_IND_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define CONNECT_IND_CIPVALUE(x) ((x)->CIPValue)
		 /* Compatibility Information Profile */
#define CONNECT_IND_CALLEDPARTYNUMBER(x) ((x)->CalledPartyNumber)
		 /* Called party number */
#define CONNECT_IND_CALLINGPARTYNUMBER(x) ((x)->CallingPartyNumber)
		 /* Calling party number */
#define CONNECT_IND_CALLEDPARTYSUBADDRESS(x) ((x)->CalledPartySubaddress)
		 /* Called party subaddress */
#define CONNECT_IND_CALLINGPARTYSUBADDRESS(x) ((x)->CallingPartySubaddress)
		 /* Calling party subaddress */
#define CONNECT_IND_BC(x) ((x)->BC)
		 /* Bearer compatibility */
#define CONNECT_IND_LLC(x) ((x)->LLC)
		 /* Low Layer Compatibility */
#define CONNECT_IND_HLC(x) ((x)->HLC)
		 /* High Layer Compatibility */
#define CONNECT_IND_ADDITIONALINFO(x) ((x)->AdditionalInfo)
		 /* Additional information elements */
#define CONNECT_IND_BCHANNELINFORMATION(x) ((x)->BChannelinformation)
#define CONNECT_IND_KEYPADFACILITY(x) ((x)->Keypadfacility)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define CONNECT_IND_USERUSERDATA(x) ((x)->Useruserdata)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define CONNECT_IND_FACILITYDATAARRAY(x) ((x)->Facilitydataarray)
		 /* which is used to transfer additional parameters coded */
		 /* according to ETS 300 102-1 / Q.931 starting from octet 1.  */
		 /* This field is used to transport one or more complete facility  */
		 /* data information elements. */
#define CONNECT_RESP_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define CONNECT_RESP_REJECT(x) ((x)->Reject)
		 /* 0: accept call */
		 /* 1: ignore call */
		 /* 2: reject call, normal call clearing */
		 /* 3: reject call, user busy */
		 /* 4: reject call, requestet circuit/channel not available */
		 /* 5: reject call, facility rejected */
		 /* 6: reject call, channel unacceptable */
		 /* 7: reject call, incompatible destination */
		 /* 8: reject call, destination out of order */
#define CONNECT_RESP_BPROTOCOL(x) ((x)->BProtocol)
		 /* B protocol to be used */
#define CONNECT_RESP_B1PROTOCOL(x) ((x)->B1protocol)
		 /* Physical layer and framing */
#define CONNECT_RESP_B2PROTOCOL(x) ((x)->B2protocol)
		 /* Data link layer */
#define CONNECT_RESP_B3PROTOCOL(x) ((x)->B3protocol)
		 /* Network layer */
#define CONNECT_RESP_B1CONFIGURATION(x) ((x)->B1configuration)
		 /* Physical layer and framing parameter */
#define CONNECT_RESP_B2CONFIGURATION(x) ((x)->B2configuration)
		 /* Data link layer parameter */
#define CONNECT_RESP_B3CONFIGURATION(x) ((x)->B3configuration)
		 /* Network layer parameter */
#define CONNECT_RESP_CONNECTEDNUMBER(x) ((x)->ConnectedNumber)
		 /* Connected number */
#define CONNECT_RESP_CONNECTEDSUBADDRESS(x) ((x)->ConnectedSubaddress)
		 /* Connected subaddress */
#define CONNECT_RESP_LLC(x) ((x)->LLC)
		 /* Low Layer Compatibility */
#define CONNECT_RESP_ADDITIONALINFO(x) ((x)->AdditionalInfo)
		 /* Additional information elements */
#define CONNECT_RESP_BCHANNELINFORMATION(x) ((x)->BChannelinformation)
#define CONNECT_RESP_KEYPADFACILITY(x) ((x)->Keypadfacility)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define CONNECT_RESP_USERUSERDATA(x) ((x)->Useruserdata)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define CONNECT_RESP_FACILITYDATAARRAY(x) ((x)->Facilitydataarray)
		 /* which is used to transfer additional parameters coded */
		 /* according to ETS 300 102-1 / Q.931 starting from octet 1.  */
		 /* This field is used to transport one or more complete facility  */
		 /* data information elements. */
#define CONNECT_ACTIVE_IND_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define CONNECT_ACTIVE_IND_CONNECTEDNUMBER(x) ((x)->ConnectedNumber)
		 /* Connected number */
#define CONNECT_ACTIVE_IND_CONNECTEDSUBADDRESS(x) ((x)->ConnectedSubaddress)
		 /* Connected subaddress */
#define CONNECT_ACTIVE_IND_LLC(x) ((x)->LLC)
		 /* Low Layer Compatibility */
#define CONNECT_ACTIVE_RESP_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define CONNECT_B3_ACTIVE_IND_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define CONNECT_B3_ACTIVE_IND_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define CONNECT_B3_ACTIVE_RESP_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define CONNECT_B3_REQ_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define CONNECT_B3_REQ_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define CONNECT_B3_CONF_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define CONNECT_B3_CONF_INFO(x) ((x)->Info)
		 /* 0: connect initiated */
		 /* 0x0001: NCPI not supported by current protocol, NCPI ignored */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal PLCI */
		 /* 0x2004: out of NCCI */
		 /* 0x3008: NCPI not supported */
#define CONNECT_B3_IND_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define CONNECT_B3_IND_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define CONNECT_B3_RESP_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define CONNECT_B3_RESP_REJECT(x) ((x)->Reject)
		 /* 0: accept call */
		 /* 2: reject call, normal call clearing */
#define CONNECT_B3_RESP_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define CONNECT_B3_T90_ACTIVE_IND_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define CONNECT_B3_T90_ACTIVE_IND_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define CONNECT_B3_T90_ACTIVE_RESP_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DATA_B3_REQ_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DATA_B3_REQ_DATA(x) ((x)->Data)
		 /* Pointer to the data to be sent */
#define DATA_B3_REQ_DATALENGTH(x) ((x)->DataLength)
		 /* Size of data area to be sent */
#define DATA_B3_REQ_DATAHANDLE(x) ((x)->DataHandle)
		 /* Referenced in DATA_B3_CONF */
#define DATA_B3_REQ_FLAGS(x) ((x)->Flags)
		 /* [0]: qualifier bit */
		 /* [1]: more data bit */
		 /* [2]: delivery confirmation bit */
		 /* [3]: expedited data */
		 /* [4] to [15]: reserved */
#define DATA_B3_CONF_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DATA_B3_CONF_DATAHANDLE(x) ((x)->DataHandle)
		 /* Identifies the data area of corresponding DATA_B3_REQ */
#define DATA_B3_CONF_INFO(x) ((x)->Info)
		 /* 0: data transmission initiated */
		 /* 0x0002: flags not supported by current protocol, flags ignored */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal NCCI */
		 /* 0x2007: illegal message parameter coding */
		 /* 0x300A: flags not supported (reserved bits) */
		 /* 0x300C: data length not supported by current protocol */
#define DATA_B3_IND_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DATA_B3_IND_DATA(x) ((x)->Data)
		 /* Pointer to data received */
#define DATA_B3_IND_DATALENGTH(x) ((x)->DataLength)
		 /* Size of data area received */
#define DATA_B3_IND_DATAHANDLE(x) ((x)->DataHandle)
		 /* handle to data area, referenced in DATA_B3_RESP */
#define DATA_B3_IND_FLAGS(x) ((x)->Flags)
		 /* 0: qualifier bit */
		 /* 1: more-data bit */
		 /* 2: delivery confirmation bit */
		 /* 3: expedited data */
		 /* 4 to 14: reserved */
		 /* [15]: framing error bit, data may be invalid (only with corresponding B2 protocol) */
#define DATA_B3_RESP_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DATA_B3_RESP_DATAHANDLE(x) ((x)->DataHandle)
		 /* Data area reference in corresponding DATA_B3_IND */
#define DISCONNECT_B3_REQ_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DISCONNECT_B3_REQ_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define DISCONNECT_B3_CONF_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DISCONNECT_B3_CONF_INFO(x) ((x)->Info)
		 /* 0: disconnect initiated */
		 /* 0x0001: NCPI not supported by current protocol, NCPI ignored */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal NCCI */
		 /* 0x2007: illegal message parameter coding */
		 /* 0x3008: NCPI not supported */
#define DISCONNECT_B3_IND_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DISCONNECT_B3_IND_REASON_B3(x) ((x)->Reason_B3)
		 /* 0: clearing according to protocol */
		 /* 0x3301: protocol error layer 1 */
		 /* 0x3302: protocol error layer 2 */
		 /* 0x3303: protocol error layer 3 */
		 /* protocol dependent values are described in chapter 6 */
#define DISCONNECT_B3_IND_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define DISCONNECT_B3_RESP_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define DISCONNECT_REQ_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define DISCONNECT_REQ_ADDITIONALINFO(x) ((x)->AdditionalInfo)
		 /* Additional information elements */
#define DISCONNECT_REQ_BCHANNELINFORMATION(x) ((x)->BChannelinformation)
#define DISCONNECT_REQ_KEYPADFACILITY(x) ((x)->Keypadfacility)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define DISCONNECT_REQ_USERUSERDATA(x) ((x)->Useruserdata)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define DISCONNECT_REQ_FACILITYDATAARRAY(x) ((x)->Facilitydataarray)
		 /* which is used to transfer additional parameters coded */
		 /* according to ETS 300 102-1 / Q.931 starting from octet 1.  */
		 /* This field is used to transport one or more complete facility  */
		 /* data information elements. */
#define DISCONNECT_CONF_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define DISCONNECT_CONF_INFO(x) ((x)->Info)
		 /* 0: disconnect initiated */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal PLCI */
		 /* 0x2007: illegal message parameter coding */
#define DISCONNECT_IND_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define DISCONNECT_IND_REASON(x) ((x)->Reason)
		 /* 0: no cause available */
		 /* 0x3301: protocol error layer 1 */
		 /* 0x3302: protocol error layer 2 */
		 /* 0x3303: protocol error layer 3 */
		 /* 0x3304: another application got that call */
		 /* 0x34xx: disconnect cause from the network according to Q.931/ETS 300 102-1. In the field 'xx' the cause value received within a cause information element (octet 4) from the network is indicated. */
#define DISCONNECT_RESP_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define FACILITY_REQ_CONTROLLER(x) ((x)->adr.adrController)
#define FACILITY_REQ_PLCI(x) ((x)->adr.adrPLCI)
#define FACILITY_REQ_NCCI(x) ((x)->adr.adrNCCI)
		 /* Depending on the facility selector */
#define FACILITY_REQ_FACILITYSELECTOR(x) ((x)->FacilitySelector)
		 /* 0: Handset Support */
		 /* 1: DTMF */
		 /* 2 to n: reserved */
#define FACILITY_REQ_FACILITYREQUESTPARAMETER(x) ((x)->FacilityRequestParameter)
		 /* Facility depending parameters */
#define FACILITY_CONF_CONTROLLER(x) ((x)->adr.adrController)
#define FACILITY_CONF_PLCI(x) ((x)->adr.adrPLCI)
#define FACILITY_CONF_NCCI(x) ((x)->adr.adrNCCI)
		 /* Depending on the facility selector */
#define FACILITY_CONF_INFO(x) ((x)->Info)
		 /* 0: request accepted */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: incorrect Controller/PLCI/NCCI */
		 /* 0x2007: illegal message parameter coding */
		 /* 0x300B: facility not supported */
#define FACILITY_CONF_FACILITYSELECTOR(x) ((x)->FacilitySelector)
		 /* 0: Handset Support */
		 /* 1: DTMF */
		 /* 2 to n: reserved */
#define FACILITY_CONF_FACILITYCONFIRMATIONPARAMETER(x) ((x)->FacilityConfirmationParameter)
		 /* Facility-depending parameters */
#define FACILITY_IND_CONTROLLER(x) ((x)->adr.adrController)
#define FACILITY_IND_PLCI(x) ((x)->adr.adrPLCI)
#define FACILITY_IND_NCCI(x) ((x)->adr.adrNCCI)
		 /* Depending on the facility selector */
#define FACILITY_IND_FACILITYSELECTOR(x) ((x)->FacilitySelector)
		 /* 0: Handset Support */
		 /* 1: DTMF */
		 /* 2 to n: reserved */
#define FACILITY_IND_FACILITYINDICATIONPARAMETER(x) ((x)->FacilityIndicationParameter)
		 /* Facility-depending parameters */
#define FACILITY_RESP_CONTROLLER(x) ((x)->adr.adrController)
#define FACILITY_RESP_PLCI(x) ((x)->adr.adrPLCI)
#define FACILITY_RESP_NCCI(x) ((x)->adr.adrNCCI)
		 /* Depending on the facility selector */
#define FACILITY_RESP_FACILITYSELECTOR(x) ((x)->FacilitySelector)
		 /* 0: Handset Support */
		 /* 1: DTMF */
		 /* 2 to n: reserved */
		 /* Facility response parameters */
		 /* struct */
		 /* Facility-depending parameters */
#define INFO_REQ_CONTROLLER(x) ((x)->adr.adrController)
#define INFO_REQ_PLCI(x) ((x)->adr.adrPLCI)
		 /* See note */
#define INFO_REQ_CALLEDPARTYNUMBER(x) ((x)->CalledPartyNumber)
		 /* Called party number */
#define INFO_REQ_ADDITIONALINFO(x) ((x)->AdditionalInfo)
		 /* Additional information elements */
#define INFO_REQ_BCHANNELINFORMATION(x) ((x)->BChannelinformation)
#define INFO_REQ_KEYPADFACILITY(x) ((x)->Keypadfacility)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define INFO_REQ_USERUSERDATA(x) ((x)->Useruserdata)
		 /* coded according to ETS 300 102-1 / Q.931 */
#define INFO_REQ_FACILITYDATAARRAY(x) ((x)->Facilitydataarray)
		 /* which is used to transfer additional parameters coded */
		 /* according to ETS 300 102-1 / Q.931 starting from octet 1.  */
		 /* This field is used to transport one or more complete facility  */
		 /* data information elements. */
#define INFO_CONF_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define INFO_CONF_INFO(x) ((x)->Info)
		 /* 0: transmission of information initiated */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal Controller/PLCI */
		 /* 0x2003: out of PLCI */
		 /* 0x2007: illegal message parameter coding */
#define INFO_IND_CONTROLLER(x) ((x)->adr.adrController)
#define INFO_IND_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define INFO_IND_INFONUMBER(x) ((x)->InfoNumber)
		 /* Information element identifier */
#define INFO_IND_INFOELEMENT(x) ((x)->InfoElement)
		 /* Information element dependent structure */
#define INFO_RESP_CONTROLLER(x) ((x)->adr.adrController)
#define INFO_RESP_PLCI(x) ((x)->adr.adrPLCI)
		 /* As in INFO_IND */
#define LISTEN_REQ_CONTROLLER(x) ((x)->adr.adrController)
#define LISTEN_REQ_INFOMASK(x) ((x)->InfoMask)
		 /* Bit field, coding as follows: */
		 /* 0: cause */
		 /* 1: date/Time */
		 /* 2: display */
		 /* 3: user-user information */
		 /* 4: call progression */
		 /* 5: facility */
		 /* 6: charging */
		 /* 7 to 31: reserved */
#define LISTEN_REQ_CIPMASK(x) ((x)->CIPmask)
		 /* Bit field, coding as follows: */
		 /* 0: any match */
		 /* 1: speech */
		 /* 2: unrestricted digital information */
		 /* 3: restricted digital information */
		 /* 4: 3.1 kHz audio */
		 /* 5: 7.0 kHz audio */
		 /* 6: video */
		 /* 7: packet mode */
		 /* 8: 56 kBit/s rate adaptation */
		 /* 9: unrestricted digital information with tones/announcements */
		 /* 10..15: reserved */
		 /* 16: telephony */
		 /* 17: fax group 2/3 */
		 /* 18: fax group 4 class 1 */
		 /* 19: Teletex service (basic and mixed), fax group 4 class 2 */
		 /* 20: Teletex service (basic and processable) */
		 /* 21: Teletex service (basic) */
		 /* 22: Videotex */
		 /* 23: Telex */
		 /* reserved for additional services */
		 /* 25: OSI applications according X.200 */
		 /* 26: 7 kHz Telephony */
		 /* 27: Video Telephony F.721, first connection */
		 /* 28: Video Telephony F.721, second connection */
		 /* 29 to 31: reserved */
#define LISTEN_REQ_CIPMASK2(x) ((x)->CIPmask2)
		 /* reserved for additional services */
#define LISTEN_REQ_CALLINGPARTYNUMBER(x) ((x)->CallingPartyNumber)
		 /* Calling party number */
#define LISTEN_REQ_CALLINGPARTYSUBADDRESS(x) ((x)->CallingPartySubaddress)
		 /* Calling party subaddress */
#define LISTEN_CONF_CONTROLLER(x) ((x)->adr.adrController)
#define LISTEN_CONF_INFO(x) ((x)->Info)
		 /* 0: listen is active */
		 /* 0x2002: illegal controller */
		 /* 0x2005: out of LISTEN-Resources */
		 /* 0x2007: illegal message parameter coding */
#define MANUFACTURER_REQ_CONTROLLER(x) ((x)->adr.adrController)
#define MANUFACTURER_REQ_MANUID(x) ((x)->ManuID)
		 /* Manufacturer specific ID (should be unique) */
		 /* Manufacturer specific */
#define MANUFACTURER_REQ_CLASS(x) ((x)->Class)
#define MANUFACTURER_REQ_FUNCTION(x) ((x)->Function)
#define MANUFACTURER_REQ_MANUDATA(x) ((x)->ManuData)
		 /* Manufacturer specific data */
#define MANUFACTURER_CONF_CONTROLLER(x) ((x)->adr.adrController)
#define MANUFACTURER_CONF_MANUID(x) ((x)->ManuID)
		 /* Manufacturer specific ID (should be unique) */
		 /* Manufacturer specific */
#define MANUFACTURER_CONF_CLASS(x) ((x)->Class)
#define MANUFACTURER_CONF_FUNCTION(x) ((x)->Function)
#define MANUFACTURER_CONF_MANUDATA(x) ((x)->ManuData)
		 /* Manufacturer specific data */
#define MANUFACTURER_IND_CONTROLLER(x) ((x)->adr.adrController)
#define MANUFACTURER_IND_MANUID(x) ((x)->ManuID)
		 /* Manufacturer specific ID (should be unique) */
		 /* Manufacturer specific */
#define MANUFACTURER_IND_CLASS(x) ((x)->Class)
#define MANUFACTURER_IND_FUNCTION(x) ((x)->Function)
#define MANUFACTURER_IND_MANUDATA(x) ((x)->ManuData)
		 /* Manufacturer specific data */
#define MANUFACTURER_RESP_CONTROLLER(x) ((x)->adr.adrController)
#define MANUFACTURER_RESP_MANUID(x) ((x)->ManuID)
		 /* Manufacturer specific ID (should be unique) */
		 /* Manufacturer specific */
#define MANUFACTURER_RESP_CLASS(x) ((x)->Class)
#define MANUFACTURER_RESP_FUNCTION(x) ((x)->Function)
#define MANUFACTURER_RESP_MANUDATA(x) ((x)->ManuData)
		 /* Manufacturer specific data */
#define RESET_B3_REQ_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define RESET_B3_REQ_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define RESET_B3_CONF_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define RESET_B3_CONF_INFO(x) ((x)->Info)
		 /* 0: reset initiated */
		 /* 0x0001: NCPI not supported by current protocol, NCPI ignored */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal NCCI */
		 /* 0x2007: illegal message parameter coding */
		 /* 0x3008: NCPI not supported */
		 /* 0x300D: reset procedure not supported by current protocol */
#define RESET_B3_IND_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define RESET_B3_IND_NCPI(x) ((x)->NCPI)
		 /* Network Control Protocol Information */
#define RESET_B3_RESP_NCCI(x) ((x)->adr.adrNCCI)
		 /* Network Control Connection Identifier */
#define SELECT_B_PROTOCOL_REQ_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define SELECT_B_PROTOCOL_REQ_BPROTOCOL(x) ((x)->BProtocol)
		 /* Protocol definition */
#define SELECT_B_PROTOCOL_REQ_B1PROTOCOL(x) ((x)->B1protocol)
		 /* Physical layer and framing */
#define SELECT_B_PROTOCOL_REQ_B2PROTOCOL(x) ((x)->B2protocol)
		 /* Data link layer */
#define SELECT_B_PROTOCOL_REQ_B3PROTOCOL(x) ((x)->B3protocol)
		 /* Network layer */
#define SELECT_B_PROTOCOL_REQ_B1CONFIGURATION(x) ((x)->B1configuration)
		 /* Physical layer and framing parameter */
#define SELECT_B_PROTOCOL_REQ_B2CONFIGURATION(x) ((x)->B2configuration)
		 /* Data link layer parameter */
#define SELECT_B_PROTOCOL_REQ_B3CONFIGURATION(x) ((x)->B3configuration)
		 /* Network layer parameter */
#define SELECT_B_PROTOCOL_CONF_PLCI(x) ((x)->adr.adrPLCI)
		 /* Physical Link Connection Identifier */
#define SELECT_B_PROTOCOL_CONF_INFO(x) ((x)->Info)
		 /* 0: protocol switch successful */
		 /* 0x2001: message not supported in current state */
		 /* 0x2002: illegal PLCI */
		 /* 0x2007: illegal message parameter coding */
		 /* 0x3001: B1 protocol not supported */
		 /* 0x3002: B2 protocol not supported */
		 /* 0x3003: B3 protocol not supported */
		 /* 0x3004: B1 protocol parameter not supported */
		 /* 0x3005: B2 protocol parameter not supported */
		 /* 0x3006: B3 protocol parameter not supported */
		 /* 0x3007: B protocol combination not supported */

/*----- tests for specific commands -----*/

#define IS_CONNECT_IND(m) ((m)->Command==0x02 && (m)->Subcommand==0x82)
#define IS_CONNECT_ACTIVE_IND(m) ((m)->Command==0x03 && (m)->Subcommand==0x82)
#define IS_CONNECT_B3_ACTIVE_IND(m) ((m)->Command==0x83 && (m)->Subcommand==0x82)
#define IS_CONNECT_B3_IND(m) ((m)->Command==0x82 && (m)->Subcommand==0x82)
#define IS_CONNECT_B3_T90_ACTIVE_IND(m) ((m)->Command==0x88 && (m)->Subcommand==0x82)
#define IS_DATA_B3_IND(m) ((m)->Command==0x86 && (m)->Subcommand==0x82)
#define IS_DISCONNECT_B3_IND(m) ((m)->Command==0x84 && (m)->Subcommand==0x82)
#define IS_DISCONNECT_IND(m) ((m)->Command==0x04 && (m)->Subcommand==0x82)
#define IS_FACILITY_IND(m) ((m)->Command==0x80 && (m)->Subcommand==0x82)
#define IS_INFO_IND(m) ((m)->Command==0x08 && (m)->Subcommand==0x82)
#define IS_MANUFACTURER_IND(m) ((m)->Command==0xff && (m)->Subcommand==0x82)
#define IS_RESET_B3_IND(m) ((m)->Command==0x87 && (m)->Subcommand==0x82)
#define IS_ALERT_CONF(m) ((m)->Command==0x01 && (m)->Subcommand==0x81)
#define IS_CONNECT_CONF(m) ((m)->Command==0x02 && (m)->Subcommand==0x81)
#define IS_CONNECT_B3_CONF(m) ((m)->Command==0x82 && (m)->Subcommand==0x81)
#define IS_DATA_B3_CONF(m) ((m)->Command==0x86 && (m)->Subcommand==0x81)
#define IS_DISCONNECT_B3_CONF(m) ((m)->Command==0x84 && (m)->Subcommand==0x81)
#define IS_DISCONNECT_CONF(m) ((m)->Command==0x04 && (m)->Subcommand==0x81)
#define IS_FACILITY_CONF(m) ((m)->Command==0x80 && (m)->Subcommand==0x81)
#define IS_INFO_CONF(m) ((m)->Command==0x08 && (m)->Subcommand==0x81)
#define IS_LISTEN_CONF(m) ((m)->Command==0x05 && (m)->Subcommand==0x81)
#define IS_MANUFACTURER_CONF(m) ((m)->Command==0xff && (m)->Subcommand==0x81)
#define IS_RESET_B3_CONF(m) ((m)->Command==0x87 && (m)->Subcommand==0x81)
#define IS_SELECT_B_PROTOCOL_CONF(m) ((m)->Command==0x41 && (m)->Subcommand==0x81)

/*----- header functions that fill the _cmsg structure with default -----*/
/*----- values. Only nonstandard parameter need to be changed -----*/

#define ALERT_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x01,0x80,Messagenumber,adr)
#define CONNECT_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x02,0x80,Messagenumber,adr)
#define CONNECT_B3_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x82,0x80,Messagenumber,adr)
#define DATA_B3_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x86,0x80,Messagenumber,adr)
#define DISCONNECT_B3_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x84,0x80,Messagenumber,adr)
#define DISCONNECT_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x04,0x80,Messagenumber,adr)
#define FACILITY_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x80,0x80,Messagenumber,adr)
#define INFO_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x08,0x80,Messagenumber,adr)
#define LISTEN_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x05,0x80,Messagenumber,adr)
#define MANUFACTURER_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0xff,0x80,Messagenumber,adr)
#define RESET_B3_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x87,0x80,Messagenumber,adr)
#define SELECT_B_PROTOCOL_REQ_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x41,0x80,Messagenumber,adr)
#define CONNECT_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x02,0x83,Messagenumber,adr)
#define CONNECT_ACTIVE_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x03,0x83,Messagenumber,adr)
#define CONNECT_B3_ACTIVE_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x83,0x83,Messagenumber,adr)
#define CONNECT_B3_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x82,0x83,Messagenumber,adr)
#define CONNECT_B3_T90_ACTIVE_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x88,0x83,Messagenumber,adr)
#define DATA_B3_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x86,0x83,Messagenumber,adr)
#define DISCONNECT_B3_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x84,0x83,Messagenumber,adr)
#define DISCONNECT_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x04,0x83,Messagenumber,adr)
#define FACILITY_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x80,0x83,Messagenumber,adr)
#define INFO_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x08,0x83,Messagenumber,adr)
#define MANUFACTURER_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0xff,0x83,Messagenumber,adr)
#define RESET_B3_RESP_HEADER(cmsg,ApplId,Messagenumber,adr) \
		      CAPI_CMSG_HEADER(cmsg,ApplId,0x87,0x83,Messagenumber,adr)

/*--------------------------------------------------------------------------*\
    CAPI-functions that correspond to the CAPI messages specified in the
    CAPI 2.0 specification. All possible parameter have to be specified.
    The CAPI message is sent immediately when calling these functions,
    return values are the same as in CAPI_PUT_MESSAGE
\*--------------------------------------------------------------------------*/

unsigned ALERT_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cstruct BChannelinformation
		,_cstruct Keypadfacility
		,_cstruct Useruserdata
		,_cstruct Facilitydataarray);
unsigned CONNECT_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cword CIPValue
		,_cstruct CalledPartyNumber
		,_cstruct CallingPartyNumber
		,_cstruct CalledPartySubaddress
		,_cstruct CallingPartySubaddress
		,_cword B1protocol
		,_cword B2protocol
		,_cword B3protocol
		,_cstruct B1configuration
		,_cstruct B2configuration
		,_cstruct B3configuration
		,_cstruct BC
		,_cstruct LLC
		,_cstruct HLC
		,_cstruct BChannelinformation
		,_cstruct Keypadfacility
		,_cstruct Useruserdata
		,_cstruct Facilitydataarray);
unsigned CONNECT_B3_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cstruct NCPI);
unsigned DATA_B3_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cdword Data
		,_cword DataLength
		,_cword DataHandle
		,_cword Flags);
unsigned DISCONNECT_B3_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cstruct NCPI);
unsigned DISCONNECT_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cstruct BChannelinformation
		,_cstruct Keypadfacility
		,_cstruct Useruserdata
		,_cstruct Facilitydataarray);
unsigned FACILITY_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cword FacilitySelector
		,_cstruct FacilityRequestParameter);
unsigned INFO_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cstruct CalledPartyNumber
		,_cstruct BChannelinformation
		,_cstruct Keypadfacility
		,_cstruct Useruserdata
		,_cstruct Facilitydataarray);
unsigned LISTEN_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cdword InfoMask
		,_cdword CIPmask
		,_cdword CIPmask2
		,_cstruct CallingPartyNumber
		,_cstruct CallingPartySubaddress);
unsigned MANUFACTURER_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cdword ManuID
		,_cdword Class
		,_cdword Function
		,_cstruct ManuData);
unsigned RESET_B3_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cstruct NCPI);
unsigned SELECT_B_PROTOCOL_REQ (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cword B1protocol
		,_cword B2protocol
		,_cword B3protocol
		,_cstruct B1configuration
		,_cstruct B2configuration
		,_cstruct B3configuration);
unsigned CONNECT_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cword Reject
		,_cword B1protocol
		,_cword B2protocol
		,_cword B3protocol
		,_cstruct B1configuration
		,_cstruct B2configuration
		,_cstruct B3configuration
		,_cstruct ConnectedNumber
		,_cstruct ConnectedSubaddress
		,_cstruct LLC
		,_cstruct BChannelinformation
		,_cstruct Keypadfacility
		,_cstruct Useruserdata
		,_cstruct Facilitydataarray);
unsigned CONNECT_ACTIVE_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr);
unsigned CONNECT_B3_ACTIVE_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr);
unsigned CONNECT_B3_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cword Reject
		,_cstruct NCPI);
unsigned CONNECT_B3_T90_ACTIVE_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr);
unsigned DATA_B3_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cword DataHandle);
unsigned DISCONNECT_B3_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr);
unsigned DISCONNECT_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr);
unsigned FACILITY_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cword FacilitySelector);
unsigned INFO_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr);
unsigned MANUFACTURER_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr
		,_cdword ManuID
		,_cdword Class
		,_cdword Function
		,_cstruct ManuData);
unsigned RESET_B3_RESP (_cmsg __far *cmsg, _cword ApplId, _cword Messagenumber
		,_cdword adr);


#ifdef __cplusplus
}
#endif

#endif /*----- _capi20_h_ -----*/
