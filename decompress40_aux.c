/* Russell Gens and Katya Malison (rgens01 & kmalis01)
 * 
 * hw4 COMP 40 - arith
 * 
 * 10/22/2015
 *
 * decompress40_aux.c - 
 * 
 * The implementation of the functions of the decompress40_aux module. 
 * This module is used by compress40.c to decompress images.
 */

#include "decompress40_aux.h"

static const int BYTE = 8; /* size of one byte in bits */
static const int MSB = 24; /* lsb of most significant byte in codeword */

/* decompress_image() - wrapper function for decompression algorithm.
 */
void decompress_image(Pnm_ppm pixmap, FILE *input)
{
        Decompress_cl cl = malloc(sizeof(*cl));
        Rgb_data rgb_data = malloc(sizeof(*rgb_data));

        cl -> rgb_data = rgb_data;
        cl -> input = input;
        cl -> rgb_data -> denominator = pixmap -> denominator;
        cl -> rgb_data -> pix_count = 1;

        pixmap -> methods -> map_default(pixmap -> pixels, decompress_apply, 
                                         &cl);
        free(rgb_data);
        free(cl);
}

/* decompress_apply() - apply function for mapping. decompresses compressed 
 * binary image file into a ppm file. 
 */
void decompress_apply(int col, int row, A2Methods_UArray2 arr,
                     A2Methods_Object *pixel, void *cl)
{
        (void) col;
        (void) row;
        (void) arr;

        Decompress_cl closure = *(Decompress_cl*) cl;
        Cv_data block_data_cv;

        if (closure -> rgb_data -> pix_count == 1) {
                uint64_t codeword = 0;
                codeword = get_codeword(closure -> input); /* read word   */ 
                block_data_cv = unpack_block(codeword); /* unpack fields  */
                convert_cv(block_data_cv, closure); /* convert from cv to */
                free(block_data_cv -> pix1); /* NEEDSWORK: make free func */
                free(block_data_cv -> pix2);
                free(block_data_cv -> pix3);
                free(block_data_cv -> pix4);
                free(block_data_cv);
        }                                          

        /* set pixel to correct RGB data */
        if (closure -> rgb_data -> pix_count ==1) /* NEEDSWORK: make func */
                *(Pnm_rgb)pixel = *(closure -> rgb_data -> pix1);
        else if (closure -> rgb_data -> pix_count == 2) 
                *(Pnm_rgb)pixel = *(closure -> rgb_data -> pix2);
        else if (closure -> rgb_data -> pix_count == 3) 
                *(Pnm_rgb)pixel = *(closure -> rgb_data -> pix3);
        else if (closure -> rgb_data -> pix_count == 4) 
                *(Pnm_rgb)pixel = *(closure -> rgb_data -> pix4);

        /* increment, reset pix_count, and free rgb pixels */
        closure -> rgb_data -> pix_count ++;
        if (closure -> rgb_data -> pix_count == 5) {
                closure -> rgb_data -> pix_count = 1;
                free(closure -> rgb_data -> pix1);
                free(closure -> rgb_data -> pix2);
                free(closure -> rgb_data -> pix3);
                free(closure -> rgb_data -> pix4);
        }
}

/* get_codeword() - return a codeword from the given file (input)  
 */
uint64_t get_codeword(FILE *input)
{
        uint64_t codeword = 0;

        for (int lsb = MSB; lsb >= 0; lsb -= BYTE) {
                codeword = Bitpack_newu(codeword, BYTE, lsb, getc(input));
        }

        return codeword;
}

/* unpack_block() - unpack codewords into component video color space 
 * pixels. This function uses many helper functions to unpack the codeword 
 * into pb and pr values, and cosine coefficients.  
 */
Cv_data unpack_block(uint64_t codeword)
{
        Cv_data pixels = malloc(sizeof(*pixels));
        pixels -> pix1 = malloc(sizeof(struct Pixel_cv));
        pixels -> pix2 = malloc(sizeof(struct Pixel_cv));
        pixels -> pix3 = malloc(sizeof(struct Pixel_cv));
        pixels -> pix4 = malloc(sizeof(struct Pixel_cv));
        
        unpack_chroma(pixels, codeword);
        unpack_luma(pixels, codeword);

        return pixels;
}

/* convert_cv() - convert pixel format from component video color space 
 * to RGB 
 */
void convert_cv(Cv_data pixels, Decompress_cl closure)
{
        Rgb_data rgb_data;
        rgb_data = closure -> rgb_data;
        
        rgb_data -> pix1 = cv_to_rgb(pixels -> pix1);
        rgb_data -> pix2 = cv_to_rgb(pixels -> pix2);
        rgb_data -> pix3 = cv_to_rgb(pixels -> pix3);
        rgb_data -> pix4 = cv_to_rgb(pixels -> pix4);
}

