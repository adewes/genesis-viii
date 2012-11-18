#include "copula_program.h"
#include "../utils/statistics.h"
#include "../utils/nr/nr.h"
#include "../gp/leaf.h"
#include "../gp/distribution_branch.h"
Matrix CopulaProgram::c,CopulaProgram::d,CopulaProgram::cdf_a,CopulaProgram::cdf_b,CopulaProgram::stocks;
float CopulaProgram::px[100];
unsigned int CopulaProgram::dim;

#include<algorithm>
#include <sstream>

#define DU 0.01
#define DX 0.01

CopulaProgram::CopulaProgram() : Program()
{
	dim=2;
}

Node *CopulaProgram::RandomNode(int depth,unsigned int type)
{
	Node *n;
	n=new DistributionBranch();
	n->Random(depth);
	return n;	
}

CopulaProgram::~CopulaProgram()
{
}

float power_copula(float *x,unsigned int d,float *p)
{
	float value=1.0;
	for(unsigned int i=0;i<2;i++)
		value*=pow(x[i],fabs(p[i]));
	return value;
}

float normal_mlp(float *x)
{
	int i;
	float c1,c2,c3,c4;
	float p=0.0;
	for(i=0;i<(float)CopulaProgram::c.Dim();i++)
	{
		c1=STAT::normal_copula((float)CopulaProgram::c[i][0]-DU/2.f,(float)CopulaProgram::c[i][1]-DU/2.f,x[1]);
		c2=STAT::normal_copula((float)CopulaProgram::c[i][0]-DU/2.f+DU,(float)CopulaProgram::c[i][1]-DU/2.f,x[1]);
		c3=STAT::normal_copula((float)CopulaProgram::c[i][0]-DU/2.f,(float)CopulaProgram::c[i][1]-DU/2.f+DU,x[1]);
		c4=STAT::normal_copula((float)CopulaProgram::c[i][0]-DU/2.f+DU,(float)CopulaProgram::c[i][1]-DU/2.f+DU,x[1]);
		if ((c1+c4-c2-c3)/DU/DU>0)
			p+=log((c1+c4-c2-c3)/DU/DU);
		else
			p-=1.0;
	}
	cout << " p =  " << p << "\n";
	return p/(float)CopulaProgram::c.Dim();
}

float gumbel_mlp(float *x)
{
	int i;
	float c1,c2,c3,c4;
	float p=0.0;
	for(i=0;i<(float)CopulaProgram::c.Dim();i++)
	{
		c1=STAT::gumbel_copula((float)CopulaProgram::c[i][0],(float)CopulaProgram::c[i][1],x[1]);
		c2=STAT::gumbel_copula((float)CopulaProgram::c[i][0]+DU,(float)CopulaProgram::c[i][1],x[1]);
		c3=STAT::gumbel_copula((float)CopulaProgram::c[i][0],(float)CopulaProgram::c[i][1]+DU,x[1]);
		c4=STAT::gumbel_copula((float)CopulaProgram::c[i][0]+DU,(float)CopulaProgram::c[i][1]+DU,x[1]);
		p+=log((c1+c4-c2-c3)/DU/DU);
	}
	return p/(float)CopulaProgram::c.Dim();
}

float frank_mlp(float *x)
{
	int i;
	float c1,c2,c3,c4;
	float p=0.0;
	for(i=0;i<(float)CopulaProgram::c.Dim();i++)
	{
		c1=STAT::frank_copula((float)CopulaProgram::c[i][0],(float)CopulaProgram::c[i][1],x[1]);
		c2=STAT::frank_copula((float)CopulaProgram::c[i][0]+DU,(float)CopulaProgram::c[i][1],x[1]);
		c3=STAT::frank_copula((float)CopulaProgram::c[i][0],(float)CopulaProgram::c[i][1]+DU,x[1]);
		c4=STAT::frank_copula((float)CopulaProgram::c[i][0]+DU,(float)CopulaProgram::c[i][1]+DU,x[1]);
		p+=log((c1+c4-c2-c3)/DU/DU);
	}
	return p/(float)CopulaProgram::c.Dim();
}

