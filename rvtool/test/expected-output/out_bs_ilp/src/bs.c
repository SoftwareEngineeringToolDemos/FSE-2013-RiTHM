#include "rithmic_globals.h"
#include "GOOMFInstrumentor.h"
/*************************************************************************/
/*                                                                       */
/*   SNU-RT Benchmark Suite for Worst Case Timing Analysis               */
/*   =====================================================               */
/*                              Collected and Modified by S.-S. Lim      */
/*                                           sslim@archi.snu.ac.kr       */
/*                                         Real-Time Research Group      */
/*                                        Seoul National University      */
/*                                                                       */
/*                                                                       */
/*        < Features > - restrictions for our experimental environment   */
/*                                                                       */
/*          1. Completely structured.                                    */
/*               - There are no unconditional jumps.                     */
/*               - There are no exit from loop bodies.                   */
/*                 (There are no 'break' or 'return' in loop bodies)     */
/*          2. No 'switch' statements.                                   */
/*          3. No 'do..while' statements.                                */
/*          4. Expressions are restricted.                               */
/*               - There are no multiple expressions joined by 'or',     */
/*                'and' operations.                                      */
/*          5. No library calls.                                         */
/*               - All the functions needed are implemented in the       */
/*                 source file.                                          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  FILE: bs.c                                                           */
/*  SOURCE : Public Domain Code                                          */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Binary search for the array of 15 integer elements.               */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/


int binary_search(int x);


struct DATA {
  int  key;
  int  value;
}  ;


	int cnt1;


struct DATA data[15] = { {1, 100},
	     {5,200},
	     {6, 300},
	     {7, 700},
	     {8, 900},
	     {9, 250},
	     {10, 400},
	     {11, 600},
	     {12, 800},
	     {13, 1500},
	     {14, 1200},
	     {15, 110},
	     {16, 140},
	     {17, 133},
	     {18, 10} };

void program_main(void)
{
	binary_search(8);
}

int binary_search(int x)
{
  int fvalue, mid;


  binary_search_low = 0; _GOOMF_nextEvent(context,0,(void*)(&binary_search_low));
  binary_search_up = 14;
  fvalue = -1 /* all data are positive */ ;
  while (binary_search_low <= binary_search_up) {
    mid = (binary_search_low + binary_search_up) >> 1;
	  usleep(1000);	
	  if ( data[mid].key == x ) {  /*  found  */
      binary_search_up = binary_search_low - 1;
      fvalue = data[mid].value;

	printf("FOUND!!\n");

    }
    else  /* not found */
      if ( data[mid].key > x ) 	{
	binary_search_up = mid - 1;

	printf("MID-1\n");

      }
      else   {
             	binary_search_low = mid + 1;

	printf("MID+1\n");

      }

	cnt1++;

  }

	printf("Loop Count : %d\n", cnt1);

  return fvalue;
}
