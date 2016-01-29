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

#ifndef muBN_H
#define muBN_H

/*
 * muBN are big endian order of native word.
 * Native word are either little or big endian
 *
 * A muBN is fixed precision number, represented in 2-complement
 * plus a carry.
 * 
 * The carry is automaticcaly set depending on the operation.
 */


#include "muBN_config.h"

#if UBN_BITS_PER_WORD == 32
#define UBN_LOG2_BITS_PER_WORD   4UL
#define UBN_WORD_BIT_MASK        0xFFFFFFFFUL
#define UBN_MAX_UWORD            0xFFFFFFFFUL
#define UBN_WORD_HIGH_BIT        0x80000000UL
#define UBN_WORD_LOW_BIT         0x00000001UL
#define UBN_WORD_CARRY_BIT       0x100000000UL

#define UBN_DWORD_HIGH_BIT       0x8000000000000000ULL

typedef uint32_t   muBN_uword_t;
typedef  int32_t   muBN_word_t;
typedef uint64_t   muBN_udword_t;
typedef  int64_t    muBN_dword_t;
typedef int32_t     muBN_size_t;

#define UBN_WORD32(x)            (x)

#ifdef UBN_LITTLE_ENDIAN
#define uword2BE(w)                                 \
  ( ((w)<<24) & 0xFF000000UL ) |                    \
  ( ((w)<<8)  & 0x00FF0000UL ) |                    \
  ( ((w)>>8)  & 0x0000FF00UL ) |                    \
  ( ((w)>>24) & 0x000000FFUL )
#endif


#elif UBN_BITS_PER_WORD == 16
#define UBN_LOG2_BITS_PER_WORD   2UL
#define UBN_WORD_BIT_MASK        0xFFFFUL
#define UBN_MAX_UWORD            0xFFFFUL
#define UBN_WORD_HIGH_BIT        0x8000UL
#define UBN_WORD_LOW_BIT         0x0001UL
#define UBN_WORD_CARRY_BIT       0x10000UL

#define UBN_DWORD_HIGH_BIT       0x80000000ULL

typedef uint16_t   muBN_uword_t;
typedef  int16_t   muBN_word_t;
typedef uint32_t   muBN_udword_t;
typedef  int32_t   muBN_dword_t;
typedef int16_t     muBN_size_t;

#define UBN_WORD32(x)            ((x)>>16) &0xFFFF, (x)&0xFFFF

#ifdef UBN_LITTLE_ENDIAN
#define uword2BE(w)                             \
  ( ((w)<<8)  & 0xFF00UL ) |                    \
  ( ((w)>>8)  & 0x00FFUL ) )
#endif

#elif UBN_BITS_PER_WORD == 8
#define UBN_LOG2_BITS_PER_WORD   1UL
#define UBN_WORD_BIT_MASK        0xFFUL
#define UBN_MAX_UWORD            0xFFUL
#define UBN_WORD_HIGH_BIT        0x80UL
#define UBN_WORD_LOW_BIT         0x01UL
#define UBN_WORD_CARRY_BIT       0x100UL

#define UBN_DWORD_HIGH_BIT       0x8000ULL

typedef uint8_t   muBN_uword_t;
typedef  int8_t   muBN_word_t;
typedef uint16_t  muBN_udword_t;
typedef  int16_t  muBN_dword_t;
typedef int16_t  muBN_size_t;

#define UBN_WORD32(x)            ((x)>>24) &0xFF, (x>>16)&0xFF, ((x)>>8) &0xFF, (x)&0xFF

#ifdef UBN_LITTLE_ENDIAN
#define uword2BE(w)              (w)
#endif

#else
#error "Unsupported UBN_BITS_PER_WORD"
#endif

#ifndef NULL
#define NULL ((void*)NULL)
#endif


#ifdef UBN_BIG_ENDIAN
#define uword2BE(w)              (w)
#endif

#define BE2uword(w)              uword2BE(w)                          

typedef struct {  
  uint8_t      OV:     1;
  uint8_t      C:      1;
  uint8_t      rfu:    6;
  muBN_size_t   wlen;
  muBN_uword_t  *v;
} muBN_t;


