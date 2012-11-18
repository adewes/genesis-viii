#include"nrutil.h"
#include<math.h>
#include<stdlib.h>
#include<iostream>
#include<stdio.h>

namespace NR
{
#define DP float
double N(const double&); // define the univariate cumulative normal distribution as a separate function
double N(const double& a, const double& b, const double& rho);
DP betai(const DP a, const DP b, const DP x);
DP betacf(const DP a, const DP b, const DP x);
float gammln(const float xx);
void gser(DP &gamser, const DP a, const DP x, DP &gln);
void gcf(DP &gammcf, const DP a, const DP x, DP &gln);
DP gammp(const DP a, const DP x);
float ran1(long *idum);
float expdev(long *idum);
float gasdev(long *idum);
float amotry(float **p, float y[], float psum[], int ndim,
	     float (*funk)(float []), int ihi, float fac);
void realft(float data[], unsigned long n, int isign);
void amoeba(float **p, float y[], int ndim, float ftol,float (*funk)(float []), int *nfunk);
void tqli(float d[], float e[], int n, float **z);
void tred2(float **a, int n, float d[], float e[]);
float pythag(float a, float b);
void lubksb(float **a, int n, int *indx, float b[]);
void ludcmp(float **a, int n, int *indx, float *d);
void mnewt(int ntrial, float x[], int n, float tolx, float tolf,void (*usrfun)(float [],int n,float *fvec,float **fjac));
void lubksb(float **a, int n, int *indx, float b[]);
void elmhes(float **a, int n);
float ran2(long *idum);
float erfcc(const float x);
void hqr(float **a, int n, float wr[], float wi[]);
void balanc(float **a, int n);
void jacobi(float **a, int n, float d[], float **v, int *nrot);
void sprsin(float **a,int **map, int n, float thresh, unsigned long nmax, float sa[],
	unsigned long ija[]);
void linbcg(unsigned long n, float b[], float x[], int itol, float tol,
	int itmax, int *iter, float *err,float sa[],unsigned long ija[],float *inv_vec,unsigned long *inv_ind);
void linbcg_single(unsigned long n, float b[], float x[], int itol, float tol,
	int itmax, int *iter, float *err,float sa[],unsigned long ija[],float *inv_vec,unsigned long *inv_ind);
void atimes(unsigned long n, float x[], float r[], int itrnsp,float sa[],unsigned long ija[]);
float snrm(unsigned long n, float sx[], int itol);
void asolve(unsigned long n, float b[], float x[], int itrnsp,float sa[],float *inv_vec,unsigned long *inv_ind);
void dsprsax(float sa[], unsigned long ija[], float x[], float b[], unsigned long n);
void dsprstx(float sa[], unsigned long ija[], float x[], float b[], unsigned long n);
void gauss_elimination(float **a,int n);
void full_gauss_elimination(float **a,float **b,int n);
void gaussj(float **a, int n, float **b, int m);


#define PI 3.14159265
}