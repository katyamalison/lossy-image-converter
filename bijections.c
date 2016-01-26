/* Russell Gens and Katya Malison (rgens01 & kmalis01)
 *
 * HW4 - arith
 * 10/22/2015
 * 
 * bijections.c - This file contains the implementation of the bijections
 *              module. The functions in this module are used for going
 *              between CV and RGB formats and for converting Y luma values
 *              into cosine space through a discrete cosine transform
 */

#include <math.h>
#include "bijections.h"

static const int ENCODE_A = 511;

/* rgb_to_cv() - A function that derives the Component Video values Y, Pb and
 *               Pr from the RGB values of the given Pnm_rgb struct.
 *               Returns a Pixel_cv struct with the new values.
 */
Pixel_cv rgb_to_cv(Pnm_rgb rgb_pix, unsigned denominator)
{
        Pixel_cv cv_pix = malloc(sizeof(*cv_pix));

        float r = (float) (rgb_pix -> red) / denominator ;
        float g = (float) (rgb_pix -> green) / denominator;
        float b = (float) (rgb_pix -> blue) / denominator;


        cv_pix -> y = ((0.299 * r) + (0.587 * g) + (0.114 * b));
        cv_pix -> pb = ((-0.168736 * r) - (0.331264 * g) + (0.5 * b));
        cv_pix -> pr = ((0.5 * r) - (0.418688 * g) - (0.081312 * b));

        return cv_pix;
}

/* cv_to_rgb() - A function that derives the RGB values red, green, and blue
 *               from the CV values of the given Pixel_cv struct.
 *               Returns a Pixel_cv struct with the new values.
 */
Pnm_rgb cv_to_rgb(Pixel_cv cv_pix)
{
        Pnm_rgb rgb_pix = malloc(sizeof(*rgb_pix));
        unsigned denominator = 255;
        
        float y = cv_pix -> y;
        float pb = cv_pix -> pb;
        float pr = cv_pix -> pr;

        float r = (y + (1.402 * pr));
        float g = (y - (0.344136 * pb) - (0.714136 * pr));
        float b = (y + (1.772 * pb));

        /* ensure that values are within bound for RGB values */
        if (r > 1) r = 1;
        if (r < 0) r = 0;

        if (g > 1) g = 1;
        if (g < 0) g = 0;

        if (b > 1) b = 1;
        if (b < 0) b = 0;

        rgb_pix -> red = (unsigned) round(r * denominator);
        rgb_pix -> green = (unsigned) round(g * denominator);
        rgb_pix -> blue = (unsigned) round(b * denominator);

        return rgb_pix;
}

/* dct() - Converts the Y values within a given CV_data struct to
 *         cosine space and returns a new Cos_coefficients struct with
 *         the converted cosine coefficients (a, b, c, and d).
 */
Cos_coefficients dct(Cv_data pixel_block)
{
        Cos_coefficients cos_data = malloc(sizeof(*cos_data));
        
        float y1 = pixel_block -> pix1 -> y;
        float y2 = pixel_block -> pix2 -> y;
        float y3 = pixel_block -> pix3 -> y;
        float y4 = pixel_block -> pix4 -> y;
        
        cos_data -> a = ((y4 + y3 + y2 + y1) / 4);
        cos_data -> b = ((y4 + y3 - y2 - y1) / 4);
        cos_data -> c = ((y4 - y3 + y2 - y1) / 4);
        cos_data -> d = ((y4 - y3 - y2 + y1) / 4);
                
        return cos_data;
}

/* inverse_dct() - Converts the cosine coefficients within a given cos_data 
 *         struct to CV values and stores them within a given Cv_data struct
 */
void inverse_dct(Cos_coefficients cos_data, Cv_data pixels)
{
        float a, b, c, d;

        a = cos_data -> a;
        b = cos_data -> b;
        c = cos_data -> c;
        d = cos_data -> d;

        pixels -> pix1 -> y = (a - b - c + d);
        pixels -> pix2 -> y = (a - b + c - d);
        pixels -> pix3 -> y = (a + b - c - d);
        pixels -> pix4 -> y = (a + b + c + d);
}
