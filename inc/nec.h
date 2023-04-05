#ifndef SEHA_NEC
#define SEHA_NEC

#include<stdlib.h>

#define nec_size_null(a) (*((int*)a-1))
#define nec_size(a) (a?nec_size_null(a):0)
#define nec_add(a) ({int s = nec_size(a)+1;(a=(void*)((int*)realloc((int*)a-(a?1:0),sizeof(int)+sizeof(*a)*s)+1),nec_size_null(a)=s);})
#define nec_push(a,v) (nec_add(a),a[nec_size(a)-1]=v)
#define nec_free(a) free((int*)a-(a?1:0))

#endif /* SEHA_NEC */

