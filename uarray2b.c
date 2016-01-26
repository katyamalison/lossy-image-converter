/*
 *        uarray2b.c
 *        by Russell W. Gens (rgens01) & Joshua N. White (jwhite17), 10/8/2015
 *        HW3 - Locality
 *
 *        This file contains the implementation of the uarray2b abstraction,
 *        a 2D blocked, polymorphic grid of homogenous data
 *
 *        Note: It is a checked runtime error to pass any function a null
 *              UArray2.
 *
 *        Invariants:
 *              - An element at the index idx of a UArray given by the
 *                below formulae corresponds to a single element in a given
 *                row (j) and col (i).
 *                
 *                First, the block coordinate will be found with 
 *                (block_i, block_j) =  (i / blocksize, j / blocksize). 
 *                Then, the coordinates of the element within that block will 
 *                be in the UArray at index 
 *                idx = blocksize * (i % blocksize) + (j %  blocksize)
 *
 *              - Every cell within a block is stored in a single UArray, and
 *                is therefore contiguous in memory (has good spatial locality.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "uarray2b.h"
#include "uarray.h"
#include "uarray2.h"
#include "assert.h"


#define T UArray2b_T

/*****************************************************************************
*                          Global Constants
******************************************************************************/
static const int BLOCK64 = 65536;

/*****************************************************************************
*                              Structs
******************************************************************************/
struct T {
        UArray2_T array;
        int width;
        int height;
        int size;
        int blocksize;
};

struct Block_data {
        int blocksize;
        int size;
};


/*****************************************************************************
*                        Function Definitions
******************************************************************************/

/*
 * Apply function for UArray2b_map()
 * 
 * Arguments:
 *      col: column coordinate of element being operated on
 *      row: row coordinate of element being operated on
 *      arr: the array being operated on
 *      *val: a pointer to the element at (col, row)
 *      *cl: closure containing element size & blocksize
 *
 */
void create_block(int col, int row, UArray2_T arr, void *val, void *cl)
{
        assert(arr != NULL);
        *(UArray_T*)val = UArray_new(((struct Block_data*)cl) -> blocksize * 
                                     ((struct Block_data*)cl) -> blocksize,
                                     ((struct Block_data*)cl) -> size);
                                     
        (void) col;
        (void) row;
        (void) arr;
}

/*
 * Creates and returns a new UArray2b
 * 
 * Arguments:
 *      width: Width of the grid
 *      height: Height of the grid
 *      size: Size in bytes of elements the grid will hold
 *      blocksize: Square root of number of cells in block
 *
 * Notes: 
 *      If memory allocation fails, terminates program. If width, height,
 *      size, blocksize are < 0, program terminates.
 */
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        T arr = malloc(sizeof(*arr));
        UArray2_T block_arr;
        struct Block_data cl = {blocksize, size};
  
        assert(width > 0 && height > 0 && blocksize > 0);
        
        assert(arr != NULL);
        
        block_arr = UArray2_new((1 + (width - 1) / blocksize), 
                    (1 + (height - 1) / blocksize), 
                    sizeof(UArray_T)); 
        UArray2_map_row_major(block_arr, create_block, &cl);
                    
        arr -> width = width;
        arr -> height = height;
        arr -> array = block_arr;
        arr -> size = size;
        arr -> blocksize = blocksize;
        
        return arr;
}

/*
 * Creates and returns a new UArray2b, such that the blocksize is as large as
 * possible while allowing the block to fit in 64KB of RAM.
 * 
 * Arguments:
 *      width: Width of the grid
 *      height: Height of the grid
 *      size: Size in bytes of elements the grid will hold
 *
 * Notes: 
 *      If memory allocation fails, terminates program. If width, height,
 *      size are < 0, program terminates.
 */
UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        T arr = malloc(sizeof(*arr));
        UArray2_T block_arr;
        int blocksize;
        struct Block_data cl;
        
        assert(size > 0); /* Prevent divide by 0 */
        
        blocksize = sqrt(BLOCK64/size);
        /* If cell is too large to fit in 64 kB, set blocksize to 1 */
        if (blocksize == 0) 
                blocksize = 1;
        
        assert(width > 0 && height > 0 && blocksize > 0);
        assert(arr != NULL);
        
        cl.blocksize = blocksize; 
        cl.size = size;
        
        block_arr = UArray2_new((1 + (width - 1) / blocksize), 
                    (1 + (height - 1) / blocksize), 
                    sizeof(UArray_T)); 
        UArray2_map_row_major(block_arr, create_block, &cl);
                    
        arr -> width = width;
        arr -> height = height;
        arr -> array = block_arr;
        arr -> size = size;
        arr -> blocksize = blocksize;
        return arr;
}

