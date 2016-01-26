#ifndef muBN_H
#define muBN_H

#include "muBN_config.h"

/*
 * muBN are big endian order of native word.
 * native word are either little or big endian
 */

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
void muBN_xor(muBN_t *r, muBN_t *a,  muBN_t *b);
void muBN_or (muBN_t *r, muBN_t *a,  muBN_t *b);
void muBN_and(muBN_t *r, muBN_t *a,  muBN_t *b);
void muBN_not(muBN_t *r, muBN_t *a);

void muBN_set_bit   (muBN_t *a, muBN_size_t  n);
void muBN_clear_bit (muBN_t *a, muBN_size_t  n);
muBN_word_t muBN_test_bit(const muBN_t *a, muBN_size_t n);

void muBN_mask_bits (muBN_t *a, muBN_size_t  n);

void muBN_setC(muBN_t *r);
void muBN_clearC(muBN_t *r);
muBN_word_t muBN_testC(muBN_t *r);

muBN_size_t muBN_count_bit(const muBN_t *a);

/* ======================================================================================= */
/*                                Z Arithmetic                                             */
/* ======================================================================================= */
/**
 * compare unsigned BN in a fast way
 * @return  0  if  a = b
 * @return >0  if  a > b
 * @return <0  if  b > a
 */
muBN_word_t muBN_ucmp(muBN_t *a,  muBN_t *b);
/**
 * compare signed BN in a fast way
 * @return  0  if  a = b
 * @return >0  if  a > b
 * @return <0  if  b > a
 */
muBN_word_t muBN_cmp(muBN_t *a,  muBN_t *b);

/**
 * Inplace r = -r 
 */
void muBN_negate(muBN_t *r);

/**
 * compare signed BN to zero in a fast way
 * @return 1 if  a  = ZERO
 * @return 0 if  a != ZERO 
 */
muBN_word_t muBN_is_zero(muBN_t *r); 

/**
 * compare signed BN to one 
 * @return 1 if  a  = ONE
 * @return 0 if  a != ONE 
 */
muBN_word_t muBN_is_one(muBN_t *r);

/**
 * compare signed BN to zero
 * @return 1 if  a is even
 * @return 0 else
 */
muBN_word_t   muBN_is_even(muBN_t *r);

/**
 * compare signed BN to zero
 * @return 1 if  odd
 * @return 0 else
 */
muBN_word_t   muBN_is_odd (muBN_t *r); 

/**
 *  r= r >> n
 *
 * @spa
 */
void muBN_rshift  (muBN_t *r, muBN_size_t n);

/**
 *  r= r >>> n
 *
 * @spa
 */
void muBN_urshift (muBN_t *r, muBN_size_t n); 

/**
 *  r= r >> 1
 *
 * @spa
 */
void  muBN_rshift1 (muBN_t *r);

/**
 *  r= r >>> 1
 *
 * @spa
 */
void muBN_urshift1(muBN_t *r);

/**
 *  r = r >>> n 
 *  clear carry
 *
 * @spa
 */
void  muBN_rshift1c(muBN_t *r);

/**
 *  r= r >> n 
 *  set hight bit to last carry
 *  clear carry
 *
 * @spa
 */
void  muBN_rshiftc (muBN_t *r, muBN_size_t  n);

/**
 *  r= r << 1
 *  clear carry
 *
 * @spa
 */
void  muBN_lshift1(muBN_t *r);

/**
 *  r= r << 1
 *
 * @spa
 */
muBN_uword_t muBN_lshift1c(muBN_t *r);

/**
 *  r= r << n
 *  clear carry
 *
 * @spa
 */
void  muBN_lshift (muBN_t *r, muBN_size_t  n);

/**
 *  r= a + b
 *
 * @spa
 */
muBN_word_t muBN_add(muBN_t *r,  muBN_t *a, muBN_t *b);
/**
 *  r= a + b
 *
 * @spa
 */
muBN_word_t muBN_add_uword(muBN_t *r,  muBN_t *a, muBN_uword_t b);
/**
 *  r= a - b
 *
 * @spa
 */
muBN_word_t   muBN_sub(muBN_t *r,  muBN_t *a, muBN_t *b);
/**
 *  r=a - b
 *
 * @spa
 */
muBN_word_t muBN_sub_uword(muBN_t *r,  muBN_t *a, muBN_uword_t b);
/**
 *  r= a * b
 *
 * @spa
 */
void muBN_mul(muBN_t *r, muBN_t *a, muBN_t *b);

/**
 *  r= a * w
 *
 * @spa
 */
