//	=================================================
//  Automatic generated Source File for error support
//  !!!! DO NOT !!!! edit this file          JW 1994
//	================================================= 
#include "stdafx.h"
#include "structs.h"
																				

//=======================================================
   int get_error_string(int errornumber,char	 *buf)
//=======================================================
{

      switch (errornumber)
      {

           case DB_OK:
                        strcpy(buf,"All OK");
                        break;
           case DB_AREAFILE_NOT_FOUND:
                        strcpy(buf,"Cannot find area description database file");
                        break;
           case DB_OUT_OF_MEMORY:
                        strcpy(buf,"Out of Memory");
                        break;
           case DB_INVALID_AREA_SIZE:
                        strcpy(buf,"Invalid size of area description database");
                        break;
           case DB_WRONG_AREADDEF_VERSION:
                        strcpy(buf,"Sizeinfo in structure does'nt match");
                        break;
           case DB_NO_SUCH_INDEX:
                        strcpy(buf,"An Area with this index does not exist");
                        break;
           case DB_WRITE_AREA_FAILED:
                        strcpy(buf,"Writing to the area description database failed");
                        break;
           case DB_AREA_INDEX_NOT_FOUND:
                        strcpy(buf,"This areaindex cannot be found");
                        break;
           case DB_AREA_CANT_CREATE:
                        strcpy(buf,"Cannot create empty area database");
                        break;
           case DB_FILE_OPEN_ERROR:
                        strcpy(buf,"cannot open area specific database file");
                        break;
           case DB_WRONG_MAILHEADER_SIZE:
                        strcpy(buf,"Mailheader structure len is not compatible");
                        break;
           case DB_MESSAGE_WRITE_FAILED:
                        strcpy(buf,"Write to message file failed");
                        break;
           case DB_HEADER_WRITE_FAILED:
                        strcpy(buf,"Write to header file failed");
                        break;
           case DB_HEADERFILE_EMPTY:
                        strcpy(buf,"There are no valid headers in the headerfile");
                        break;
           case DB_CANNOT_CREATE_TEMP:
                        strcpy(buf,"Cannot create temp file");
                        break;
           case DB_WRITE_ERROR_TEMP_FILE:
                        strcpy(buf,"Write error on temp file");
                        break;
           case DB_CANNOT_DELETE_HEADER:
                        strcpy(buf,"Cannot delete old Header  database");
                        break;
           case DB_CANNOT_DELETE_MESSAGE:
                        strcpy(buf,"Cannot delete old Message database");
                        break;
           case DB_CANNOT_RENAME_HEADER:
                        strcpy(buf,"Cannot rename old Header  database");
                        break;
           case DB_CANNOT_RENAME_MESSAGE:
                        strcpy(buf,"Cannot rename old Message database");
                        break;
           case DB_SEEK_FOR_HEADER_FAILED:
                        strcpy(buf,"SEEK for a header index failed");
                        break;
           case DB_HEADER_READ_FAILED:
                        strcpy(buf,"Mailheader read failed after SEEK");
                        break;
           case DB_INDEX_HEADER_NEXIST:
                        strcpy(buf,"This mailheader is marked as invalid");
                        break;
           case DB_SEEK_FOR_MESSAGE_FAILED:
                        strcpy(buf,"SEEK for message failed");
                        break;
           case DB_MESSAGE_READ_FAILED:
                        strcpy(buf,"Message READ failed");
                        break;
           case DB_SEEK_FOR_DELETE_FAILED:
                        strcpy(buf,"SEEK failed for header deletion");
                        break;
           case DB_HEADERR_ON_DELETE:
                        strcpy(buf,"Read header failed on deletion");
                        break;
           case DB_WRITE_ERROR_ON_HDR:
                        strcpy(buf,"Write error header write (deletion)");
                        break;
           case DB_AREA_NAME_NF:
                        strcpy(buf,"Area Name not found");
                        break;
           case DB_CREATE_NAME:
                        strcpy(buf,"Cannot find a name for new area");
                        break;
           case TOSS_OK:
                        strcpy(buf,"All OK");
                        break;
           case TOSS_NO_VALID_PACKET:
                        strcpy(buf,"No valid packet");
                        break;
           case TOSS_INVALID_HEADER:
                        strcpy(buf,"Invalid header");
                        break;
           case TOSS_FILE_NOT_FOUND:
                        strcpy(buf,"File not found");
                        break;
           case TOSS_NO_ARCHIVE:
                        strcpy(buf,"No archive");
                        break;
           case TOSS_UNKNOWN_PACKER:
                        strcpy(buf,"Unknown Packer");
                        break;
           default:
                       strcpy(buf,"Unknown Error");
                       break;
      }
   return 1;
}


// End of meldgen
