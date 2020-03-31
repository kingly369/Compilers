/* fib.c */

int printf(char *s, ...), scanf(char *s, ...);

/*
 * return the nth fibonacci number
 */

int main(void)
{
    int n;
	n = 5;
	while(n>0)
	{	
    	printf("%d\n", n);
		n--;
	}

	for(n = 0; n < 10; n++)
	{
		printf("%d\n", n);
		break;
	}
}
