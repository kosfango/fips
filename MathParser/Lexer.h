#ifndef _LEXER_H_INCLUDED_
#define _LEXER_H_INCLUDED_
#include <stdlib.h>

#include "SymTable.h"

typedef unsigned char uchar;

typedef enum {
    CH_LETTER = 0x01, CH_DIGIT = 0x02, CH_SEPARAT = 0x04,
    CH_SYMBOL = 0x08, CH_QUOTE = 0x10,
    CH_UNKNOWN= 0x7E, CH_FINAL = 0x7F
} hqCharType;

typedef enum {
    TOK_ERROR, TOK_NONE, TOK_FINAL, TOK_INT, TOK_FLOAT, TOK_SYMBOL,
    TOK_NAME, TOK_STRING
} hqTokenType;

class CLexer{
public:
    // input params
    int		cssn;	// Comment Start Symbol Number. -1 if none
    char    *ComEnd;	// End of comment
    CSymTable *SymTable;
    hqCharType *CharTypeTable;

    // output params
    char       *Name;
    int		NameLen;
    double	ExtValue;
    int		IntValue;
    hqTokenType PrevTokenType;
    hqCharType	CharType;
    int		NoIntegers;
	int SetParseString(const char *str );
	hqTokenType GetNextToken();
	~CLexer(){
		if(str_)
			free(str_);
	};
	char* GetCurrentPos();
	CLexer():str_(NULL){
	};
private:
    char *SS, *str_;
};

/* Misc */

void InitCharTypeTable( hqCharType *CharTypeTable, int CharTypes );

extern char const Win1251UpcaseTbl[];

#endif //_LEXER_H_INCLUDED_