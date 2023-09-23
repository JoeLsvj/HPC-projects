### MKL libraries
###
###
MKL= -L${MKLROOT}/lib/intel64  -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl

### OpenBLAS libraries 
OPENBLASROOT=${OPENBLAS_ROOT}
### BLIS library
BLISROOT=/u/dssc/slippo00/PROJECT_sl_and_gv/Foundations_of_HPC_2022/Assignment/exercise2/blis


cpu: sgemm_mkl.x sgemm_oblas.x sgemm_blis.x dgemm_mkl.x dgemm_oblas.x dgemm_blis.x sgemm_mkl_opt.x dgemm_mkl_opt.x sgemm_oblas_opt.x dgemm_oblas_opt.x sgemm_blis_opt.x dgemm_blis_opt.x




sgemm_mkl.x: gemm.c
	gcc -DUSE_FLOAT -DMKL $^ -m64 -I${MKLROOT}/include $(MKL)  -o $@

sgemm_oblas.x: gemm.c
	gcc -DUSE_FLOAT -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp

sgemm_blis.x: gemm.c
	gcc -DUSE_FLOAT -DBLIS $^ -m64 -I${BLISROOT}/include/skx -L/${BLISROOT}/lib/skx -o $@ -lpthread  -lblis -fopenmp -lm

dgemm_mkl.x: gemm.c
	gcc -DUSE_DOUBLE -DMKL $^ -m64 -I${MKLROOT}/include $(MKL)  -o $@

dgemm_oblas.x: gemm.c
	gcc -DUSE_DOUBLE -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp

dgemm_blis.x: gemm.c
	gcc -DUSE_DOUBLE -DBLIS $^ -m64 -I${BLISROOT}/include/skx -L/${BLISROOT}/lib/skx -o $@ -lpthread  -lblis -fopenmp -lm

sgemm_mkl_opt.x: gemm.c
	gcc -O3 -march=native -DUSE_FLOAT -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) -o $@

dgemm_mkl_opt.x: gemm.c
	gcc -O3 -march=native -DUSE_DOUBLE -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) -o $@

sgemm_oblas_opt.x: gemm.c
	gcc -O3 -march=native -DUSE_FLOAT -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp

dgemm_oblas_opt.x: gemm.c
	gcc -O3 -march=native -DUSE_DOUBLE -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp

sgemm_blis_opt.x: gemm.c
	gcc -O3 -march=native -DUSE_FLOAT -DBLIS $^ -m64 -I${BLISROOT}/include/blis -L/${BLISROOT}/lib -o $@ -lpthread -lblis -fopenmp -lm

dgemm_blis_opt.x: gemm.c
	gcc -O3 -march=native -DUSE_DOUBLE -DBLIS $^ -m64 -I${BLISROOT}/include/blis -L/${BLISROOT}/lib -o $@ -lpthread -lblis -fopenmp -lm

clean:
	rm -rf *.x
