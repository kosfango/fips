/*--------------------------------------------------------------------------*\

    FAX.H	Version 1.1					    1995 AVM

    Header file for FAX.C

\*--------------------------------------------------------------------------*/
#ifndef _fax_h_
#define _fax_h_


/*----- FAX Resolutions -----------------------------------------------*/
#define FAX_STANDARD_RESOLUTION 	0
#define FAX_HIGH_RESOLUTION		1

/*----- FAX Formats ---------------------------------------------------*/
#define FAX_SFF_FORMAT			0
#define FAX_PLAIN_FORMAT		1
#define FAX_PCX_FORMAT			2
#define FAX_DCX_FORMAT			3
#define FAX_TIFF_FORMAT 		4
#define FAX_ASCII_FORMAT		5
#define FAX_EXTENDED_ASCII_FORMAT	6
#define FAX_BINARY_FILE_TRANSFER_FORMAT 7


/*----- the pragma pack (1) directive tells the compiler to pack the -----*/
/*----- members of the structure on 1 byte boundaries. The default is 2 -----*/
#pragma pack (1)
typedef struct fax3proto3
{	unsigned char	len;
	unsigned short	resolution;
	unsigned short	format;
	unsigned char	Infos[100];
} B3_PROTO_FAXG3;
#pragma pack ()


/*--------------------------------------------------------------------------*\
\*--------------------------------------------------------------------------*/
void SetupB3Config(B3_PROTO_FAXG3  *B3conf,
		   int		   FAX_Format);


#endif	/*----- _fax_h_ -----*/
