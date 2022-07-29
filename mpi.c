#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

// RESULTADO EM W MOD N_PROCS - 1

int my_rank, n_procs;
double time;

// A utility function that returns
// maximum of two integers
int max(int a, int b) {return (a > b) ? a : b;}


int** get_matrix(int rows, int columns) {   
    int **mat;
    int i;
    
    // for each line
    mat = (int**) calloc(rows, sizeof (int*));
    
    mat[0] = (int*) calloc(rows * columns, sizeof (int));

    // set up pointers to rows
    for (i = 1; i < rows; i++)
        mat[i] = mat[0] + i * columns;

    return mat;
}

void free_matrix(int** mat) {
    free(mat[0]);
    free(mat);
}


int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n)
{
    // Matrix-based solution
    int** V = get_matrix(n + 1, MAXIMUM_CAPACITY + 1);

    int *aux = (int*) calloc(MAXIMUM_CAPACITY+1, sizeof(int));

    // V Stores, for each (1 + i, j), the best profit for a knapscak
    // of capacity `j` considering every item k such that (0 <= k < i)
    int i, j;

    double start = MPI_Wtime();

    // evaluate item `i`
    for(i = 0; i < n; i++) {
        for(j = my_rank; j <= MAXIMUM_CAPACITY; j += n_procs) {
            if(wt[i] <= j) { // could put item in knapsack
                int previous_value = V[1 + i - 1][j];
                int replace_items = val[i] + V[1 + i - 1][j - wt[i]];

                // is it better to keep what we already got,
                // or is it better to swap whatever we have in the bag that weights up to `j`
                // and put item `i`?
                aux[j] = max(previous_value, replace_items);
            }
            else {
                // can't put item `i`
                aux[j] = V[1 + i - 1][j];
			}
  
        }

        MPI_Allreduce(aux, V[1 + i], MAXIMUM_CAPACITY+1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    }

    double end = MPI_Wtime();

    time = end - start;
 
    int retval = V[1 + n - 1][MAXIMUM_CAPACITY]; 
    
    free_matrix(V);
    free(aux);
    
    return retval;
}

// Driver program to test above function
int main(int argc, char **argv)
{
	int n, W;

    // inicialização do MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    // Leitura da entrada. Somente Rank = 0 recebe.
	scanf("%d %d", &n, &W);
	int *val = (int*) calloc(n, sizeof(int));
	int *wt = (int*) calloc(n, sizeof(int));

	int i;
	for (i = 0; i < n; ++i) {
		scanf("%d %d", &(val[i]), &(wt[i])); 
	}

    // Broadcast dos valores contidos em n, W, val e wt do rank = 0 para os demais ranks

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&W, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(val, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(wt, n, MPI_INT, 0, MPI_COMM_WORLD);

    int *resultado;
    *resultado = knapsack(W, wt, val, n);
    if(my_rank == 0) {
        printf("%d\n", *resultado);
        printf("time: %f ms\n", time);
    }
    

    // finalização do MPI
    MPI_Finalize();

    return 0;
}