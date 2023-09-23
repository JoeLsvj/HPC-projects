#!/bin/bash
#SBATCH --job-name="OMP_scalability"
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=02:00:00

module purge
module load openMPI/4.1.5/gnu/12.2.1

make clean
make

export OMP_PLACES=cores
export OMP_PROC_BIND=close
export I_MPI_PIN_DOMAIN=socket

resultdir=DATA
k=10000
e=1
p=EPYC
n=10

mpirun -np 2 ./main.x -i -k $k

echo size, threads, time > $resultdir/OMP-ordered-size-$k-$n-$p-1socket.csv

for i in {1..64}
do
	export OMP_NUM_THREADS=$i
	for j in {1..5}
	do
		echo -n $k,$i >> $resultdir/OMP-ordered-size-$k-$n-$p-1socket.csv
		mpirun -np 1 --map-by node --bind-to socket ./main.x -r -n $n -e $e >> $resultdir/OMP-ordered-size-$k-$n-$p-1socket.csv
	done
done

make clean
module purge
