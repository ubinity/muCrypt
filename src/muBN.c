/*
 *  Copyright 2015-2016, Ubinity SAS, cedric.mesnil@ubinity.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "muBN.h"

/* ======================================================================================= */
/*                                     Init                                                */
/* ======================================================================================= */

static void muBN_memmove(uint8_t *dest, const uint8_t *src, int n) {
  if (dest>src) {
    while(n--) {
      dest[n] = src[n];
    }
  } else {
    int i;
    for (i= 0; i<n; i++) {
      dest[i] = src[i];
    }
  }
}


void muBN_init(muBN_t *r, muBN_uword_t  *a ,muBN_size_t wlen ) {
  r->OV   = 0;
  r->C    = 0;
  r->rfu  = 0;
  r->wlen = wlen;
  r->v    = a;
}

void muBN_init_zero(muBN_t *r, muBN_uword_t  *a ,muBN_size_t wlen ) {
  muBN_init(r,a,wlen);
  muBN_zero(r);
}

void muBN_zero(muBN_t *r) {
  muBN_uword_t wlen = r->wlen;
  while (wlen--) {
    r->v[wlen] = 0;    
  }
  r->C = 0;  
  r->OV    = 0;    
}
void muBN_one(muBN_t *r) {
  muBN_zero(r);
  r->v[r->wlen-1] = 1;
}

void muBN_copy(muBN_t *r, muBN_t *a) {
  muBN_size_t awlen;
  muBN_size_t rwlen;
  muBN_size_t wlen;

  awlen = a->wlen;
  rwlen = r->wlen;
  wlen = (awlen<rwlen) ? awlen :rwlen;
  while (wlen--) {
    rwlen--;awlen--;
    r->v[rwlen] = a->v[awlen];
  }
  wlen = (a->wlen<r->wlen) ?r->wlen - a->wlen :0;
  while (wlen--) {
    r->v[wlen] = 0;
  }
  r->C  = a->C;
  r->OV = 0;  
}

void muBN_clone(muBN_t *r, muBN_t *a) {
  r->OV   = a->OV;
  r->C    = a->C;
  r->rfu  = a->rfu;
  r->wlen = a->wlen;
  r->v    = a->v;
}

muBN_size_t muBN_ubn2bin (muBN_t *r,  uint8_t *to,  muBN_size_t  blen, uint8_t mode)  {
  //MODE = UBN_FULL:   full length
  //MODE = UBN_OPTIMAL: no leading zero
  //MODE = UBN_DER:     one leading zero if first byte > 0x80
  muBN_uword_t *dest;
  muBN_size_t  wlen;
  muBN_uword_t  w;
  muBN_size_t   i;

  if (blen < (muBN_size_t)(r->wlen*sizeof(muBN_uword_t))) {
    return -1;
  }
  dest = ( muBN_uword_t *)to;
  wlen = r->wlen;
  while (wlen--) {
    w = r->v[wlen];
    dest[wlen] = uword2BE(w);
  }
  wlen =  r->wlen*sizeof(muBN_uword_t);

  switch (mode) {
  case UBN_FULL:
    break;
  case UBN_DER:
    if (to[0] & 0x80) {
      muBN_memmove(to+1, to, wlen);
      wlen++;
      to[0] = 0;
      break;
    }
    //fall through
  case UBN_OPTIMAL:
    for (i = 0; i<wlen; i++) {
      if (to[i]) break;      
    }
    muBN_memmove(to, to+i, wlen -i);
    wlen -=i;
    break;
  }
  return wlen;
}


muBN_size_t muBN_bin2ubn (muBN_t *r,  uint8_t *from,  muBN_size_t  blen)  {
  muBN_uword_t *src;
  muBN_size_t  wlen;
  muBN_uword_t  w;
    
  if (blen > (muBN_size_t)(r->wlen*sizeof(muBN_uword_t))) {
    return -1;
  }
  
  src = ( muBN_uword_t *)from;
  wlen = r->wlen;
  while (wlen--) {
    w = src[wlen];
    r->v[wlen] = BE2uword(w);
  }
  return r->wlen;
}

static char HEX(unsigned char v) {
  return  v<10 ? v+'0':v-10+'a';
}

muBN_size_t muBN_ubn2hex (muBN_t *r,  uint8_t *to,  muBN_size_t  blen)  {
  muBN_size_t  wlen;
  muBN_uword_t  w;
  muBN_size_t  i;

  if (blen < (muBN_size_t)(r->wlen*sizeof(muBN_uword_t)*2)) {
    return -1;
  }


  blen = 0;
  wlen = r->wlen;
  for (i = 0; i<wlen; i++) {    
    w = r->v[i];
#if UBN_BITS_PER_WORD == 32
    to[blen+0] = HEX(((w>>28) &0x0F));
    to[blen+1] = HEX(((w>>24) &0x0F));
    to[blen+2] = HEX(((w>>20) &0x0F));
    to[blen+3] = HEX(((w>>16) &0x0F));
    to[blen+4] = HEX(((w>>12) &0x0F));
    to[blen+5] = HEX(((w>> 8) &0x0F));
    to[blen+6] = HEX(((w>> 4) &0x0F));
    to[blen+7] = HEX(( w      &0x0F));
#elif UBN_BITS_PER_WORD == 16
    to[blen+0] = HEX(((w>>12) &0x0F));
    to[blen+1] = HEX(((w>> 8) &0x0F));
    to[blen+2] = HEX(((w>> 4) &0x0F));
    to[blen+3] = HEX(( w      &0x0F));
#else
    to[blen+0] = HEX(((w>> 4) &0x0F));
    to[blen+1] = HEX(( w      &0x0F));
#endif
    blen  += 2*(UBN_BITS_PER_WORD/8);
  }
  to[blen] = 0;
  return blen;
}


