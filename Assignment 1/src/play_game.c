#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>
#include<mpi.h>

#include"play_game.h"
#include"ordered_evolution.h"
#include"static_evolution.h"
#include"read_write_pgm_image.h" 


// structure for time measurements
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
    The following function checks the type of evolution selected by the user (-e flag)
    and then calls the correct function to play the game of life on the playground.
    The input arguments are the following:
    filename: name of the file containing the initial configuration of the playground
    k: size of the squre matrix that rapresents the playground
    n: number of generations for which the game of life will run
    s: frequency for saving a snapshot of the playground
    e: flag for the evolution mode: 0 -> ordered evolution, 1 -> static evolution
*/
void play_game(const char *filename, unsigned const int k, unsigned const int n, unsigned const int s, const char e)
{
	/*
        initalize MPI as suggested in the assignment description (in multiple mode)
    */
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
    if (e==0)   // ordered evolution
    {	
		double time_start = CPU_TIME;
        /*
            finalize MPI because the ordered evolution is a serial task by definition,
            from the point of view of MPI
        */
		MPI_Finalize();
        ordered_evolution(filename, k, n, s);
        double time_end = CPU_TIME;
        printf( ", %f\n", time_end-time_start);

    }
    else        // e==1: static evolution
    {
        double time_start = CPU_TIME;
        static_evolution(filename, k, n, s, rank, size);
        double time_end = CPU_TIME;
        if (rank == 0)
        printf( ", %f\n", time_end-time_start);

        MPI_Finalize();
    }
    
    return;
}

