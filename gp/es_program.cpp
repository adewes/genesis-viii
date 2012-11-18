#include "es_program.h"
#include "leaf.h"
#include "../utils/logger.h"
#include "../utils/nr/nr.h"
#include "../utils/statistics.h"

#include <sstream>
#include <cmath>
#include <algorithm>

Matrix ESProgram::es;

ESProgram::ESProgram() : VaRProgram()
{
}

ESProgram::~ESProgram()
{
}

int var_columns[]={VAR_VC,VAR_HS,VAR_HS_EWMA,VAR_HS_GARCH,VAR_VC_GARCH,VAR_VC_EWMA,VAR_VC_GARCH_T,VAR_HS_GARCH_T,VAR_EVT,VAR_GP,0};
const char *var_str[]={"VAR_VC","VAR_HS","VAR_HS_EWMA","VAR_HS_GARCH","VAR_VC_GARCH","VAR_VC_EWMA","VAR_VC_GARCH_T","VAR_HS_GARCH_T","VAR_EVT","VAR_GP"};
void ESProgram::Setup(std::string directory,std::vector<std::string> parameters)
{	
	int i,j;
	static bool Initialized=false;
	if (Initialized==true)
		return;
	Initialized=true;
	n.Load(parameters[0]);
	es.Clear();
	i=0;
	Matrix var,waiting_times;
	Logger *LL=LL->i();
	stringstream stream;
	int begin=101;
	float x_mean,var_mean,p1,p2,sum_var,sum_x,alpha,beta,ssr;
	//We calculate the performance of VaR based ES measures.
	for(i=begin;i<n.Dim()-2;i++)
	{
		if ((float)n[i][1]>(float)n[i][7])
		{
			es.Set(es.Dim(),n[i]);
//			es.Set(es.Dim()-1,0,es.Dim()-1);
		}
	}
	i=0;
	while(var_columns[i]!=0)
	{
		sum_var=0.0;
		sum_x=0.0;
		ssr=0.0;
		x_mean=0.0;
		var_mean=0.0;
		p1=0.0;
		p2=0.0;
		for(j=begin;j<n.Dim()-2;j++)
		{
			if ((float)n[j][1]>(float)n[j][var_columns[i]])
				var.Set(var.Dim(),n[j]);
		}
		cout << "# violations for " << var_str[i] << ": " << var.Dim() << "\n";
		int division=50;
		vector<unsigned int> wt;
		Matrix wt_cdf;
		std::make_heap(wt.begin(),wt.end());
		for(j=0;j<var.Dim();j++)
		{
			if (j<var.Dim()-1)
			{
				wt.push_back(-(unsigned int)((float)var[j+1][0]-(float)var[j][0]));
				push_heap(wt.begin(),wt.end());
//				waiting_times.Set((unsigned int)((float)var[j+1][0]-(float)var[j][0])/division,0,(unsigned int)((float)var[j+1][0]-(float)var[j][0]));
//				waiting_times.Set((unsigned int)((float)var[j+1][0]-(float)var[j][0])/division,var_columns[i],(float)waiting_times[(unsigned int)((float)var[j+1][0]-(float)var[j][0])/division][var_columns[i]]+1);
			}
			x_mean+=(float)var[j][1];
			var_mean+=(float)var[j][var_columns[i]];
		}
		j=0;
		while(wt.size()>0)
		{
			pop_heap(wt.begin(),wt.end());
			wt_cdf.Set(wt_cdf.Dim(),0,(float)j/((float)var.Dim()-1.f));
			wt_cdf.Set(wt_cdf.Dim()-1,1,-wt[wt.size()-1]);
			wt.pop_back();
			j++;
		}
		stringstream file;
		file << directory << "/wt_cdf_" << var_str[i] << ".tsv";
		wt_cdf.Write(file.str());
		float d_max=0.0;
		for(j=0;j<wt_cdf.Dim();j++)
		{
			float cdf=1.0-pow(0.99f,(float)wt_cdf[j][1]);
			float dist=fabs(cdf-(float)wt_cdf[j][0]);
			if (dist>d_max)
				d_max=dist;
		}
		float chi_sq=0.0;
		int n_bins=wt_cdf.Dim()/5;
		float cstart,cend,cbins,cmin,cmax,cint,cexp;
		cmin=(float)wt_cdf[0][1];
		cmax=(float)wt_cdf[wt_cdf.Dim()-1][1];
		cint=(cmax-cmin)/(float)n_bins;
		cout << "Interval size:" << cint << ", number of bins: " << n_bins << "\n";
		unsigned int k;
		for(j=0;j<n_bins;j++)
		{
			cstart=j*cint;
			cend=(j+1)*cint;
			cbins=0;
			for(k=0;k<wt_cdf.Dim();k++)
			{
				if ((float)wt_cdf[k][1]>=cstart && (float)wt_cdf[k][1]<cend)
					cbins++;
				else if ((float)wt_cdf[k][1]>cend)
					break;
			}
			cexp=(pow(0.99f,cstart)-pow(0.99f,cend))*wt_cdf.Dim();
			cout << "Interval:" << cstart << ", " << cend << ", counts:" << cbins << ", expected: " << cexp << "\n"; 
			chi_sq+=pow(cbins-cexp,2.f)/cexp;
		}
		stream << "d_max(" << var_str[i] << ") = " << d_max << ", n = " << wt_cdf.Dim() << ", c = " << 1.36/sqrt((float)wt_cdf.Dim()) << "\n";
		stream << "chi_sq(" << var_str[i] << ") = " << chi_sq << ", m = " << n_bins-1 << ", critical = " << STAT::inv_chi_square_cdf(0.95,n_bins-1) << "\n";
		x_mean/=(float)var.Dim();
		var_mean/=(float)var.Dim();
		for(j=0;j<var.Dim();j++)
		{
			p1+=((float)var[j][1]-x_mean)*((float)var[j][var_columns[i]]-var_mean);
			p2+=pow((float)var[j][var_columns[i]]-var_mean,2.f);
		}
		alpha=p1/p2;
		beta=x_mean-alpha*var_mean;
		for(j=0;j<var.Dim();j++)
		{
			ssr+=pow((float)var[j][1]-(float)var[j][var_columns[i]]*alpha-beta,2.f);
		}
		ssr/=var.Dim()/1e5;
		stream << "VaR column:" << var_columns[i] << "(" << var_str[i] << "), alpha = " << alpha << ", beta = " << beta << ", ssr = " << ssr << "\n";
		i++;
		var.Clear();
	}
	cout << stream.str();
	LL->Event(stream.str());
	stream.str("");
	stream << directory << "/waiting_times.tsv";
	waiting_times.Write(stream.str());
	stream.str("");
	stream << directory << "/es_matrix.tsv";
	es.Write(stream.str());
	
	cout << "Number of violations:" << es.Dim() << "\n";
	
	TerminalManager *TM=TM->i();
	
	TM->Clear();

	TM->Add("min",&x1);
	TM->Add("max",&x2);
	TM->Add("sigma",&x3);
	TM->Add("sigma_tot",&x4);
	TM->Add("mu_tot",&x5);
	TM->Add("max_tot",&x6);
	TM->Add("min_tot",&x7);
	TM->Add("ma",&x8);
	TM->Add("sigma_garch",&x9);
	TM->Add("sigma_t",&x10);
}

