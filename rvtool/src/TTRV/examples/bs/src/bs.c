
int binary_search(int x);


struct DATA {
  int  key;
  int  value;
}  ;


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

void main(void)
{
	binary_search(8);
}


int binary_search(int x)
{
  int binary_search_fvalue, mid, binary_search_up, binary_search_low ;
  binary_search_low = 0;
  binary_search_up = 14;
  binary_search_fvalue = -1 /* all data are positive */ ;
  while (binary_search_low <= binary_search_up) {
    mid = (binary_search_low + binary_search_up) >> 1;
    if ( data[mid].key == x ) {  /*  found  */
      binary_search_up = binary_search_low - 1;
      binary_search_fvalue = data[mid].value;
    }
    else  /* not found */
      if ( data[mid].key > x ) 	{
	binary_search_up = mid - 1;
      }
      else   {
             	binary_search_low = mid + 1;
      }
  }
  return binary_search_fvalue;
}
