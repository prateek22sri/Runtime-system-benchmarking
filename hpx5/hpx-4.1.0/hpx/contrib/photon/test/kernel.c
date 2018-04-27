//////// kernel()
#define ARRSZ 256

extern double junk;

//////// This is kernel k1
void do_kernel(int maxIter) {
  register int i,j;
  int array[ARRSZ];
  double tmp;

  for(i=0; i<maxIter; i++) {
    for(j=0; j<128; j++) {
      tmp = (array[(i+1)%ARRSZ] + array[j%ARRSZ]) / 2.0;
      array[(i+j/2)%ARRSZ] = ((int)(0.37*tmp) + (int)(0.63*junk))%12345678;
    }
  }
  junk = array[0];

}