float clayton_mlp(float *x)
{
	int i;
	float c1,c2,c3,c4;
	float p=0.0;
	for(i=0;i<(float)CopulaProgram::c.Dim();i++)
	{
		c1=STAT::clayton_copula((float)CopulaProgram::c[i][0],(float)CopulaProgram::c[i][1],x[1]);
		c2=STAT::clayton_copula((float)CopulaProgram::c[i][0]+DU,(float)CopulaProgram::c[i][1],x[1]);
		c3=STAT::clayton_copula((float)CopulaProgram::c[i][0],(float)CopulaProgram::c[i][1]+DU,x[1]);
		c4=STAT::clayton_copula((float)CopulaProgram::c[i][0]+DU,(float)CopulaProgram::c[i][1]+DU,x[1]);
		p+=log((c1+c4-c2-c3)/DU/DU);
	}
	return p/(float)CopulaProgram::c.Dim();
}

float normal_copula(float *x,unsigned int d,float *p)
{
	return STAT::normal_copula(x[0],x[1],p[0]);
}

float clayton_density(float *x,unsigned int d,float *p)
{
	return STAT::clayton_copula(x[0],x[1],p[0]);
}

float frank_copula(float *x,unsigned int d,float *p)
{
	return STAT::frank_copula(x[0],x[1],p[0]);
}

float gumbel_density(float *x,unsigned int d,float *p)
{
	return STAT::gumbel_copula(x[0],x[1],p[0]);
}

float CopulaProgram::Fitness()
{
	float ll=0.0;
	float u2,x2;
	float v1,v2,v3,v4,p;
	px[0]=0.5;
	px[1]=0.5;
	for(unsigned int i=0;i<c.Dim();i++)
	{
		px[0]=(float)c[i][0]-DU/2.;
		px[1]=(float)c[i][1]-DU/2.;
		if (px[1]<0)
			px[1]=0;
		if (px[0]<0)
			px[0]=0;
		v1=Eval();
		px[0]+=DU;
		v2=Eval();
		px[1]+=DU;
		v4=Eval();
		px[0]-=DU;
		v3=Eval();
		p=(v1+v4-v2-v3)/DU/DU;
		if (p!=p)
		{	
			return log(0.f);
			cout << Print() << "\n";
			cout << v1 << ", " << v2 << ", " << v3 << ", " << v4 << "\n";
			cout << "C= " << Eval() << "\n";
			cout << "x = " << px[0] << ", y = " << px[1] << ", p = " << p << "\n";
		}
		if (p>0)
			ll+=log(p);
		else
		{
			cout << "p = " << p << "\n";
			ll-=1.0;
			return log(0.f);
		}
	}
	return ll/c.Dim()-0.0*log(Size()+1.0);
}

inline Matrix generate_inv_cdf(Matrix c)
{
	
}

inline float cdf(Matrix c,float x)
{
	float u,dx,dist;
	unsigned int i=0;
	return STAT::erf(x);
	while((float)c[i][1]<x*0.05 && i<c.Dim())i++;
	if (i==0)
		return (float)c[i][0];
	dist=(float)c[i][1]-x*0.05;
	dx=(float)c[i][1]-(float)c[i-1][1];
	dist/=dx;
//	cout << "x = " << x << ", returning " << (float)c[i][0] << "\n";
	return (float)c[i-1][0];//*dist+(float)c[i][0]*(1.f-dist);
}

float clayton_dummy(float x,float y,float *params)
{
	return STAT::clayton_copula(x,y,params[0]);
}

float gumbel_dummy(float x,float y,float *params)
{
	return STAT::gumbel_copula(x,y,params[0]);
}

float frank_dummy(float x,float y,float *params)
{
	return STAT::frank_copula(x,y,params[0]);
}

float normal_dummy(float x,float y,float *params)
{
	return STAT::normal_copula(x,y,params[0]);
}

