#!/bin/bash
#SBATCH -J "MPI-strong"
#SBATCH --get-user-env
#SBATCH --partition=THIN
#SBATCH --nodes=2
#SBATCH --exclusive
#SBATCH --time=02:00:00

module purge
module load intelMPI/2021.7.1

make clean
make

export OMP_PLACES=cores
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=1

resultdir=DATA

k=25000
n=10
p=THIN
e=1

mpirun ./main.x -i -k $k

echo size,cores,time > $resultdir/MPI_strong-ordered-size-$k-$n-$p.csv

for i in {1..48}
do
	for j in {1..5}
	do
        echo -n $k, $i >> $resultdir/MPI_strong-ordered-size-$k-$n-$p.csv
        mpirun -np $i --map-by core ./main.x -r -n $n -s 0 -e $e >> $resultdir/MPI_strong-ordered-size-$k-$n-$p.csv
	done
done

##make clean
##module purge

