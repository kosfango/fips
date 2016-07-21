/*--------------------------------------------------------------------------*\

    INIT.H	Version 1.1					    1995 AVM

    Header file for INIT.C

\*--------------------------------------------------------------------------*/
#ifndef _init_h_
#define _init_h_

extern unsigned short	Appl_Id;

/*--------------------------------------------------------------------------*\
 * RegisterCAPI: Check for CAPI, allocate memory for CAPI-buffer and
 * register application. This function has to be called before using any
 * other CAPI functions.
\*--------------------------------------------------------------------------*/
unsigned RegisterCAPI (void);

/*--------------------------------------------------------------------------*\
 * ReleaseCAPI: deregister application
\*--------------------------------------------------------------------------*/
void ReleaseCAPI (void);


#endif	/*----- _init_h_ -----*/
