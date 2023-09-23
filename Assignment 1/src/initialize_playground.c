#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#include<mpi.h>

#include "initialize_playground.h"
#include "read_write_pgm_image.h"


// structure for the time measurements:
#if defined(_OPENMP)
    #define CPU_TIME ({struct  timespec ts; clock_gettime( CLOCK_REALTIME, &ts ),\
	    (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;})
    #define CPU_TIME_th ({struct  timespec myts; clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ),\
	    (double)myts.tv_sec + (double)myts.tv_nsec * 1e-9;})
#else
#define CPU_TIME ({struct  timespec ts; clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ),\
    (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;})
#endif


/*
    The following function initializes the playground, which is implemented with an array
    of one dimension, of size k*k. The decision to implement a 2D square grid with an array 
    of one dimension is done to let the data be more contiguous in memory. It is convenient,
    for memory management reasons, to divide the case in which the playground is initialized without MPI
    (so with only a single independent process) from the case in which MPI is used to initialize the 
    playground in parallel. The following function, though, uses two accessory functions. The input
    parameters are the following:
    filename: name of the file which will contain the initialized playground (pbm format)
    k: size of the squre matrix that's going to rapresent the playground
*/
void initialize_playground(const char *filename, unsigned const int k)
{

    int mpi_provided_thread_level;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &mpi_provided_thread_level);
    if ( mpi_provided_thread_level < MPI_THREAD_FUNNELED ) 
    {
        printf("Error: MPI thread support is lower than the demanded\n");
        MPI_Finalize();
        exit(1);
    }
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size == 1)
    {
        MPI_Finalize();
        double time_start = CPU_TIME;
        initialize(filename, k);
        double time_end = CPU_TIME;
        printf("time required to initialize the playground of size %d: %f\n", k, time_end-time_start);
        return; 
    }
    else // size > 1: parallel initialization
    {
        double time_start = CPU_TIME;
        initialize_MPI(filename, k, rank, size);
        double time_end = CPU_TIME;
        if (rank == 0) printf("time required to initialize the playground of size %d: %f\n", k, time_end-time_start);
        MPI_Finalize();
        return;
    }
}

/*
    This function is called only if MPI is not used (or if only one MPI process is used).
    The initialization is then serial in this case, from the point of view of MPI. openMP
    is used instead.
*/
void initialize(const char *filename, unsigned const int k)
{
    char *playground;
    playground = (char *)malloc(k*k*sizeof(char));
    int seed = time(NULL);
    srand(seed);
    #pragma omp parallel for schedule(static) shared(playground, k)
    for (unsigned long i = 0; i < k*k; i++) playground[i] = rand() % 100 < 15 ? 255 : 0;
    write_pgm_image(playground, 255, k, k, filename);
    free(playground);
    return; 
}

/*
    This function is called when the initialization is done in parallel, using multiple
    MPI processes
*/
void initialize_MPI(const char *filename, unsigned const int k, int rank, int size)
{
    int seed = time(NULL);
    // int seed = 10;
    srand(seed);
    /*
        Distribute the computational work among the MPI processes: each independent MPI process
        initializes only its chunck (or slice) of the global playground. In this case, chuncks
        are created with (k*k)/size for each MPI process, and the master process (rank=0) gets
        also the remainder values, in case the division (k*k)/size is not exact equal to an integer value.
        The master process, so, has more computational work to do, but this amount is usually negligible,
        since k>>size in most cases.
    */
    
    unsigned char *playground;
    
    //unsigned int chunk;
    //chunk = (rank != 0) ? (k*k)/size : (k*k)/size + (k*k)%size;
    //unsigned char *local_playground = (char *)malloc(chunk*sizeof(char));
    
    
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
    
    
    /*
        Each MPI process spawns a certain number of openMP threads to initialize in parallel 
        its own chunck of the playground.
    */
    #pragma omp parallel for schedule(static)
    for (unsigned long i = 0; i < lengths[rank]; i++) local_playground[i] = rand() % 100 < 15 ? 255 : 0;
    /*
        The master process (with rank = 0) allocates (dynamically on the heap) the array containing
        the whole playground, then, collects all the partial results (local_playground) from the other MPI processes 
        and stores them in the playground variable. Finally, writes it to a file in the pbm format.
    */
    if (rank == 0) playground = (unsigned char *)malloc(k*k*sizeof(unsigned char));
    MPI_Barrier(MPI_COMM_WORLD);
    //MPI_Gather(local_playground, chunk, MPI_CHAR, playground, chunk, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Gatherv((void *)local_playground, lengths[rank], MPI_UNSIGNED_CHAR, (void *)playground, (int *)lengths, (int *)displacements, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    if (rank == 0) write_pgm_image(playground, 255, k, k, filename);
    free(local_playground);
    if (rank == 0) free(playground);
    return;
}
