#include "distribution_program.h"
#include "../utils/statistics.h"
#include "../utils/nr/nr.h"
#include "leaf.h"
#include "distribution_branch.h"

#include <sstream>

Matrix DistributionProgram::bins,DistributionProgram::d;

DistributionProgram::DistributionProgram() : CopulaProgram()
{

}

#define DATA_COL 6
#define DX 0.001
float DistributionProgram::Fitness()
{
	float ll=0.0;
	float p1,p2,p;
	for(unsigned int i=0;i<d.Dim();i++)
	{
		px[0]=(float)d[i][DATA_COL];
		p1=Eval();
		px[0]+=DX;
		p2=Eval();
		p=(p2-p1)/DX;
		if (p>0)
			ll+=log(p);
		else
			return log(0.f);
	}
	return ll-0.00*log(Size()+1.0);
}

DistributionProgram::~DistributionProgram()
{

}

void DistributionProgram::Write(std::string directory,unsigned int generation)
{
	Matrix m,quantile;
	float min,max,p1,p2,p,p_tot,dx;
	min=d.Min(DATA_COL);
	max=d.Max(DATA_COL);
	stringstream stream;
	dx=(max-min)/400.0;
	p_tot=0.0;
	unsigned int cnt=0;
	for(float x=min;x<=max;x+=dx)
	{
		px[0]=x;
		p1=Eval();
		px[0]+=DX;
		p2=Eval();
		p=(p2-p1)/DX;
		p_tot+=p*dx;
		m.Set(cnt,0,x);
		m.Set(cnt++,1,p);
	}
	float in=0.0;
	for(unsigned int i=0;i<m.Dim();i++)
	{
		m.Set(i,1,(float)m[i][1]/p_tot);
		in+=(float)m[i][1]*dx;
		m.Set(i,2,in);
		px[0]=(float)m[i][0];
	}
	for(unsigned int i=0;i<bins.Dim();i++)
	{
		quantile.Set(i,0,bins[i][3]);
		px[0]=bins[i][0];
		quantile.Set(i,1,Eval());
	}
	stream << directory << "/bins_" << generation << ".tsv";
	m.Write(stream.str());
	stream.str("");
	stream << directory << "/quantile_" << generation << ".tsv";
	quantile.Write(stream.str());
}

float distri_garch11(float *params)
{
	unsigned int i=0;
	float predictor,ssr;
	float ll=1.0;
	ssr=0.0;
	predictor=0.0;
	for(i=1;i<=2;i++)
		if (params[i]<0)
			params[i]=-params[i];
	if (fabs(params[2])<0.5)
		return 1e10;
	for(i=100;i<1000;i++)
	{
		predictor=params[1]*pow((float)((float)DistributionProgram::c[i-1][1]),2.0f)+params[2]*predictor;
		
		if (predictor>0.0)
			ll+=log(1.f/sqrt(predictor)*STAT::normal((float)DistributionProgram::c[i][1]/sqrt(predictor)));
		else
			return -log(0.f);
	}
	return -ll;
}


float student_t_cdf(float *x,unsigned int d,float *p)
{
	if (p[2]<2)
		return 0.f;
	if (p[1]<0)
		p[1]=-p[1];
	return STAT::student_t_distribution((x[0]-p[0])/p[1],p[2]);
}

float normal_cdf(float *x,unsigned int d,float *p)
{
	if (p[1]<0)
		p[1]=-p[1];
	float e=STAT::erf((x[0]-p[0])/p[1]);
	return e;
}

float extreme_cdf(float *x,unsigned int d,float *p)
{
	if (p[1]<0)
		p[1]=-p[1];
	float e=STAT::extreme((x[0]-p[0])/p[1],p[2]);
	return e;
}

