#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#include<mpi.h>

#include"ordered_evolution.h"
#include"evolve_cell.h"
#include"read_write_pgm_image.h"


// required for the function that reads the pbm images
unsigned int omval;                   
unsigned int *omaxVal = &omval;


/*
    The following function performs the ordered evolution of the playground
    and saves the final configuration into a pbm file. In the ordered evolution of the playground 
    the next state  of a cell is computed and updated before moving to the next cell. 
    The ordered evolution can be performed only in serial from the point of view of MPI. 
    The input arguments are the following:
    filename: name of the file containing the initial configuration of the playground
    k: size of the squre matrix which rapresents the playground
    n: number of generations for which the game of life will run
    s: snapshot saving frequency
    rank: rank of the process
    size: number of processes
*/
void ordered_evolution(const char *filename, unsigned int k, unsigned const int n, unsigned const int s) 
{
    /*
        read the initial configuration of the playground from the pbm file
    */
    unsigned char *playground; 
    
    playground = malloc(k*k*sizeof(char));
    read_pgm_image((void**)&playground, omaxVal, &k, &k, filename);
    for(unsigned int step = 0; step < n; step++)
    {
        /*
            compute the evolution of the playground and decide if a cell will live or die
        */
        //#pragma omp parallel for schedule(static) shared(playground)
        for (unsigned long i = 0; i < k*k; i++) playground[i] = evolve_cell(k, i, playground, omaxVal);
        /*
            check if it's time to save a snapshot of the playground
        */
        if (s==0)
            continue;
        if (step%s == 0)
        {
            
            char *snapshot = malloc(50*sizeof(char));   // 50 should be enough
            sprintf(snapshot, "snapshots/snapshot_%06d.pgm", step); 
            write_pgm_image((void*)playground, omval, k, k, snapshot);
            free(snapshot);
        }
    }

    char *filename_final = malloc (20*sizeof(char));
    sprintf(filename_final, "snapshot_final.pgm");
    write_pgm_image((void*)playground, omval, k, k, filename_final);
    free(filename_final);
    free(playground);
    return;
}

