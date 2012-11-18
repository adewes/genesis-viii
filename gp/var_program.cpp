#include "var_program.h"
#include "../utils/statistics.h"
#include "../utils/nr/nr.h"
#include "../utils/logger.h"
#include "../gp/leaf.h"

#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

float VaRProgram::avg_vol;
float VaRProgram::garch_ssr;
float VaRProgram::var_evt,VaRProgram::var_vc,VaRProgram::var_hs,VaRProgram::var_vc_garch,VaRProgram::var_hs_garch,VaRProgram::var_hs_garcht,VaRProgram::var_vc_garcht,VaRProgram::var_vc_ewma,VaRProgram::var_hs_ewma,VaRProgram::x10,VaRProgram::x11,VaRProgram::x9,VaRProgram::x0,VaRProgram::x1,VaRProgram::x2,VaRProgram::x3,VaRProgram::x4,VaRProgram::x5,VaRProgram::x6,VaRProgram::x7,VaRProgram::x8,VaRProgram::trend,VaRProgram::curvature,VaRProgram::mean,VaRProgram::open,VaRProgram::close,VaRProgram::volume,VaRProgram::high,VaRProgram::low;
Matrix VaRProgram::evt,VaRProgram::m,VaRProgram::n;
float VaRProgram::sigma_tot,VaRProgram::mu_tot,VaRProgram::max_tot,VaRProgram::min_tot;

#define COL_X
#define COL_SIGMA
#define COL_SIGMA_MA
#define COL_SIGMA_REAL
#define COL_VAR_95
#define COL_VAR_99
#define COL_VAR_999
#define COL_MAX
#define COL_MIN
#define COL_MA
#define COL_GARCH


VaRProgram::VaRProgram() : Program()
{
}

VaRProgram::~VaRProgram()
{

}

float VaRProgram::Fitness()
{
//	if (FitnessCalculated)
//		return FitnessValue;
	Logger *ML=ML->i();
	float f,x;
	float y;
	unsigned int i,j;
	bool status=false;
	float average_var=0.0;
	unsigned int offset=1;
	cash=0.0;
	ssr=0.0;
	int cnt=0;
	int cnt2=0;
	unsigned int start,end;
	start=101;
	float diff,var_sigma,last_f,ssr;
	var_sigma=0.0;
	end=n.Dim()*PERCENT;
	stringstream stream;
	int n_observations=end-start;
	diff=0.0;
	last_f=0.0;
	f=0.0;
	ssr=0.0;
	average_var=0.0;
	cnt=0;
	Matrix var;
	for(i=start;i<end;i++)
	{
		x1=(float)n[i-1][4];//min
		x2=(float)n[i-1][5];//max
		x3=sqrt((float)n[i][3]);//sigma
		x8=(float)n[i-1][11];//moving average
		x9=(float)sqrt((float)n[i-1][6]);//sigma-garch
		x10=(float)sqrt((float)n[i-1][26]);//sigma-garch-t
		volume=(float)sqrt((float)n[i-1][COL_VOL]);//ma sigma

		var_vc=(float)n[i][VAR_VC];
		var_hs=(float)n[i][VAR_HS];
		var_vc_garch=(float)n[i][VAR_VC_GARCH];
		var_hs_garch=(float)n[i][VAR_HS_GARCH];
		var_vc_garcht=(float)n[i][VAR_VC_GARCH_T];
		var_hs_garcht=(float)n[i][VAR_HS_GARCH_T];
		var_vc_ewma=(float)n[i][VAR_VC_EWMA];
		var_hs_ewma=(float)n[i][VAR_HS_EWMA];
		var_evt=(float)n[i][VAR_EVT];

		last_f=f;
		f=Eval();
		n.Set(i,7,f);
		if ((float)n[i][1]>f)
		{
			cnt++;
			var.Set(var.Dim(),n[i]);
		}
		ssr+=pow(f-(float)n[i][VAR_COL],2.f);
		average_var+=f;
		diff+=pow(f-(float)n[i][1],2.f);
		if (f!=f)//#=>f=+-#1.INF!
			return log(0.f);
	}

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
		}
/*		x_mean+=(float)var[j][1];
		var_mean+=(float)var[j][var_columns[i]];*/
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
//	cout << "Interval size:" << cint << ", number of bins: " << n_bins << "\n";
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
//		cout << "Interval:" << cstart << ", " << cend << ", counts:" << cbins << ", expected: " << cexp << "\n"; 
		chi_sq+=pow(cbins-cexp,2.f)/cexp;
	}
	float d_crit=1.36/sqrt((float)wt_cdf.Dim());