void DistributionProgram::Setup(std::string directory,vector<std::string> parameters)
{
	unsigned int i,j;
	static bool Initialized=false;
	if (Initialized==true)
		return;
	stringstream stream;
	Initialized=true;
	TerminalManager *TM=TM->i();
	TM->Clear();
	DistributionBranch b;
	b.AddDistribution("student_t",3,student_t_cdf);
	b.AddDistribution("normal",2,normal_cdf);
	b.AddDistribution("extreme",3,extreme_cdf);
	Matrix m;
	if (parameters.size()>=3)
	{	
		unsigned int dcol=atoi(parameters[1].data());
		m.Load(parameters[0],parameters[2],NULL);
		cout << "Data column:" << dcol << "\n";
		unsigned int cnt=0;
		for(i=0;i<m.Dim()-1;i++)
		{
			c.Set(i,0,i);
			c.Set(i,1,-((float)m[m.Dim()-i-2][dcol]-(float)m[m.Dim()-i-1][dcol])/((float)m[m.Dim()-i-1][dcol]));
		}
	}
	else
		c=STAT::GARCH_extreme_sample(1000,0.0,1.0,0.2,0.002);
	unsigned int bin;
	float mu=0.0;
	float sigma=0.0;
	for(i=0;i<c.Dim();i++)
	{
		mu+=(float)c[i][1];
	}
	mu/=(float)c.Dim();
	for(i=0;i<c.Dim();i++)
	{
		sigma+=pow((float)c[i][1]-mu,2.f);
	}
	sigma/=(float)c.Dim()-1.f;
	cout << "mu = " << mu << ", sigma = " << sigma << "\n";
	for(i=0;i<c.Dim();i++)
	{
		c.Set(i,1,((float)c[i][1]-mu)/sqrt(sigma));
	}
	float sigma_ma=0.0;
	unsigned int P=10;
	float sigma_ewma=pow((float)c[P-1][1]-mu,2.f);
	float integr=0.0;
	float alpha=0.9;

	int nfunc=0;


	float *vy=NR::vector(1,3);
	float **mm=NR::matrix(1,3,1,1);

	/*We fit a GARCH(1,1) process to the sample variance*/
	for(int y=1;y<=3;y++)
    {
        mm[y][1]=1.0+(float)(rand()%100-rand()%100)/1000.0;
        mm[y][2]=1.0+(float)(rand()%100-rand()%100)/100.0;
    }

	nfunc=0;
	
	for(int x=1;x<=3;x++)
		vy[x]=distri_garch11(mm[x]);

	float garch_ssr;

	NR::amoeba(mm,vy,2,1e-5,distri_garch11,&nfunc);
    cout << "Amoeba converged!\n";
	cout << "beta_1 = " << mm[1][1] << "\n";
	cout << "alpha_1  = " << mm[1][2] << "\n";
	cout << "SSR:" << (garch_ssr=distri_garch11(mm[1])) << "\n";


	c.Set(0,3,sqrt((float)mm[1][1]));
	for(i=1;i<c.Dim();i++)
		c.Set(i,3,(float)mm[1][1]*pow((float)((float)c[i-1][1]),2.0f)+(float)mm[1][2]*(float)c[i-1][3]);//sigma GARCH(1,1)
	for(i=P+1;i<c.Dim();i++)
	{
		sigma_ma=0.0;
		for(j=i-P-1;j<i;j++)
		{
			sigma_ma+=pow((float)c[j][1]-mu,2.f);
		}
		sigma_ma/=(float)P;
		d.Set(i-P,0,i-P);
		d.Set(i-P,1,(float)c[i][1]);
		integr+=-(float)c[i][1];
		d.Set(i-P,4,integr);
		d.Set(i-P,2,((float)c[i][1]-mu)/sqrt(sigma_ma));
		d.Set(i-P,5,((float)c[i][1]-mu)/sqrt(sigma_ewma));
		d.Set(i-P,6,((float)c[i][1]-mu)/sqrt((float)c[i][3]));
		d.Set(i-P,7,sqrt((float)c[i][3]));
		d.Set(i-P,3,sqrt(sigma_ma));
		sigma_ewma=alpha*sigma_ewma+(1.-alpha)*pow((float)c[i][1]-mu,2.f);
	}
	bins=STAT::generate_histogram(d,DATA_COL,100);
/*	cout << "min = " << min << ", max = " << max << "\n";
	for(i=0;i<=100;i++)
	{
		bins.Set(i,0,(float)i);
		bins.Set(i,1,min+(max-min)*((float)i)/100.f);
	}
	for(i=0;i<c.Dim();i++)
	{
		bin=(unsigned int)(((float)c[i][1]-min)/(max-min)*100.0f);
		value=(float)bins[bin][2];
		bins.Set(bin,2,value+1.f);
	}
	float in=0.0;
	for(i=0;i<bins.Dim();i++)
	{
		bins.Set(i,2,(float)bins[i][2]/(float)c.Dim()/(max-min)*100.f);
		in+=(float)bins[i][2]*(max-min)/100.f;
		bins.Set(i,3,in);
	}*/
	stream << directory << "/bins_real.tsv"; 
	bins.Write(stream.str());
	stream.str("");
	stream << directory << "/values_corrected.tsv";
	d.Write(stream.str());
	cout << "Setup done...\n";
}