/* ======================================================================================= */
/*                                     Init                                                */
/* ======================================================================================= */

void  muBN_init     (muBN_t *r, muBN_uword_t  *a ,muBN_size_t wlen);
void  muBN_init_zero(muBN_t *r, muBN_uword_t  *a ,muBN_size_t wlen);

void  muBN_zero(muBN_t *r);
void  muBN_one(muBN_t *r);

void  muBN_copy(muBN_t *r, muBN_t *a);
void  muBN_clone(muBN_t *r, muBN_t *a);

muBN_size_t   muBN_bin2ubn(muBN_t *r,  uint8_t *from, muBN_size_t blen);
muBN_size_t   muBN_ubn2bin(muBN_t *r,  uint8_t *to,   muBN_size_t blen, uint8_t mode);
muBN_size_t   muBN_ubn2hex(muBN_t *r,  uint8_t *to,  muBN_size_t  blen);
muBN_size_t   muBN_hex2ubn(muBN_t *r,  uint8_t *to,  muBN_size_t  blen);

enum {
  UBN_FULL,
  UBN_OPTIMAL,
  UBN_DER
};
/* ======================================================================================= */
/*                                 Logic Arithmetic                                        */
/* ======================================================================================= */


/**
 * r = a ^b
 * 
 * @pre r,a,b have the word-length
 *
 * @param [out] r
 * @param [in]  a
 * @param [in]  b
 *
 * @spa
 */
void muBN_xor(muBN_t *r, muBN_t *a,  muBN_t *b);

/**
 * r = a | b
 *
 * @pre r,a,b have the word-length
 *
 * @param [out] r
 * @param [in]  a
 * @param [in]  b
 *
 * @spa
 */
void muBN_or (muBN_t *r, muBN_t *a,  muBN_t *b);

/**
 * r = a & b
 *
 * @pre r,a,b have the word-length
 *
 * @param [out] r
 * @param [in]  a
 * @param [in]  b
 *
 * @spa
 */
void muBN_and(muBN_t *r, muBN_t *a,  muBN_t *b);

/**
 * r = ~a 
 *
 * @pre r,a  have the word-length
 *
 * @param [out] r
 * @param [in]  a
 *
 * @spa
 */
void muBN_not(muBN_t *r, muBN_t *a);

/**
 * Set to 'one' the specified bit. 
 * Bit zero is the least significant one
 * 
 * @param [in/out] a
 * @param [in]     n
 *
 * @spa
 */
void muBN_set_bit   (muBN_t *a, muBN_size_t  n);

/**
 * Set to 'zero' the specified bit. 
 * Bit zero is the least significant one
 * 
 * @param [in/out] a
 * @param [in]     n
 *
 * @spa
 */
void muBN_clear_bit (muBN_t *a, muBN_size_t  n);


/**
 * Test to 'zero' the specified bit. 
 * Bit zero is the least significant one
 * 
 * @param [in]  a
 * @param [in]  n
 *
 * @return 1 if bit is set to 'one', 0 else
 *
 * @spa
 */
muBN_word_t muBN_test_bit(const muBN_t *a, muBN_size_t n);

/**
 * Set the carry to one
 *
 * @param [in/out] a
 *
 * @spa
 */
void muBN_setC(muBN_t *r);

/**
 * Set the carry to zero
 *
 * @param [in/out] a
 *
 * @spa
 */
void muBN_clearC(muBN_t *r);

/**
 * Test is carry is set
 *
 * @param [in] a
 *
 * @return 1 if carry is set, 0 else
 *
 * @spa
 */
muBN_word_t muBN_testC(muBN_t *r);

/**
 * Return the significant bit of 'a'. Carry is not considered.
 * @param [in] a
 *
 * @return   significant bit length of 'a'
 *
 */
muBN_size_t muBN_count_bit(const muBN_t *a);

/* ======================================================================================= */
/*                                Z Arithmetic                                             */
/* ======================================================================================= */
/**
 * Compare unsigned BN in a fast way
 *
 * @pre a,b have the same word-length
 *
 * @param [in] a
 * @param [in] b
 *
 * @return  0  if  a = b
 * @return >0  if  a > b
 * @return <0  if  b > a
 */
