#include <stdio.h>

int calc(int n)
{
	int i = 2;
	int Fnew, Fold, temp;

	while( i <= n ) 
	{
		temp = Fnew;
		Fnew = Fnew + Fold;
      		Fold = temp;
      		i++;
    	}

	return Fnew;
}

int main()
{
	int ans, n = 30;

	ans = calc(n);
  	
	printf("%d\n", ans);

	return 0;
}
