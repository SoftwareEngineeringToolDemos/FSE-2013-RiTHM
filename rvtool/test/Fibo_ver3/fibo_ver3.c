#include <stdio.h>
#include <unistd.h>
   
int main()
{
	int  i, Fnew, Fold, temp,ans, n = 30;

	Fnew = 1;  Fold = 0;
	i = 2;

    	while( i <= n ) 
	{
      		temp = Fnew;
		Fnew = Fnew + Fold;
      		Fold = temp;
      		i++;
		usleep(1000);
    	}

    	ans = Fnew;
  	
	printf("%d\n", ans);

	return 0;
}
