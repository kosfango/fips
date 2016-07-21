/*--------------------------------------------------------------------------*\

    DATA.H	Version 1.1					    1995 AVM

    Functions concerning data transmission

\*--------------------------------------------------------------------------*/
#ifndef _data_h_
#define _data_h_


#define SendBlockSize	2048	/*----- must not be greater than MaxB3DataBlockSize in req.c-----*/

/*--------------------------------------------------------------------------*\
 * SendData: Sends one block with data over the specified channel
\*--------------------------------------------------------------------------*/
unsigned SendData(ConnectionID	  Connection,
		  void __far	  *Data,
		  unsigned short  DataLength,
		  unsigned short  DataHandle);

/*--------------------------------------------------------------------------*\
 * DataConf: signals the successful sending of a datablock
 * This function is called after receiving a DATA_B3_CONFirmation. CAPI signals
 * that the datablock identified by DataHandle has been sent and the memory
 * area may be freed. The DataHandle is the same as specified in SendBlock.
 * This function is implemented in the main program.
\*--------------------------------------------------------------------------*/
void DataConf(ConnectionID    Connection,
	      unsigned short  DataHandle,
	      unsigned short  Info);

/*--------------------------------------------------------------------------*\
 * DataAvailable: signals received data blocks
 * This function is called after a DATA_B3_INDication is received. The flag
 * DiscardData tells CAPI to free the memora area directly after the return
 * of this function when set to TRUE (1) which is the preset. When the flag
 * is set to FALSE (0) the data area MUST be freed later with ReleaseData.
 * The datahandle identifies the memory area. When reaching 7 unconfirmed
 * blocks, no more incoming data will be signaled until freeing at least
 * one block.
 * This function is implemented in the main program.
\*--------------------------------------------------------------------------*/
void DataAvailable(ConnectionID    Connection,
		   void __far	   *Data,
		   unsigned short  DataLength,
		   unsigned short  DataHandle,
		   int		   *DiscardData);

/*--------------------------------------------------------------------------*\
 * ReleaseData: allows CAPI to reuse the memory area of the specified block.
 * CAPI allows max. 7 unconfirmed Blocks. If the maximum of 7 is reached,
 * no more DATA_B3_INDications will come up.
\*--------------------------------------------------------------------------*/
unsigned ReleaseData(ConnectionID    Connection,
		     unsigned short  DataHandle);

#endif	/*----- _data_h_ -----*/
