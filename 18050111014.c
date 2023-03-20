#include <hellomake.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // call a function in another file
    myPrintHelloMake();

    int rows, cols, i, j;
    double **matrix;

    printf("Enter number of rows and columns e.g(5 6): ");
    scanf("%d %d", &rows, &cols);

    matrix = (double **)calloc(rows, sizeof(double *));
    for (i = 0; i < rows; i++) {
        matrix[i] = (double *)calloc(cols, sizeof(double));
    }

    srand((unsigned int) 18050111014L);

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            matrix[i][j] = (double)(rand() % 100 + 1) + ((double)(rand() % 1000) / 10.0);
        }
    }

    char filename[100];
    FILE *fp;

    printf("Enter the name of the file e.g(output.txt): ");
    scanf("%s", filename);

    fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("Error: Unable to create file!");
        return -1;
    }

    // Write the matrix to the file
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            fprintf(fp, "%10.2f ", matrix[i][j]);
        }
        fprintf(fp, "\n");
    }

    printf("File created successfully!\n");

    fclose(fp);

    // Free the memory allocated for the matrix
    for (i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