//	cout << "d_max = " << d_max << ", n = " << wt_cdf.Dim() << ", c = " << 1.36/sqrt((float)wt_cdf.Dim()) << "\n";
//	cout << "chi_sq = " << chi_sq << ", m = " << n_bins-1 << "\n";
	average_var/=(float)n_observations;
	diff/=(float)n_observations;
	var_sigma/=(float)n_observations;
	float p1,p2,p3,p4;
	p1=log(1.0+pow((float)cnt-VAR_LEVEL*(float)n_observations,2.0));
	p2=log(1.0+diff/sqrt(sigma_tot));
	p3=log(1.0+var_sigma/sqrt(sigma_tot));
//	cout << "\nProgram:" << Print() << "\n";
//	cout << "\nCnt:" << cnt << "\n";
	p4=log(1.0+fabs(average_var));//log(1.0+average_var);
	FitnessValue=-(p1+1.0*p2+d_max*0.4);//We want to predict 0.99 VaR!//*(1.0+sqrt(1.0+(float)Size()))
	FitnessCalculated=true;
	return FitnessValue;
}


float garch11_ols(float *params)
{
	unsigned int i=0;
	float predictor,ssr;
	float ll=1.0;
	ssr=0.0;
	predictor=0.0;
	for(i=1;i<=3;i++)
		if (params[i]<0)
			params[i]=-params[i];
	for(i=100;i<1000;i++)
	{
		predictor=params[1]+params[2]*pow((float)((float)VaRProgram::n[i-1][1]),2.0f)+params[3]*pow((float)VaRProgram::n[i-1][3],2.f);
		ssr+=pow(predictor-pow((float)VaRProgram::n[i][3],2.f),2.f);
	}
	return log(ssr);
}

float garch11(float *params)
{
	unsigned int i=0;
	float predictor,ssr;
	float ll=1.0;
	ssr=0.0;
	predictor=0.0;
	for(i=1;i<=3;i++)
		if (params[i]<0)
			params[i]=-params[i];
	if (fabs(params[3])<0.5)
		return 1e10;
	for(i=100;i<1000;i++)
	{
		predictor=params[1]*0+params[2]*pow((float)((float)VaRProgram::n[i-1][1]),2.0f)+params[3]*predictor;
		
		if (predictor>0.0)
			ll+=log(1.f/sqrt(predictor)*STAT::normal((float)VaRProgram::n[i][1]/sqrt(predictor)));
		else
			return -log(0.f);
//		ssr+=pow(predictor-pow((float)VaRProgram::n[i][12],2.f),2.f);
	}
	return -ll;
}

float evt_ll(float *params)
{
	unsigned int i=0;
	if (params[1]<0)
		params[1]=-params[1];
	if (params[2]<0)
		params[2]=-params[2];
	float ll=-(float)VaRProgram::evt.Dim()*log(params[1]);
	for(i=0;i<VaRProgram::evt.Dim();i++)
	{
		ll+=-(1.f+1.f/params[2])*log(1.f+params[2]*(float)VaRProgram::evt[i][1]/params[1]);
	}
	return -ll;
}

float garch11_t(float *params)
{
	unsigned int i=0;
	float predictor,ssr;
	float ll=1.0;
	ssr=0.0;
	predictor=0.0;
	if (params[4]<=2.)
		return -log(0.f);
	params[4]=(int)params[4];
	float norm=sqrt(params[4]/(params[4]-2.f));
	for(i=1;i<=4;i++)
		if (params[i]<0)
			params[i]=-params[i];
	if (fabs(params[3])<0.5)
		return 1e10;
	for(i=100;i<1000;i++)
	{
		predictor=params[1]*0+params[2]*pow((float)((float)VaRProgram::n[i-1][1]),2.0f)+params[3]*predictor;
		
		if (predictor>0.0)
			ll+=log(1.f/sqrt(predictor)*STAT::student_t((float)VaRProgram::n[i][1]/sqrt(predictor)*norm,(float)(int)params[4]));
		else
			return -log(0.f);
	}
	return -ll;
}

