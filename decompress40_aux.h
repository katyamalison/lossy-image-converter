/* Russell Gens and Katya Malison
 *
 * hw4 COMP 40 - arith
 * 
 * decompress40_aux.h - 
 * 
 * The header file for the decompress_40_aux module. This file holds the 
 * declarations for a set offunctions used by compress40.c to assist in the 
 * decompression of images.
 * 
 * Detailed descriptions of functions are included in the .c file
 */

#ifndef DECOMPRESS_AUX
#define DECOMPRESS_AUX

#include <stdlib.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "bijections.h"
#include "codewords.h"

/* closure struct for mapping function for decompression */
typedef struct Decompress_cl {
        Rgb_data rgb_data;
        FILE* input;
} *Decompress_cl;

/* overhead function that calls a mapping function */
void decompress_image(Pnm_ppm pixmap, FILE *input);

/* apply function for A2Methods mapping function to decompress codewords */
void decompress_apply(int col, int row, A2Methods_UArray2 arr,
                     A2Methods_Object *pixel, void *cl);

/* read a codeword for a 2x2 block of pixels from given file */
uint64_t get_codeword(FILE *input);

/* unpack the pr, pb and cosine coefficient values from the codeword */
Cv_data unpack_block(uint64_t codeword);

/* Converts a group of 4 pixels from CV to RGB representation */
void convert_cv(Cv_data pixels, Decompress_cl closure);

#endif
