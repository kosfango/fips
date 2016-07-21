// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>         // MFC extensions
#include <afxcview.h>
#include <afxtempl.h>
#include <string>
#include <vector>
using namespace std;
#include "structs.h"
#include "globals.h"
#include "resource.h"
#include "binkd\tools.h"
#include "include.hh"       // VH Help ID's

#define AFX_ZERO_INIT_OBJECT(base_class) \
memset(((base_class*)this)+1, 0, sizeof(*this) - sizeof(class base_class));