static char VAL(unsigned char c) {
  return c <= '9' ? c-'0' : c-'a' +10;
}

muBN_size_t muBN_hex2ubn (muBN_t *r,  uint8_t *from,  muBN_size_t  blen)  {
  muBN_size_t  wlen;
  muBN_uword_t  w;
    
  if (blen > (muBN_size_t)(r->wlen*sizeof(muBN_uword_t)*2)) {
    return -1;
  }
  
  wlen = r->wlen;  
  muBN_zero(r);
  while (blen > (muBN_size_t)(UBN_BITS_PER_WORD/8)) {
    blen -= UBN_BITS_PER_WORD/8;
    wlen --;
#if UBN_BITS_PER_WORD == 32
    w = 
      (VAL(from[blen+0]) << 28) |
      (VAL(from[blen+1]) << 24) |
      (VAL(from[blen+2]) << 20) |
      (VAL(from[blen+3]) << 16) |
      (VAL(from[blen+4]) << 12) |
      (VAL(from[blen+5]) <<  8) |
      (VAL(from[blen+6]) <<  4) |
      (VAL(from[blen+7])     );
    
#elif UBN_BITS_PER_WORD == 16
    w =
      (VAL(from[blen+1]) << 12) |
      (VAL(from[blen+2]) <<  8) |
      (VAL(from[blen+3]) <<  4) |
      (VAL(from[blen+4])     );
#else
    w = 
      (VAL(from[i+0]) << 4) |
      (VAL(from[i+1])     );
#endif    
    r->v[wlen]= w;       
  }    
  if (blen) {
    wlen--;
    w = 0;
    while(blen--) {
      w = w << 4;
      w |= VAL(*from);
      from++;
    }
    r->v[wlen]= w;  
  }
  return r->wlen;
}



/* ======================================================================================= */
/*                                 Logic Arithmetic                                        */
/* ======================================================================================= */
void muBN_xor(muBN_t *r, muBN_t *a,  muBN_t *b) {
  muBN_size_t wlen =r->wlen;
  r->OV = 0;
  r->C = 0;
  while (wlen--) {    
    r->v[wlen] = a->v[wlen] ^ b->v[wlen];
  }  
}
void muBN_or(muBN_t *r, muBN_t *a,  muBN_t *b) {
  muBN_size_t wlen =r->wlen;
  r->OV = 0;
  r->C = 0;
  while (wlen--) {    
    r->v[wlen] = a->v[wlen] | b->v[wlen];
  }  
}
void muBN_and(muBN_t *r, muBN_t *a,  muBN_t *b) {
  muBN_uword_t wlen =r->wlen;
  r->OV = 0;
  r->C = 0;
  while (wlen--) {    
    r->v[wlen] = a->v[wlen] & b->v[wlen];
  }  
}
void muBN_not(muBN_t *r, muBN_t *a) {
  muBN_size_t wlen =r->wlen;
  r->OV = 0;
  r->C = 0;
  while (wlen--) {    
    r->v[wlen] = ~a->v[wlen];
  }  
}

void muBN_setC(muBN_t *r) {
  r->C = 1; 
}
void muBN_clearC(muBN_t *r) {
  r->C = 1; 
}
muBN_word_t muBN_testC(muBN_t *r) {
  return r->C ;
}

muBN_size_t   muBN_count_bit(const muBN_t *a) {
  muBN_size_t wlen  = a->wlen;
  muBN_uword_t nbits = wlen*UBN_BITS_PER_WORD;
  muBN_size_t  i ;
  for (i =0; i<wlen; i++) {
    if (a->v[i]) {
      break;
    }    
    nbits -= UBN_BITS_PER_WORD;
  }

  wlen = (a->v[i]);
  for (i = 0; i<(muBN_size_t)(UBN_BITS_PER_WORD);i++) {
    if (wlen & UBN_WORD_HIGH_BIT) {
      break;
    }
    nbits--;
    wlen = wlen<<1;
  }
  return nbits;
}

muBN_word_t  muBN_test_bit(const muBN_t *a, muBN_size_t  n) {
  return
    (  (a->v[a->wlen-1-n/UBN_BITS_PER_WORD])
       & (1<<(n%UBN_BITS_PER_WORD)) )
    ?1:0;
}
/* ======================================================================================= */
/*                                   Z Arithmetic                                          */
/* ======================================================================================= */
muBN_word_t muBN_ucmp(muBN_t *a,  muBN_t *b ){
  muBN_size_t wlen  = a->wlen;
  muBN_size_t i;

  if (a->C != b->C) {
    return a->C ? 1 : -1;
  }

  for (i =0; i<wlen; i++) {
    if (a->v[i] != b->v[i]) {
      return (a->v[i] < b->v[i]) ? -1:1;
    }
  }
  return 0;
}
  
muBN_word_t muBN_cmp(muBN_t *a,  muBN_t *b ){
  muBN_size_t wlen  = a->wlen;
  muBN_size_t i;

  if ((a->v[0]&UBN_WORD_HIGH_BIT) != (b->v[0]&UBN_WORD_HIGH_BIT)) {
    return (a->v[0]&UBN_WORD_HIGH_BIT) ? -1 : 1;
  }


  for (i =0; i<wlen; i++) {
    if (a->v[i] != b->v[i]) {
      i = (a->v[i] < b->v[i]) ? -1:1;
      if (a->v[0]&UBN_WORD_HIGH_BIT) {
        return -i;
      } else {
        return i;
      }
    }
  }
  return 0;
}

