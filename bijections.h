/* Russell Gens and Katya Malison (rgens01 & kmalis01)
 * hw4 COMP 40 - arith
 * 10/22/2015
 *
 * bijections.h - declaration of all structs and functions of the bijections
 *              module. The functions in this module are used for going
 *              between CV and RGB formats and for converting Y luma values
 *              into cosine space through a discrete cosine transform
 *             
 */

#ifndef BIJECTIONS_INCLUDED
#define BIJECTIONS_INCLUDED

#include <stdlib.h>
#include "assert.h"
#include "pnm.h"

/**************************************************************************/
/*                           Structs                                      */
/**************************************************************************/

typedef struct Pixel_cv {
        float y;
        float pb;
        float pr;
} *Pixel_cv; 

typedef struct Rgb_data {
        Pnm_rgb pix1, pix2, pix3, pix4;
        int pix_count;
        unsigned denominator;
} *Rgb_data;

typedef struct Cv_data {
        Pixel_cv pix1, pix2, pix3, pix4;
} *Cv_data;

typedef struct Cos_coefficients {
        float a, b, c, d;
} *Cos_coefficients;

/**************************************************************************/
/*                        Function Prototypes                             */
/**************************************************************************/

/* converts a single RGB pixel to CV representation. Returns the CV pixel */
Pixel_cv rgb_to_cv(Pnm_rgb rgb_pix, unsigned denominator);

/* converts a single CV pixel to RGB representation. Returns the RGB pixel */
Pnm_rgb cv_to_rgb(Pixel_cv cv_pix);

/* performs a discrete cosine transform on 4 CV pixels - returns a struct
 * holding the a, b, c, and d cosine coefficients.
 */
Cos_coefficients dct(Cv_data pixel_block); 

/* performs an inverse dct on a group of cosine coefficients and stores the 
 * resultant CV vals in a given Cv_data struct
 */
void inverse_dct(Cos_coefficients cos_data, Cv_data pixels);

#endif
