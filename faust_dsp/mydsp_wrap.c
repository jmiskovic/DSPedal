#include "CUI.h"
#include <fastmath.h>

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/* UGLY insertation of faust-generated source code */
#include "mydsp.c"

static mydsp __attribute__((section("DISCARD"))) __attribute__((used)) ___mydsp_instance____; /* this is used for */

_Static_assert(sizeof(mydsp) < 32 * 1024 * 1024, "DSP data does not fit in SDRAM");
