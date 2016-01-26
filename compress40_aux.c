/* Russell Gens and Katya Malison (rgens01 & kmalis01)
 * 
 * HW4 - arith
 * 10/22/2015
 *
 * compress40_aux.c - The implementation of the functions used by
 *                    compress40.c to compress images.
 */

#include "compress40_aux.h"

static const int BYTE = 8; /*size of one byte (in bits)*/
static const int MSB = 24; /*lsb of most significant byte in codeword*/

/**************************************************************************/
/*                        Function Definitions                            */
/**************************************************************************/


/*
 * fix_image() - Takes in a given pixmap and alters it to have an even
 * number of rows, an even number of columns and a blocksize of two.
 */     
void fix_image(Pnm_ppm pixmap) 
{
        int new_width = pixmap -> width;
        int new_height = pixmap -> height;
        const A2Methods_T methods = uarray2_methods_blocked;
        A2Methods_UArray2 fixed_pixmap;

        if ((pixmap -> width % 2) != 0) {
                new_width = (pixmap -> width - 1);
        }
        if ((pixmap -> height % 2) != 0) {
                new_height = (pixmap -> height - 1);
        }

        fixed_pixmap = methods -> new_with_blocksize(new_width, new_height,
                                                     sizeof(struct Pnm_rgb),
                                                     BLOCKSIZE);
        methods -> map_default(fixed_pixmap, copy_apply, pixmap -> pixels);

        methods -> free(&(pixmap -> pixels));
        
        pixmap -> pixels = fixed_pixmap;
        pixmap -> height = new_height;
        pixmap -> width = new_width;
}

/*
 * copy_apply() - copy elements from the A2Methods_UArray2 in the closure to 
 * A2Methods_UArray2 being mapped on (array2).
 */
void copy_apply(int col, int row, A2Methods_UArray2 array2, 
                A2Methods_Object *ptr, void *cl)
{
        (void) array2;
        A2Methods_T methods = uarray2_methods_blocked; 
        
        *(Pnm_rgb) ptr = *(Pnm_rgb) methods -> at((A2Methods_UArray2)cl, col,
                                                  row);
}

/*
 * compress_image() - Takes in a given pixmap and calls a mapping function to
 *                    compress each 2 x 2 block of pixels into a 32 bit
 *                    codeword.
 */     
void compress_image(Pnm_ppm pixmap)
{
        Rgb_data cl = malloc(sizeof(*cl)); 
        
        cl -> denominator = pixmap -> denominator;
        cl -> pix_count = 1;
        
        pixmap -> methods -> map_default(pixmap -> pixels, compress_pixels,
                                         &cl);
        free(cl);
}

/*
 * compress_pixels() - mapping apply function called by compress_image()
 *                     stores the 4 rgb pixels in a block and compresses them
 *                     into a 32 bit codeword which sent to stdout
 *
 * Note: This function calls many helper functions from a number of modules 
 *       to execute each step of the compression process for the given block
 */
void compress_pixels(int col, int row, A2Methods_UArray2 arr,
                     A2Methods_Object *pixel, void *cl)
{
        (void) col;
        (void) row;
        (void) arr;

        Rgb_data block_data_rgb = *(Rgb_data*) cl;
        Cv_data block_data_cv;
        
        /* store current pixel in closure */
        get_pixels(&block_data_rgb, pixel);

        /* if all pixels in block have been stored, write codeword */
        if (block_data_rgb -> pix_count == 4) {
                uint64_t codeword = 0;

                block_data_cv = convert_rgb(block_data_rgb);
                
                codeword = pack_codeword(block_data_cv);
                write_codeword(codeword);
                
                free(block_data_cv -> pix1);
                free(block_data_cv -> pix2);
                free(block_data_cv -> pix3);
                free(block_data_cv -> pix4);
                free(block_data_cv);
                block_data_rgb -> pix_count = 0;
        }
        
        block_data_rgb -> pix_count++;
}

/*
 * get_pixels() - fill the given Rgb_data struct (closure) with the pixel
 * that the mapping function is currently visiting.
 */
void get_pixels(Rgb_data *cl, A2Methods_Object *pixel)
{
        Rgb_data closure = *cl;
        Pnm_rgb pix = (Pnm_rgb)pixel;

        if (closure -> pix_count % 4 == 1) {
                closure -> pix1 = pix;
        }
        else if (closure -> pix_count % 4 == 2)
                closure -> pix2 = pix;
        else if (closure -> pix_count % 4 == 3)
                closure -> pix3 = pix;
        else if (closure -> pix_count % 4 == 0) {
                closure -> pix4 = pix;
        }
}



/*
 * convert_rgb() - Takes a 2x2 block of rgb pixels and converts them to CV
 *                 representation.
 */
Cv_data convert_rgb(Rgb_data pixels)
{
        Cv_data cv_data = malloc(sizeof(*cv_data));
        unsigned denominator = pixels -> denominator;
        cv_data -> pix1 = rgb_to_cv(pixels -> pix1, denominator);
        cv_data -> pix2 = rgb_to_cv(pixels -> pix2, denominator);
        cv_data -> pix3 = rgb_to_cv(pixels -> pix3, denominator);
        cv_data -> pix4 = rgb_to_cv(pixels -> pix4, denominator);

        return cv_data;
}


/*
 * pack_codeword() - This function takes a group of 4 CV pixels and
 * compresses/encodes their data into a 32 bit codeword
 */    
uint64_t pack_codeword(Cv_data block_data_cv)
{
        uint64_t codeword = 0;
        
        codeword = compress_chroma(block_data_cv, codeword);
        codeword = compress_luma(block_data_cv, codeword);

        return codeword;
}

/*
 * write_codeword() - This function writes a given 32 bit codeword to stdout
 * in big-endian order.
 */
void write_codeword(uint64_t codeword)
{
        for (int lsb = MSB; lsb >= 0; lsb -= BYTE) {
                uint64_t c = Bitpack_getu(codeword, BYTE, lsb);
                putchar((int) c);
        }
}

/*
 * write_header() - This function writes the header of a compressed binary
 *                  image file to stdout using a given width and height
 *                  of the origninal image.
 */
void write_header(unsigned width, unsigned height)
{
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
}