float ESProgram::Fitness()
{
	unsigned int i;
	unsigned int start,end;
	float f,ssr;
	ssr=0.0;
	for(i=0;i<es.Dim();i++)
	{
		x1=(float)es[i-1][4];//min
		x2=(float)es[i-1][5];//max
		x3=sqrt((float)es[i][3]);//sigma
		x8=(float)es[i-1][11];//moving average
		x9=(float)sqrt((float)es[i-1][3]);//ma sigma
		x10=(float)sqrt((float)es[i-1][26]);//ma sigma

		f=fabs(Eval());
		ssr+=pow(f+(float)es[i][VAR_GP]-(float)es[i][1],2.f);
	}
	return -log(1.0+ssr);
}

void ESProgram::Write(std::string directory,unsigned int generation)
{
	unsigned int i=0;
	float es;
	float ssr=0.0;
	for(i=101;i<n.Dim()-2;i++)
	{
		x1=(float)n[i-1][4];//min
		x2=(float)n[i-1][5];//max
		x3=sqrt((float)n[i-1][3]);//sigma
		x8=(float)n[i-1][11];//moving average
		x9=(float)sqrt((float)n[i-1][6]);//GARCH sigma
		x10=(float)sqrt((float)n[i-1][26]);//GARCH-t sigma
		es=fabs(Eval())+(float)n[i][VAR_GP];
		n.Set(i,ES_GP,es);
		if ((float)n[i][1]>(float)n[i][VAR_GP])
			ssr+=pow((float)n[i][1]-es,2.f);
	}
	cout << "SSR = " << ssr << "\n";
	stringstream stream;
	stream << directory << "/best_" << generation << ".tsv";
	n.Write(stream.str());
}