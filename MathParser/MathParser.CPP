#include "stdafx.h"
#include <math.h>
#include "MathParser.h"

//#define MY_DEBUG 1

#ifndef M_E
# define M_E		2.7182818284590452354
#endif
#ifndef M_PI
# define M_PI		3.14159265358979323846
#endif

const double DblErR = -1.68736462823243E308;
const double DblNiN = -1.68376462823243E308;

//Error messages

static char eBrackets [] = "#Brackets not match!";
static char eSyntax   [] = "#Syntax error!";
static char eInternal [] = "#Internal error!";
static char eExtraOp  [] = "#Extra operation!";
static char eInfinity [] = "#Infinity somewhere!";
static char eInvArg   [] = "#Invalid argument!";
static char eUnknFunc [] = "# %s - Unknown function/variable!";
static char eExtrnFunc[] = "#External function error!";
static char eLogicErr [] = "#Logical expression error!";
static char eCalcErr  [] = "#Calculation error!";
static char eUnexpEnd [] = "#Unexpected end of script!";
static char eExpVarRet[] = "#Variable name or return expected!";
static char eExpAssign[] = "#Assignment expected!";
static char eValSizErr[] = "#Value too big for operation!";
static char eInvPrmCnt[] = "#Invalid parameters count for function call!";

static char StdSymbols[] = "+-/*^~()<>%$,?:=&|;";

static char func_names[] =
    "ATAN\000COS\000SIN\000TAN\000ABS\000"
    "EXP\000LN\000LG\000SQRT\000FRAC\000"
    "TRUNC\000FLOOR\000CEIL\000ROUND\000ASIN\000"
    "ACOS\000SGN\000NEG\000E\000PI\000";

/* Indexes of some functions in func_names[] array */
#define FUNC_ROUND	13
#define FUNC_E		18
#define FUNC_PI		19

static double _neg_(double);
static double _frac_(double);
static double _trunc_(double);
static double _sgn_(double);
static char* _round_( int paramcnt, double *args, CStrMap *strparams, double *result );

typedef double (*dfd)(double);
static dfd func_addresses[]= {
    &atan, &cos, &sin, &tan, &fabs,
    &exp, &log, &log10, &sqrt, &_frac_,
    &_trunc_, &floor, ceil, (double(*)(double)) &_round_, &asin,
    &acos, &_sgn_, &_neg_, NULL, NULL };

inline void TypeTableAddChars( hqCharType *CharTypeTable, char *Symbols,
			hqCharType CharType )
{
    while (*Symbols)
	CharTypeTable[ (uchar) *Symbols++] = CharType;
}

	

// CMathParser implementation

//static members

const char CMathParser::OpPriorities[CMathParser::OP_FUNC_MULTIARG+1] = {
		5, 5, 5,    2, 2, 2, 2, 2,    -1, -1,   0,
		3, 3,    4, 4, 4, 4,
		5, 5, 5, 5,    2, 2, 2,   1, 2, 0, 2,
		-1, 6, 6 };

CStrMap CMathParser::IntFunctions;

char CMathParser::errbuf[256];

const CMathParser::Operation CMathParser::BrOp = {OP_OBR};

const CMathParser::Operation CMathParser::NegOp = { OP_FUNC_ONEARG, (void*)&_neg_, 0, NULL };

int CMathParser::initializations_performed = 0;

hqCharType CMathParser::MathCharTypeTable[256];

int CMathParser::refCounter = 0;

CMathSymTable CMathParser::MathSymTable;


