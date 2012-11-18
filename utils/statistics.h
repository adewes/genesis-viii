#include "matrix.h"
#include <math.h>
#include <iostream>
#include <time.h>

using namespace std;

namespace LuaStat
{
	void Setup(void);
};

namespace STAT
{
float chi_square_cdf(float u,float n);
float inv_chi_square_cdf(float u,float n);
float extreme(float x,float xi);
float bivariate_normal_density(float x,float y,float rho);
float normal_copula(float u,float v,float rho);
Matrix copula_density(unsigned int n,float (*copula)(float x,float y),float accuracy);
float inv_student_t_distribution(float u,float nu);
float student_t_distribution(float x,float nu);
float student_t(float x,float nu);
float normal(float x);
Matrix gauss_sample(unsigned int n,float mu,float alpha_0,float alpha_1,float beta_1);
Matrix generate_copula_density(float DU,float (*copula)(float x,float y,float *params),float *params);
Matrix generate_histogram(Matrix m,unsigned int col,unsigned int bins);
float inv_extreme(float x,float xi,float mu,float sigma);
float erf(float x);
float inv_normal(float u);
float random_number(float amp,bool sign);
Matrix bivariate_sample(unsigned int n,float (*probability)(float x,float y),float range,float accuracy);
Matrix copula_sample(unsigned int n,float (*inv_margin1)(float x),float (*inv_margin2)(float x),float (*copula)(float x,float y),float range,float accuracy);
Matrix GARCH_extreme_sample(unsigned int n,float xi,float alpha_0,float alpha_1,float beta_1);
float gumbel_copula(float u,float v,float theta);
float gumbel_copula_density(float u,float v,float theta);
float clayton_copula_density(float u,float v,float theta);
float clayton_copula(float u,float v,float theta);
float frank_copula(float u,float v,float theta);
};