#include "population.h"
#include "../utils/scriptmanager.h"

#include <iostream>
#include <float.h>
#include <limits>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include "../utils/scriptmanager.h"
#include "../utils/statistics.h"
#include "../utils/logger.h"
using namespace std;

#include <fstream>

namespace LuaPopulation
{

	int MutationRate(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<1)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		if (n==2)
		{
			float rate=lua_tonumber(L,-n+1);
			p->MutationRate(rate);
		}		
		lua_pushnumber(L,p->MutationRate());
		return 1;
	}

	int ShrinkRate(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<1)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		if (n==2)
		{
			float rate=lua_tonumber(L,-n+1);
			p->ShrinkRate(rate);
		}		
		lua_pushnumber(L,p->ShrinkRate());
		return 1;
	}

	int CrossoverSize(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<1)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		if (n==2)
		{
			float rate=lua_tonumber(L,-n+1);
			p->CrossoverSize(rate);
		}		
		lua_pushnumber(L,p->CrossoverSize());
		return 1;
	}

	int TournamentSize(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<1)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		if (n==2)
		{
			int size=lua_tonumber(L,-n+1);
			p->TournamentSize(size);
		}		
		lua_pushnumber(L,p->TournamentSize());
		return 1;
	}

	int New(lua_State *L)
	{
		Population *p=new Population();
		lua_pushlightuserdata(L,(void *)p);
		return 1;
	}

	int OutputDirectory(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<1)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		if (n==2)
		{
			std::string name=lua_tostring(L,-n+1);
			p->SetOutputDirectory(name);
		}
		lua_pushstring(L,(const char *)p->GetOutputDirectory().data());
		return 1;
	}

	int Run(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<2)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		unsigned int T=(unsigned int)lua_tonumber(L,-n+1);
		for(unsigned int i=0;i<T;i++)
		{
			p->Evaluate();
			p->Evolve();
		}
		return 0;
	}

	int Create(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<3)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		unsigned int n_progs=(unsigned int)lua_tonumber(L,-n+1);
		unsigned int depth=(unsigned int)lua_tonumber(L,-n+2);
		Program *(*generator)(void)=NULL;
		if (n>=4)
			generator=(Program *(*)(void))lua_touserdata(L,-n+3);
		vector<std::string> params;
		for(int i=4;i<n;i++)
			params.push_back(lua_tostring(L,-n+i));
		cout << "Creating new population of size " << n_progs << " and depth " << depth << "\n";
		p->SetParameters(params);
		p->Create(n_progs,depth,generator);
		return 0;
	}

	int Evolve(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<1)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		p->Evolve();
		return 0;
	}

	int Evaluate(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<1)
			return 0;
		Population *p=(Population *)lua_touserdata(L,-n);
		p->Evaluate();
		return 0;
	}

	void Setup(void)
	{
		ScriptManager *SM=SM->i();
		SM->RegisterFunction("output_dir","pop",OutputDirectory);
		SM->RegisterFunction("run","pop",Run);
		SM->RegisterFunction("new","pop",New);
		SM->RegisterFunction("eval","pop",Evaluate);
		SM->RegisterFunction("evolve","pop",Evolve);
		SM->RegisterFunction("create","pop",Create);
		SM->RegisterFunction("shrink_rate","pop",ShrinkRate);
		SM->RegisterFunction("mutation_rate","pop",MutationRate);
		SM->RegisterFunction("tournament_size","pop",TournamentSize);
		SM->RegisterFunction("crossover_size","pop",CrossoverSize);
	}
};

Population::Population()
{
	Generation=0;
	_TournamentSize=4;
	_CrossoverSize=0.8;
	_ShrinkRate=0.025;
	_MutationRate=0.05;
	OutputDirectory="data/default";
}

float Population::MutationRate(float rate)
{
	if (rate>0 && rate<1)
		_MutationRate=rate;
	return _MutationRate;
}

float Population::MutationRate()
{
	return _MutationRate;
}

float Population::ShrinkRate(float rate)
{
	if (rate>0 && rate<1)
		_ShrinkRate=rate;
	return _ShrinkRate;
}

float Population::ShrinkRate()
{
	return _ShrinkRate;
}

float Population::CrossoverSize(float size)
{
	if (size>0 && size<1)
		_CrossoverSize=size;
	return _CrossoverSize;
}

float Population::CrossoverSize()
{
	return _CrossoverSize;
}

int Population::TournamentSize(int size)
{
	if (size>0)
		_TournamentSize=size;
	return _TournamentSize;
}

int Population::TournamentSize()
{
	return _TournamentSize;
}


void Population::SetParameters(vector<std::string> params)
{
	Parameters=params;
}

Population::~Population()
{

}

std::string Population::GetOutputDirectory()
{
	return OutputDirectory;
}

