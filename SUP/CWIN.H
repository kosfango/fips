/*--------------------------------------------------------------------------*\

    CWIN.H	Version 1.1					    1995 AVM

    Operation system specific CAPI functions for Windows

\*--------------------------------------------------------------------------*/
#ifndef _cwin_h_
#define _cwin_h_

#include "capi20.h"

unsigned		CAPI20_ISINSTALLED (void);
_cword			CAPI20_REGISTER (unsigned MsgBufSize, unsigned MaxB3Connection, unsigned MaxB3Blks, unsigned MaxSizeB3, CAPI_REGISTER_ERROR *ErrorCode);
MESSAGE_EXCHANGE_ERROR	CAPI20_RELEASE (unsigned ApplId);
MESSAGE_EXCHANGE_ERROR	CAPI20_PUT_MESSAGE (CAPI_MESSAGE Msg, unsigned ApplId);
MESSAGE_EXCHANGE_ERROR	CAPI20_GET_MESSAGE (unsigned ApplId, CAPI_MESSAGE  __far * ReturnMessage);
MESSAGE_EXCHANGE_ERROR	CAPI20_SET_SIGNAL (unsigned ApplId, void (* CallBack) (void), void *CallBackPar);
CAPI_MESSAGE		CAPI20_GET_MANUFACTURER (CAPI_MESSAGE LpBuffer);
CAPI_MESSAGE		CAPI20_GET_VERSION (CAPI_MESSAGE version);
CAPI_MESSAGE		CAPI20_GET_SERIAL_NUMBER (CAPI_MESSAGE LpBuffer);
MESSAGE_EXCHANGE_ERROR	CAPI20_GET_PROFILE (unsigned Controller, CAPI_MESSAGE LpBuffer);

#endif /*----- _cwin_h_ -----*/
