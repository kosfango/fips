/*--------------------------------------------------------------------------*\

    Header file for UNIPACK.CPP

\*--------------------------------------------------------------------------*/
#ifndef _unipack_h
#define _unipack_h

int UnPack(char *InFileName,char *OutDir,char *Pw,char *FileListName,
	char *FileMask,char *OnFileExist);

int Pack(char *OutFileName,char *Pw,char *ArchiveType,char *FileListName,
	char *FileMask);

#endif	/*----- _unipack_h -----*/
