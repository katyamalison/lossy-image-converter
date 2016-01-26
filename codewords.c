/* Russell Gens and Katya Malison (rgens01 & kmalis01)
 * 
 * hw4 COMP 40 - arith
 * 
 * 10/22/2015
 *
 * codewords.c -
 * 
 * Definitions of all functions for codewords module
 * 
 */

#include "codewords.h"

static const float MIN_COEF = (-0.3); /* component video format min value */
static const float MAX_COEF = (0.3);  /* component video format max value */
static const unsigned QUANT_A = 63;  /* value for coefficient a division */
static const float QUANT_FACTOR = 103; /* to decode and encode luma values */
static const uint64_t ZERO = 0;

/* compress_chroma() - wrapper function for averaging and compression of 
 * chroma (pr and pb) values of the 2x2 block of pixels. returns codeword 
 * containing chroma codes in correct location.
 */
uint64_t compress_chroma(Cv_data block_data, uint64_t codeword)
{
        float avg_pr = 0;
        float avg_pb = 0;
        unsigned index_pr = 0;
        unsigned index_pb = 0;

        average_chroma(block_data, &avg_pr, &avg_pb);

        index_pr = Arith40_index_of_chroma(avg_pr);
        index_pb = Arith40_index_of_chroma(avg_pb);

        codeword = pack_pr(codeword, index_pr);
        codeword = pack_pb(codeword, index_pb);

        return codeword;
}

/* average_chroma() - helper function for compress_chroma(). averages the 
 * pr and pb values of all pixels in the given 2x2 block (block_data)
 */
void average_chroma(Cv_data block_data, float *avg_pr, float *avg_pb)
{
        *avg_pr += block_data -> pix1 -> pr;
        *avg_pr += block_data -> pix2 -> pr;
        *avg_pr += block_data -> pix3 -> pr;
        *avg_pr += block_data -> pix4 -> pr;

        *avg_pb += block_data -> pix1 -> pb;
        *avg_pb += block_data -> pix2 -> pb;
        *avg_pb += block_data -> pix3 -> pb;
        *avg_pb += block_data -> pix4 -> pb;

        *avg_pb = *avg_pb/4; 
        *avg_pr = *avg_pr/4;
}

/* compress_luma() - wrapper function for dct and compression of 
 * luma (a, b, c and d) values of the 2x2 block of pixels. returns codeword 
 * containing luma fields in correct location in the given codeword.
 */
uint64_t compress_luma(Cv_data block_data, uint64_t codeword)
{
        Cos_coefficients cos_data;

        cos_data = dct(block_data); 
        codeword = pack_luma(cos_data, codeword);        

        free(cos_data);

        return codeword;
}

/* pack_luma() - helper function for compress_luma(). pack each luma value 
 * into the given codeword
 */
uint64_t pack_luma(Cos_coefficients cos_data, uint64_t codeword)
{
        unsigned a;
        int b, c, d;
        
        a = (unsigned) round(cos_data -> a * QUANT_A); /* encode a */
        b = encode_luma(cos_data -> b);
        c = encode_luma(cos_data -> c);
        d = encode_luma(cos_data -> d);
        
        codeword = pack_a(codeword, a);
        codeword = pack_b(codeword, b);
        codeword = pack_c(codeword, c);
        codeword = pack_d(codeword, d);

        return codeword;
}

/* encode_luma() - helper function to pack_luma(). limits coefficient values
 * to be between the min and max (as defined by the specs) and encodes 
 * luma to be packed
 */
int encode_luma(float coefficient)
{
        if (coefficient < (MIN_COEF)) coefficient = (MIN_COEF);
        if (coefficient > (MAX_COEF)) coefficient = (MAX_COEF);
        
        return (int) round(coefficient * QUANT_FACTOR);
}       

