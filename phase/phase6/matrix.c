/* matrix.c */

int *malloc(int n), free(int *p);
int printf(char *s, ...), scanf(char *s, ...);

int **allocate(int n)
{
    int i;
    int **a;
	printf("%d\n", n);
    i = 0;
    a = (int **) malloc(n * sizeof(int *));
    while (i < n) {
	a[i] = malloc(n * sizeof(int));
	i = i + 1;
	/*printf("%d\n", n);*/
    }

    return a;
}

int initialize(int **a, int n)
{
    int i, j;


    i = 0;

    while (i < n) {
	j = 0;

	while (j < n) {
		/*printf("%d ", i);
		printf("%d\n", j);*/
	    a[i][j] = i + j;
	    /**(*(a + i) + j) = i + j;*/
	    j = j + 1;
	}

	i = i + 1;
    }
}

int display(int **a, int n)
{
    int i, j;
    int *p;

    i = 0;

    while (i < n) {
	j = 0;

	while (j < n) {
	    p = a[i];
		/*printf("%d ", j);*/
	    printf("%d ", p[j]);
	    j = j + 1;
	}

	i = i + 1;
	printf("\n");
    }
}

int deallocate(int **a, int n)
{
    int i;

    i = 0;

    while (i < n) {
	free(a[i]);
	i = i + 1;
    }

    free((int *) a);
}

int main(void)
{
    int **a;
    int n;

    scanf("%d", &n);
    printf("%d\n", n);
	a = allocate(n);
    initialize(a, n);
    display(a, n);
    deallocate(a, n);
}
