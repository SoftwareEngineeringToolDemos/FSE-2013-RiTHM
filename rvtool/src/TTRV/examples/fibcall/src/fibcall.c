

#ifdef DEBUG
	int cnt;
#endif 

fib(n)
     int  n;
{
  int  i, Fnew, Fold, temp,ans;

    Fnew = 1;  Fold = 0;
    i = 2;
    while( i <= n ) {
      temp = Fnew;
      Fnew = Fnew + Fold;
      Fold = temp;
      i++;
#ifdef DEBUG
	cnt++;
#endif
    }
    ans = Fnew;
#ifdef DEBUG
	printf("Loop Count : %d\n", cnt);
#endif
  return ans;
}
    
main()
{
  int a;

  a = 30;
  fib(a);
}



