/*--------------------------------------------------------------------------*\

    MAIN.H							    1995 AVM

    Header file for MAIN.C and FAXMAIN.C

\*--------------------------------------------------------------------------*/
#ifndef _main_h_
#define _main_h_


/*--------------------------------------------------------------------------*\
 * MainDataAvailable: signals received data blocks
 * This function is called after a DATA_B3_INDication is received. After
 * leaving this function, the data is invalid so it must be copied for
 * further processing.
 * Set DiscardData to FALSE (0) if you want to free the data area later
 * with ReleaseData. DiscardData is preset with TRUE (1).
\*--------------------------------------------------------------------------*/
void MainDataAvailable(ConnectionID  Connection,
		       void __far    *Data,
		       _cword	     DataLength,
		       _cword	     DataHandle,
		       int	     *DiscardData);

/*--------------------------------------------------------------------------*\
 * MainDataConf: signals the successful sending of a datablock
 * This function is called after receiving a DATA_B3_CONFirmation. CAPI signals
 * that the datablock identified by DataHandle has been sent and the memory
 * area may be freed. The DataHandle is the same as specified in SendBlock.
\*--------------------------------------------------------------------------*/
void MainDataConf(ConnectionID	  Connection,
		  _cword	  DataHandle,
		  unsigned short  Info);

/*--------------------------------------------------------------------------*\
 * MainStateChange: signals a state change on both B-channels (connected, disconnected)
 * Whenever a channel changes his state this function is called
\*--------------------------------------------------------------------------*/
void MainStateChange(ConnectionID     Connection,
		     ConnectionState  State);

/*--------------------------------------------------------------------------*\
 * MainIncomingCall: signals an incoming call
 * This function will be executed if a CONNECT_INDication appears to
 * inform the user.
\*--------------------------------------------------------------------------*/
void MainIncomingCall(ConnectionID  Connection,
		      char	    *CallingPartyNumber);


#endif	/*----- _main_h_ -----*/
