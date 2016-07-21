/*--------------------------------------------------------------------------*\

    CONTR.H	Version 1.1					    1995 AVM

    Header file for CONTR.C

\*--------------------------------------------------------------------------*/
#ifndef _contr_h_
#define _contr_h_

/*--------------------------------------------------------------------------*\
 * GetNumController: Returns the number of controllers detected by CAPI
\*--------------------------------------------------------------------------*/
unsigned GetNumController (void);

/*--------------------------------------------------------------------------*\
 * GetNumOfSupportedBChannels: Returns the number of supported B-channels
 * for the specified controller
\*--------------------------------------------------------------------------*/
unsigned GetNumOfSupportedBChannels (long Controller);

/*--------------------------------------------------------------------------*\
 * GetFreeController: Returns the number of the first controller that has
 * one free B-channel, or INVAL_CONTROLLER if none found.
\*--------------------------------------------------------------------------*/
long GetFreeController (void);


#endif	/*----- _contr_h_ -----*/