void  muBN_negate(muBN_t *r) {
  muBN_size_t wlen = r->wlen;
  muBN_uword_t carry = 0;
  muBN_uword_t a;
  r->OV = 0;
  carry      =  1;
  while (wlen--) {
    a = ~ r->v[wlen];
    r->v[wlen] = a+carry;   
    carry      =  r->v[wlen] < a;
  }  
  r->C = ~r->C  + carry;
  
}


static char muBN_UCMP_CASES[] = { 
  //d!=0 ==> d2=2
  //d=0,C=0  d=0,C=1  d2=2,C=0  d2=2,C=1       
  0,          0,      1,         -1};

muBN_word_t muBN_ucmp_sec(muBN_t *a,  muBN_t *b ) {
  muBN_size_t wlen  = a->wlen;
  muBN_udword_t sub;
  muBN_uword_t d2;
  muBN_uword_t  carry;

  carry = 0;
  d2 = 0;
  while (wlen--) {
    sub          = (muBN_udword_t)(a->v[wlen])-(muBN_udword_t)(b->v[wlen])-carry;
    d2         |= sub;
    carry      = (sub >> UBN_BITS_PER_WORD)?1:0;
  }
  d2 = d2?2:0;
  carry =  a->C - b->C - carry ? 1 : 0;
  return muBN_UCMP_CASES[d2|carry];
}

/*   a      b            d
 * 0(>0) 1(<0)      00(eq), 01(a>b) 10(a<b)
 *  0     0   00      0
 *  0     0   01      1
 *  0     0   10     -1
 *  0     0   11     na

 *  0     1   00      0
 *  0     1   01      1
 *  0     1   10      1
 *  0     1   11     na

 *  1     0   00      0
 *  1     0   01     -1
 *  1     0   10     -1
 *  1     0   11     na

 *  1     1   00      0
 *  1     1   01     -1
 *  1     1   10      1
 *  1     1   11     na
 */
static int8_t muBN_CMP_CASES[] = {
  0,  1, -1, 2,
  0,  1,  1, 2,
  0, -1, -1, 2,
  0, -1,  1, 2,
};

muBN_word_t muBN_cmp_sec(muBN_t *a,  muBN_t *b ){
  muBN_word_t d =  muBN_ucmp(a,b);
  d = d < 0 ? 2:d;
  d = (a->v[0]&UBN_WORD_HIGH_BIT << 2) | (b->v[0]&UBN_WORD_HIGH_BIT << 3) | d;
  return muBN_CMP_CASES[d];
}


muBN_word_t   muBN_is_zero(muBN_t *r) { 
  muBN_size_t wlen = r->wlen;

  while(wlen--) {
    if (r->v[wlen]) {
      return 0;
    }
  }
  return 1;
}

muBN_word_t   muBN_is_zero_sec(muBN_t *r) { 
  muBN_size_t wlen  = r->wlen;
  muBN_udword_t d;

  d = 0;
  while(wlen--) {
    d |= r->v[wlen];
  }
  return d==0;
}


muBN_word_t  muBN_is_one(muBN_t *r) { 
  muBN_size_t wlen = r->wlen;

  if (r->v[wlen-1] != 1) {    
    return 0;
  }
  wlen--;
  while(wlen--) {
    if (r->v[wlen]) {
      return 0;
    }
  }
  return 1;
}

muBN_word_t   muBN_is_one_sec(muBN_t *r) { 
  muBN_size_t wlen  = r->wlen;
  muBN_udword_t d;
  muBN_udword_t d2;

  d = r->v[wlen-1];
  wlen--;
  d2 = 0;
  while(wlen--) {
    d2 |= r->v[wlen];
  }
  d+=d2;
  return (d==1);
}

/*
muBN_word_t   muBN_is_even(muBN_t *r) { 
  return !(r->v[r->wlen-1]&1);
}
muBN_word_t   muBN_is_odd(muBN_t *r) { 
  return r->v[r->wlen-1]&1;
}
*/
// r = r>>1
void muBN_rshift1(muBN_t *r) {  
  r->C = (r->v[0]&UBN_WORD_HIGH_BIT)?1:0;
  muBN_rshift1c(r);
}
// r = r>>1
void muBN_urshift1(muBN_t *r) {  
  r->C = 0;
  muBN_rshift1c(r);
}

// r = r>>1
void muBN_urshift1c(muBN_t *r) {  
  muBN_uword_t c;
  c =  r->C?UBN_WORD_HIGH_BIT:0;
  r->C = 0;
  muBN_rshift1c(r);
  r->v[0] |= c;
}

void muBN_rshift1c(muBN_t *r) {
  muBN_size_t wlen =r->wlen;
  muBN_uword_t v;
  muBN_uword_t c;
  muBN_size_t i;

  c = r->C ? 1<<(UBN_BITS_PER_WORD-1) : 0;
  for (i = 0; i<wlen; i++) {
    v = r->v[i];
    r->v[i] = v>>1|c;
    c = v << (UBN_BITS_PER_WORD-1);
  } 
  r->C = 0;
}

// r = r>>n
void muBN_rshift(muBN_t *r, muBN_size_t  n) {
  r->C = (r->v[0]&UBN_WORD_HIGH_BIT)?1:0;
  muBN_rshiftc(r,n);
}
void muBN_urshift(muBN_t *r, muBN_size_t  n) {
  r->C = 0;
  muBN_rshiftc(r,n);
}

