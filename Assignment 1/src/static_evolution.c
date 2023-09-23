#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>
#include<mpi.h>

#include"static_evolution.h"
#include"evolve_cell.h"
#include"read_write_pgm_image.h"


// needed because the read_pbn function requires a pointer to an integer
unsigned int smval;                   
unsigned int *smaxVal = &smval;


/*
    The following function performs the static evolution of the playground and saves 
    the final state of the playground (and eventually some intermediate snapshot)
    to a pgm file. The static evolution means that the evolution of the playground
    is done "freezing" the playground in the current state and and computing for each 
    cell if it should be alive or dead, updating the state of the playground at the 
    end of the computation. It is convenient to divide the case in which there is only one
    MPI process (in this case the evolution is serial of course) from the case in which 
    there are multiple independent MPI processes performing the static evolution in parallel.
    For this purpose, the following function calls two functions, in which the actual
    computation is done. The input arguments are the following:
    filename: name of the file containing the initial state of the playground
    k: size of the squre matrix that rapresents the playground
    n: number of generations for which the game of life will run
    s: snapshot saving frequency
    rank: rank of the process
    size: number of MPI processes
*/
void static_evolution(const char *filename, unsigned int k, unsigned const int n, unsigned int s, int rank, int size)
{
    if (size > 1) 
    {   
        parallel_static(filename, k, n, s, rank, size);
        return;
    
    } else // size == 1
    {
        serial_static(filename, k, n, s);
        return;
    }
}


/*
    This function is called only if the program is executed on a single process, 
    and performs the static evolution is serial, saving the final configuration into a pgm file.
    The threadization with openMP is done within the single MPI process used.
*/
void serial_static(const char *filename, unsigned int k, unsigned const int n, unsigned int s)
{
    /*
        read the initial state of the playground from the file,
        and initialize the variable `playground`
    */
    unsigned char *playground; 
    unsigned char *new_playground;
    playground = malloc(k*k*sizeof(char));
    read_pgm_image((void**)&playground, smaxVal, &k, &k, filename);
    
    for (unsigned int step = 0; step < n; step++)
    {
        new_playground = malloc(k*k*sizeof(char));
        /*
            compute the static evolution of the playground and decide if a cell will live or die,
            saving the evolved states in new_playground
        */
        #pragma omp parallel for schedule(static) shared(new_playground, playground, k, smaxVal)
        for (unsigned long i = 0; i < k*k; i++) new_playground[i] = evolve_cell(k, i, playground, smaxVal);
        unsigned char *tmp = playground;
        playground = new_playground;
        new_playground = tmp;
        free(new_playground);
        /*
            check if it's time to save a snapshot of the playground
        */
        if (s==0)
            continue;
        if (step%s == 0)
        {
            char *snapname = malloc(50*sizeof(char));
            sprintf(snapname, "snapshots/snapshot_%06d.pgm", step);
            write_pgm_image((void*)playground, smval, k, k, snapname);
            free(snapname);
        }
        
    }

    char *filename_final = malloc (21*sizeof(char));
    sprintf(filename_final, "snapshot_final.pgm");
    write_pgm_image((void*)playground, smval, k, k, filename_final);
    free(filename_final);
    free(playground);
    return;
}


/*
    This function is called only if the program is executed on more than one process,
    and performs the static evolution of the playground in parallel with multiple MPI 
    processes. Each independent MPI process then spawns a certain number of openMP threads,
    in order to augment the level of parallelization.
    The final configuration of the playground is saved into a pgm file.
*/
void parallel_static(const char *filename, unsigned int k, unsigned const int n, unsigned int s, int rank, int size)
{
    /*
        Every process reads the initial configuration of the playground from the file autonomously
        and stores it in the variable `playground`. In this way all the independent MPI processes have a copy
        of the whole playground of the game, and there is no need of MPI communications among them to exchange
        the boundary values, needed for the computation of the next state of a cell of the boundary of the local_playground.
        This choice has the advantage to reduce the MPI communications among the processes (this can lead to 
        less computational time) but is more expensive for the point of view of the memory. In fact, each process has to 
        allocate memory for storing the state of cells that it is not going to use. This can be a problem if the 
        dimension of the grid is very large.
    */
    unsigned char *playground;
    playground = malloc(k*k*sizeof(unsigned char));
    read_pgm_image((void **)&playground, smaxVal, &k, &k, filename);                            

    /* 
        Some needed variables to distribute the computational work among the processes
        lenghts: array containing the dimension of the playground of each independent process
        displacements: displacements of the local playground w.r.t. the global playground, for each process
        This variables are needed for the MPI function MPI_Allgatherv().
    */
    unsigned int *displacements = malloc(size*sizeof(unsigned long));
    unsigned int *lengths = malloc(size*sizeof(unsigned int));
    for (unsigned long i = 0; i < size; i++)
    {
        lengths[i] = k*k/size;
        if (lengths[i]*size < k*k && i < k*k-lengths[i]*size)
            lengths[i]++;
        displacements[i] = i == 0 ? 0 : displacements[i-1]+lengths[i-1]; 
    }

    unsigned char *local_playground = malloc(lengths[rank]*sizeof(unsigned char));

    for (unsigned int step = 0; step < n; step++)
    {

        MPI_Barrier(MPI_COMM_WORLD);
        /*
            Each process computes the evolution of its local fragment of the playground. 
            Then every process sends its result to all the others. openMP threadization is used
            to augment the parallelization effect
        */
        #pragma omp parallel for schedule(static) shared(local_playground, playground, displacements, lengths, k, rank, smaxVal)
        for (unsigned long i = 0; i < lengths[rank]; i++) 
            local_playground[i] = evolve_cell(k, i+displacements[rank], playground, smaxVal);
        /*
            The partial results of all processes (stored in the variable local_playground) 
            are gathered and saved in the variable playground for all the processes. In this way 
            all the MPI processes have a full updated version of the whole playground. 
        */
        MPI_Allgatherv((void *)local_playground, lengths[rank], MPI_UNSIGNED_CHAR, (void *)playground, (int *)lengths, (int *)displacements, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
        /*
            check if it's time to save a snapshot of the playground and do it if it's needed
        */
        if (rank == 0)
        {
            if ((s!=0) && ( step%s == 0))
            {
                char *snapname = malloc(31*sizeof(char));
                sprintf(snapname, "snapshots/snapshot_%06d.pgm", step);
                write_pgm_image((void*)playground, smval, k, k, snapname);
                free(snapname);
            }
        }
    }
    /*
        The master process saves the final state of the playground into a file
    */
    if (rank == 0)
    {
        char *filename_final = malloc (20*sizeof(char));
        sprintf(filename_final, "snapshot_final.pgm");
        write_pgm_image((void*)playground, smval, k, k, filename_final);
        free(filename_final);
    }
    free(playground);
    free(local_playground);
    free(displacements);
    free(lengths);
    return; 
}
