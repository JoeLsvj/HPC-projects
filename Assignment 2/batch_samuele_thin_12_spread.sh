#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="sam_job"
#SBATCH --partition=THIN
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks-per-node=12
#SBATCH --mem=490G
#SBATCH --time=02:00:00

# Load necessary modules (e.g., for MKL, BLIS, or other dependencies)
#module load architecture/AMD           # EPYC case
#module load architecture/Intel                 # THIN case
module load mkl/latest
module load openBLAS/0.3.23-omp

# Policy
export OMP_PLACES=cores
export OMP_PROC_BIND=spread
export OMP_NUM_THREADS=12
export BLIS_NUM_THREADS=12
export LD_LIBRARY_PATH=/u/dssc/slippo00/PROJECT_sl_and_gv/Foundations_of_HPC_2022/Assignment/exercise2/blis/lib/skx:$LD_LIBRARY_PATH

# Parameter
current_type=THIN
current_nodes_number=12
current_policy=spread

# For each file
# I can add or remove in the for: sgemm_blis.x dgemm_blis.x
for FILE in sgemm_mkl.x dgemm_mkl.x sgemm_oblas.x dgemm_oblas.x sgemm_blis.x dgemm_blis.x sgemm_mkl_opt.x dgemm_mkl_opt.x sgemm_oblas_opt.x dgemm_oblas_opt.x sgemm_blis_opt.x dgemm_blis_opt.x; do

        # Define your matrix multiplication code's executable path
        matrix_multiply_executable="./${FILE}"

        # Create a directory for storing the CSV results
        results_dir="csv_results_${current_type}"
        mkdir -p "$results_dir"

        # Define the CSV file path to store the summary results
        summary_csv="${results_dir}/${matrix_multiply_executable}_${current_type}_${current_nodes_number}_${current_policy}_summary.csv"

        # Define the CSV file path to store the detailed results
        detailed_csv="${results_dir}/${matrix_multiply_executable}_${current_type}_${current_nodes_number}_${current_policy}_detailed.csv"

        # Define the number of iterations for each size
        num_iterations=5

        # Initialize the summary CSV file with header
        echo "Matrix Size,Elapsed Time (s),GFLOPS" > "$summary_csv"

        # Initialize the detailed CSV file with header
        echo "Matrix Size,Iteration,Elapsed Time (s),GFLOPS" > "$detailed_csv"

        # Loop over matrix sizes
        for size in {2000..20000..2000}; do
            total_time=0
            total_gflops=0

            for ((iteration = 1; iteration <= num_iterations; iteration++)); do
                # Run your matrix multiplication code for the current size
                output=$($matrix_multiply_executable $size $size $size)

                # Extract time and GFLOPS from the output using grep and awk
                # Extract time and GFLOPS from the output using grep and awk
                time=$(echo "$output" | grep -oP 'Elapsed time \K[0-9.]+')
                gflops=$(echo "$output" | grep -oP '[0-9.]+ GFLOPS' | awk '{print $1}')


                total_time=$(echo "$total_time + $time" | bc -l)
                total_gflops=$(echo "$total_gflops + $gflops" | bc -l)

                # Append the detailed results to the detailed CSV file
                echo "$size,$iteration,$time,$gflops" >> "$detailed_csv"
            done

            # Calculate the average time and GFLOPS
            avg_time=$(echo "scale=5; $total_time / $num_iterations" | bc)
            avg_gflops=$(echo "scale=5; $total_gflops / $num_iterations" | bc)

            # Append the summary results to the summary CSV file
            echo "$size,$avg_time,$avg_gflops" >> "$summary_csv"
        done
done
