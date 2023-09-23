#!/bin/bash
#SBATCH -J "MPI-strong"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=2
#SBATCH --exclusive
#SBATCH --time=02:00:00

##module purge
module load openMPI/4.1.5/gnu/12.2.1

##make clean
##make

export OMP_PLACES=cores
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=1

resultdir=DATA

k=10000
n=10
p=EPYC
e=1

mpirun -n 2 ./main.x -i -k $k

echo size,cores,time > $resultdir/MPI_strong-ordered-size-$k-$n-$p.csv

for i in {1..256}
do
	echo -n $k, $i >> $resultdir/MPI_strong-ordered-size-$k-$n-$p.csv
	mpirun -np $i --map-by core ./main.x -r -n $n -s 0 -e $e >> $resultdir/MPI_strong-ordered-size-$k-$n-$p.csv
done

##make clean
##module purge