void  muBN_mul_uword(muBN_t *r, muBN_t *a, muBN_uword_t w);

/**** secured function ****/ 
/**
 * compare signed BN to zero 
 * @return 1 if  a  = ZERO
 * @return 0 if  a != ZERO 
 * @spa
 */
muBN_word_t   muBN_is_zero_sec(muBN_t *r);

/**
 * compare signed BN to one 
 * @return 1 if  a  = ONE
 * @return 0 if  a != ONE 
 * @spa
 */
muBN_word_t   muBN_is_one_sec(muBN_t *r);

/**
 * compare unsigned BN
 * @return  0 if  a = b
 * @return  1 if  a !=  b
 * @spa
 */
muBN_word_t   muBN_ucmp_sec(muBN_t *a,  muBN_t *b );
/**
 * compare unsigned BN
 * @return  0 if  a = b
 * @return  1 if  a !=  b
 * @spa
 */
muBN_word_t   muBN_cmp_sec(muBN_t *a,  muBN_t *b );

/**
 *  randomize r
 */
void  muBN_rand(muBN_t *r);

/* ======================================================================================= */
/*                                Z/nZ  Arithmetic                                         */
/* ======================================================================================= */
/**
 * r= a % m
 *
 * @param r 
 * @param a 
 * @param m 
 * @param temp  temporary buffer with a word length a least equals to a.wlen*2 + 2 
 *
 */
void muBN_mod(muBN_t *r,  muBN_t *a, muBN_t *m, muBN_uword_t  *tmp);

void  muBN_mod_add(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m);
void  muBN_mod_sub(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m);
void  muBN_mod_mul(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m,  muBN_uword_t *temp);

/**
 * r = a⁻¹ mod m 
 *
 * @param r 
 * @param a 
 * @param m     moduli, prime or not
 * @param temp  temporary buffer with a word length a least equals to m.wlen*4
 *
 * @return 1 if inverse exist
 * @return 0 else
 *
 */
muBN_word_t muBN_mod_inv(muBN_t *r, muBN_t *a, muBN_t *m,  muBN_uword_t *temp) ;

/*** Secured function ***/
/*
 * r = a+b mod m 
 */
void  muBN_mod_add_sec(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, muBN_uword_t  *tmp);
/*
 * r = a-b mod m 
 */
void  muBN_mod_sub_sec(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m, muBN_uword_t  *tmp);


/* ======================================================================================= */
/*                                Mongomery  Arithmetic                                    */
/* ======================================================================================= */

/**
 * j0 = 
 * j1 = 
 *
 * @param m 
 * @param j1 
 * @param temp  temporary buffer with a word length a least equals to m.wlen*5 + 4
 *                                                                    
 *
 * @spa
 */
muBN_uword_t muBN_mgt_cst( muBN_t *m, muBN_t *j1, muBN_uword_t *temp);

/**
 *  r = Mont⁻¹(a), convert a number from  Montgomery representation into Z/pZ space.
 */
void muBN_mgt_mgt2z(muBN_t *r,  muBN_t *a, muBN_t *m, muBN_t *one, muBN_uword_t j0);

/**
 *  r = Mont(a),  convert a number from  Z/pZ space into Montgomery representation. 
 * 
 */
void muBN_mgt_z2mgt(muBN_t *r,  muBN_t *a, muBN_t *m, muBN_t *j1, muBN_uword_t j0);

/**
 * mr = MultMont(ma,mb),  with r = a*b mod m
 *
 * ma is the Mongomery representetation of a: ma = a.R mod m
 * mb is the Mongomery representetation of b: mb = b.R mod m
 * mr is the Mongomery representetation of r: mr = r.R mod m

 * @param a
 * @param b
 * @param m
 * @param j0
 *
 * @spa
 */
void muBN_mgt_mul(muBN_t *r,  muBN_t *a, muBN_t *b, muBN_t *m,  muBN_uword_t j0);


/**
 *  mr = ma⁻¹ mod m,  with r = a⁻¹ mod m 
 * 
 * ma is the Mongomery representetation of a: ma = a.R mod m
 * mr is the Mongomery representetation of r: mr = r.R mod m
 *  
  *
 * @param mr
 * @param ma
 * @param m
 * @param j0
 * @param j1
 * @param temp temporary buffer with a word length a least equals to m.wlen*3
 *
 */
muBN_uword_t muBN_mgt_inv(muBN_t *mr, muBN_t *ma, muBN_t *m,  muBN_uword_t j0, muBN_t *j1,
                        muBN_uword_t *temp);
/**
 * r = a * b mod m , using two montgomery multiplications
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
