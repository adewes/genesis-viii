#include "symbolic_program.h"
#include "../utils/statistics.h"
#include "../utils/nr/nr.h"
#include "../gp/leaf.h"

Matrix SymbolicProgram::m,SymbolicProgram::n;
float SymbolicProgram::X;

#include<sstream>

SymbolicProgram::SymbolicProgram() : Program()
{
}


void SymbolicProgram::Setup(std::string directory,vector<std::string> parameters)
{
	static bool Initialized=false;
	if (Initialized==true)
		return;
	cout << "Setting up VaR program...\n";
	Initialized=true;
	unsigned int i;
	float f;
	float x;
	for(i=0;i<1000;i++)
	{
		x=(float)i/1000.0*2.0*PI;
		f=Function(x);
		m.Set(i,0,x);
		m.Set(i,1,f);
	}
	TerminalManager *TM=TM->i();
	TM->Add("x",&X);
}

void SymbolicProgram::Write(std::string directory,unsigned int generation)
{
	unsigned int i;
	for(i=0;i<m.Dim();i++)
	{
		X=(float)m[i][0];
		m.Set(i,2,Eval());
	}
	stringstream stream;
	stream << directory << "/best_" << generation << ".tsv";
	m.Write(stream.str());
}

float SymbolicProgram::Function(float x)
{
	static long idnum=(long)time(NULL);
	return sin(x*x)*sqrt(2.0*x)+cos(x*3.)+NR::gasdev(&idnum)*0.4;
}

float SymbolicProgram::Fitness()
{
	unsigned int i;
	float ssr=0.0;
	for(i=0;i<m.Dim();i++)
	{
		X=(float)m[i][0];
		ssr+=pow(Eval()-(float)m[i][1],2.f);
	}
	return -log(1.0+ssr)-log(1.0+Size())*0.0;
}

SymbolicProgram::~SymbolicProgram()
{

}
