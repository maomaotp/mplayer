#include <stdio.h>

int main()
{
	int a, b, c;
	sscanf("2014:3:12", "%d:%d:%d", a, b, c);
	printf("%d>>>%d>>>%d\n", a, b, c);

	return 0;
}