void muBN_rshiftc(muBN_t *r, muBN_size_t  n)  { 
  muBN_uword_t v;
  muBN_uword_t c;
  muBN_size_t i;
  muBN_uword_t w,b,m;
 
  n = n%(r->wlen*UBN_BITS_PER_WORD);
  w = n / UBN_BITS_PER_WORD;
  b = n % UBN_BITS_PER_WORD;
  m = b?UBN_WORD_BIT_MASK:0;
  c = 0;
  i = r->wlen-1;
 l1:
  if ((i^w) == 0) goto z;

  c = r->v[i-w-1] << (UBN_BITS_PER_WORD-b);
  c &= m;
  v =  r->v[i-w]>>b|c;
  r->v[i] = v;
  i--;
  goto l1;
  
 z:  
  c = r->C?UBN_WORD_BIT_MASK:0;
  c = c << (UBN_BITS_PER_WORD-b);
  c &= m;
  v =  r->v[i-w]>>b|c;
  r->v[i] = v;
  i--;

  c = r->C?UBN_WORD_BIT_MASK:0;
 l2:
  if ((i^(-1))  == 0) goto end;
  r->v[i] = c;
  i--;
  goto l2;

 end:
  r->C = 0;
  return;
}

void muBN_lshift1(muBN_t *r) {
  muBN_lshift1c(r);
  r->C = 0;
}
// r = r<<1
muBN_uword_t muBN_lshift1c(muBN_t *r) {
  muBN_size_t wlen =r->wlen;
  muBN_uword_t v;
  muBN_uword_t c;

  c = 0;
  while(wlen--) {  
    v = r->v[wlen];
    r->v[wlen] = (v<<1)|c;
    c = v >> (UBN_BITS_PER_WORD-1);
  }
  r->C = c;
  return c;
}

// r = r<<n
void muBN_lshift(muBN_t *r, muBN_size_t  n)  {
  muBN_size_t wlen1, wlen2;
  muBN_uword_t c = 0;
  muBN_size_t i;
  muBN_uword_t w,b,m;
  muBN_uword_t v;
 
  n = n%(r->wlen*UBN_BITS_PER_WORD);
  w = n / UBN_BITS_PER_WORD;
  b = n % UBN_BITS_PER_WORD;
  m = b?UBN_WORD_BIT_MASK:0;
  c = 0;
  i = 0;
  wlen2 = r->wlen ;
  wlen1 = r->wlen - w -1;
 l1: 
  if ((i^wlen1) == 0) goto z;
  c = r->v[i+w+1] >> (UBN_BITS_PER_WORD-b);
  c &= m;
  v =  r->v[i+w]<<b|c;
  r->v[i] = v;
  i++;
  goto l1;

 z:  
  v =  r->v[i+w] << b;
  r->v[i] = v;
  i++;

 l2:
  if ((i^wlen2)  == 0) goto end;
  r->v[i] = 0;
  i++;
  goto l2;

 end:
  r->C = 0;
  return;

}


// r = a+b 
muBN_word_t muBN_add(muBN_t *r,  muBN_t *a, muBN_t *b) {
#ifdef ADDSUB_WITH_DOUBLE_WORD
  muBN_size_t wlen =r->wlen;
  muBN_udword_t carry = 0;
  muBN_udword_t add;
  r->OV = 0;

  while (wlen--) {
    add = (muBN_udword_t)(a->v[wlen])+(muBN_udword_t)(b->v[wlen])+carry;
    r->v[wlen] = add;
    carry      =  add >>UBN_BITS_PER_WORD;
  }  
  add  = a->C+b->C+carry;
  r->C = add&1;
  return r->C;
#else
  muBN_size_t wlen =r->wlen;
  muBN_uword_t carry, cy1,cy2;
  muBN_uword_t add,tadd,va,vb;
  muBN_uword_t *pa, *pb,*pr;
  r->OV = 0;
  carry = 0;
  pa = &a->v[wlen];
  pb = &b->v[wlen];
  pr = &r->v[wlen];
  while (wlen--) {
    va = *--pa;
    vb = *--pb;
    tadd = va+vb;
    cy1 = tadd<va;
    add = tadd+carry;
    cy2 = add<tadd;
    carry = cy1|cy2;
    *--pr = add;
  }  
  add  = a->C+b->C+carry;
  r->C = add&1;
  return r->C;
#endif
}

// r = a+b, b > 0
muBN_word_t muBN_add_uword(muBN_t *r,  muBN_t *a,  muBN_uword_t vb) {
#ifdef ADDSUB_WITH_DOUBLE_WORD
  muBN_size_t wlen = r->wlen;
  muBN_udword_t carry = 0;
  muBN_udword_t add;
  r->OV = 0;
  wlen--;
  r->v[wlen]   = a->v[wlen]+vb;
  carry      =  r->v[wlen] < a->v[wlen];
  while (wlen--) {
    add =  (muBN_udword_t)(a->v[wlen])+carry; 
    r->v[wlen] = add; 
    carry      =  add >>UBN_BITS_PER_WORD;
  }  
  add =  a->C + carry;
  r->C = add&1;
  return r->C;
#else
  muBN_size_t wlen =r->wlen;
  muBN_uword_t carry;
  muBN_uword_t add,va;
  muBN_uword_t *pa, *pr;

  r->OV = 0;
  pa = &a->v[wlen];
  pr = &r->v[wlen];

  va = *--pa;
  add = va+vb;
  carry = add<va;
  *--pr = add;
  wlen--;

  while (wlen--) {
    va = *--pa;
    add = va+carry;
    carry= add<va;
    *--pr = add;
  }  
  r->C  = (a->C+carry)&1;
  return r->C;
#endif
}

