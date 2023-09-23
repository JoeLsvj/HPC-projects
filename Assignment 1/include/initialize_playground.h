#ifndef INITIALIZE_PLAYGROUND_H
#define INITIALIZE_PLAYGROUND_H
    void initialize_playground(const char *filename, unsigned const int k);
#endif

#ifndef INITIALIZE_H
#define INITIALIZE_H
    void initialize(const char *filename, unsigned const int k);
#endif

#ifndef INITIALIZE_MPI_H
#define INITIALIZE_MPI_H
    void initialize_MPI(const char *filename, unsigned const int k, int rank, int size);
#endif
