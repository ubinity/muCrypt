#include <stdlib.h>
#include "muBN.h"

void  muBN_rand(muBN_t *r) {
  muBN_size_t l;
  l = r->wlen;
  while(l--) {
    r->v[l--] = rand();
  }
}