// r = a-b
muBN_word_t muBN_sub(muBN_t *r,  muBN_t *a, muBN_t *b) {
#ifdef ADDSUB_WITH_DOUBLE_WORD
  muBN_size_t wlen = r->wlen;
  muBN_udword_t carry = 0;
  muBN_udword_t sub;

  r->OV = 0;
  carry = 0;
  while (wlen--) {
    sub        = (muBN_udword_t)(a->v[wlen])-(muBN_udword_t)(b->v[wlen])-carry;   
    r->v[wlen] = sub;
    carry      = (sub >> UBN_BITS_PER_WORD)?1:0;
  }
  sub =  a->C - b->C - carry ? 1 : 0;
  r->C = sub ? 1 : 0;
  return r->C;
#else
  muBN_size_t wlen =r->wlen;
  muBN_uword_t carry, cy1,cy2;
  muBN_uword_t sub,tsub,va,vb;
  muBN_uword_t *pa, *pb,*pr;
  r->OV = 0;
  carry = 0;
  pa = &a->v[wlen];
  pb = &b->v[wlen];
  pr = &r->v[wlen];
  while (wlen--) {
    va = *--pa;
    vb = *--pb;
    tsub = va-vb;
    cy1 = tsub>va;
    sub = tsub-carry;
    cy2 = sub>tsub;
    carry = cy1|cy2;
    *--pr = sub;
  }  
  r->C  = (a->C-b->C-carry)&1;
  return r->C;
#endif
}

muBN_word_t muBN_sub_uword(muBN_t *r,  muBN_t *a, muBN_uword_t vb) {
#ifdef ADDSUB_WITH_DOUBLE_WORD
  muBN_size_t wlen = r->wlen;
  muBN_udword_t carry = 0;
  muBN_udword_t sub;
  r->OV = 0;
  wlen--;
  r->v[wlen] = a->v[wlen] - vb;
  carry      =  r->v[wlen] > a->v[wlen];
  while (wlen--) {
    sub        =   (muBN_udword_t)(a->v[wlen])-carry;
    r->v[wlen] =   sub;
    carry      = (sub >> UBN_BITS_PER_WORD)?1:0;
  }

  sub =  a->C - carry ? 1 : 0;
  r->C = sub ? 1 : 0;
  return r->C;
#else
  muBN_size_t wlen =r->wlen;
  muBN_uword_t carry;
  muBN_uword_t sub,va;
  muBN_uword_t *pa, *pr;

  r->OV = 0;
  carry = 0;
  pa = &a->v[wlen];
  pr = &r->v[wlen];

  va = *--pa;
  sub = va-vb;
  carry = sub>va;
  *--pr = sub;
  wlen--;
  
  while (wlen--) {
    va = *--pa;
    sub = va-carry;
    carry = sub>va;
    *--pr = sub;
  }  
  r->C  = (a->C-carry)&1;
  return r->C;
#endif
}


void muBN_mul(muBN_t *r, muBN_t *a, muBN_t *b) {
  muBN_udword_t ab;
  muBN_udword_t carry;
  muBN_uword_t  *rv;
  muBN_size_t    awlen,bwlen;
  muBN_size_t   j;

  awlen = a->wlen;
  bwlen = b->wlen;
  carry = 0;
  muBN_zero(r);  
  rv = r->v+r->wlen-awlen;
  while(awlen--) {
    rv[0] = carry;
    carry = 0;
    j = bwlen;  
    while(j--) {
      ab = rv[j]+ (muBN_udword_t)(a->v[awlen])*(muBN_udword_t)(b->v[j])+carry;
      rv[j] = ab;
      carry = ab>>UBN_BITS_PER_WORD;
    }
    rv--;
  }
  rv[0] = carry;

}

void muBN_mul_uword(muBN_t *r, muBN_t *a, muBN_uword_t w) {
  muBN_udword_t ab;
  muBN_udword_t carry;
  muBN_uword_t  *rv;
  muBN_size_t    awlen;

  awlen = a->wlen;
  carry = 0;
  muBN_zero(r);  
  rv = r->v+r->wlen-awlen;  
  while(awlen--) {
    ab = (muBN_udword_t)(a->v[awlen])*(muBN_udword_t)(w)+carry;
    rv[awlen] = ab;
    carry = ab>>UBN_BITS_PER_WORD;   
  }
  rv--;
  rv[0] = carry;

}




/* ======================================================================================= */
/*                                Z/nZ  Arithmetic                                         */
/* ======================================================================================= */



muBN_word_t muBN_mod_inv(muBN_t *r, muBN_t *in, muBN_t *m,  muBN_uword_t *temp) {
  muBN_t u, v,  a, b;

  muBN_init_zero(&u, temp+m->wlen*0, m->wlen);
  muBN_init_zero(&v, temp+m->wlen*1, m->wlen);
  muBN_init_zero(&a, temp+m->wlen*2, m->wlen);
  muBN_init_zero(&b, temp+m->wlen*3, m->wlen);
  
  //int c;
  muBN_copy(&a,in);
  muBN_copy(&b,m);
  muBN_one(&u);
  muBN_zero(&v);
  while(muBN_ucmp(&a,&b)) {
    if (muBN_is_even(&a)) {
      muBN_urshift1(&a);
      if (!muBN_is_even(&u)) {
        muBN_add(&u,&u,m);
      }
      muBN_rshift1c(&u);            
    }
    else if (muBN_is_even(&b)) {
      muBN_urshift1(&b);
      if (!muBN_is_even(&v)) {
        muBN_add(&v,&v,m);
      }
      muBN_rshift1c(&v);      
    }
    else if (muBN_ucmp(&a,&b) > 0) {
      muBN_sub(&a,&a,&b);
      muBN_urshift1(&a);
      if(muBN_sub(&u,&u,&v)) {
        muBN_add(&u,&u,m);
      }
      if (!muBN_is_even(&u)) {
        muBN_add(&u,&u,m);        
      }
      muBN_rshift1c(&u); 
    } else {
      muBN_sub(&b,&b,&a);
      muBN_urshift1(&b);
      if(muBN_sub(&v,&v,&u)) {
        muBN_add(&v,&v,m);
      }
      if (!muBN_is_even(&v)) {
        muBN_add(&v,&v,m);
      }
      muBN_rshift1c(&v);    
    }
  }
  if (muBN_is_one(&a) && muBN_is_one(&b)) {
    muBN_copy(r,&u);
    return 1;
  } else {
    muBN_zero(r);
    return 0;
  }
}

