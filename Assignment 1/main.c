// include standard libraries
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h> 
#include<omp.h>
#include<mpi.h>

// include own headers
#include "initialize_playground.h"
#include "play_game.h"

// flag default variables
#define INITIALIZE 1                            // -i: initialize the playground with a size specified with the flag -k
#define RUN 2						            // -r : run the game for a loaded configuration
#define ORDERED 0                               // -e : select ordered or static evolution. 0 for ordered evolution,
#define STATIC 1                                //      1 for static evolution
#define K_DEFAULT 100                           // -k : the lenght of an edge of the playground, so the the total size is k*k
#define N_DEFAULT 10                            // -n : number of generations to run the game
#define S_DEFAULT 0                             // -s : frequency for saving a snapshot of the playgroud
#define F_DEFAULT "initial_configuration.pgm"   // -f : filename of the configuration to generate or to load



// variables for command line arguments (initialized with default values):
char action, e; 
unsigned int k = K_DEFAULT;
unsigned int n = N_DEFAULT;
unsigned int s = S_DEFAULT;
char *filename = NULL;

// main() function: parsing the arguments
int main(int argc, char *argv[])
{
    char* optstring = "irk:e:f:n:s:";    
    int c; 
    while ((c = getopt(argc, argv, optstring)) != -1)
    {
        switch (c)
        {
            case 'i':
                action = INITIALIZE;
                break;
            case 'r':
                action = RUN;
                break;
            case 'k':
                k = atoi(optarg);
                break;
            case 'e':
                e = atoi(optarg);
                break;
            case 'f':
                filename = (char*)malloc(50*sizeof(char)); // 50 should be enough
                sprintf(filename, "%s", optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 's':
                s = atoi(optarg);
                break;
            default :
                printf("argument -%c not known\n", c );
                break;  
        }
    }

    if (action != INITIALIZE && action != RUN)
    {
        printf("To run the program, specify the appropriate flag: \n");
        printf("-i (for generate the initial configuration) or -r (to load and run a configuration) \n");
        return 0;
    }
    if (filename == NULL)
    {
        filename = (char*)malloc( 50*sizeof(char) );     
        sprintf(filename, "%s", F_DEFAULT);
    }
    if (e != ORDERED && e != STATIC)
    {
        printf("Please specify the value of the flag -e: \n");
        printf("0 for the ordered evolution, 1 for the static evolution\n");
        return 0;
    }

    if (action == INITIALIZE)
            initialize_playground(filename, k);
    if (action == RUN)
            play_game(filename, k, n, s, e);

    free(filename);
    return 0;
}
