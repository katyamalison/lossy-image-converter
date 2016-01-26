/* Russell Gens and Katya Malison (rgens01 & kmalis01)
 * 
 * hw4 COMP 40 - arith
 * 
 * 10/22/2015
 *
 * codewords.h - 
 * 
 * This file includes the function declarations for the codewords module
 * This file is concerned with all basic codeword manipulations for the 
 * compression and decompression of a ppm image. 
 *
 * Detailed descriptions of each function are provided in the .c file
 */

#ifndef CODEWORDS
#define CODEWORDS

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "bitpack.h"
#include "bijections.h"
#include "arith40.h"

static const unsigned SIZE_CHROMA = 4; // number of bits to store  Pb and Pr
static const unsigned LSB_PR = 0;      // least significant bit of Pr
static const unsigned LSB_PB = 4;      // least significant bit of Pb

static const unsigned SIZE_A = 6;      /* all the SIZE constants are used */
static const unsigned SIZE_B = 6;      /* to determine how many bits each */
static const unsigned SIZE_C = 6;      /* cosine coefficient take up in   */
static const unsigned SIZE_D = 6;      /* the codeword (incase you guys   */
static const unsigned LSB_A = 26;      /* switch this in the challenge)   */
static const unsigned LSB_B = 20;     
static const unsigned LSB_C = 14;      /* the LSB constants serve a simi- */
static const unsigned LSB_D = 8;       /* lar purpose, but to store the   */
                                       /* least significant bit for each  */

/* compress cv chroma values into fields to be packed in a 32-bit codeword */
uint64_t compress_chroma(Cv_data block_data, uint64_t codeword);

/* average pb and pr values for a 2x2 block pixel */
void average_chroma(Cv_data block_data, float *avg_pr, float *avg_pb);

/* compress cv luma values into fields to be packed in a codeword */
uint64_t compress_luma(Cv_data block_data, uint64_t codeword);

/* pack luma values into their position in a 32-bit codeword */
uint64_t pack_luma(Cos_coefficients cos_data, uint64_t codeword);

/* helper function for compress luma */
int encode_luma(float coefficient);

/* unpack chroma values from their position in a 32-bit codeword */
void unpack_chroma(Cv_data pixels, uint64_t codeword);

/* unpack luma values from their position in a 32-bit codeword */
void unpack_luma(Cv_data pixels, uint64_t codeword);

/* helper function for unpack_luma */
float decode_luma(int code);

/* pack given codefields into correct position in the given codeword */
uint64_t pack_pr(uint64_t codeword, unsigned idx_pr);
uint64_t pack_pb(uint64_t codeword, unsigned idx_pb);
uint64_t pack_d(uint64_t codeword, int d);
uint64_t pack_c(uint64_t codeword, int c);
uint64_t pack_b(uint64_t codeword, int b);
uint64_t pack_a(uint64_t codeword, unsigned a);

/* unpack codefields from given codeword */
unsigned unpack_pr(uint64_t codeword);
unsigned unpack_pb(uint64_t codeword);
int unpack_d(uint64_t codeword);
int unpack_c(uint64_t codeword);
int unpack_b(uint64_t codeword);
unsigned unpack_a(uint64_t codeword);

#endif