muBN_word_t muBN_ucmp(muBN_t *a,  muBN_t *b);
/**
 * Compare signed BN in a fast way
 *
 * @pre a,b have the same word-length
 *
 * @param [in] a
 * @param [in] b
 *
 * @return  0  if  a = b
 * @return >0  if  a > b
 * @return <0  if  b > a
 */
muBN_word_t muBN_cmp(muBN_t *a,  muBN_t *b);

/**
 * r = -r 
 *
 * @param [in/out] r
 *
 */
void muBN_negate(muBN_t *r);

/**
 * Compare signed BN to zero in a fast way
 *
 * @param [in] a
 * @param [in] b
 * 
 * @return 1 if  a  = ZERO
 * @return 0 if  a != ZERO 
 */
muBN_word_t muBN_is_zero(muBN_t *r); 

/**
 * Compare signed BN to one 
 *
 * @param [in] a
 * @param [in] b
 *
 * @return 1 if  a  = ONE
 * @return 0 if  a != ONE 
 */
muBN_word_t muBN_is_one(muBN_t *r);

/**
 * Test is 'r' is even.
 *
 * @param [in] r
 *
 * @return 1 if  a is even
 * @return 0 else
 */
#define muBN_is_even(r) (!((r)->v[(r)->wlen-1]&1))

/**
 * Compare signed BN to zero
 *
 * @param [in] r
 *
 * @return 1 if  odd
 * @return 0 else
 */
#define muBN_is_odd(r) (((r)->v[(r)->wlen-1]&1))

/**
 *  r= r >> n
 *
 * @param [in/out] r
 *
 * @spa
 */
void muBN_rshift  (muBN_t *r, muBN_size_t n);

/**
 *  r= r >>> n
  *
 * @param [in/out] r
*
 * @spa
 */
void muBN_urshift (muBN_t *r, muBN_size_t n); 

/**
 *  r= r >> 1
 *
 * @param [in/out] r
 *
 * @spa
 */
void  muBN_rshift1 (muBN_t *r);

/**
 *  r= r >>> 1
  *
 * @param [in/out] r
 *
 * @spa
 */
void muBN_urshift1(muBN_t *r);

/**
 *  r = r >>> 1, and propagate carry if any 
 *  clear carry
 *
 * @param [in/out] r
 *
 * @spa
 */
void  muBN_rshift1c(muBN_t *r);

/**
 *  r= r >> n, and propagate carry if any 
 *  clear carry
 *
 * @param [in/out] r
 * @param [in]     n
 *
 * @spa
 */
void  muBN_rshiftc (muBN_t *r, muBN_size_t  n);

/**
 *  r= r << 1, and propagate carry if any 
 *  clear carry
 *
 * @param [in/out] r
 *
 * @spa
 */
void  muBN_lshift1(muBN_t *r);

/**
 *  r= r << 1,  and set carry if any 
 *
 * @param [in/out] r
 *
 * @spa
 */
muBN_uword_t muBN_lshift1c(muBN_t *r);

/**
 *  r= r << n,  and clear carry 
 *
 * @param [in/out] r
 * @param [in]     n
 *
 * @spa
 */
void  muBN_lshift (muBN_t *r, muBN_size_t  n);

/**
 *  r= a + b, and set carry
 *
 * @pre r,a,b have the word-length
 *
 * @param [out] r
 * @param [in] n
 *
 * @spa
 */
muBN_word_t muBN_add(muBN_t *r,  muBN_t *a, muBN_t *b);

/**
 *  r= a + b,  and set carry
 *
 * @pre r,a have the word-length
 *
 * @param [out] r
 * @param [in] a
 * @param [in] w
 *
 * @spa
 */
muBN_word_t muBN_add_uword(muBN_t *r,  muBN_t *a, muBN_uword_t w);

/**
 *  r= a - b,  and set carry
 *
 * @pre r,a,b have the same word-length
 *
 * @param [out] r
 * @param [in] a
 * @param [in] b
 *
 * @spa
 */
