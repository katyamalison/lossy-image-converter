/* Russell Gens and Katya Malison (rgens01 & kmalis01)
 * 
 * hw4 COMP 40 - arith
 * 
 * 10/22/2015
 *
 * compress40_aux.h - The header file for the compress40_aux module. This file
 *                    holds the declarations for a set of
 *                    functions used by compress40.c to assist in the 
 *                    compression & decompression of images.
 */

#ifndef COMPRESS_AUX
#define COMPRESS_AUX

#include <stdlib.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "bijections.h"
#include "codewords.h"

static const int BLOCKSIZE = 2;

/**************************************************************************/
/*                        Function Prototypes                             */
/**************************************************************************/

/* crops rows & cols of an image to even vals*/
void fix_image(Pnm_ppm pixmap);

/* apply func used by fix image to copy elements between arrays */
void copy_apply(int col, int row, A2Methods_UArray2 array2, 
                A2Methods_Object *ptr, void *cl); /* NEEDSWORK name */

/* writes all 2x2 blocks of pixels in an image to stdout as 32 bit codewords*/
void compress_image(Pnm_ppm pixmap);

/* apply func used to compress a block of pixels & write them as a codeword */
void compress_pixels(int col, int row, A2Methods_UArray2 arr,
                     A2Methods_Object *pixel, void *cl);

/* used by compress_pixels to gather data from a block of 4 pixels */
void get_pixels(Rgb_data *cl, A2Methods_Object *pixel);

/* Converts a group of 4 pixels from RGB to CV representation*/
Cv_data convert_rgb(Rgb_data pixels);

/* takes 4 CV pixels and encodes them into a 32 bit codeword */
uint64_t pack_codeword(Cv_data block_data_cv);

/* writes the header of the compressed img to stdout */
void write_header(unsigned width, unsigned height);

/* write codeword to standard out */
void write_codeword(uint64_t codeword);

#endif
