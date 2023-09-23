#!/bin/bash
#SBATCH --job-name="MPI-weak"
#SBATCH --get-user-env
#SBATCH --partition=THIN
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00

##module purge
module load openMPI/4.1.4/gnu/12.2.1

##make clean
##make

export OMP_PLACES=cores
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=16

n=10
e=1
resultdir=DATA

sizes=(10000 10000 14142 17321 20000 22361 24496 26458 28284 30000)

echo size, cores, time > $resultdir/MPI-weak-static-16T.csv

for i in {1..9}
do
    mpirun -np 1 ./main.x -i ${sizes[i]}
    for j in {1..5}
    do
        echo -n ${sizes[i]},$i >> $resultdir/MPI-weak-static-16T.csv
        mpirun -np $i --map-by core ./main.x -r -n $n -e $e >> $resultdir/MPI-weak-static-16T.csv
    done
done

##make clean
##module purge