CMathParser::CMathParser( char *MoreLetters):ExtFunctions(sizeof(void*)){
    if (refCounter++ == 0) {
	 // init character tables
	 InitCharTypeTable( MathCharTypeTable,
			   CH_LETTER | CH_DIGIT | CH_SEPARAT | CH_QUOTE );
	 TypeTableAddChars( MathCharTypeTable, StdSymbols, CH_SYMBOL );
	 if (MoreLetters)
	    TypeTableAddChars( MathCharTypeTable, MoreLetters, CH_LETTER );
	 // init function maps
	 IntFunctions.CreateFromChain( sizeof(void*),
						(char*)func_names,
						func_addresses );
	 initializations_performed = 1;

    }
	ParamFuncParam = NULL;
	MoreParams = NULL;
	VarParams = NULL;
    Lexer.NoIntegers = 1;
    Lexer.SymTable = &MathSymTable;
    Lexer.CharTypeTable = MathCharTypeTable;
    Lexer.cssn = 8;
    Lexer.ComEnd = "*/";
}

CMathParser::~CMathParser(){}

char* CMathParser::PrepareFormula()
{
    int BrCnt = 0;
    char *SS = Lexer.GetCurrentPos();

    // Brackets Matching
    while ( (!script && *SS) || (script && *SS != ';') ) {
	if (*SS=='(')
	    ++BrCnt;
	else if (*SS==')' && --BrCnt<0)
	    goto brkerr;
	++SS;
    }
    if (BrCnt != 0)
brkerr:	return eBrackets;

    OpTop = 0;
    ValTop = -1;
    OpStack[0].OperType = OP_OBR;
    ObrDist = 2;
    return NULL;
}

char* CMathParser::Parse(const char *Formula, double *result )
{
    if (!Formula || !*Formula) {
	*result = 0.0;
	return NULL;
    }

    script = *Formula == '#' && *(Formula+1) == '!'
		     && MathCharTypeTable[ (uchar)*(Formula+2) ] == CH_SEPARAT;

    if ( script )	Formula += 3;
	
    Lexer.SetParseString( Formula );

    return script?ParseScript(result):ParseFormula(result);
}

