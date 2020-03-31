/* math.c */

int printf(char *s, ...);

int main(void)
{
    int a, b, c, x, y;
    double d, e, z;
	char k;
    x = 100;
    y = 30;
    z = 2.5;
	k = 1;

    a = x + y + z + k;
    b = x - y - z;
    c = x * y * z;
    d = x / y + z;
    e = x % y - z;

    printf("%d\n", a);
    printf("%d\n", b);
    printf("%d\n", c);
    printf("%f\n", d);
    printf("%f\n", e);
}