void Population::Create(unsigned int size,unsigned int depth,Program *(*generator)(void))
{
	Generation=1;
	InitialDepth=depth;
	InitialSize=size;
	Clear();
	for(unsigned int i=0;i<size;i++)
	{
		Program *p;
		if (generator!=NULL)
			p=generator();
		else
			p=new Program();
		p->Setup(OutputDirectory,Parameters);
		p->Random(rand()%depth);
//		cout << i << " = " << p->Print() << "\n";
		Programs.push_back(p);
	}
	Statistics.Set(0,0,"#Generation");
	Statistics.Set(0,1,"Deleted programs");
	Statistics.Set(0,2,"Maximum size");
	Statistics.Set(0,3,"Minimum size");
	Statistics.Set(0,4,"Average size");
	Statistics.Set(0,5,"Maximum fitness");
	Statistics.Set(0,6,"Minimum fitness");
	Statistics.Set(0,7,"Average fitness");
	Statistics.Set(0,8,"Best program");
}

bool operator>=(const Citizen a,const Citizen b)
{
	return a.Fitness>=b.Fitness;
}


bool operator<=(const Citizen a,const Citizen b)
{
	return a.Fitness<=b.Fitness;
}


bool operator!=(const Citizen a,const Citizen b)
{
	return a.Fitness!=b.Fitness;
}


bool operator==(const Citizen a,const Citizen b)
{
	return a.Fitness==b.Fitness;
}
bool operator>(const Citizen a,const Citizen b)
{
	return a.Fitness>b.Fitness;
}


bool operator<(const Citizen a,const Citizen b)
{
	return a.Fitness<b.Fitness;
}

void Population::SetOutputDirectory(std::string name)
{
	OutputDirectory=name;
	ScriptManager *SM=SM->i();
	cout << "Creating output directory " << OutputDirectory << "\n";
	std::string cmd="mkdir \"";
	cmd.append(OutputDirectory);
	cmd.append("\"");
	system((const char *)cmd.data());
	ScriptEntry se=SM->Get("startup");
	std::stringstream stream;
	stream << OutputDirectory << "/script.lua";
	std::ofstream out;
	out.open(stream.str().data(),ios::out);
	out << se.script;
	out.close();
}