void CopulaProgram::Write(std::string directory,unsigned int generation)
{
	stringstream stream;
//	stream << directory << "/best_" << generation << ".tsv";
//	c.Write(stream.str());
	Matrix m,n;
	float c1,c2,c3,c4,p;
	unsigned int i=0;
	unsigned int j=0;
	cout << "Generating copula density...\n";
	for(float u=0;u<1.0;u+=DU)
		for(float v=0;v<1.0;v+=DU)
		{
			px[0]=u;
			if (px[0]>1)
				px[0]=1;
			if (px[1]>1)
				px[1]=1;
			px[1]=v;
			c1=Eval();
			px[0]-=DU;
			if (px[0]<0)
				px[0]=0;
			c2=Eval();
			px[1]-=DU;
			if (px[1]<0)
				px[1]=0;
			c3=Eval();
			px[0]+=DU;
			c4=Eval();
			m.Set(i,0,u);
			m.Set(i,1,v);
			m.Set(i,2,c1);
			m.Set(i,3,(float)d[i][2]);
			m.Set(i,4,c1-(float)d[i][2]);
			p=(c1+c3-c2-c4)/DU/DU;
			if (fabs(p)<1e3 && p>0 && u>0 && v>0)
				m.Set(i,5,p);
			i++;
		}
	float chi_2=0.0;
	float expected;
	unsigned int row,col;
	//We calculate the chi^2 statistic
	Matrix chi_matrix;
	for(i=0;i<c.Dim();i++)
	{
		row=(float)c[i][0]*10;
		col=(float)c[i][1]*10;
		chi_matrix.Set(row,col,(float)chi_matrix[row][col]+1);
	}
	for(i=0;i<10;i++)
		for(j=0;j<10;j++)
		{
			px[0]=(float)i/10.0;
			px[1]=(float)j/10.0;
			c1=Eval();
			px[0]=(float)i/10.0+0.1;
			px[1]=(float)j/10.0;
			c2=Eval();
			px[0]=(float)i/10.0;
			px[1]=(float)j/10.0+0.1;
			c3=Eval();
			px[0]=(float)i/10.0+0.1;
			px[1]=(float)j/10.0+0.1;
			c4=Eval();
			p=(c4+c1-c2-c3);
			expected=p*c.Dim();
			cout << "i = " << i << ", j = " << j << ", expected: " << expected << ", found:" << (float)chi_matrix[i][j] << "\n";
			chi_2+=pow(expected-(float)chi_matrix[i][j],2.f)/expected;
		}
	cout << "Chi^2 = " << chi_2 << "\n";
	i=0;
	float x,y,p1,p2,p3,p4,pa,pb,dx,dy,du,dv;
	float min_x,max_x,min_y,max_y;
	max_x=c.Max(2);
	min_x=c.Min(2);
	max_y=c.Max(3);
	min_y=c.Min(3);
	dx=(max_x-min_x)/100.f;
	dy=(max_y-min_y)/100.f;
	float offset_x=(max_x-min_x)/4.*0;
	float offset_y=(max_y-min_y)/4.*0;
	cout << "Generating probability density...\n";
	for(x=min_x+offset_x;x<=max_x-offset_x;x+=dx)
		for(y=min_x+offset_y;y<max_y-offset_y;y+=dy)
		{
//			cout << "x = " << x << ", y = " << y << "\n";
			px[0]=STAT::erf(x);//cdf(cdf_a,x);
			px[1]=STAT::erf(y);
			p1=Eval(); 
			pa=px[0];
			pb=px[1];
			du=px[0];
			dv=px[1];
			px[0]=STAT::erf(x+dx);
			du=px[0]-du;
			pa=-(pa-px[0])/dx;
			p2=Eval();
			px[1]=STAT::erf(y+dy);
			dv=px[1]-dv;
			pb=-(pb-px[1])/dy;
			p3=Eval();	
			px[0]=STAT::erf(x);
			p4=Eval();
			p=(p1+p3-p2-p4);//*pa*pb;
			n.Set(i,0,x+dx/2.f);
			n.Set(i,1,y+dy/2.f);
			n.Set(i,2,p);
			i++;
		}
	stream.str("");
	stream << directory << "/density_" << generation << ".tsv";
	n.Write(stream.str());
	stream.str("");
	stream << directory << "/copula_" << generation << ".tsv";
	m.Write(stream.str());
}