uint64_t pack_pr(uint64_t codeword, unsigned idx_pr)
{
        return Bitpack_newu(codeword, SIZE_CHROMA, LSB_PR, (uint64_t)idx_pr);
}

uint64_t pack_pb(uint64_t codeword, unsigned idx_pb)
{
        return Bitpack_newu(codeword, SIZE_CHROMA, LSB_PB, (uint64_t)idx_pb);
}

uint64_t pack_d(uint64_t codeword, int d)
{
        return Bitpack_news(codeword, SIZE_D, LSB_D, (int64_t)d);
}

uint64_t pack_c(uint64_t codeword, int c)
{
        return Bitpack_news(codeword, SIZE_C, LSB_C, (int64_t)c);
}

uint64_t pack_b(uint64_t codeword, int b)
{
        return Bitpack_news(codeword, SIZE_B, LSB_B, (int64_t)b);
}
uint64_t pack_a(uint64_t codeword, unsigned a)
{
        return Bitpack_newu(codeword, SIZE_A, LSB_A, (uint64_t)a);
}

/* unpack_chroma() - unpack index pr and index pb values from the given 
 * codeword
 */
void unpack_chroma(Cv_data pixels, uint64_t codeword)
{
        unsigned idx_pb = unpack_pb(codeword);
        unsigned idx_pr = unpack_pr(codeword);
        float avg_pb = Arith40_chroma_of_index(idx_pb);
        float avg_pr = Arith40_chroma_of_index(idx_pr);

        pixels -> pix1 -> pr = avg_pr;
        pixels -> pix2 -> pr = avg_pr;
        pixels -> pix3 -> pr = avg_pr;
        pixels -> pix4 -> pr = avg_pr;

        pixels -> pix1 -> pb = avg_pb;
        pixels -> pix2 -> pb = avg_pb;
        pixels -> pix3 -> pb = avg_pb;
        pixels -> pix4 -> pb = avg_pb;
        
}

/* unpack_luma() - unpack luma values (a, b, c, d) from the given codeword 
 * into component video color space pixels
 */
void unpack_luma(Cv_data pixels, uint64_t codeword)
{
        Cos_coefficients cos_data = malloc(sizeof(*cos_data));
        unsigned a;
        int b, c, d;
        
        a = unpack_a(codeword);
        b = unpack_b(codeword);
        c = unpack_c(codeword);
        d = unpack_d(codeword);

        cos_data -> d = decode_luma(d);
        cos_data -> c = decode_luma(c);
        cos_data -> b = decode_luma(b);
        cos_data -> a = (float)(a / (float) QUANT_A);

        inverse_dct(cos_data, pixels);

        free(cos_data);
}

/* decode_luma() - helper function to unpack_luma that specifically turns 
 * an unpacked luma fieldcodes into  cv values. 
 */
float decode_luma(int code)
{
        float cos_coef;
        cos_coef = (float)(code / (float) QUANT_FACTOR);

        if (cos_coef > MAX_COEF) return MAX_COEF;
        if (cos_coef < MIN_COEF) return MIN_COEF;

        return cos_coef;
}

unsigned unpack_pr(uint64_t codeword)
{
        return (unsigned) Bitpack_getu(codeword, SIZE_CHROMA, LSB_PR);
}

unsigned unpack_pb(uint64_t codeword)
{
        return (unsigned) Bitpack_getu(codeword, SIZE_CHROMA, LSB_PB);
}

int unpack_d(uint64_t codeword)
{
        return (int) Bitpack_gets(codeword, SIZE_D, LSB_D);
}

int unpack_c(uint64_t codeword)
{
        return (int) Bitpack_gets(codeword, SIZE_C, LSB_C);
}
int unpack_b(uint64_t codeword)
{
        return (int) Bitpack_gets(codeword, SIZE_B, LSB_B);
}

unsigned unpack_a(uint64_t codeword)
{
        return (unsigned) Bitpack_getu(codeword, SIZE_A, LSB_A);
}
