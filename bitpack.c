/* Bitpack.c
 * 
 * By - Katya M. Malison & Russell W. Gens (rgens01 & kmalis01)
 * 10/22/2015
 *
 * This file holds the implementation of the Bitpack interface. All bitshifts
 * were implemented using static inline functions
 */

#include "bitpack.h"
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

/* Three functions implemented to simulate true hardware shifts. When
 * asked to shift a word by wordlength (64) they do something "reasonable"
 * - in this case all bits are flipped to zero with the exception of
 * performing a signed shift on a negative value (all bits flip to 1)
 */
static inline uint64_t shift_left(uint64_t word, unsigned shift);
static inline uint64_t shift_rightu(uint64_t word, unsigned shift);
static inline uint64_t shift_rights(int64_t word, unsigned shift);
uint64_t make_mask(unsigned width, unsigned offset);

static const unsigned WORD_SIZE = 64;
static const uint64_t ZERO = 0;
static const uint64_t ONE = 1;
static const int64_t NEG_ONE = (-1);

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * Bitpack_fitsu() - This function takes an unsigned 64 bit value
 *                   and a field width and returns a bool indicating whether
 *                   or not the given value can be represented in "width" 
 *                   bits.
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        if (width >= WORD_SIZE) return true; /*the >= operator is used here*/
        if (width == ZERO) return false;     /*since a width greater than  */
                                             /*wordsize will always fit    */
                                             /*the word                    */

        return (n < shift_left(ONE, width));
}

/*
 * Bitpack_fitss() - This function takes a signed 64 bit value
 *                   and a field width and returns a bool indicating whether
 *                   or not the given value can be represented in "width" 
 *                   bits.
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        if (n >= (int64_t) ZERO)
                return Bitpack_fitsu((uint64_t) n, width - 1);

        return (Bitpack_fitsu((uint64_t)(~n), width - 1));
}

/*
 * Bitpack_getu() - This function takes an unsigned 64 bit word, a field
 *                  width to extract a value from, and the least significant
 *                  bit of the field to be extracted. This function returns
 *                  the unsigned value extracted from the original word
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert((width + lsb) <= WORD_SIZE);

        if (width == 0) return ZERO;

        uint64_t mask;
        mask = make_mask(width, lsb);
        
        return shift_rightu((mask & word), lsb);
}

/*
 * Bitpack_gets() - This function takes an unsigned 64 bit word, a field
 *                  width to extract a value from, and the least significant
 *                  bit of the field to be extracted. This function returns
 *                  the signed value extracted from the original word
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        word = Bitpack_getu(word, width, lsb);
        word = shift_left(word, (WORD_SIZE - width));

        return shift_rights(word, (WORD_SIZE - width));
}

/*
 * Bitpack_newu() - This function takes an unsigned 64 bit word, a field
 *                  width to update within that word, the least significant
 *                  bit of the field to be updated, and an unsigned value to 
 *                  insert into the field being updated. This function 
 *                  returns the updated word.
 *
 *                  note: if the word being inserted is too large for its
 *                        field width, the exception Bitpack_Overflow is
 *                        raised.
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        assert((width + lsb) <= 64);

        if (! Bitpack_fitsu(value, width)) RAISE(Bitpack_Overflow);
        
        uint64_t mask;

        mask = ~(make_mask(width, lsb));
        word = (word & mask);
        word = (word | shift_left(value, lsb));

        return word;
}

/*
 * Bitpack_news() - This function takes an unsigned 64 bit word, a field
 *                  width to update within that word, the least significant
 *                  bit of the field to be updated, and a signed value to 
 *                  insert into the field being updated. This function 
 *                  returns the updated word.
 *
 *                  note: if the word being inserted is too large for its
 *                        field width, the exception Bitpack_Overflow is
 *                        raised.
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  
                      int64_t value)
{
        if (! Bitpack_fitss(value, width)) RAISE(Bitpack_Overflow);

        return Bitpack_newu(word, width, lsb, 
                            Bitpack_getu(value, width, ZERO));
}

/*
 * shift_left() - This function takes a given 64 bit word and shifts it left
 *                by "shift" bits. If the shift is by 64 bits or more, all
 *                bits are left as zero's
 */
static inline uint64_t shift_left(uint64_t word, unsigned shift)
{
        if (shift >= WORD_SIZE) return ZERO;

        return (word << shift);
}

/*
 * shift_rightu() - This function takes a given 64 bit word and performs an 
 *                unsigned shift right by "shift" bits (new bits enter as
 *                zero's). If the shift is by 64 bits or more, all
 *                bits are left as zero's
 */
static inline uint64_t shift_rightu(uint64_t word, unsigned shift)
{
        if (shift >= WORD_SIZE) return ZERO;

        return (word >> shift);
}

/*
 * shift_rights() - This function takes a given 64 bit word and performs a 
 *                signed shift right by "shift" bits (new bits enter as
 *                the sign bit of the word being shifted). If the shift is 
 *                by 64 bits or more, all bits propagate as the sign bit
 */
static inline uint64_t shift_rights(int64_t word, unsigned shift)
{
        if (shift >= WORD_SIZE) {
                if (word < 0) return NEG_ONE;
                return ZERO;
        }

        return (word >> shift);
}

/*
 * make_mask() - This function takes a width and an "offset" to create a
 *               mask of 1's over a field of width bits with an lsb of 
 *               offset. This funciton returns the mask.
 */
uint64_t make_mask(unsigned width, unsigned offset)
{
        uint64_t mask = ~0;
        mask = shift_left(shift_rightu(mask, (WORD_SIZE - width)), offset);
        return mask;
}
