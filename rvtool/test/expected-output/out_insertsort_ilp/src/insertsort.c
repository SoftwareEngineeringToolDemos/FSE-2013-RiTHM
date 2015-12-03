#include "GOOMFInstrumentor.h"
#include "rithmic_globals.h"
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
/*  FILE: insertsort.c                                                   */
/*  SOURCE : Public Domain Code                                          */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Insertion sort for 10 integer numbers.                            */
/*     The integer array a[] is initialized in main function.            */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/




int cnt1, cnt2;


void program_main()
{
  int temp;

  int a[11];

  a[0] = 0;   /* assume all data is positive */
  a[1] = 11; a[2]=10;a[3]=9; a[4]=8; a[5]=7; a[6]=6; a[7]=5;
  a[8] =4; a[9]=3; a[10]=2;
  main_i = 2; _GOOMF_nextEvent(context,0,(void*)(&main_i));
  while(main_i <= 10){

      cnt1++;

	  usleep(1000);
      main_j = main_i;

	cnt2=0;

      while (a[main_j] < a[main_j-1]) 
      {

	cnt2++;

	temp = a[main_j];
	a[main_j] = a[main_j-1];
	a[main_j-1] = temp;
	main_j--;
      }

	printf("Inner Loop Counts: %d\n", cnt2);

      main_i++; _GOOMF_nextEvent(context,0,(void*)(&main_i));
    }

    printf("Outer Loop : %d ,  Inner Loop : %d\n", cnt1, cnt2);

}

	
