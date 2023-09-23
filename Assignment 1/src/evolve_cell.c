/*
This function computes the state of a single cell (of index i) in the next generation of the playground.
The function returns 0 if the cell will die, or maxVal (is the value requested by the function write_pbm) 
in case the cell will live.
*/
unsigned char evolve_cell(unsigned const int k,  unsigned const long i, unsigned const char *world, unsigned const int *maxVal)
{
    int new_state = 0;
    register unsigned const int row_index = i/k;
    register unsigned const int col_index = i%k;
    /*
    In the code implementation the playground (that is an infinite 2D square grid) is implemented using 
    an array of one dimension to let the data be contiguous in memory, with periodic boundary conditions.
    We have to take into account this choice here, in correclty selecting the neighbours of the cell i even 
    when the cell i is on the boundary of the grid.
    */
    new_state  = world[(k+row_index-1)%k*k + (k+col_index-1)%k]     // top left with periodic boundary conditions
            + world[(k+row_index+0)%k*k + (k+col_index-1)%k]        // top middle with periodic boundary conditions
            + world[(k+row_index+1)%k*k + (k+col_index-1)%k]        // top right with periodic boundary conditions
            + world[(k+row_index-1)%k*k + (k+col_index+0)%k]        // middle left with periodic boundary conditions
            + world[(k+row_index+1)%k*k + (k+col_index+0)%k]        // middle right with periodic boundary conditions
            + world[(k+row_index-1)%k*k + (k+col_index+1)%k]        // bottom left with periodic boundary conditions
            + world[(k+row_index+0)%k*k + (k+col_index+1)%k]        // bottom middle with periodic boundary conditions
            + world[(k+row_index+1)%k*k + (k+col_index+1)%k];       // bottom right with periodic boundary conditions
    new_state /= (*maxVal);
    // the cell will live if there are 2 or 3 live neighbours:
    new_state = ((new_state == 2) || (new_state == 3)) ? *maxVal : 0;
    return (unsigned char)new_state;
}