muBN_word_t   muBN_sub(muBN_t *r,  muBN_t *a, muBN_t *b);

/**
 *  r=a - b,  and set carry
 *
 * @pre r,a have the same word-length
 *
 * @param [out] r
 * @param [in] a
 * @param [in] w
 *
 * @spa
 */
muBN_word_t muBN_sub_uword(muBN_t *r,  muBN_t *a, muBN_uword_t w);

/**
 *  r= a * b, and clear carry
 *
 * @pre r have length a lesat equal to twice the 'a' word-length + 'b' word-length + 1
 *
 * @param [out] r
 * @param [in] a
 * @param [in] b
 *
 * @spa
 */
void muBN_mul(muBN_t *r, muBN_t *a, muBN_t *b);

/**
 *  r= a * w, and clear carry
 *
 * @pre r have length a lesat equal to twice the 'a' word-length + 1
 *
 * @param [out] r
 * @param [in]  a
 * @param [in]  w
 *
 * @spa
 */
void  muBN_mul_uword(muBN_t *r, muBN_t *a, muBN_uword_t w);

/**** secured function ****/ 
/**
 * Compare signed BN to zero 
 *
 * @param [in] r
 *
 * @return 1 if  a  = ZERO
 * @return 0 if  a != ZERO 
 *
 * @spa
 */
muBN_word_t   muBN_is_zero_sec(muBN_t *r);

/**
 * Compare signed BN to one 
 *
 * @param [in] r
 *
 * @return 1 if  a  = ONE
 * @return 0 if  a != ONE 
 * @spa
 */
muBN_word_t   muBN_is_one_sec(muBN_t *r);

/**
 * Compare unsigned BN
 *
 * @param [in] a
 * @param [in] b
 *
 * @return  0 if  a = b
 * @return  1 if  a !=  b
 *
 * @spa
 */
muBN_word_t   muBN_ucmp_sec(muBN_t *a,  muBN_t *b );
/**
 * Compare unsigned BN
 *
 * @param [in] a
 * @param [in] b
 *
 * @return  0 if  a = b
 * @return  1 if  a !=  b
 *
 * @spa
 */
muBN_word_t   muBN_cmp_sec(muBN_t *a,  muBN_t *b );

/**
 *  Randomize r
 *
 * @param [out] r
 */
void  muBN_rand(muBN_t *r);

/* ======================================================================================= */
/*                                Z/nZ  Arithmetic                                         */
/* ======================================================================================= */
/**
 * r= a % m
 *
 * @pre r,m have the same word-length
 *
 * @param r 
 * @param a 
 * @param m 
 *
 * @param temp  temporary buffer with a word length a least equals to a.wlen*2 + 2 
 *
 */
void muBN_mod(muBN_t *r,  muBN_t *a, muBN_t *m, muBN_uword_t  *tmp);

/**
 * r= a+b % m
 *
 * @pre r,a,b,m have the same word-length
 * @pre a<m and b<m
 *
 * @param r 
 * @param a 
 * @param m 
 *
 */
void  muBN_mod_add(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m);

/**
 * r= a-b % m
 *
 * @pre r,a,b,m have the same word-length
 * @pre a<m and b<m
 *
 * @param r 
 * @param a 
 * @param m 
 *
 */
void  muBN_mod_sub(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m);

/**
 * r= a*b % m
 *
 * @pre r,a,b,m have the same word-length
 * @pre a<m and b<m
 *
 * @param r 
 * @param a 
 * @param m 
 *
 * @param temp  temporary buffer with a word length a least equals to a.wlen*2 + 2 
 *
 */
void  muBN_mod_mul(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m,  muBN_uword_t *temp);

/**
 * r = a⁻¹ mod m 
 *
 * @pre m shall be odd
 * @pre r,a,m have the same word-length
 * @pre a<m and b<m
 *
 * @param r 
 * @param a 
 * @param m     odd moduli, prime or not
 * @param temp  temporary buffer with a word length a least equals to m.wlen*4
 *
 * @return 1 if inverse exist
 * @return 0 else
 *
 */
muBN_word_t muBN_mod_inv(muBN_t *r, muBN_t *a, muBN_t *m,  muBN_uword_t *temp) ;