//r = a % m, 
void muBN_mod(muBN_t *r,  muBN_t *b, muBN_t *m, muBN_uword_t  *tmp)  {
  //  int cnt;

  muBN_uword_t     *pa;
  muBN_uword_t     *pm;
  muBN_size_t       shf;
  muBN_size_t       t,n;
  muBN_size_t       i,j;
  muBN_udword_t     sum, muldw; 
  muBN_uword_t      carry; 
  muBN_uword_t      qi;
  muBN_uword_t      mq0,mq1,mq2;  
  muBN_uword_t      pm0,pm1;
  muBN_uword_t      pa0,pa1, pa2;

  muBN_t            mod;
  muBN_t            a;

  //dup m, to modify it
  muBN_init(&mod, tmp, m->wlen);
  muBN_copy(&mod,m);
  m = &mod;
  //dup b to modify it
  muBN_init(&a, tmp+ m->wlen, b->wlen+2);
  muBN_copy(&a,b);

  //1. normalize
  // ->normalize n
  t  = m->wlen;
  pm = m->v;
  while ((!*pm) && (t)) {
    pm++;
    t--;
  }
  if (t == 0) {
    return ;
  }
  mq0 = pm[0];
  shf = 0;
  while (mq0 < (1UL<<(UBN_BITS_PER_WORD-1))) {
    mq0 = mq0<<1;
    shf++;
  }
  muBN_lshift(m, shf);
  // ->normalize a
  muBN_lshift(&a, shf);
  n  = a.wlen;
  pa = a.v;
  while ((!*pa) && (n)) {
    pa++;
    n--;
  }
  if (n == 0) {
    muBN_zero(r);
    return;
  }

  //2. adjust first step
  qi = 0;
  for(;;) {
    for (i=0; i <t; i++) {  
      if (pm[i] == pa[i]) continue;
      if (pm[i]>pa[i])    goto step3;
      break;
    }
    carry = 0;    
    for (i = t-1; i>=0; i--) {
      sum   = (muBN_udword_t)pa[i]-(muBN_udword_t)pm[i]-(muBN_udword_t)carry;
      pa[i] = (muBN_word_t)sum;
      carry = (((muBN_uword_t)(sum>>UBN_BITS_PER_WORD))&UBN_WORD_HIGH_BIT) ? 1 : 0;
    }
    
  }

  
  //3. 
 step3:  
  pm0 = pm[0];
  if (t==1) {
    pm1 = 0;   
  } else {
    pm1 = pm[1]; 
  }
  
  for (i = n; i>=t+1; i--) {
    
    pa0 = pa[0];
    pa1 = pa[1];
    if ((t==1) &&(i==1)) {
      pa2 = 0;
    } else {
      pa2 = pa[2];
    }
    
    //3.1
    if (pa0 == pm0){
      qi = UBN_MAX_UWORD;
    } else {
      qi = (((muBN_udword_t)pa0<< UBN_BITS_PER_WORD)+(muBN_udword_t)pa1)/pm0;
    }

    //3.2 at most twice
    //loop:
    sum = (muBN_udword_t)qi*(muBN_udword_t)pm1;
    mq2 = (muBN_uword_t)sum;
    carry = (muBN_uword_t)(sum>>UBN_BITS_PER_WORD);
    sum = (muBN_uword_t)qi*(muBN_udword_t)pm0+carry;;
    mq1 = (muBN_uword_t)sum;
    carry = (muBN_word_t)(sum>>UBN_BITS_PER_WORD);
    mq0 = carry;
    
    //mqx > pax ==> cont
    carry = 0;
    carry = ((muBN_udword_t)pa2-(muBN_udword_t)mq2-carry )&UBN_WORD_CARRY_BIT ?1 :0;
    carry = ((muBN_udword_t)pa1-(muBN_udword_t)mq1-carry )&UBN_WORD_CARRY_BIT ?1 :0;
    carry = ((muBN_udword_t)pa0-(muBN_udword_t)mq0-carry )&UBN_WORD_CARRY_BIT ?1 :0;
    if (carry) {
      qi--;
      //goto loop;
    } else {
      mq0--;
    }

    //3.3
    carry = 0;
    muldw = 0;
    for (j = t-1; j >= 0; j--) {
      muldw = (muBN_udword_t)pm[j]*(muBN_udword_t)qi + muldw;
      sum = (muBN_udword_t)pa[j+1]-(muBN_uword_t)muldw-carry;
      pa[j+1] = (muBN_uword_t)sum;
      carry = (((muBN_uword_t)(sum>>UBN_BITS_PER_WORD))&UBN_WORD_HIGH_BIT) ? 1 : 0;
      muldw = (muBN_uword_t)(muldw>>UBN_BITS_PER_WORD);
    }
    sum = (muBN_udword_t)pa[0]-(muBN_uword_t)muldw-carry;
    pa[0] = (muBN_uword_t)sum;
    carry = (((muBN_uword_t)(sum>>UBN_BITS_PER_WORD))&UBN_WORD_HIGH_BIT) ? 1 : 0;    
    
    //3.4     
    if (carry) {
      carry = 0;
      for (j = t-1; j >= 0; j--) {
        muldw = pa[j+1];
        sum = (muBN_udword_t)pa[j+1]+(muBN_udword_t)pm[j]+carry;
        pa[j+1] = (muBN_uword_t)sum;
        carry = (muBN_uword_t)(sum >> UBN_BITS_PER_WORD);
      }
      muldw = pa[0];
      sum = (muBN_udword_t)pa[0]+carry;
      pa[0] = (muBN_uword_t)sum;
    } else {
      carry = 0;
      for (j = t-1; j >= 0; j--) {
        muldw = pa[j+1];
        sum = (muBN_udword_t)pa[j+1]+(muBN_udword_t)pm[j]+carry;
        pa[j+1] = (muBN_uword_t)muldw;
        carry = (muBN_uword_t)(sum >> UBN_BITS_PER_WORD);
      }
      muldw = pa[0];
      sum = (muBN_udword_t)pa[0]+carry;
      pa[0] = (muBN_uword_t)muldw;
    }
    
    pa++;
  }
  muBN_urshift(&a, shf);
  muBN_urshift(m, shf);
  muBN_copy(r,&a);
  /*
    if (muBN_is_neg(r)) {
    muBN_add(r,r,m);
    }
  */
}

