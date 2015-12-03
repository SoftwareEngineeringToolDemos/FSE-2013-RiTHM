

#ifdef DEBUG
int cnt1, cnt2;
#endif

main()
{
  int  i,j, temp, a[11];

  a[0] = 0;   /* assume all data is positive */
  a[1] = 11; a[2]=10;a[3]=9; a[4]=8; a[5]=7; a[6]=6; a[7]=5;
  a[8] =4; a[9]=3; a[10]=2;
  i = 2;
  while(i <= 10){
#ifdef DEBUG
      cnt1++;
#endif
      j = i;
#ifdef DEBUG
	cnt2=0;
#endif
      while (a[j] < a[j-1]) 
      {
#ifdef DEBUG
	cnt2++;
#endif
	temp = a[j];
	a[j] = a[j-1];
	a[j-1] = temp;
	j--;
      }
#ifdef DEBUG
	printf("Inner Loop Counts: %d\n", cnt2);
#endif
      i++;
    }
#ifdef DEBUG
    printf("Outer Loop : %d ,  Inner Loop : %d\n", cnt1, cnt2);
#endif
}

	
