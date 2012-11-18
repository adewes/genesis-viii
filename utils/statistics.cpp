#include "statistics.h"
#include "nr/nr.h"
#include "scriptmanager.h"

namespace LuaStat
{
	int gumbel_copula(lua_State *L)
	{
		float x,y,copula;
		int n=lua_gettop(L);
		if (n>=2)
		{
			x=lua_tonumber(L,-n);
			y=lua_tonumber(L,-n+1);
			copula=STAT::gumbel_copula(x,y,1.0);
			lua_pushnumber(L,copula);
		}
		return 1;
	}

	int inv_extreme(lua_State *L)
	{
		float x,xi,mu,sigma,inv;
		sigma=1.0;
		mu=0.0;
		xi=0.0;
		int n=lua_gettop(L);
		if (n>=1)
		{
			if (lua_isnumber(L,-n))
				x=lua_tonumber(L,-n);
			if (lua_isnumber(L,-n+1) && n>=2)
				xi=lua_tonumber(L,-n+1);
			if (lua_isnumber(L,-n+2) && n>=3)
				mu=lua_tonumber(L,-n+2);
			if (lua_isnumber(L,-n+3) && n>=4)
				sigma=lua_tonumber(L,-n+3);
			inv=STAT::inv_extreme(x,xi,mu,sigma);
			lua_pushnumber(L,inv);
		}
		return 1;
	}

	void Setup(void)
	{
		ScriptManager *SM=SM->i();
		SM->RegisterFunction("inv_extreme","stat",inv_extreme);
		SM->RegisterFunction("gumbel_copula","stat",gumbel_copula);
	}
};