void muBN_mod_add_sec(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, muBN_uword_t  *temp) {

  muBN_size_t wlen;
  muBN_udword_t cadd;
  muBN_udword_t csub;
  muBN_udword_t add;
  muBN_t        rsub;

 
  wlen  = r->wlen;
  cadd  = 0;
  csub  = 0;
  muBN_init(&rsub, temp, m->wlen);
  while (wlen--) {
    add = (muBN_udword_t)(a->v[wlen])+(muBN_udword_t)(b->v[wlen])+cadd;
    r->v[wlen] = add;
    cadd =  add >>UBN_BITS_PER_WORD;

    add = (muBN_udword_t)(r->v[wlen])-(muBN_udword_t)(m->v[wlen])-csub;
    rsub.v[wlen] = add;
    csub = (add >> UBN_BITS_PER_WORD)?1:0;
  }  
  if (csub) {
    muBN_copy(r,r);
  } else {
    muBN_copy(r,&rsub);
  }
}

void muBN_mod_sub_sec(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, muBN_uword_t  *temp) {
  muBN_size_t wlen;
  muBN_udword_t cadd;
  muBN_udword_t csub;
  muBN_udword_t add;
  muBN_t        radd;

 
  wlen  = r->wlen;
  cadd  = 0;
  csub  = 0;
  muBN_init(&radd, temp, m->wlen);
  while (wlen--) {
    add = (muBN_udword_t)(a->v[wlen])-(muBN_udword_t)(b->v[wlen])-csub;
    r->v[wlen] = add;
    csub =  (add >> UBN_BITS_PER_WORD)?1:0;

    add = (muBN_udword_t)(r->v[wlen])+(muBN_udword_t)(m->v[wlen])+cadd;
    radd.v[wlen] = add;
    cadd = add >>UBN_BITS_PER_WORD;
  }  
  if (csub) {
    muBN_copy(r,&radd);
  } else {
    muBN_copy(r,r);
  }
}


void muBN_mod_add(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m) {
  muBN_uword_t c = muBN_add(r,a,b);
  if (c || muBN_ucmp(r,m)>0) {
    muBN_sub(r,r,m);
  }
  
}

void muBN_mod_sub(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m) {
  if (muBN_sub(r,a,b)) {
    muBN_add(r,r,m);
  }  
}

void muBN_mod_mul(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, muBN_uword_t  *temp) {
  muBN_t dr;
  muBN_init(&dr, temp, m->wlen);
  muBN_mul(&dr,a,b);
  muBN_mod(r,&dr,m,temp+m->wlen);   
}


/* ======================================================================================= */
/*                                Mongomery  Arithmetic                                    */
/* ======================================================================================= */


//Z => Montgomery
void muBN_mgt_z2mgt(muBN_t *r,  muBN_t *a, muBN_t *m, muBN_t *j1, muBN_uword_t j0) {
  muBN_mgt_mul(r,a,j1,m,j0);
}

//Montgomery => Z 
void muBN_mgt_mgt2z(muBN_t *r,  muBN_t *a, muBN_t *m, muBN_t *one, muBN_uword_t j0) {
  muBN_mgt_mul(r,a,one,m,j0);
}

/* Compute 
 * j0 = -m mod 2^b, with b bitLength(muBN_uword_t)
 * j1 = R² mod m,   with R = 2^bitLength(m)
 */
muBN_uword_t muBN_mgt_cst(muBN_t *m, muBN_t *j1, muBN_uword_t *temp) { 
  muBN_t ubn_tmp;
  muBN_uword_t j0;
  muBN_uword_t a;
  muBN_size_t k;

  //Inversion of 'a' with Mersene moduli 2^b
  // Set x = 1
  // Perform k times: Set x = x*(2-a*x) mod 2^b
  //   with k = log2(b)
  a =-(m->v[m->wlen-1]) % ((muBN_udword_t)UBN_MAX_UWORD+1);
  j0 = 1;
  for (k = 0; k<(muBN_size_t)(UBN_LOG2_BITS_PER_WORD); k++) {
    j0 = j0*(2-a*j0);
  }

  // --- R² mod m ---
  k =  m->wlen*2+1;
  muBN_init_zero(&ubn_tmp, temp, k);
  ubn_tmp.v[0] = 1;
  muBN_mod(j1, &ubn_tmp, m, temp+k);

  // return 
  return j0;

}