char* CMathParser::ParseFormula( double *result ){
    char *ErrorMsg;
    hqTokenType ToTi;

    if ( (ErrorMsg = PrepareFormula()) != NULL )
	return ErrorMsg;

    ToTi = Lexer.GetNextToken();
    for (;;) {
	--ObrDist;
	switch (ToTi) {
	    case TOK_ERROR:
		return eSyntax;
	    case TOK_FINAL:
formulaend:	if ( (ErrorMsg = CalcToObr()) != NULL )
		    return ErrorMsg;
		goto getout;
	    case TOK_FLOAT:
		ValStack[++ValTop] = Lexer.ExtValue;
		break;
	    case TOK_SYMBOL:
		switch ( Lexer.IntValue ) {
		    case OP_OBR:	// (
			OpStack[++OpTop] = BrOp;
			ObrDist = 2;
			break;
		    case OP_CBR:	// )
			if ( (ErrorMsg = CalcToObr()) != NULL )
			    return ErrorMsg;
			break;
		    case OP_COMMA: {	// ,
			Operation *pOp;
			if ( (ErrorMsg = CalcToObr()) != NULL )
			    return ErrorMsg;
			if ( (pOp = &OpStack[OpTop])->OperType
			     == OP_FUNC_MULTIARG ) {
			    OpStack[++OpTop] = BrOp;
			    ObrDist = 2;
			} else
			    return eSyntax;
			break;
		    }
		    default: {
			Operation Op;
			Op.OperType = (OperType_t) Lexer.IntValue;
			switch (Op.OperType) {
			    case OP_FORMULAEND:
				if (script)
				    goto formulaend;
				else
				    return eSyntax;
			    case OP_ADD:
				if (ObrDist >= 1)
				    goto next_tok;
				break;
			    case OP_SUB:
				if (ObrDist >= 1) {
				    OpStack[++OpTop] = NegOp;
				    goto next_tok;
				}
				break;
			    case OP_LOGIC:
			    case OP_LOGIC_SEP:
				ObrDist = 2;
			    default:
				break;
			}
			while ( OpPriorities[ Op.OperType ]
				<= 
				OpPriorities[ OpStack[OpTop].OperType ] ) {
			    if ( (ErrorMsg = Calc()) != NULL )
				return ErrorMsg;
			}
			OpStack[++OpTop] = Op;
			break;
		    }
		}
		break;
	    case TOK_NAME: {
		Operation Op;
		double *value, dblval;
		void **func;
		int funcnum, i, namelen = Lexer.NameLen;
		
		char *SS = Lexer.Name;
		for (i = namelen; i>0; --i)
		    *SS++ = Win1251UpcaseTbl[ (int)*SS ];
		
		funcnum = IntFunctions.IndexOf(Lexer.Name,Lexer.NameLen,(void**) &func );
		if ( funcnum >= 0 ) {
		    Op.Func = *func;
		    switch ( funcnum ) {
			case FUNC_E:
			    ValStack[++ValTop] = M_E;
			    break;
			case FUNC_PI:
			    ValStack[++ValTop] = M_PI;
			    break;
			case FUNC_ROUND:
			    Op.OperType = OP_FUNC_MULTIARG;
			    Op.PrevValTop = ValTop;
			    Op.StrParams = NULL;
			    OpStack[++OpTop] = Op;
			    break;
			default:// Internal function
			    Op.OperType = OP_FUNC_ONEARG;
			    OpStack[++OpTop] = Op;
		    }
		} else if (Parameters.IndexOf(Lexer.Name,Lexer.NameLen,(void**) &value )>= 0) {
		    if (*value==DblErR) {
			return eInternal;
		    } else
			ValStack[++ValTop] = *value;
		} else if (ExtFunctions.IndexOf(Lexer.Name,Lexer.NameLen,(void**) &func ) >= 0) {
		    Op.Func = *func;
		    Op.OperType = OP_FUNC_MULTIARG;
		    Op.PrevValTop = ValTop;
		    Op.StrParams = NULL;
		    OpStack[++OpTop] = Op;
		} else if (VarParams
			   &&
			   VarParams->IndexOf(Lexer.Name,Lexer.NameLen,(void**) &value )	>= 0
			  ) {
		    if (*value==DblErR) {
			return eInternal;
		    } else
			ValStack[++ValTop] = *value;
		} else if (MoreParams
			   &&
			   (*MoreParams)( Lexer.Name,
						  Lexer.NameLen,
						  &dblval,
						  ParamFuncParam )
			  ) {
		    ValStack[++ValTop] = dblval;
		} else {
		    char buf[256];
		    strncpy( buf, Lexer.Name, Lexer.NameLen );
		    buf[ Lexer.NameLen ] = '\0';
		    sprintf( errbuf, eUnknFunc, buf );
		    return errbuf;
		}
		break;
	    }
	    case TOK_STRING: {
		Operation *pOp;
		if (OpTop < 1)
		    return eSyntax;
		if ( (pOp = &OpStack[OpTop-1])->OperType
		     == OP_FUNC_MULTIARG ) {
		    if (!pOp->StrParams)
			pOp->StrParams = new CStrMap(0);
		    pOp->StrParams->AddStr(Lexer.Name,Lexer.NameLen, NULL );
		    ValStack[++ValTop] = DblNiN;
		} else
		    return eSyntax;
		break;
	    }
	    default:
		return eSyntax;
	}
next_tok:
    ToTi = Lexer.GetNextToken();
    } // forever

getout:
    if (OpTop != -1 || ValTop != 0)
	return eInternal;
	
    *result = ValStack[0];
    return NULL;
}

