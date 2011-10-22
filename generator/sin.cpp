#include <math.h>
#include <iostream>
#define abs(x) (x>0?x:(-x))
using std::cerr;
using std::endl;

double f(double x) {
 return(abs(x));
}

int main() {
 unsigned int N=50,set_min=0,set_max=4095;
 double maxx=2*M_PI,zoom=maxx/(N*1.),offset=-maxx/2;

 double* V=new double[N];
 double min=0,max=0,t;
 unsigned int x;

 for(x=0;x<N;x++) {
  t=f(x*zoom+offset);
  if(!x||min>t)
   min=t;
  if(!x||max<t)
   max=t;
  V[x]=t;
 }

 cerr << "uint8_t N=" << N << ";" << endl;
 cerr << "uint16_t values[]={";
 for(x=0;x<N;x++) {
  V[x]=((V[x]-min)/(max-min)*(set_max-set_min))+set_min;
  if(x) cerr << ",";
  cerr << round(V[x]);
 }
 cerr << "};" << endl;
}
