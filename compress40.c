/* Russell Gens and Katya Malison (rgens01 &  kmalis01)
 *
 * HW4 - arith
 * 10/22/2015
 * 
 * compress40.c - This file is the implementation of the compress40 module
 *              using a fleet of helper modules it can either compress or
 *              decompress an image from a given file
 */

#include <stdlib.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "compress40_aux.h"
#include "decompress40_aux.h"

#define BLOCKSIZE 2
#define DENOMINATOR 255

/*
 * compress40() - Takes an open file and compresses the image within. If
 *              the image has an uneven number of rows and columns it is
 *              cropped. The compressed image is written to stdout
 */
extern void compress40  (FILE *input)
{
        Pnm_ppm pixmap;
        A2Methods_T methods = uarray2_methods_blocked;

        pixmap = Pnm_ppmread(input, methods);

        fix_image(pixmap);
        
        write_header(pixmap -> width, pixmap -> height);
        compress_image(pixmap);
        Pnm_ppmfree(&pixmap);

}

/* 
 * decompress40() - Takes an open file containing a compressed binary image,
 *                decompresses it and writes the resultant PPM to stdout
 */
extern void decompress40(FILE *input)
{
        Pnm_ppm pixmap = malloc(sizeof(*pixmap));
        A2Methods_T methods = uarray2_methods_blocked;
        A2Methods_UArray2 pixels;
        unsigned height, width;

        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                          &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        pixels = methods -> new_with_blocksize(width, height,
                                               sizeof(struct Pnm_rgb),
                                               BLOCKSIZE);
        pixmap -> pixels = pixels;
        pixmap -> width = width;
        pixmap -> height = height;
        pixmap -> denominator = DENOMINATOR;
        pixmap -> methods = methods;

        decompress_image(pixmap, input);

        Pnm_ppmwrite(stdout, pixmap);

        Pnm_ppmfree(&pixmap);

        return;

}
