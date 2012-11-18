#include "distribution_branch.h"
#include "leaf.h"
#include "copula_program.h"

#include <sstream>
vector<Distribution> DistributionBranch::Distributions;

DistributionBranch::DistributionBranch()
{
	Distri=-1;
	if (Distributions.size()==0)
	{
		//Add distributions here...
	}
}

Node *DistributionBranch::CopyAttributes()
{
	DistributionBranch *NewBranch=new DistributionBranch;
	*NewBranch=*this;
	return (Node *)NewBranch;
}


void DistributionBranch::AddDistribution(std::string name,unsigned int parameters,float (*f)(float *x,unsigned int d,float *p))
{
	Distribution d;
	d.name=name;
	d.parameters=parameters;
	d.f=f;
	Distributions.push_back(d);
}

float DistributionBranch::Eval()
{
	if (Children.size()==0)
		return 0.f;
	if (Op==OP_MIX)
	{
		float alpha=Children.at(0)->Eval();
		if (Children.size()<3)
			return 0.f;
		return alpha*Children.at(1)->Eval()+(1.f-alpha)*Children.at(2)->Eval();
	}
	float params[100],x[100];
	for(unsigned int i=0;i<Children.size();i++)
	{
		params[i]=Children.at(i)->Eval();
	}
	return Distributions[Distri].f(CopulaProgram::px,CopulaProgram::dim,params);
}

void DistributionBranch::Random(unsigned int depth)
{
	Op=rand()%2;
	if (depth<=0)
		Op=OP_DISTRI;
	ClearChildren();
	if (Op==OP_DISTRI && Distributions.size()>0)
	{
		Distri=rand()%Distributions.size();
		for(unsigned int i=0;i<Distributions[Distri].parameters;i++)
		{
			Leaf *l=new Leaf;
			l->Random(0);
			Attach(l);
		}
	}
	else
	{
		Leaf *l=new Leaf;
		l->Limits(0.0f,1.0f);
		l->Random(0);
		Attach(l);
		DistributionBranch *b1,*b2;
		b1=new DistributionBranch();
		b2=new DistributionBranch();
		b1->Random(depth-1);
		b2->Random(depth-1);
		Attach(b1);
		Attach(b2);
	}
}

void DistributionBranch::MutateAttributes()
{
	Random(1);
	return;
}

unsigned int DistributionBranch::RequiredChildren()
{
	return 0;
}
int DistributionBranch::RequiredChildTypeAt(unsigned int i)
{
	return 0;
}

std::string DistributionBranch::Print()
{
	std::stringstream stream;
	if (Op==OP_MIX)
		stream << "mix(" << Children.at(0)->Print() << "," << Children.at(1)->Print() << "," << Children.at(2)->Print() << ")";
	else
	{
		stream << Distributions[Distri].name << "(";
		for(unsigned int i=0;i<Distributions[Distri].parameters;i++)
		{
			stream << Children.at(i)->Print();
			if (i<Distributions[Distri].parameters-1)
				stream << ",";
		}
		stream << ")";
	}
	return stream.str();
}