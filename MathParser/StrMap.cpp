#include "stdafx.h"
#include "StrMap.h"

CStrMap::CStrMap(int extrabytes){
	FList = NULL;
	FCount = 0;
	FCapacity = 0;
    FExtraLen = extrabytes;
    FRecordLen = sizeof(char*) + sizeof(int) + extrabytes;
}

void CStrMap::FillFromChain(char *strchain, void *data){
    while ( *strchain ) {
		int len = strlen( strchain );
		AddStr(strchain, len, data );
		strchain += len+1;
		data = (char*)data + FExtraLen;
    }
}

void CStrMap::CreateFromChain( int extrabytes, char *strchain, void *data ){
    FExtraLen = extrabytes;
    FRecordLen = sizeof(char*) + sizeof(int) + extrabytes;
    FillFromChain( strchain, data );
    ShrinkMem();
}

CStrMap::~CStrMap(){
	Clear();
}

void CStrMap::AddString( const char *str, void *data ){
     AddStr( str, strlen(str), data );
}
void CStrMap::DelString( const char *str){
     DelStr( str, strlen(str));
}

void CStrMap::AddStr( const char *str, int len, void *data ){
    int i;
    char *Rec = FList;
	
    //Проверка на удаленные переменные под которые уже выделена память
	for (i=0; i<FCount; i++) {
        int recLen = *(int*)(Rec + sizeof(char*));
		if (recLen==0 && (*(char**)Rec)==0 ) 
		{
		*(char**)Rec = (char *)malloc(len + FExtraLen + sizeof(int));
		strncpy(*(char**)Rec,str,len);
		*(int*)(Rec + sizeof(char*)) = len;
		if (data) 
		{
		void *recdata = (Rec + sizeof(char*) + sizeof(int));
		memcpy( recdata, data, FExtraLen );
		return ;}
		}
		Rec += FRecordLen;
    }

	if (FCount >= FCapacity )
	{
		int delta = (FCapacity > 64) ? FCapacity / 4 : 16;
		SetCapacity( FCapacity + delta );
    }
    Rec = FList + FCount * FRecordLen;
    *(char**)Rec = (char *)malloc(len + FExtraLen + sizeof(int));
	strncpy(*(char**)Rec,str,len);
    *(int*)(Rec + sizeof(char*)) = len;
    if (data) {
	void *recdata = (Rec + sizeof(char*) + sizeof(int));
	memcpy( recdata, data, FExtraLen );
    }
    FCount++;
}
void CStrMap::DelStr( const char *str, int len){
    int i;
    char *Rec = FList;
    for (i=0; i<FCount; i++) {
        int recLen = *(int*)(Rec + sizeof(char*));
		if (recLen==len && strncmp( str, *(char**)Rec, recLen )==0 ) 
		{
			free( *(char**)Rec);
			*(char**)Rec=NULL;
			*(int*)(Rec + sizeof(char*))=NULL;
			return ;
		}
		Rec += FRecordLen;
    }
    return ;
}

void CStrMap::ShrinkMem(){
    SetCapacity( FCount );
}

void CStrMap::Trim(int NewCount){
    FCount = NewCount;
}

void CStrMap::TrimClear(int NewCount){
	if(NewCount < FCount){
		int i;
		char *Rec = FList + NewCount * FRecordLen;
		for (i = NewCount; i < FCount; i++) {
			free( *(char**)Rec );
			Rec += FRecordLen;
		}
		FCount = NewCount;
	}
}

void CStrMap::SetCapacity(int NewCapacity){
    FCapacity = NewCapacity;
    if (FCount >FCapacity )
        FCount = FCapacity;

    FList = (char*) realloc(FList, FCapacity*FRecordLen );
}

int CStrMap::IndexOf(char *str, void **data){
    return IndexOf( str, strlen(str), data );
}

int CStrMap::IndexOf( char *str, int len, void **data ){
    int i;
    char *Rec = FList;
    for (i=0; i<FCount; i++) {
        int recLen = *(int*)(Rec + sizeof(char*));
		if (recLen==len && strncmp( str, *(char**)Rec, recLen )==0 ) {
			*data = (Rec + sizeof(char*) + sizeof(int));
			return i;
		}
		Rec += FRecordLen;
    }
    *data = NULL;
    return -1;
}

int CStrMap::Replace( char *str, void *data ){
	return Replace( str, strlen(str), data );
}

int CStrMap::Replace( char *str, int len, void *data ){
    int i;
    char *Rec = FList;
    for (i=0; i<FCount; i++) {
        int recLen = *(int*)(Rec + sizeof(char*));
		if (recLen==len && strncmp( str, *(char**)Rec, recLen )==0 ) {
			void *recdata = (Rec + sizeof(char*) + sizeof(int));
			memcpy( recdata, data, FExtraLen );			
			return i;
		}
		Rec += FRecordLen;
    }
    return -1;
}

char* CStrMap::GetString( int index, int *len, void **data ){
    char *Rec = FList + index * FRecordLen;
    *len =  *(int*)(Rec + sizeof(char*));
    if (data!=NULL && FExtraLen>0)
        *data = (Rec + sizeof(char*) + sizeof(int));
    return *(char**)Rec;
}

void CStrMap::Clear(){
	TrimClear( 0 );
    if (FList){
		free(FList);
		FList = NULL;
	}
}