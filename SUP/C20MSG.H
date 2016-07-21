/*--------------------------------------------------------------------------*\

    C20MSG.H	Version 1.1					    1995 AVM

    CAPI 2.0 Messages

\*--------------------------------------------------------------------------*/
#ifndef _c20msg_h_
#define _c20msg_h_


/*--------------------------------------------------------------------------*\
 * Decode_Info: Returns a string with an error description
 * Note: infos with values of 0x00xx are only warnings and the corresponding
 * messages have been processed.
 * The description for all info values but 0x34xx is taken from the CAPI 2.0
 * specification february 1994.
 * The description for the 0x34xx values is taken from ETS 300 102-1/Q.931
\*--------------------------------------------------------------------------*/
char *Decode_Info (unsigned int Info);

/*---------------------------------------------------------------------------*\
 * Decode_Command: Returns a textstring with the CAPI-commandname
\*---------------------------------------------------------------------------*/
char *Decode_Command (unsigned char Command);

/*--------------------------------------------------------------------------*\
 * Decode_Sub: Returns a textstring with the CAPI-subcommandname
\*--------------------------------------------------------------------------*/
char *Decode_Sub (unsigned char Sub);


#endif /*----- _c20msg_h_ -----*/
