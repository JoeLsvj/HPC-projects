# High Performance Computing 
This is a project about High Performance Computing. The implemented the Game Of Life game in parallel, we analysed and we studied its scalability. Moreover, we run many different benchmarks on ORFEO (a super computer which access is allowed by the Università degli studi di Trieste).

## The files
### The report
In the report pdf there is a deep explanation on all the work done. 

### 1. Game Of Life
We have implemented the 'Game of Files' in C. Several implementations have been carried out, including a serial version and various parallel versions. We incorporated different forms of parallelism by combining MPI and OpenMP. Lastly, we conducted several scalability studies.

### 2. Benchmarks on ORFEO
We run a matrix multiplication benchmark on ORFEO. We tested the scalability on the number of nodes (with the same size of the matrix) and on the size of the problem (incresing the number of cores) with different node kinds, libraries, precisions and policy of core assignation.  



