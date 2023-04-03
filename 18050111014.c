#include <hellomake.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MATRIX_SIZE (18050111014 % 10000)
#define SMALL_MATRIX_SIZE (100 + 18050111014 % 100)

int main(int argc, char** argv) {
    int rank, num_procs;
    double *matrix, *small_matrix, *result, *local_matrix, *local_result;
    int i, j, k;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 4) {
        if (rank == 0) {
            printf("Usage: mpirun -n <num_procs> ./program_name <rows> <cols> <filename>\n");
        }
        MPI_Finalize();
        exit(1);
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    char *filename = argv[3];



    // Allocate memory for matrices and result vector
    if (rank == 0) {
        matrix = (double *) malloc(sizeof(double) * MATRIX_SIZE * MATRIX_SIZE);
        small_matrix = (double *) malloc(sizeof(double) * SMALL_MATRIX_SIZE * SMALL_MATRIX_SIZE);
        result = (double *) malloc(sizeof(double) * MATRIX_SIZE);
        
        // Initialize the matrices and result vector
        for (i = 0; i < MATRIX_SIZE; i++) {
            for (j = 0; j < MATRIX_SIZE; j++) {
                matrix[i * MATRIX_SIZE + j] = (double) (rand() % 100);
            }
        }
        
        for (i = 0; i < SMALL_MATRIX_SIZE; i++) {
            for (j = 0; j < SMALL_MATRIX_SIZE; j++) {
                small_matrix[i * SMALL_MATRIX_SIZE + j] = (double) (rand() % 100);
            }
        }
    }
    
    // Broadcast the small matrix to all processes
    MPI_Bcast(small_matrix, SMALL_MATRIX_SIZE * SMALL_MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calculate local matrix size
    int local_matrix_size = MATRIX_SIZE / num_procs;
    if (rank < MATRIX_SIZE % num_procs) {
        local_matrix_size++;
    }
    
    // Allocate memory for local matrix and local result vector
    local_matrix = (double *) malloc(sizeof(double) * local_matrix_size * MATRIX_SIZE);
    local_result = (double *) malloc(sizeof(double) * local_matrix_size);
    
    // Scatter the matrix to all processes
    int *sendcounts = (int *) malloc(sizeof(int) * num_procs);
    int *displs = (int *) malloc(sizeof(int) * num_procs);
    for (i = 0; i < num_procs; i++) {
        sendcounts[i] = MATRIX_SIZE / num_procs;
        if (i < MATRIX_SIZE % num_procs) {
            sendcounts[i]++;
        }
        displs[i] = i * (MATRIX_SIZE / num_procs) + (i < MATRIX_SIZE % num_procs ? i : MATRIX_SIZE % num_procs);
    }
    MPI_Scatterv(matrix, sendcounts, displs, MPI_DOUBLE, local_matrix, local_matrix_size * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Perform local matrix-vector multiplication
    start_time = MPI_Wtime();
    for (i = 0; i < local_matrix_size; i++) {
        local_result[i] = 0;
        for (j = 0; j < MATRIX_SIZE; j++) {
            local_result[i] += local_matrix[i * MATRIX_SIZE +  j] * small_matrix[j];
        }
    }
    end_time = MPI_Wtime();

    // Gather the local results from all processes to the root process
    MPI_Gatherv(local_result, local_matrix_size, MPI_DOUBLE, result, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Print the elapsed time on the root process
    if (rank == 0) {
        printf("Elapsed time is %lf seconds for parallel mxv with %d processes\n", end_time - start_time, num_procs);
    }


    double **matrix1;

    matrix1 = (double **)calloc(rows, sizeof(double *));
    for (int i = 0; i < rows; i++) {
        matrix1[i] = (double *)calloc(cols, sizeof(double));
    }

    srand((unsigned int) 18050111014L);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix1[i][j] = (double)(rand() % 100 + 1) + ((double)(rand() % 1000) / 10.0);
        }
    }

    FILE *fp;

    fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("Error: Unable to create file!");
        return -1;
    }

    // Write the matrix to the file
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%10.2f ", matrix1[i][j]);
        }
        fprintf(fp, "\n");
    }

    printf("File created successfully!\n");

    fclose(fp);

    // Free the memory allocated for the matrix
    for (int i = 0; i < rows; i++) {
        free(matrix1[i]);
    }
    free(matrix1);



    // Free memory
    free(local_matrix);
    free(local_result);
    if (rank == 0) {
        free(matrix);
        free(small_matrix);
        free(result);
    }
    MPI_Finalize();

    return 0;
}
