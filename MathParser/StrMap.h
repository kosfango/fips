#ifndef _STRMAP_H_INCLUDED_
#define _STRMAP_H_INCLUDED_

class CStrMap{
public:
	CStrMap( int extrabytes=1024);
	void CreateFromChain( int extrabytes, char *strchain, void *data );
	~CStrMap();
	void AddString(const char *str, void *data);
	void DelString(const char *str);
	void ShrinkMem();
	void Clear();
	void SetCapacity(int NewCapacity );
	int IndexOf(char *str, void **data );
	int Replace( char *str,void *data );
	char* GetString(int index, int *len, void **data );
	void FillFromChain(char *strchain, void *data );
	int IndexOf(char *str, int len, void **data );
	void AddStr(const char *str, int len, void *data );
	void DelStr(const char *str, int len);
	int Replace( char *str, int len, void *data );
private:
	void Trim(int NewCount );
	void TrimClear(int NewCount );
	int   FCount, FCapacity;
    int   FExtraLen, FRecordLen;
    char *FList;
};

#endif //_STRMAP_H_INCLUDED_