void VaRProgram::Write(std::string directory,unsigned int generation)
{
	Fitness();
	float a_gp,a_hs,a_vc,a_vc_ewma,a_vc_garch,a_vc_garch_t,a_hs_ewma,a_hs_garch,a_hs_garch_t,a_evt;
	unsigned int i,j,cnt,cnt_evt,cnt_real,cnt_hs,cnt_vc,cnt_hs_ewma,cnt_hs_garch,cnt_vc_garch,cnt_vc_ewma,cnt_vc_garch_t,cnt_hs_garch_t;
	float VaR;
	cnt=0;
	a_gp=0.0;
	a_hs=0.0;
	a_vc=0.0;
	a_hs_ewma=0.0;
	a_hs_garch=0.0;
	a_hs_garch_t=0.0;
	a_vc_ewma=0.0;
	a_vc_garch=0.0;
	a_vc_garch_t=0.0;
	a_evt=0.0;
	cnt_real=0;
	cnt_hs_garch_t=0;
	cnt_vc_garch_t=0;
	cnt_hs=0;
	cnt_vc=0;
	cnt_evt=0;
	cnt_vc_ewma=0;
	cnt_hs_ewma=0;
	cnt_hs_garch=0;
	cnt_vc_garch=0;
	unsigned int start=101;
	unsigned int end=n.Dim();
	float mm=1.0;
	Matrix var;
	for(i=start;i<end;i++)
	{
		x1=(float)n[i-1][4];//min
		x2=(float)n[i-1][5];//max
		x3=sqrt((float)n[i][3]);//sigma
		x8=(float)n[i-1][11];//moving average
		x9=(float)sqrt((float)n[i-1][3]);//ma sigma
		x10=(float)sqrt((float)n[i-1][26]);//ma sigma
		volume=(float)sqrt((float)n[i-1][COL_VOL]);//ma sigma

		var_vc=(float)n[i][VAR_VC];
		var_hs=(float)n[i][VAR_HS];
		var_vc_garch=(float)n[i][VAR_VC_GARCH];
		var_hs_garch=(float)n[i][VAR_HS_GARCH];
		var_vc_garcht=(float)n[i][VAR_VC_GARCH_T];
		var_hs_garcht=(float)n[i][VAR_HS_GARCH_T];
		var_vc_ewma=(float)n[i][VAR_VC_EWMA];
		var_hs_ewma=(float)n[i][VAR_HS_EWMA];
		var_evt=(float)n[i][VAR_EVT];

		/*Calculate the average value*/
		a_vc+=(float)n[i][VAR_VC];
		a_hs+=(float)n[i][VAR_HS];
		a_vc_garch+=(float)n[i][VAR_VC_GARCH];
		a_hs_garch+=(float)n[i][VAR_HS_GARCH];
		a_vc_garch_t+=(float)n[i][VAR_VC_GARCH_T];
		a_hs_garch_t+=(float)n[i][VAR_HS_GARCH_T];
		a_vc_ewma+=(float)n[i][VAR_VC_EWMA];
		a_hs_ewma+=(float)n[i][VAR_HS_EWMA];
		a_evt+=(float)n[i][VAR_EVT];

		VaR=Eval();
		a_gp+=VaR;
		n.Set(i,7,VaR);
		if ((float)n[i][1]*mm>VaR)
		{
			cnt++;
			var.Set(var.Dim(),n[i]);
		}
		if ((float)n[i][1]*mm>(float)n[i][VAR_COL])
			cnt_real++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_HS])
			cnt_hs++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_VC])
			cnt_vc++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_HS_EWMA])
			cnt_hs_ewma++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_HS_GARCH])
			cnt_hs_garch++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_VC_GARCH])
			cnt_vc_garch++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_VC_EWMA])
			cnt_vc_ewma++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_VC_GARCH_T])
			cnt_vc_garch_t++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_HS_GARCH_T])
			cnt_hs_garch_t++;
		if ((float)n[i][1]*mm>(float)n[i][VAR_EVT])
			cnt_evt++;
	}
	a_gp/=(float)(end-start);
	a_vc/=(float)(end-start);
	a_hs/=(float)(end-start);
	a_vc_ewma/=(float)(end-start);
	a_vc_garch/=(float)(end-start);
	a_vc_garch_t/=(float)(end-start);
	a_hs_ewma/=(float)(end-start);
	a_hs_garch/=(float)(end-start);
	a_hs_garch_t/=(float)(end-start);
	a_evt/=(float)(end-start);


	std::stringstream stream;