/*** Secured function ***/
/**
 * r= a+b % m
 *
 * @pre r,a,b,m have the same word-length
 * @pre a<m and b<m
 *
 * @param r 
 * @param a 
 * @param m 
 * @param temp  temporary buffer with a word length a least equals to a.wlen
 *
 */
void  muBN_mod_add_sec(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, muBN_uword_t  *tmp);

/**
 * r= a-b % m
 *
 * @pre r,a,b,m have the same word-length
 * @pre a<m and b<m
 *
 * @param r 
 * @param a 
 * @param m 
 * @param temp  temporary buffer with a word length a least equals to a.wle
 *
 */
void  muBN_mod_sub_sec(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, muBN_uword_t  *tmp);


/* ======================================================================================= */
/*                                Montgomery  Arithmetic                                    */
/* ======================================================================================= */

/**
 * Compute the two Montgomery constants for the given modulus:
 * . J0 = -m mod 2^b   (b, muBN_word bit length)
 * . J1 = 2^2l mod m   (l, modulus bit length)
 * 
 *
 * @pre j1,m have the same word-length
 *
 * @param [in]  m     modulus
 * @param [out] j1    where to store J1
 * @param [in]  temp  temporary buffer with a word length a least equals to m.wlen*5 + 4
 *
 * @return j0                                                                   
 *
 * @spa
 */
muBN_uword_t muBN_mgt_cst( muBN_t *m, muBN_t *j1, muBN_uword_t *temp);

/**
 *  r = Mont⁻¹(a), convert a number from  Montgomery representation into Z/mZ space.
 *
 * @param [in]  m     modulus
 * @param [out] j1    where to store J1
 * @param [in]  temp  temporary buffer with a word length a least equals to m.wle */
void muBN_mgt_mgt2z(muBN_t *r,  muBN_t *a, muBN_t *m, muBN_t *one, muBN_uword_t j0);

/**
 *  r = Mont(a),  convert a number from  Z/mZ space into Montgomery representation. 
 * @pre mr,a,m have the same word-length
 * @pre ma<m 
 *
 * @param a
 * @param b
 * @param m
 * @param j0
 * 
 */
void muBN_mgt_z2mgt(muBN_t *mr,  muBN_t *a, muBN_t *m, muBN_t *j1, muBN_uword_t j0);

/**
 * mr = MultMont(ma,mb),  with r = a*b mod m
 *
 * ma is the Montgomery representetation of a: ma = a.R mod m
 * mb is the Montgomery representetation of b: mb = b.R mod m
 * mr is the Montgomery representetation of r: mr = r.R mod m
 *
 * @pre mr,ma,mb,m have the same word-length
 * @pre ma<m  and m <m
 *
 * @param a
 * @param b
 * @param m
 * @param j0
 *
 * @spa
 */
void muBN_mgt_mul(muBN_t *mr,  muBN_t *ma, muBN_t *mb, muBN_t *m,  muBN_uword_t j0);


/**
 *  mr = ma⁻¹ mod m,  with r = a⁻¹ mod m 
 * 
 * ma is the Montgomery representetation of a: ma = a.R mod m
 * mr is the Montgomery representetation of r: mr = r.R mod m
 *
 * @pre mr,ma,m have the same word-length
 * @pre ma<m 
 *
 * @param mr
 * @param ma
 * @param m
 * @param j0
 * @param j1
 * @param temp temporary buffer with a word length a least equals to m.wlen*3
 *
 */
muBN_uword_t muBN_mgt_inv(muBN_t *mr, muBN_t *ma, muBN_t *m,  
                        muBN_uword_t *temp);
/**
 * r = a * b mod m , using two Montgomery multiplications
 *
 * @pre r,a,b,m have the same word-length
 * @pre a<m and b<m
 *
 * @param a
 * @param b
 * @param m
 * @param j0
 * @param j1
 * @param temp  temporary buffer with a word length a least equals to m.wlen
 *
 * @spa
 */
void muBN_mgt_zmul(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, 
                  muBN_uword_t j0, muBN_t *j1, muBN_uword_t *tmp);
#endif
