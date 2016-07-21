
/* ------------------------------------------------------------------ */
/* GLOBAL.H - RSAREF types and constants
 */

/* RFC 1321              MD5 Message-Digest Algorithm            April 1992 */
/* PROTOTYPES should be set to one if and only if the compiler supports
  function argument prototyping.
  The following makes PROTOTYPES default to 0 if it has not already
  been defined with C compiler flags.
 */
#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif
/* end of GLOBAL.H ---------------------------------------------------------- */

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

#define MD5_DIGEST_LEN 16

/* MD5 digest */
typedef unsigned char MDcaddr_t[MD5_DIGEST_LEN];

#define MD_CHALLENGE_LEN 16
#include "prothlp.h"
#include "readcfg.h"
#include "ftnnode.h"
#include "iphdr.h"
#include "protoco2.h"

char *MD_getChallenge(char *src, STATE *st);
char *MD_buildDigest(char *pw, unsigned char *challenge);
void MD_toString(char *rs, int len, unsigned char *digest);