void CopulaProgram::Mutate()
{
	std::vector<Node *> Nodes;
	unsigned int r=0;
	if (rand()%2 && 0) 
	{
		cout << "Before mutation:" << Print() << "\n";
		Node *b=RandomNode(1,BRANCH);
		r=rand()%b->Children.size();
		b->ReplaceChild(b->Children[r],Trunk->GetRoot());
		Trunk->SetRoot(b);
		cout << "After mutation :" << Print() << "\n";
		return;
	}
	Nodes=Trunk->Root->GetNodes();
	r=rand()%Nodes.size();
	Node *Mutator=Nodes[r];
	if (Mutator->Parent==NULL)
		return;
//	cout << "Before mutation:" << Print() << ", " << Size() << "\n";
	Mutator->MutateAttributes();
//	cout << "After mutation-:" << Print() << "\n";
}


void CopulaProgram::Shrink()
{
	return;
	unsigned int r,rk;
	unsigned int cnt=0;
	std::vector<Node *> Nodes=Trunk->GetRoot()->GetBranches();
	if (Nodes.size()<=1)
		return;
	r=rand()%Nodes.size();
	while(Nodes[r++%Nodes.size()]->Parent==NULL && cnt++<Nodes.size());
	r%=Nodes.size();
	if (Nodes[r]->Parent==NULL)
		return;
	if (rand()%2 || Nodes[r]->Parent->Parent==NULL)
	{
		Node *l=new Leaf();
		l->Random(0);
		Nodes[r]->Parent->ReplaceChild(Nodes[r],l);
	}
	else
	{
		Node *n=Nodes[r]->Copy();
//		cout << "Before shrink:" << Print() << "\n";
		Nodes[r]->Parent->Parent->ReplaceChild(Nodes[r]->Parent,n);
//		cout << "After shrink :" << Print() << "\n";
	}
}	


Program *CopulaProgram::Crossover(Program *p)
{
	Program *R=Copy();

	Tree *NewTree=p->Trunk->Copy();

	Node *From,*To;

	vector<Node *> nodes;

//	cout << "Crossover between " << Print() << " and " << p->Print() << "\n";
	int type=rand()%2;
	if (type==0 || 1)
	{
		nodes=NewTree->Root->GetBranches();
		From=nodes[rand()%nodes.size()];
		nodes=Trunk->Root->GetBranches();
		To=nodes[rand()%nodes.size()];
		//We select two branches for crossover
	}
	else
	{
		nodes=NewTree->Root->GetLeafs();
		From=nodes[rand()%nodes.size()];
		nodes=Trunk->Root->GetLeafs();
		To=nodes[rand()%nodes.size()];
		//We select two leafs for crossover
	}
//	cout << "From:" << From->Print() << "\n";
//	cout << "To  :" << To->Print() << "\n";
	if (From==NewTree->Root)
	{
		delete NewTree->Root;
		NewTree->Root=To->Copy();
		NewTree->Root->Parent=NULL;
	}
	else
		NewTree->Replace(NewTree->Root,From,To);
	R->SetTrunk(NewTree);
//	cout << "Template:" << Print() << "\n";
//	cout << "Original:" << p->Print() << "\n";
//	cout << "New     :" << R->Print() << "\n";
	return R;
}

float mix(float alpha,float a,float b)
{
	return alpha*a+(1.f-alpha)*b;
}


float my_copula(float u,float v)
{
	return mix(0.5,STAT::normal_copula(u,v,-0.7),mix(0.4,STAT::gumbel_copula(u,v,4.0),STAT::clayton_copula(u,v,2.0)));
}

float my_copula_dummy(float u,float v,float *p)
{
	return my_copula(u,v);
}

Value importer(char *str,unsigned int col)
{
	Value v;
	int year,month,day;
	if (col==0)
	{
//		sscanf(str,"%.4d-%.2d-%.2d",&year,&month,&day);
//		v=(float)((year-1900)*365+month*31+day);
		v=(std::string)str;
	}
	else
		v=atof(str);
	return v;
}