char* CMathParser::ParseScript( double *result )
{
    char *ErrorMsg = NULL;
    hqTokenType ToTi;
    int expectvar = 1, was_return = 0;
    char *varname = NULL;
    int varnamelen = 0;

    VarParams = new CStrMap();

	//VarParams->Create( sizeof(double), 0 );-it's in default constructor

    ToTi = Lexer.GetNextToken();
    for (;;) {
	switch (ToTi) {
	    case TOK_FINAL:
		ErrorMsg = eUnexpEnd;
		goto getout;
	    case TOK_NAME: {
		if (!expectvar) {
		    ErrorMsg = eExpVarRet;
		    goto getout;
		} else {
		    int  i;
		    char *SS = Lexer.Name;

		    varnamelen = Lexer.NameLen;

		    for (i = varnamelen; i>0; --i)
			*SS++ = Win1251UpcaseTbl[ (int) (uchar) *SS ];
		}
		varname = Lexer.Name;

		was_return = strncmp( varname, "RETURN", varnamelen ) == 0;
		if ( was_return ) {
		    ErrorMsg = ParseFormula(result);
		    goto getout;
		}
		expectvar = 0;
	    	break;
	    }
	    case TOK_SYMBOL: {
		double *value;

		if ( Lexer.IntValue != OP_ASSIGN || expectvar ) {
		    ErrorMsg = eExpAssign;
		    goto getout;
		}
		ErrorMsg = ParseFormula(result);
		if (ErrorMsg)
		  goto getout;

		if (VarParams->IndexOf(varname, varnamelen,(void**) &value ) >= 0 )
		    *value = *result;
		else
		    VarParams->AddStr(varname, varnamelen, result);
		expectvar = 1;
		break;
		}
	    default:
		ErrorMsg = eSyntax;
		goto getout;
	}
    ToTi = Lexer.GetNextToken();
    } // forever

getout:
    delete VarParams;
    return ErrorMsg;
}