muBN_uword_t muBN_mgt_inv_internal(muBN_t *r, muBN_t *a, muBN_t *m,
                                   muBN_uword_t *temp,
                                   muBN_uword_t mode) {
  
  /* mode:
   *  0: no phase 2
   *  1: mgt phase 2, input is in montgomery form  a.R
   *  2: Z phase 2, input is in Z/mZ space
   */
  muBN_t ubn_u, ubn_v, ubn_x2;
  muBN_size_t k;
#define x1 r
#define x2 (&ubn_x2)
#define uu  (&ubn_u)
#define vv  (&ubn_v)


  //step1: r = a⁻¹.2^k, n <= k<= 2n
  muBN_init(uu, temp+m->wlen*0, m->wlen);
  muBN_init(vv, temp+m->wlen*1, m->wlen);
  muBN_init(x2, temp+m->wlen*3, m->wlen);
  
  //int c;
  muBN_copy(uu,a);
  muBN_copy(vv,m);
  muBN_one(x1);
  muBN_zero(x2);
  k = 0;
  while(!muBN_is_zero(vv)) {
    
    if (muBN_is_even(vv)) {
      muBN_urshift1(vv);
      muBN_lshift1c(x1);
    }
    else if (muBN_is_even(uu)) {
      muBN_urshift1c(uu);
      muBN_lshift1c(x2);
    }
    else if (muBN_ucmp(vv,uu) >= 0) {
      muBN_sub(vv,vv,uu);
      muBN_urshift1c(vv);      
      muBN_add(x2,x2,x1);
      muBN_lshift1c(x1);      
    } else {
      muBN_sub(uu,uu,vv);
      muBN_urshift1c(uu); 
      muBN_add(x1,x1,x2);
      muBN_lshift1c(x2); 
    }
    k++;
  }
  
  if (!muBN_is_one(uu)) {
    muBN_zero(r);
    return 0;
  }
  if (muBN_ucmp(r,m)>0) {
    muBN_sub(r,r,m);
  }

  switch (mode) {
  case 1:
    k = (2*(m->wlen*sizeof(muBN_uword_t))*8)-k;
#if 0
    //v = 2^(2n-k)
    muBN_one(vv);
    muBN_lshift(vv,k); 
    muBN_mgt_mul(uu, r, j1, m, j0);
    muBN_mgt_mul(r, uu, vv,  m, j0);
#else
    while (k--) {
      if(muBN_lshift1c(r)) {
        muBN_sub(r,r,m);
      }
      if (muBN_ucmp(r,m)>=0) {
        muBN_sub(r,r,m);
      }
    }
#endif
    break;
    
  case 2:
    while (k--) {
      if (!muBN_is_even(r)) {
         muBN_add(r,r,m);
      }
      muBN_urshift1c(r); 
    }
    break;

  default :
    return k;
  }
  
  return  1;
}

muBN_uword_t muBN_mgt_inv(muBN_t *r, muBN_t *a, muBN_t *m,
                          muBN_uword_t *temp) {
  return muBN_mgt_inv_internal(r,a,m,temp,1);
}
                                          

void muBN_mgt_mul(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m,  muBN_uword_t j0) {
  /*                    a          x         y         m */
  muBN_udword_t dwRk, dwSk, dwTk;
  
  muBN_udword_t carry;
  muBN_uword_t  b0, r0, ai, ui,r0C;
  muBN_size_t   wlen;
  muBN_size_t   i,j;
  
  r0C = 0;
  wlen = m->wlen;
  b0 = b->v[wlen-1]; 
  muBN_zero(r);
  //1. R = 0
  for (i = wlen-1; i>=0; i--) {
    j = wlen-1;
    //2. ui = (r0+ai*b0)*j0
    ai = a->v[i];
    r0 = r->v[j];
    ui = ((r0 + ai*b0)*j0);
    //3. r = (r + ai*b + ui*m) / base
    // unroll first loop, and discard LSB word, only keep carry
    dwSk = (muBN_udword_t)ui*m->v[j];
    dwTk = (muBN_udword_t)ai*b->v[j];    
    dwRk = r0+(dwSk&UBN_WORD_BIT_MASK)+(dwTk&UBN_WORD_BIT_MASK);
    carry = 
      (dwSk>>(UBN_BITS_PER_WORD)) +
      (dwTk>>(UBN_BITS_PER_WORD)) +
      (dwRk>>(UBN_BITS_PER_WORD)) ;
    for (j = wlen-2; j >=0; j--) {
      dwSk = (muBN_udword_t)ui*m->v[j];
      dwTk = (muBN_udword_t)ai*b->v[j];
      dwRk =
        r->v[j] +
        (dwSk&UBN_WORD_BIT_MASK)+
        (dwTk&UBN_WORD_BIT_MASK)+
        (carry&UBN_WORD_BIT_MASK);
      r->v[j+1] = dwRk;
      carry = 
        (dwSk>>(UBN_BITS_PER_WORD)) +
        (dwTk>>(UBN_BITS_PER_WORD)) +
        (dwRk>>(UBN_BITS_PER_WORD)) +
        (carry>>(UBN_BITS_PER_WORD));
    }
    dwRk  = r0C+carry;
    r->v[0] = dwRk;
    r0C = dwRk>>UBN_BITS_PER_WORD;
  }

  carry = 0;
  if (r0C | (muBN_ucmp(r,m)>0)) {
    while (wlen--) {
      r0         = r->v[wlen];
      carry        = (muBN_udword_t)(r->v[wlen])-(muBN_udword_t)(m->v[wlen])-carry;
      r->v[wlen] = carry;
      carry      = (carry >> UBN_BITS_PER_WORD)?1:0;
    }
  } else {
    while (wlen--) {
      r0         = r->v[wlen];
      carry        = (muBN_udword_t)(r->v[wlen])-(muBN_udword_t)(m->v[wlen])-carry;
      r->v[wlen] = r0;
      carry      = (carry >> UBN_BITS_PER_WORD)?1:0;      
    }
  }
}


void muBN_mgt_zmul(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m,   
                  muBN_uword_t j0, muBN_t *j1, muBN_uword_t *tmp) {
  muBN_t t;
  muBN_init(&t, tmp, m->wlen);
  muBN_mgt_mul(&t,  j1, a, m, j0);
  muBN_mgt_mul(r,   &t, b, m, j0);
}
