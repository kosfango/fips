#if !defined(_SYMTABLE_H_INCLUDED_)
#define _SYMTABLE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSymTable{
public:
	typedef struct {
		char Sym[4];
		char Len;
		char Index;
		char More;
	} SymbolRec;
	virtual ~CSymTable();

	void PrepareSymbols( char *symbols );
	int FindSymbol( char *str, int *nchars );
protected:
	CSymTable(){};
private:
	SymbolRec* table[256];
};

class CMathSymTable: public CSymTable{
public:
	CMathSymTable();
	virtual ~CMathSymTable(){};
};


#endif // !defined(_SYMTABLE_H_INCLUDED_)