char* CMathParser::Calc()
{
    double Res, ValR;
    Operation Op = OpStack[OpTop--];

    // multi-argument external or internal fucntion
    if ( Op.OperType == OP_FUNC_MULTIARG ) {
	int paramcnt = ValTop - Op.PrevValTop;
	char *ErrorMsg;
#ifdef MY_DEBUG
	printf( "ValTop = %d, OpTop = %d, PrevValTop = %d\n",
		ValTop, OpTop, Op.PrevValTop );
#endif
	ValTop = Op.PrevValTop;
	ErrorMsg = (*(MultiArgFunc)Op.Func)( paramcnt,
					&ValStack[ValTop+1],
					Op.StrParams, &Res );
	if (ErrorMsg)
	    return ErrorMsg;
	if (Op.StrParams)
	    delete Op.StrParams;
	ValStack[++ValTop] = Res;
#ifdef MY_DEBUG
	printf("ValTop = %d, OpTop = %d\n", ValTop, OpTop );
#endif
	return NULL;
    }

    if (Op.OperType==OP_LOGIC)
	return NULL; 

    // get right arg
    if (ValTop<0)
	return eExtraOp;
    ValR = ValStack[ValTop--];

    // one arg operations
    if (Op.OperType==OP_NOT) {
	if (ValR >= INT_MIN && ValR <= INT_MAX)
	    Res = ~((int) ValR);
	else
	    return eValSizErr;
    } else if (Op.OperType==OP_FUNC_ONEARG) {
	Res = (*(OneArgFunc)Op.Func)( ValR );
    } else {
	// get left arg
	double ValL;
	if (ValTop<0)
	    return eExtraOp;
	ValL = ValStack[ValTop--];
	switch (Op.OperType) {
	    // Binary
	    case OP_SHL:
		if (ValL >= INT_MIN && ValL <= INT_MAX && ValR >= INT_MIN && ValR <= INT_MAX)
		    Res = (int) ValL << (int) ValR;
		else
		    return eValSizErr;
		break;
	    case OP_SHR:
		if (ValL >= INT_MIN && ValL <= INT_MAX && ValR >= INT_MIN && ValR <= INT_MAX)
		    Res = (int) ValL >> (int) ValR;
		else
		    return eValSizErr;
		break;
	    case OP_POW:
		Res = pow( ValL, ValR );	break;
	    // Logical
	    case OP_LOGIC_NEQ:
		Res = ValL != ValR;		break;
	    case OP_LOGIC_GEQ:
		Res = ValL >= ValR;		break;
	    case OP_LOGIC_LEQ:
		Res = ValL <= ValR;		break;
	    case OP_LOGIC_AND:
		Res = ValL && ValR;	break;
	    case OP_LOGIC_OR:
		Res = ValL || ValR;	break;
	    // Arithmetic
	    case OP_ADD:
		Res = ValL + ValR;		break;
	    case OP_SUB:
		Res = ValL - ValR;		break;
	    case OP_MUL:
		Res = ValL * ValR;		break;
	    case OP_DIV:
		if (ValR == 0.0)
		    return eInfinity;
		Res = ValL / ValR;
		break;
	    case OP_MOD:
		Res = fmod(ValL, ValR);	break;
	    case OP_UNK:
		if (ValL<=0)
		    Res = 0.0;
		else if (ValR==0.0)
		    return eInfinity;
		else
		    Res = ceil(ValL / ValR);
		break;
	    // Bitwise
	    case OP_XOR:
		if (ValL >= INT_MIN && ValL <= INT_MAX && ValR >= INT_MIN && ValR <= INT_MAX)
			Res = (int) ValL ^ (int) ValR;
		else
			return eValSizErr;
		break;
	    case OP_AND:
		if (ValL >= INT_MIN && ValL <= INT_MAX && ValR >= INT_MIN && ValR <= INT_MAX)
			Res = (int) ValL & (int) ValR;
		else
			return eValSizErr;
		break;
	    case OP_OR:
		if (ValL >= INT_MIN && ValL <= INT_MAX && ValR >= INT_MIN && ValR <= INT_MAX)
			Res = (int) ValL | (int) ValR;
		else
			return eValSizErr;
		break;
	    // Logical
	    case OP_EQU:
		Res = ValL == ValR;		break;
	    case OP_GREATER:
		Res = ValL > ValR;		break;
	    case OP_LESS:
		Res = ValL < ValR;		break;
	    case OP_LOGIC_SEP: {
		// needs three arguments
		double ValLL;
		if (OpTop < 0
		    || OpStack[OpTop--].OperType != OP_LOGIC)
		    return eLogicErr;
		ValLL = ValStack[ ValTop-- ];
		Res = ValLL ? ValL : ValR;
		break;
	    }
	    default:
		return eInternal;
	}
    }
    ValStack[++ValTop] = Res;
    return NULL;
}

char* CMathParser::CalcToObr()
{
    while ( OpStack[OpTop].OperType != OP_OBR ) {
	char *ErrorMsg;
	if ( (ErrorMsg = Calc()) != NULL )
	    return ErrorMsg;
    }
    --OpTop;
    return NULL;
}

/* misc functions */

static double _frac_( double x )
{
    double y;
    return modf(x, &y);
}

static double _trunc_( double x )
{
    return (x >= 0.0) ? floor(x) : ceil(x);
}

static double _sgn_( double x )
{
    return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

static double _neg_( double x )
{
    return -x;
}

/* "Advanced" round function; second argument - sharpness */
static char* _round_( int paramcnt, double *args, CStrMap *strparams, double *result )
{
    int i, sharpness;
    double x, coef;

    if (paramcnt == 1)
	sharpness = 0;
    else if (paramcnt == 2)
	sharpness = (int) args[1];
    else
	return eInvPrmCnt;

    x = args[0];
    if (sharpness < 0) {
	coef = 0.1;
        sharpness = -sharpness;
    } else
	coef = 10;

    for (i = 0; i < sharpness; i++)
	x *= coef;

    x = (x + ( (x >= 0) ? 0.5 : -0.5 ) );
    if (x >= 0.0)
	x = floor(x);
    else
	x = ceil(x);

    for (i = 0; i < sharpness; i++)
	x /= coef;

    *result = x;

    return NULL;
}