/*We calculate the waiting time distribution function*/
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
		}
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
	float d_max=0.0;
	stringstream file;
	file << directory << "/wt_cdf_" << generation << ".tsv";
	wt_cdf.Write(file.str());
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
		chi_sq+=pow(cbins-cexp,2.f)/cexp;
	}

	stream << "d_max = " << d_max << ", n = " << wt_cdf.Dim() << ", c = " << 1.36/sqrt((float)wt_cdf.Dim()) << "\n";
	stream << "chi_sq = " << chi_sq << ", m = " << n_bins-1 << "\n";


	stream << "# Program: " << Print() << "\n"; 
	stream << "#" <<  "Avg sigma:" << x4 << "\n";
	stream << "#" <<  "Fitness:" << Fitness() << "\n";
	stream << "#" <<  "VaR prediction:" << VaR << "\n";
	stream << "#" <<  "Expected violations:" << (end-start)*VAR_LEVEL << ", observed genetic violations:" << cnt << "\n";
	stream << "#" <<  "Expected violations:" << (end-start)*VAR_LEVEL << ", observed real violations:" << cnt_real << "\n";
	stream << "#" <<  "HS violations      :" << cnt_hs << "\n";
	stream << "#" <<  "VC violations      :" << cnt_vc << "\n";
	stream << "#" <<  "HS-GARCH violations      :" << cnt_hs_garch << "\n";
	stream << "#" <<  "VC-GARCH violations      :" << cnt_vc_garch << "\n";
	stream << "#" <<  "HS-EWMA violations       :" << cnt_hs_ewma << "\n";
	stream << "#" <<  "VC-EWMA violations       :" << cnt_vc_ewma << "\n";
	stream << "#" <<  "HS-GARCH-t violation     :" << cnt_hs_garch_t << "\n";
	stream << "#" <<  "VC-GARCH-t violations    :" << cnt_vc_garch_t << "\n";
	stream << "#" <<  "EVT violations           :" << cnt_evt << "\n";
	stream << "#GP Average value             :" << a_gp << "\n";
	stream << "#VC Average value             :" << a_vc << "\n";
	stream << "#HS Average value             :" << a_hs << "\n";
	stream << "#VC-GARCH Average value       :" << a_vc_garch << "\n";
	stream << "#HS-GARCH Average value       :" << a_hs_garch << "\n";
	stream << "#VC-GARCH-t Average value     :" << a_vc_garch_t << "\n";
	stream << "#HS-GARCH-t Average value     :" << a_hs_garch_t << "\n";
	stream << "#VC-EWMA Average value        :" << a_vc_ewma << "\n";
	stream << "#HS-EWMA Average value        :" << a_hs_ewma << "\n";
	stream << "#EVT Average value            :" << a_evt << "\n";
	n.SetHeader(stream.str());
	cout << stream.str();
	stream.str("");
	stream << directory << "/best_" << generation << ".tsv";
	n.Write(stream.str());
}

Matrix calculate_hs(Matrix n,int col,int target,int start)
{
	unsigned int i,j;
	vector<float> realizations,popper;
	make_heap(realizations.begin(),realizations.end());
	for(i=start;i<n.Dim();i++)
	{
		realizations.push_back((float)n[i][col]);
		push_heap(realizations.begin(),realizations.end());
		popper.clear();
		if (VAR_LEVEL*i>0)
		{
			for(j=0;j<=VAR_LEVEL*i && realizations.size()>0;j++)
			{
				pop_heap(realizations.begin(),realizations.end());
				popper.push_back(realizations.at(realizations.size()-1));
				realizations.pop_back();
			}
			n.Set(i,target,popper.at(popper.size()-1));//We set the HS VaR
		}
		else
		{
			pop_heap(realizations.begin(),realizations.end());
			n.Set(i,target,realizations.at(realizations.size()-1));//We set the HS VaR
		}
		for(j=0;j<popper.size();j++)
		{
			realizations.push_back(popper.at(j));
			push_heap(realizations.begin(),realizations.end());
		}
	}
	return n;
}

