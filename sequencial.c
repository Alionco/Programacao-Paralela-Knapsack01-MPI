#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

double tempo;

/*  Retorna tempo em milisegundos

    Forma de uso:
 
    double tempo;
    tempo = timestamp();
    <trecho de programa do qual se deseja medir tempo>
    tempo = timestamp() - tempo;
*/

double timestamp(void) {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return((double)(tp.tv_sec*1000.0 + tp.tv_usec/1000.0));
}

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

    // V Stores, for each (1 + i, j), the best profit for a knapscak
    // of capacity `j` considering every item k such that (0 <= k < i)
    int i, j;

    tempo = timestamp();

    // evaluate item `i`
    for(i = 0; i < n; i++) {
        for(j = 0; j <= MAXIMUM_CAPACITY; j++) {
            if(wt[i] <= j) { // could put item in knapsack
                int previous_value = V[1 + i - 1][j];
                int replace_items = val[i] + V[1 + i - 1][j - wt[i]];

                // is it better to keep what we already got,
                // or is it better to swap whatever we have in the bag that weights up to `j`
                // and put item `i`?
                V[1 + i][j]= max(previous_value, replace_items);
            }
            else {
                // can't put item `i`
                V[1 + i][j] = V[1 + i - 1][j];
			}
        }
    }

    tempo = timestamp() - tempo;

    int retval = V[1 + n - 1][MAXIMUM_CAPACITY]; 
    
    free_matrix(V);
    
    return retval;
}

// Driver program to test above function
int main()
{
	int n, W;

	scanf("%d %d", &n, &W);
	int *val = (int*) calloc(n, sizeof(int));
	int *wt = (int*) calloc(n, sizeof(int));

	int i;
	for (i = 0; i < n; ++i) {
		scanf("%d %d", &(val[i]), &(wt[i])); 
	}

    printf("%d\n", knapsack(W, wt, val, n));
    printf("time: %f ms\n", tempo / 1000);
    return 0;
}