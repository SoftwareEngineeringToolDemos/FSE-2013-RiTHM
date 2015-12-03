#include "GOOMFInstrumentor.h"
#include "rithmic_globals.h"
#include <stdio.h>

   
int program_main()
{
	int Fnew, Fold, temp, ans;
	main_n = 30; _GOOMF_nextEvent(context,1,(void*)(&main_n));


	Fnew = 1;  Fold = 0;
	main_i = 2; _GOOMF_nextEvent(context,0,(void*)(&main_i));

    	while( main_i <= main_n ) 
	{
      		temp = Fnew;
		Fnew = Fnew + Fold;
      		Fold = temp;
      		main_i++;
		usleep(1000);
    	}

    	ans = Fnew;
  	
	printf("%d\n", ans);

	return 0;
}