void VaRProgram::Setup(std::string directory,vector<std::string> params)
{
	static bool Initialized=false;
	if (Initialized==true)
		return;
	cout << "Setting up VaR program...\n";
	Initialized=true;
	Logger *l=l->i();
	stringstream stream;
	stream << "Fitting VaR on " << PERCENT*100 << " % of the available data...\n";
	l->Script(stream.str());
	stream.str("");
	unsigned int i,j,k,h;
//	cout << "Student-T:" << STAT::inv_student_t_distribution(STAT::student_t_distribution(-3.43221,2.0),2.0) << "\n";
	char str[1024];
	avg_vol=0.0;
	float x,y,p,p_me,sigma2,sigma2_ma;
	TerminalManager *TM=TM->i();
	//m=STAT::GARCH_extreme_sample(1e4,0.1,1.0,0.9,2e-2);
	float alpha_0,alpha_1,beta_1;
	alpha_0=1.0;
	alpha_1=0.0;//0.9
	beta_1=0.0;//2e-2
	m=STAT::gauss_sample(1e4,0.f,alpha_0,alpha_1,beta_1);
	stream << "Generating a GARCH(1,1) normal sample with alpha_0 = " << alpha_0 << ", alpha_1 = " << alpha_1 << ", beta_1 = " << beta_1 << "\n";
	unsigned int cnt_1,cnt_5,cnt_01;	
	cnt_1=0;
	cnt_5=0;
	cnt_01=0;
	for(i=0;i<m.Dim();i++)
	{
		if ((float)m[i][1]>(float)m[i][2])
			cnt_5++;
		if ((float)m[i][1]>(float)m[i][3])
			cnt_1++;
		if ((float)m[i][1]>(float)m[i][4])
			cnt_01++;
	}
	cout << "0.1 % VaR Counts:" << cnt_01 << ", expected:" << m.Dim()*0.001 << "\n";
	cout << "1.0 % VaR Counts:" << cnt_1 << ", expected:" << m.Dim()*0.01 << "\n";
	cout << "5.0 % VaR Counts:" << cnt_5 << ", expected:" << m.Dim()*0.05 << "\n";
	stream << "0.1 % VaR Counts:" << cnt_01 << ", expected:" << m.Dim()*0.001 << "\n";
	stream << "1.0 % VaR Counts:" << cnt_1 << ", expected:" << m.Dim()*0.01 << "\n";
	stream << "5.0 % VaR Counts:" << cnt_5 << ", expected:" << m.Dim()*0.05 << "\n";
	unsigned int off=0;
	sigma_tot=0.0;
	mu_tot=0.0;
	max_tot=0.0;
	min_tot=0.0;
	unsigned end=2001;
	int dcol=0;
	if (params.size()>=3) 
	{
		m.Load(params[0],params[2],NULL);
	}
	else if (params.size()>1)
	{
		m.Load(params[0]);
	}
	if (params.size()>1)
		dcol=(int)atof(params[1].data());
	float alpha=0.90;
	float alpha_vol=0.99;
	float av_vol=0.0;
	float ma_vol=0.0;
	float n_int=0.0;
	for(i=0;i<m.Dim();i++)
	{
		n.Set(i,0,i);//Time
		if (params.size()<2)
		{
			n_int+=(float)m[i][1];
			n.Set(i,COL_INT_N,n_int);
			n.Set(i,1,(float)m[i][1]);//Time series value
		//We push the value to the heap
			n.Set(i,8,(float)m[i][2]);//95 % VaR
			n.Set(i,9,(float)m[i][3]);//99 % VaR
			n.Set(i,10,(float)m[i][4]);//99.9 % VaR
			n.Set(i,12,(float)m[i][5]);
		}
		else
		{
			if (i>0)
			{
				n.Set(i,1,-((float)m[m.Dim()-i-2][dcol]-(float)m[m.Dim()-i-1][dcol])/((float)m[m.Dim()-i-1][dcol]));
				n.Set(i,COL_INT_N,m[m.Dim()-i-1][dcol]);
				if (i==1)
					ma_vol=(float)m[m.Dim()-i][dcol-1];
				else
					ma_vol=ma_vol*alpha_vol+(1.f-alpha_vol)*(float)m[m.Dim()-i][dcol-1];
				n.Set(i,COL_VOL,(float)m[m.Dim()-i-1][dcol-1]/ma_vol);//the trading volume!
			}
			else
				n.Set(i,1,0.f);
		}
		if(i<end)
			mu_tot+=(float)n[i][1];
		if (i==0)
		{
			max_tot=(float)n[i][1];
			min_tot=(float)n[i][1];
		}
		else if (i<end)
		{
			if ((float)n[i][1]>max_tot)
				max_tot=(float)n[i][1];
			if ((float)n[i][1]<min_tot)
				min_tot=(float)n[i][1];
		}
	}
	av_vol/=(float)m.Dim();
	mu_tot/=(float)end;
	float u=max_tot-(max_tot-min_tot)*0.5;//This is our u.
	float ma=0.0;
	for(i=0;i<n.Dim();i++)
	{
		ma=0.9*ma+0.1*(float)n[i][1];
		n.Set(i,11,ma);
		if (i<end)
			sigma_tot+=pow((float)n[i][1]-mu_tot,2.f);
	}
	sigma_tot/=(float)end;

	cout << "Avg sigma = " << sigma_tot << "\n";
	cout << "Avg value = " << mu_tot << "\n";
	
	stream << "Avg sigma = " << sigma_tot << "\n";
	stream << "Avg value = " << mu_tot << "\n";


	unsigned int period=10;
	unsigned int offset=1;
	float avg,min,max,mu;
	float weight,factor;
	float sigma2_tot=0.0;

	float vc_unc=mu_tot+STAT::inv_normal(1.-VAR_LEVEL)*sqrt(sigma_tot);
	avg=(float)n[0][1];
	sigma2_ma=pow((float)n[0][1],2.f);
	mu=(float)n[0][1];
	for(i=1;i<n.Dim();i++)
	{
		n.Set(i,2,avg);

		sigma2=0.0;

		max=n[i-period][1];
		min=n[i-period][1];

		for(j=i-period>=0?i-period:0;j<i;j++)
		{
			if ((float)n[j][1]>max)
				max=n[j][1];
			if ((float)n[j][1]<min)
				min=n[j][1];

			sigma2+=pow((float)n[j][1]-mu/(float)i,2);//Should we take the moving average as E(X)??? (instead of mu_tot)
		}
		sigma2/=i>=period?(float)period-1.f:(float)i-1.f;
		n.Set(i,3,sqrt(sigma2));//Sample sigma
		n.Set(i,4,min);//Sample minimum
		n.Set(i,5,max);//Sample maximum
		n.Set(i,VAR_VC,mu/(float)i+STAT::inv_normal(1.-VAR_LEVEL)*sqrt(sigma2_tot/(float)i));//Unconditional variance-covariance VaR estimate
		n.Set(i,16,sqrt(sigma2_ma));//Sample sigma
		n.Set(i,18,((float)n[i][1]-mu/(float)i)/sqrt(sigma2));//normal sigma
		n.Set(i,19,((float)n[i][1]-avg)/sqrt(sigma2_ma));//moving average
		n.Set(i,25,mu/(float)i);
		n.Set(i,30,sqrt(sigma2_tot/(float)i));

		//We update the values
		sigma2_ma=alpha*sigma2_ma+(1.-alpha)*pow((float)n[i][1]-avg,2.f);
		avg=alpha*avg+(1.-alpha)*(float)n[i][1];
		mu+=(float)n[i][1];
		sigma2_tot+=pow((float)n[i][1]-mu/(float)i,2.f);
	}
#ifdef _CALCULATE_HURST_EXP
	Matrix hurst;
	int max_d=100;
	int col_hurst=COL_INT_N;
	for(i=max_d;i<n.Dim();i++)
	{
		hurst.Clear();
		cout << "i = " << i << "\n";
		int d;
		for(d=max_d/2;d<max_d;d++)
		{
			float sigma_hurst=0.0;
			float mu_hurst=0.0;
			for(j=i-d;j<i;j++)
				mu_hurst+=(float)n[j][col_hurst];
			mu_hurst/=(float)d;
			for(j=i-d;j<i;j++)
				sigma_hurst+=pow((float)n[j][col_hurst]-mu_hurst,2.f);
			sigma_hurst/=(float)d;
			float max_r,min_r;
			float y_hurst;
			for(j=1;j<d;j++)
			{
				y_hurst=0.0;
				for(k=0;k<j;k++)
					y_hurst+=(float)n[i-d+k][col_hurst];
				y_hurst-=mu_hurst*(float)(j-1);
				if (j==1)
				{
					max_r=y_hurst;
					min_r=y_hurst;
				}
				else
				{
					if (y_hurst>max_r)
						max_r=y_hurst;
					if (y_hurst<min_r)
						min_r=y_hurst;
				}
			}
			float h_coeff=log((max_r-min_r)/sqrt(sigma_hurst));
			if (h_coeff!=h_coeff || h_coeff < -1e10 || h_coeff> 1e10);
			else
			{
				hurst.Set(hurst.Dim(),0,log((float)d));
				hurst.Set(hurst.Dim()-1,1,h_coeff);
			}
		}
		float hurst_exp=0.0;
		float x_mean,y_mean;
		x_mean=0.0;
		y_mean=0.0;
		for(j=0;j<hurst.Dim();j++)
		{
			x_mean+=(float)hurst[j][0];
			y_mean+=(float)hurst[j][1];
		}
		x_mean/=(float)hurst.Dim();
		y_mean/=(float)hurst.Dim();
		float p1,p2;
		p1=0.0;
		p2=0.0;
		for(j=0;j<hurst.Dim();j++)
		{	
			p1+=((float)hurst[i][0]-x_mean)*((float)hurst[i][1]-y_mean);
			p2+=pow((float)hurst[i][0]-x_mean,2.f);
		}
		stringstream stream;
		stream << directory << "/hurst.tsv";
		hurst.Write(stream.str());
		hurst_exp=p1/p2;
		cout << "hurst = " << hurst_exp << "\n";
		n.Set(i,COL_HURST,hurst_exp);
	}

#endif

	float *vy_evt=NR::vector(1,3);
	float **mm_evt=NR::matrix(1,3,1,2);
    int nfunc=0;


	float *vy=NR::vector(1,4);
	float **mm=NR::matrix(1,4,1,3);

	float *vy_t=NR::vector(1,5);
	float **mm_t=NR::matrix(1,5,1,4);

	/*We fit a GARCH(1,1) process to the sample variance*/
	for(int y=1;y<=4;y++)
    {
		mm[y][1]=sigma_tot*(1.0+(float)(rand()%100-rand()%100)/1000.0);
        mm[y][2]=1.0+(float)(rand()%100-rand()%100)/1000.0;
        mm[y][3]=1.0+(float)(rand()%100-rand()%100)/100.0;
    }

	nfunc=0;
	
	for(int x=1;x<=4;x++)
		vy[x]=garch11(mm[x]);

	//We first fit the GARCH(1,1) model to the sample variance to get some first parameter estimates.
//	NR::amoeba(mm,vy,3,1e-3,garch11_ols,&nfunc);
//	cout << "OLS fit complete...\n";
  //  for(int x=1;x<=4;x++)
//		vy[x]=garch11(mm[x]);
	NR::amoeba(mm,vy,3,1e-5,garch11,&nfunc);
    cout << "Amoeba converged!\n";
	cout << "alpha_0 = " << mm[1][1] << "\n";
	cout << "beta_1 = " << mm[1][2] << "\n";
	cout << "alpha_1  = " << mm[1][3] << "\n";
	cout << "SSR:" << (garch_ssr=garch11(mm[1])) << "\n";

	stream << "Amoeba converged!\n";
	stream << "alpha_0 = " << mm[1][1] << "\n";
	stream << "beta_1 = " << mm[1][2] << "\n";
	stream << "alpha_1  = " << mm[1][3] << "\n";
	stream << "SSR:" << (garch_ssr=garch11(mm[1])) << "\n";

	nfunc=0;

	for(int y=1;y<=5;y++)
    {
		mm_t[y][1]=sigma_tot*(1.0+(float)(rand()%100-rand()%100)/1000.0);
		mm_t[y][2]=1.0+(float)(rand()%100-rand()%100)/1000.0;
		mm_t[y][3]=1.0+(float)(rand()%100-rand()%100)/100.0;
        mm_t[y][4]=12.0+(float)(rand()%100-rand()%100)/100.0;
    }

	for(int x=1;x<=5;x++)
		vy_t[x]=garch11_t(mm_t[x]);


	NR::amoeba(mm_t,vy_t,4,1e-5,garch11_t,&nfunc);
    cout << "Amoeba(t) converged!\n";
	cout << "alpha_0 = " << mm_t[1][1] << "\n";
	cout << "beta_1 = " << mm_t[1][2] << "\n";
	cout << "alpha_1  = " << mm_t[1][3] << "\n";
	cout << "nu  = " << mm_t[1][4] << "\n";
	cout << "SSR:" << (garch_ssr=garch11_t(mm_t[1])) << "\n";

	stream << "Amoeba(t) converged!\n";
	stream << "alpha_0 = " << mm_t[1][1] << "\n";
	stream << "beta_1 = " << mm_t[1][2] << "\n";
	stream << "alpha_1  = " << mm_t[1][3] << "\n";
	stream << "nu  = " << mm_t[1][4] << "\n";
	stream << "SSR:" << (garch_ssr=garch11_t(mm_t[1])) << "\n";

	float inv_norm=STAT::inv_normal(1.-VAR_LEVEL);
	float inv_t=STAT::inv_student_t_distribution(1.-VAR_LEVEL,mm_t[1][4])/(mm_t[1][4]/(mm_t[1][4]-2.f));

	cout << "VaR t-value:" << inv_t << "\n";
	n.Set(0,6,0.0);
	for(i=0;i<n.Dim();i++)
	{
		n.Set(i,6,sqrt(mm[1][1]*0+(float)mm[1][2]*pow((float)((float)n[i-1][1]),2.0f)+(float)mm[1][3]*pow((float)n[i-1][6],2.f)));//sigma GARCH(1,1)
		if ((float)n[i][1]>u)
		{
			evt.Set(evt.Dim(),0,evt.Dim());
			evt.Set(evt.Dim()-1,1,((float)n[i][1]-u-(float)n[i][25])/(float)n[i][6]);
		}
		n.Set(i,26,(sqrt(mm_t[1][1]*0+(float)mm_t[1][2]*pow((float)((float)n[i-1][1]),2.0f)+(float)mm_t[1][3]*pow((float)n[i-1][26],2.f))));//sigma GARCH-t(1,1)
		n.Set(i,VAR_VC_GARCH,(float)n[i][25]+(float)n[i][6]*inv_norm);//VC GARCH
		n.Set(i,VAR_VC_GARCH_T,(float)n[i][25]+(float)n[i][26]*inv_t);//VC GARCH-t
		n.Set(i,VAR_VC_EWMA,(float)n[i][2]+(float)n[i][16]*inv_norm);//VC EWMA
		n.Set(i,22,((float)n[i][1]-(float)n[i][25])/(float)n[i][6]);//normalized entries for HS-GARCH
		n.Set(i,29,((float)n[i][1]-(float)n[i][25])/(float)n[i][26]);//normalized entries for HS-GARCH-t
	}


	nfunc=0;

	for(int y=1;y<=3;y++)
    {
		mm_evt[y][1]=1.0+(rand()%100-rand()%100)/1000.0;
        mm_evt[y][2]=1.0+(float)(rand()%100-rand()%100)/100.0;
    }

	for(int x=1;x<=3;x++)
		vy_evt[x]=evt_ll(mm_evt[x]);

	NR::amoeba(mm_evt,vy_evt,2,1e-5,evt_ll,&nfunc);

	cout << "Extreme value estimates:\n";
	cout << "beta = " << mm_evt[1][1] << "\n";
	cout << "xi   = " << mm_evt[1][2] << "\n";
	stream << "Extreme value estimates:\n";
	stream << "beta = " << mm_evt[1][1] << "\n";
	stream << "xi   = " << mm_evt[1][2] << "\n";

	n=calculate_hs(n,1,VAR_HS,period);//HS
	n=calculate_hs(n,18,20,period);
	n=calculate_hs(n,19,VAR_HS_EWMA,period);
	n=calculate_hs(n,22,VAR_HS_GARCH,period);
	n=calculate_hs(n,29,VAR_HS_GARCH_T,period);
	float es_factor,es_const;
	es_const=(mm_evt[1][1]-mm_evt[1][2]*u)/(1.f-mm_evt[1][2]);
	es_factor=1.f/(1.f-mm_evt[1][2]);
	float evt_factor=mm_evt[1][1]/mm_evt[1][2]*(pow((float)(VAR_LEVEL)*(float)n.Dim()/(float)evt.Dim(),-(float)mm_evt[1][2])-1);
	cout << "EVT-factor:" << evt_factor << "\n";
	cout << "ES-factor:" << es_factor << "\n";
	cout << "ES-Constant:" << es_const << "\n";
	for(i=0;i<n.Dim();i++)
	{
		n.Set(i,VAR_EVT,u+(float)n[i][26]*evt_factor);//EVT GARCH-t
		n.Set(i,ES_EVT,(float)n[i][VAR_EVT]*es_factor);//EVT GARCH-t
		n.Set(i,20,(float)n[i][20]*(float)n[i][3]);//HS - normal sigma
		n.Set(i,VAR_HS_EWMA,(float)n[i][VAR_HS_EWMA]*(float)n[i][16]);//HS-EWMA
		n.Set(i,VAR_HS_GARCH,(float)n[i][VAR_HS_GARCH]*(float)n[i][6]);//HS-GARCH
		n.Set(i,VAR_HS_GARCH_T,(float)n[i][VAR_HS_GARCH_T]*(float)n[i][26]);//HS-GARCH-t
	}
	srand(time(NULL));
	
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
	TM->Add("volume",&volume);

	//If the fourth parameter is equal to 1, we add the conventional VaR estimates as terminals.
	if (params.size()>=4)
		if (atof(params[3].data())==1)
		{
			TM->Add("VaR_vc",&var_vc);
			TM->Add("VaR_hs",&var_hs);
			TM->Add("VaR_vc_garch",&var_vc_garch);
			TM->Add("VaR_hs_garch",&var_hs_garch);
			TM->Add("VaR_vc_garcht",&var_vc_garcht);
			TM->Add("VaR_hs_garcht",&var_hs_garcht);
			TM->Add("VaR_vc_ewma",&var_vc_ewma);
			TM->Add("VaR_hs_ewma",&var_hs_ewma);
			TM->Add("VaR_evt",&var_evt);
		}
	x4=sigma_tot;
	x5=mu_tot;
	x6=max_tot;
	x7=min_tot;

	l->PlainScript(stream.str());

	stream.str("");

	stream << directory << "/evt.tsv";

	evt.Write(stream.str());

	stream.str("");

	stream << directory << "/matrix.tsv";

	n.Write(stream.str());

	cout << "VaR program ready...\n";
}