namespace STAT
{

Matrix generate_copula_density(float DU,float (*copula)(float x,float y,float *params),float *params)
{
	float px[2];
	Matrix m;
	float c1,c2,c3,c4,p;
	unsigned int i=0;
	for(float u=0;u<=1.0;u+=DU)
		for(float v=0;v<=1.0;v+=DU)
		{
			px[0]=u+DU/2.f;
			if (px[0]>1)
				px[0]=1;
			if (px[1]>1)
				px[1]=1;
			px[1]=v+DU/2.f;
			c1=copula(px[0],px[1],params);
			px[0]-=DU;
			if (px[0]<0)
				px[0]=0;
			c2=copula(px[0],px[1],params);
			px[1]-=DU;
			if (px[1]<0)
				px[1]=0;
			c3=copula(px[0],px[1],params);
			px[0]+=DU;
			c4=copula(px[0],px[1],params);
			m.Set(i,0,u);
			m.Set(i,1,v);
			m.Set(i,2,c1);
			p=(c1+c3-c2-c4)/DU/DU;
			if (fabs(p)<1e3 && p>0 && u>0 && v>0)
				m.Set(i,5,p);
			i++;
		}
		return m;
}
Matrix generate_histogram(Matrix m,unsigned int col,unsigned int bins)
{
	unsigned int i,bin;
	float max,min;
	Matrix r;
	max=m.Max(col);
	min=m.Min(col);
	float dx=(max-min)/(float)bins;
	for(i=0;i<=bins;i++)
	{
		r.Set(i,0,min+dx*(float)i);
		r.Set(i,1,0.);
	}
	for(i=0;i<m.Dim();i++)
	{
		bin=(unsigned int)(((float)m[i][col]-min)/(max-min)*(float)bins);
		r.Set(bin,1,(float)r[bin][1]+1);
	}
	float integral=0.0;
	float cdf;
	for(i=0;i<r.Dim();i++)
	{
		integral+=dx*(float)r[i][1];
	}
	cdf=0.0;
	for(i=0;i<r.Dim();i++)
	{
		r.Set(i,1,(float)r[i][1]/integral);
		cdf+=(float)r[i][1]*dx;
		r.Set(i,2,(float)i*dx);
		r.Set(i,3,cdf);
	}
	return r;
}

float extreme(float x,float xi)
{
	return exp(-pow(1+xi*x,-1.f/xi));
}

float inv_extreme(float x,float xi,float mu,float sigma)
{
	if (xi!=0)
		return (pow(-log(x),-xi)-1)/xi*sigma+mu;
	else
		return -log(-log(x))*sigma+mu;
}

//This is the modified error function!
float erf(float x)
{
	return 1./2.*(2.-NR::erfcc(x/sqrt(2.)));
}

float inv_student_t_distribution(float u,float nu)
{
	float v=0.0;
	float dv;
	float x=student_t_distribution(v,nu);
	nu=(float)(int)nu;
	while(fabs(u-x)>1e-6)
	{
		dv=(student_t_distribution(v+1e-3,nu)-student_t_distribution(v,nu))/1e-3;
		v+=(u-x)/dv;
		x=student_t_distribution(v,nu);
	}
	return v;

}

float student_t_distribution(float x,float nu)
{
	nu=(float)(int)nu;
	if (x>0)
		return 1.0f-0.5f*NR::betai(nu/2.f,0.5f,nu/(nu+x*x));
	else
		return 0.5f*NR::betai(nu/2.f,0.5f,nu/(nu+x*x));
}

float student_t(float x,float nu)
{
	nu=(float)(int)nu;
	return exp(NR::gammln((nu+1.f)/2.f))/sqrt(nu*PI)/exp(NR::gammln(nu/2.f))*pow(1.f+x*x/nu,-(nu+1.f)/2.f);
}

float normal(float x)
{
	return exp(-pow(x,2.f)/2.)/sqrt(2.f*PI);
}

DP chi_square_cdf(float x,float n)
{
	if (x<=0)
		return 0.f;
	return NR::gammp(n/2.f,x/2.f);///exp(NR::gammln(n/2.f));
}

DP inv_chi_square_cdf(float u,float n)
{
	float v=0.01;
	float dv;
	float interval=0.1;
	float x=chi_square_cdf(v,n);
	//simple interval search....
	while(fabs(x-u)>1e-5)
	{
		if (x<u)
		{
			interval/=2.f;
			while(x<u)
			{
				v+=interval;
				x=chi_square_cdf(v,n);
			}

		}
		if (x>u)
		{
			interval/=2.f;
			while(x>u)
			{
				v-=interval;
				x=chi_square_cdf(v,n);
			}
		}
	}
	return v;
}


float inv_normal(float u)
{
	float v=0.0;
	float dv;
	float x=erf(v);
	while(fabs(u-x)>1e-6)
	{
		dv=(erf(v+1e-3)-erf(v))/1e-3;
		v+=(u-x)/dv;
		x=erf(v);
	}
	return v;
}

float random_number(float amp,bool sign)
{
	static long id=-(long)time(NULL);
	if (sign)
		return (NR::ran2(&id)-NR::ran2(&id))*amp;
	return NR::ran2(&id)*amp;
}

Matrix bivariate_sample(unsigned int n,float (*probability)(float x,float y),float range,float accuracy)
{
	Matrix multi;
	unsigned int i=0;
	float x,y,p,p_me;
	while(i<n)
	{
		x=random_number(range,true);
		y=random_number(range,true);
		p=probability(x,y)*accuracy*accuracy;
		p_me=random_number(1.,false);
		if (p_me<p)
		{
			cout << "Value(" << i << "):" << x << ", " << y << "\n";
			multi.Set(i,0,x);
			multi.Set(i,1,y);
			i++;
		}
	}
	return multi;
}

Matrix copula_density(unsigned int n,float (*copula)(float x,float y),float accuracy)
{
	Matrix multi;
	unsigned int i=0;
	float x,y,p,p_me,u,v,u2,v2;
	while(i<n)
	{
		u=random_number(1.0-accuracy/2.f,false);
		v=random_number(1.0-accuracy/2.f,false);
		u2=u+accuracy;
		v2=v+accuracy;
		if (u2>1.0)
			u2=1.0;
		if (v2>1.0)
			v2=1.0;
		p=copula(u2,v2)+copula(u,v)-copula(u2,v)-copula(u,v2);
		p_me=random_number(1.,false);
		if (p_me<p)
		{
			multi.Set(i,0,u+accuracy/2.f);
			multi.Set(i,1,v+accuracy/2.f);
			x=inv_normal(u);
			y=inv_normal(v);
			cout << "C(u,v) = " << copula(u+accuracy/2.f,v+accuracy/2.f) << "\n";
			cout << "Value(" << i << "):" << x << ", " << y << "\n";
			multi.Set(i,2,x);
			multi.Set(i,3,y);
			i++;
		}
	}
	return multi;

}

Matrix copula_sample(unsigned int n,float (*inv_margin1)(float x),float (*inv_margin2)(float x),float (*copula)(float x,float y),float range,float accuracy)
{
	Matrix multi;
	unsigned int i=0;
	float x,y,p,p_me,u,v,u2,v2;
	while(i<n)
	{
		u=random_number(1.0-accuracy,false);
		v=random_number(1.0-accuracy,false);
		u2=u+accuracy;
		v2=v+accuracy;
		if (u2>1.0)
			u2=1.0;
		if (v2>1.0)
			v2=1.0;
		p=copula(u2,v2)+copula(u,v)-copula(u2,v)-copula(u,v2);
		p_me=random_number(1.,false);
		if (p_me<p)
		{
			x=inv_margin1(u+accuracy/2.f);
			y=inv_margin2(v+accuracy/2.f);
			cout << "Value(" << i << "):" << x << ", " << y << "\n";
			multi.Set(i,0,x);
			multi.Set(i,1,y);
			i++;
		}
	}
	return multi;
}

Matrix gauss_sample(unsigned int n,float mu,float alpha_0,float alpha_1,float beta_1)
{
	Matrix gauss;
	long idnum=time(NULL);
	float x;
	float sigma,sigma_gas;
	sigma=sqrt(alpha_0);
	for(int i=1;i<=n;i++)
	{
		x=random_number(1.0,false);
		gauss.Set(i,0,(float)i);
		gauss.Set(i,1,sigma*inv_normal(x)+mu);
		gauss.Set(i,2,mu+sigma*inv_normal(0.95));//95.0 % VaR
		gauss.Set(i,3,mu+sigma*inv_normal(0.99));//99.0 % VaR
		gauss.Set(i,4,mu+sigma*inv_normal(0.999));//99.5 % VaR
		gauss.Set(i,5,sigma);//sigma
		sigma_gas=NR::gasdev(&idnum)*1.0;
		sigma=sqrt(alpha_0+alpha_1*sigma*sigma+beta_1*pow((float)gauss[i][1],2.f));
	}
	return gauss;
}

float bivariate_normal_density(float x,float y,float rho)
{
	if (rho>=1.0)
		rho=1.0;
	if (rho<0)
		rho=0.0;
	return 1.f/(2.f*PI*sqrt(1-rho*rho))*exp(-1.f/(2.f*(1.f-rho*rho))*(x*x+y*y-2*rho*x*y));
}

float normal_copula(float u,float v,float rho)
{
	if (u>=1)
	{
		if (v<=1)
			return v;
		else
			return 1.f;
	}
	if (v>=1)
	{
		if (u<=1)
			return u;
		else
			return 1.f;
	}
	if (u<=0 || v<=0)
		return 0.0f;
	if (rho>1.0 || rho<-1.0)
		rho=0.0;
	return NR::N(inv_normal(u),inv_normal(v),rho);//bivariate_normal_density(inv_normal(u),inv_normal(v),rho);
}

Matrix GARCH_extreme_sample(unsigned int n,float xi,float alpha_0,float alpha_1,float beta_1)
{
	Matrix extreme;
	unsigned int i;
	float sigma_gas;
	long idnum=time(NULL);
	float x,y,sigma,mu;
	mu=0.0;
	extreme.Set(0,0,"#no");
	extreme.Set(0,1,"R");
	extreme.Set(0,2,"0.95 VaR");
	extreme.Set(0,3,"0.99 VaR");
	extreme.Set(0,3,"0.999 VaR");
	extreme.Set(0,3,"0.5 Gaussian sd");
	extreme.Set(0,4,xi);
	extreme.Set(0,5,alpha_0);
	extreme.Set(0,6,alpha_1);
	extreme.Set(0,7,beta_1);
	sigma=sqrt(alpha_0);
	for(i=1;i<=n;i++)
	{
		x=random_number(1.0,false);
		y=inv_extreme(x,xi,mu,sigma);
		extreme.Set(i,0,i);
		extreme.Set(i,1,y);
		extreme.Set(i,2,inv_extreme(0.95,xi,mu,sigma));//95 % VaR
		extreme.Set(i,3,inv_extreme(0.99,xi,mu,sigma));//99 % VaR
		extreme.Set(i,4,inv_extreme(0.999,xi,mu,sigma));//99.9 % VaR
		extreme.Set(i,5,sigma);
//		cout << "x = " << y << "\n";
		sigma_gas=NR::gasdev(&idnum)*1.0;
		sigma=sqrt(alpha_0+alpha_1*sigma*sigma+beta_1*y*y);
	}
	return extreme;
}

float frank_copula(float u,float v,float theta)
{
	if (u>=1)
	{
		if (v<=1)
			return v;
		else
			return 1.f;
	}
	if (v>=1)
	{
		if (u<=1)
			return u;
		else
			return 1.f;
	}
	if (u<=0 || v<=0)
		return 0.0f;
	if (theta<=0)
	{
		theta=-theta;
	}
	return -1./theta*log((exp(-theta*u)-1)*(exp(-theta*v)-1)/(exp(-theta)-1)+1.f);
}

float gumbel_copula(float u,float v,float theta)
{
	if (u>=1)
	{
		if (v<=1)
			return v;
		else
			return 1.f;
	}
	if (v>=1)
	{
		if (u<=1)
			return u;
		else
			return 1.f;
	}
	if (u<=0 || v<=0)
		return 0.0f;
	if (theta<0)
	{
		theta=-theta;
	}
	return exp(-pow((pow(-log(u),(float)theta)+pow(-log(v),(float)theta)),(float)1./theta));
}

float gumbel_copula_density(float u,float v,float theta)
{
	float u2,v2;
	float du,dv;
	du=0.001;
	dv=0.001;
	u2=u+du;
	v2=v+dv;
	return (gumbel_copula(u2,v2,theta)+gumbel_copula(u,v,theta)-gumbel_copula(u2,v,theta)-gumbel_copula(u,v2,theta))/du/dv;
}

float clayton_copula_density(float u,float v,float theta)
{
	if (theta<0)
		theta=-theta;
	return (1.f+theta)*pow(u*v,-theta-1.f)*pow(pow(u,-theta)+pow(v,-theta)-1.f,-1.f/theta-2.f);
}

float clayton_copula(float u,float v,float theta)
{
	if (u>=1)
	{
		if (v<=1)
			return v;
		else
			return 1.f;
	}
	if (v>=1)
	{
		if (u<=1)
			return u;
		else
			return 1.f;
	}
	if (u<=0 || v<=0)
		return 0.0f;
	if (theta<0)
	{
		theta=-theta;
	}
	return pow(pow(u,-theta)+pow(v,-theta)-1.,-1./theta);
}

};