/*
 * Returns width of passed UArray2b.
 * 
 * Arguments:
 *      array2b: The UArray2b
 */
int UArray2b_width (T array2b) 
{
        assert(array2b != NULL);
        return array2b -> width;
}

/*
 * Returns height of passed UArray2b.
 * 
 * Arguments:
 *      array2b: The UArray2b
 */
int UArray2b_height (T array2b) 
{
        assert(array2b != NULL);
        return array2b -> height;
}

/*
 * Returns element size of passed UArray2b.
 * 
 * Arguments:
 *      array2b: The UArray2b
 */
int UArray2b_size (T array2b) 
{
        assert(array2b != NULL);
        return array2b -> size;
}

/*
 * Returns blocksize of passed UArray2b.
 * 
 * Arguments:
 *      array2b: The UArray2b
 */
int UArray2b_blocksize (T array2b) 
{
        assert(array2b != NULL);
        return array2b -> blocksize;
}

/*
 * Returns pointer to element of a UArray2b
 *
 * Arguments:
 *      array2b: The UArray2b
 *      i: Column coordinate of element
 *      j: Row coordinate of element
 *
 * Notes: If attempting to index out of bounds, program will terminate.
 */
void *UArray2b_at(T array2b, int i, int j)
{        
        assert(array2b != NULL);
        UArray_T block;
        int blocksize = array2b -> blocksize;
        
        /* Check if in bounds */
        assert(i >= 0 && i < array2b -> width
               && j >= 0 && j < array2b -> height);
        
        block = *(UArray_T*)(UArray2_at(array2b -> array, 
                                        i / blocksize, j / blocksize));
               
        return (UArray_at(block, 
                          blocksize * (i % blocksize) + (j % blocksize)));
}


/*
 * Helper function to UArray2b_map. Calls apply function on all elements in a 
 * block in row-major order.
 *
 * Arguments:
 *      arr2b: The UArray2b.
 *      i: Column coordinate of an element
 *      j: Row coordinate of an element
 *      apply: the apply function to be called on each element 
 *             (see UArray2b_map for parameters)
 *      *cl: function closure
 */

void map_block(UArray2b_T arr2b, int i, int j, 
               void apply(int i, int j, T array2b, void *elem, void *cl),
               void *cl)
{
        assert(arr2b != NULL);
        int blocksize = arr2b -> blocksize;
        UArray_T block = *(UArray_T *)UArray2_at(arr2b -> array, i/blocksize, 
                                                 j/blocksize);
        int x;
        int y;
        void *elem;
        for (y = 0; y < blocksize; y++) {
                for (x = 0; x < blocksize; x++) {
                        /* check arr2b coordinates not out of bounds */
                        if (x + i >= arr2b -> width || y + j >= arr2b -> height)
                                continue;
                        elem = UArray_at(block, blocksize * 
                                        ((x + i) % blocksize) 
                                        + ((y + j) % blocksize));
                        apply(x + i, y + j, arr2b, elem, cl);
                }
        }
                        
}

/*
 * Calls apply function on all elements in UArray2 in block-major order.
 *
 * Arguments:
 *      arr: The UArray2b
 *      apply: Apply function with following parameters
 *              i: Column coordinate of an element
 *              j: Row coordinate of an element
 *              array2b: The UArray2b
 *              *val: Pointer to the value at the passed coordinates
 *              *cl: Function closure
 *      *cl: Function closure
 */
void UArray2b_map(T array2b, void apply(int i, int j, T array2b, 
                                        void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);
        int col;
        int row;
        int blocksize = array2b -> blocksize;
        int height = array2b -> height;
        int width = array2b -> width;
        
        for (row = 0; row < height; row += blocksize) {
                for (col = 0; col < width; col += blocksize) {
                        map_block(array2b, col, row, apply, cl);
                }
        }
        
}

/*
 * Frees the memory associated with the UArray2b
 *
 * Arguments:
 *      array2b: The UArray2b
 */
void UArray2b_free (T *array2b)
{
        assert(array2b != NULL && *array2b != NULL);
        int col;
        int row; 
        int blocksize = (*array2b) -> blocksize;
        int height = (*array2b) -> height;
        int width = (*array2b) -> width;
        
        for (row = 0; row < height; row += blocksize) {
                for (col = 0; col < width; col += blocksize) {
                        /* NEEDSWORK : find way to clean up */
                        UArray_T arrayp = *(UArray_T*)UArray2_at((*array2b) -> 
                                                                 array, 
                                                                 col/blocksize, 
                                                                 row/blocksize);
                        UArray_free(&arrayp);
                }
        } 
        UArray2_free(&(*array2b) -> array);
        
        free(*array2b);
}

#undef T