void Population::Evaluate()
{
	unsigned int i;
	Logger *ML=ML->i();
	stringstream stream;
	Program *p;
	if (Programs.size()==0)
		return;
	Fitnesses.clear();
	Sizes.clear();
	Heap.clear();
	make_heap(Heap.begin(),Heap.end());

	AvgSize=0;
	AvgFitness=0;
	MinFitness=0;
	MaxFitness=0;
	Citizen c;

	float f;
	unsigned int s,deleted;
	cout << "Evaluating...\n";
	deleted=0;
	Histogram.Clear();
	for(i=0;i<Programs.size();i++)
	{
		p=Programs.at(i);
		s=p->Size();
		f=p->Fitness();
		cout <<"Fitness(" << i << ") = " << f << "\r";
		if (fabs(f)==std::numeric_limits<float>::infinity() || f!=f)
		{
			//Bad program, we delete it...
			deleted++;
			delete *(Programs.begin()+i);
			Programs.erase(Programs.begin()+i);
			i--;
		}
		else
		{
			AvgSize+=s;
			if (s>MaxSize || i==0)
				MaxSize=s;
			if (s<MinSize || i==0)
				MinSize=s;
			Histogram.Set(i,0,f);
			Fitnesses.push_back(f);
			Sizes.push_back(s);
			c.Fitness=f;
			c.p=Programs[i];
			Heap.push_back(c);
			push_heap(Heap.begin(),Heap.end());
			if (f<MinFitness || i==0)
			{
				MinFitness=f;
				Worst=p;
			}
			if (f>MaxFitness || i==0)
			{
				MaxFitness=f;
				Best=p;
			}
			AvgFitness+=f;
		}
	}
	AvgFitness/=(float)Programs.size();
	AvgSize/=(float)Programs.size();

	stream << Generation << "-th generation:\n"; 
	stream << "Deleted programs:" << deleted << "\n";
	stream << "Max size        :" << MaxSize << "\n";
	stream << "Min size        :" << MinSize << "\n";
	stream << "Avg size        :" << AvgSize << "\n";
	stream << "Max fitness     :" << MaxFitness << "\n";
	stream << "Min fitness     :" << MinFitness << "\n";
	stream << "Avg fitness     :" << AvgFitness << "\n";
	stream << "Best solution   :" << Best->Print() << "\n";


	cout << Generation << "-th generation:\n"; 
	cout << "Deleted programs:" << deleted << "\n";
	cout << "Max size        :" << MaxSize << "\n";
	cout << "Min size        :" << MinSize << "\n";
	cout << "Avg size        :" << AvgSize << "\n";
	cout << "Max fitness     :" << MaxFitness << "\n";
	cout << "Min fitness     :" << MinFitness << "\n";
	cout << "Avg fitness     :" << AvgFitness << "\n";
	cout << "Best solution   :" << Best->Print() << "\n";

	Best->Write(OutputDirectory,Generation);

	Statistics.Set(Generation,0,Generation);
	Statistics.Set(Generation,1,deleted);
	Statistics.Set(Generation,2,MaxSize);
	Statistics.Set(Generation,3,MinSize);
	Statistics.Set(Generation,4,AvgSize);
	Statistics.Set(Generation,5,MaxFitness);
	Statistics.Set(Generation,6,MinFitness);
	Statistics.Set(Generation,7,AvgFitness);
	Statistics.Set(Generation,8,Best->Print());

	std::string filename=OutputDirectory;
	filename.append("/");
	filename.append("stat.tsv");
	Statistics.Write(filename);

	stringstream fstream;

	Histogram=STAT::generate_histogram(Histogram,0,InitialSize/5.0);
	fstream.str("");
	fstream.clear();
	fstream << OutputDirectory << "/histo_" << Generation << ".tsv";
	Histogram.Write(fstream.str());
	Matrix programs;
	for(i=0;i<Programs.size();i++)
	{
		programs.Set(i,0,Programs[i]->Print());
	}
	fstream.str("");
	fstream.clear();
	fstream << OutputDirectory << "/programs_" << Generation << ".tsv";
	programs.Write(fstream.str());

	ML->Event(stream.str());
}
#include <limits>
void Population::Evolve()
{
	unsigned int i=0;
	std::vector<Program *> RPool,GPool;
	std::vector<Citizen> Tournament;
	std::vector<unsigned int> Positions;
	Logger *ML=ML->i();
	stringstream stream;
	Citizen Winner;
	float fitness;
	unsigned int best_i,rand_pos;
	//We select the above average solutions from the sample set...

	//Tournament selection:
	while(RPool.size()<0.01*Programs.size())
	{
			pop_heap(Heap.begin(),Heap.end());
			RPool.push_back(Heap[Heap.size()-1].p);
			Heap.pop_back();
	}
	Program *Dad,*Mum,*Son;
	Dad=NULL;
	while(RPool.size()<InitialSize*(1.0-_CrossoverSize) && Heap.size()>=_TournamentSize)
	{
		Tournament.clear();
		Positions.clear();
		for(i=0;i<_TournamentSize;i++)
		{
			rand_pos=rand()%Heap.size();
			Positions.push_back(rand_pos);
			Tournament.push_back(Heap[rand_pos]);
		}
		Winner=Tournament[0];
		best_i=0;
		for(i=1;i<_TournamentSize;i++)
			if (Tournament[i].Fitness>Winner.Fitness)
			{
				best_i=i;
				Winner=Tournament[i];
			}
/*		if (Dad==NULL)
			Dad=Winner.p;
		else
		{
			Mum=Winner.p;
			Son=Dad->Crossover(Mum);
			RPool.push_back(Son);
			Son=Mum->Crossover(Dad);
			RPool.push_back(Son);
			if (rand()%10==0)
				RPool.push_back(Mum);
			else
				delete Mum;
			if (rand()%10==0)
				RPool.push_back(Dad);
			else
				delete Dad;
			Dad=NULL;
		}*/
		RPool.push_back(Winner.p);
		Heap.erase(Heap.begin()+Positions[best_i]);
	}

	for(i=0;i<Heap.size();i++)
		delete Heap[i].p;
	stream << "Generation " << Generation << ":\n";
	stream << "Reproduction pool size:" << RPool.size() << " of " << Programs.size() << "\n";
	stream << "Mutation rate:" << _MutationRate << "\n";
	stream << "Shrink rate:" << _ShrinkRate << "\n";
	stream << "Crossover size:" << _CrossoverSize << "\n";
	stream << "Tournament size:" << _TournamentSize << "\n";
	cout << "Reproduction pool size:" << RPool.size() << " of " << Programs.size() << "\n";

	ML->Event(stream.str());

	while(RPool.size() < InitialSize)
	{
		Program *A,*B,*C;
		A=RPool[rand()%RPool.size()];
		B=RPool[rand()%RPool.size()];
		C=A->Crossover(B);
		RPool.push_back(C);
	}
	for(i=0;i<_MutationRate*RPool.size();i++)
		RPool[rand()%RPool.size()]->Mutate();
	for(i=0;i<_ShrinkRate*RPool.size();i++)
		RPool[rand()%RPool.size()]->Shrink();
	Programs.clear();
	Programs=RPool;
	Generation++;
}


void Population::AddProgram(Program *p)
{
	Programs.push_back(p);
}

void Population::Clear()
{
	for(unsigned int i=0;i<Programs.size();i++)
		delete Programs.at(i);
	Programs.clear();
}

std::vector<Program *> Population::GetPrograms()
{
	return Programs;
}