void CopulaProgram::Setup(std::string directory,vector<std::string> params)
{
	static bool Initialized=false;
	if (Initialized==true)
		return;
#ifdef _GENERATE_EXAMPLES
	float pm[1];
	pm[0]=0.0;
	Matrix cp=STAT::generate_copula_density(DU,normal_dummy,pm);
	stringstream str;
	str << directory << "/normal_density_rho=" << pm[0] << ".tsv";
	cp.Write(str.str());

	pm[0]=0.0;
	cp=STAT::generate_copula_density(DU,frank_dummy,pm);
	str.str("");
	str << directory << "/frank_density_theta=" << pm[0] << ".tsv";
	cp.Write(str.str());

	pm[0]=0.5;
	cp=STAT::generate_copula_density(DU,gumbel_dummy,pm);
	str.str("");
	str << directory << "/gumbel_density_theta=" << pm[0] << ".tsv";
	cp.Write(str.str());

	pm[0]=0.0;
	cp=STAT::generate_copula_density(DU,clayton_dummy,pm);
	str.str("");
	str << directory << "/clayton_density_theta=" << pm[0] << ".tsv";
	cp.Write(str.str());

	cout << "Done generating densities...\n";
#endif
	Initialized=true;
	TerminalManager *TM=TM->i();
	TM->Clear();
	DistributionBranch b;
	b.AddDistribution("normal",1,normal_copula);
//	b.AddDistribution("power",2,power_copula);
	b.AddDistribution("clayton",1,clayton_density);
	b.AddDistribution("frank",1,frank_copula);
	b.AddDistribution("gumbel",1,gumbel_density);

	if (params.size()<2)
	{
		cout << "Generating dummy sample...\n";
		stringstream stream;
		float pm[1];
		Matrix cd=STAT::generate_copula_density(DU,my_copula_dummy,pm);
		stream.str("");
		stream << directory << "/copula_density.tsv";
		cd.Write(stream.str());

		c=STAT::copula_density(2000,my_copula,0.01);
		stream.str("");
		stream << directory << "/copula.tsv";
		c.Write(stream.str());
		return;
	}

	float u,v;
	unsigned int cnt=0;

	Matrix n;
	Matrix stock_a,stock_b;

	cout << "Loading matrix A from " << params[0] << "\n";
	cout << "Loading matrix B from " << params[1] << "\n";

	stock_a.Load(params[0],",",importer);
	stock_b.Load(params[1],",",importer);

	unsigned int i,j;

	float da,db;

	cnt=0;

	vector<float> values_a,values_b;

	make_heap(values_a.begin(),values_a.end());
	make_heap(values_b.begin(),values_b.end());

	Matrix swap;
	if (stock_a.Dim()>stock_b.Dim())
	{	
		swap=stock_a;
		stock_a=stock_b;
		stock_b=swap;
	}
	int data_col=6;
	int last_j=0;
	for(i=2;i<stock_a.Dim();i++)
	{
		cout << "i = " << i << "\r";
		j=last_j;
		while((std::string)stock_b[j%stock_b.Dim()][0]!=(std::string)stock_a[i][0] && j-last_j<=stock_b.Dim())
			j++;
		last_j=j%stock_b.Dim();
		if ((std::string)stock_b[j%stock_b.Dim()][0]==(std::string)stock_a[i][0] && (std::string)stock_b[(j-1)%stock_b.Dim()][0]==(std::string)stock_a[i-1][0])
		{
			stocks.Set(cnt,0,stock_a[i][0]);
			stocks.Set(cnt,1,cnt);
			da=((float)stock_a[i][data_col]-(float)stock_a[i-1][data_col])/(float)stock_a[i][data_col];
			db=((float)stock_b[j%stock_b.Dim()][data_col]-(float)stock_b[((j-1)%stock_b.Dim())][data_col])/(float)stock_b[j%stock_b.Dim()][data_col];
			if (da!=0 && db!=0)//we select only non-zero realizations!
			{
				stocks.Set(cnt,2,da);
				stocks.Set(cnt,3,db);
				stocks.Set(cnt,4,(float)stock_a[i][data_col]);
				stocks.Set(cnt,5,(float)stock_b[j%stock_b.Dim()][data_col]);
				values_a.push_back((float)stocks[cnt][2]);
				push_heap(values_a.begin(),values_a.end());
				values_b.push_back((float)stocks[cnt][3]);
				push_heap(values_b.begin(),values_b.end());
				cnt++;
			}
		}
	}
	//We generate the CDF's
	int s=values_a.size();
	for(i=0;i<s;i++)
	{
		pop_heap(values_a.begin(),values_a.end());
		cdf_a.Set(s-i-1,0,(float)(s-1-i)/((float)s)+1.f/2.f/(float)s);
		cdf_a.Set(s-i-1,1,values_a[values_a.size()-1]);
		values_a.pop_back();
		pop_heap(values_b.begin(),values_b.end());
		cdf_b.Set(s-i-1,0,(float)(s-1-i)/((float)s)+1.f/2.f/(float)s);
		cdf_b.Set(s-i-1,1,values_b[values_b.size()-1]);
		values_b.pop_back();
	}
	//We generate the copula
	for(i=0;i<stocks.Dim();i++)
	{
		float va,vb;
		va=stocks[i][2];
		vb=stocks[i][3];
		j=0;
		while((float)cdf_a[j][1]<va && j<cdf_a.Dim())j++;
		stocks.Set(i,6,cdf_a[j][0]);
		j=0;
		while((float)cdf_b[j][1]<vb && j<cdf_b.Dim())j++;
		stocks.Set(i,7,cdf_b[j][0]);
	}

	stringstream stream;
	stream.str("");
	stream << "#" << "Matrix A = " << params[0] << "\n";
	stream << "#" << "Matrix B = " << params[1] << "\n";
	stocks.SetHeader(stream.str());
	stream.str("");
	stream << directory << "/stocks.tsv";
	stocks.Write(stream.str());
	stream.str("");
	stream << directory << "/cdf_a.tsv";
	cdf_a.Write(stream.str());
	stream.str("");
	stream << directory << "/cdf_b.tsv";
	cdf_b.Write(stream.str());
	float x,y,p1,p2,p3,p4,p;
	float min_x,max_x,min_y,max_y;
	c.Clear();

	for(i=0;i<stocks.Dim();i++)
	{
		c.Set(i,0,stocks[i][6]);
		c.Set(i,1,stocks[i][7]);
		c.Set(i,2,stocks[i][2]);
		c.Set(i,3,stocks[i][3]);
	}

	stream.str("");
	stream << directory << "/copula.tsv";
	c.Write(stream.str());

	max_x=c.Max(2);
	min_x=c.Min(2);
	max_y=c.Max(3);
	min_y=c.Min(3);

	i=0;
	for(x=min_x;x<=max_x;x+=(max_x-min_x)/200.f)
		for(y=min_x;y<max_y;y+=(max_y-min_y)/200.f)
		{

			px[0]=STAT::erf(x);	
			px[1]=STAT::erf(y);
			p1=my_copula(px[0],px[1]);
			px[0]=STAT::erf(x+DX);
			p2=my_copula(px[0],px[1]);
			px[1]=STAT::erf(y+DX);
			p3=my_copula(px[0],px[1]);
			px[0]=STAT::erf(x);
			p4=my_copula(px[0],px[1]);
			p=(p1+p3-p2-p4)/DX/DX*STAT::normal(x)*STAT::normal(y);
			n.Set(i,0,x);
			n.Set(i,1,y);
			n.Set(i,2,p);
			i++;
		}
	stream.str("");
	stream << directory << "/density_real.tsv";
	n.Write(stream.str());

//We generate MLE estimates to compare our models with standard cases (Frank, Gumbel, Clayton, Normal)
	
	int nfunc=0;


	float *vy=NR::vector(1,2);
	float **mm=NR::matrix(1,2,1,1);

	for(int y=1;y<=2;y++)
    {
		mm[y][1]=(float)(rand()%1000-rand()%1000)/100.0;
    }

	for(int x=1;x<=2;x++)
		vy[x]=normal_mlp(mm[x]);

	NR::amoeba(mm,vy,1,1e-5,normal_mlp,&nfunc);

	cout << "MLE estimate normal-rho:" << mm[1][1] << "\n";

}
