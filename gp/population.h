#ifndef _POPULATION
#define _POPULATION

#include <vector>
#include "program.h"

class Citizen
{
public:
	float Fitness;
	Program *p;
};

class Population
{
public:
	Population();
	void AddProgram(Program *p);
	void Clear();
	void Evaluate();
	void SetOutputDirectory(std::string name);
	std::string GetOutputDirectory();
	void Evolve();
	float MutationRate(float rate);
	float MutationRate();
	float ShrinkRate(float rate);
	float ShrinkRate();
	float CrossoverSize(float size);
	float CrossoverSize();
	int TournamentSize(int size);
	int TournamentSize();
	void Create(unsigned int size,unsigned int depth,Program *(*generator)(void));
	void SetParameters(vector<std::string> params);
	std::vector<Program *> GetPrograms();
	~Population();
	Program *Best,*Worst;
private:
	float _MutationRate,_ShrinkRate,_CrossoverSize;
	int _TournamentSize;
	vector<std::string> Parameters;
	unsigned int InitialDepth,InitialSize,Generation;
	float MaxFitness,MinFitness,AvgFitness,AvgSize,MinSize,MaxSize;
	std::vector<float> Fitnesses;
	std::vector<unsigned int> Sizes;
	std::vector<Program *> Programs;
	std::vector<Citizen> Heap;
	std::string OutputDirectory;
	Matrix Statistics,Histogram;

protected:

};

bool operator==(const Citizen a,const Citizen b);
bool operator!=(const Citizen a,const Citizen b);
bool operator>=(const Citizen a,const Citizen b);
bool operator<=(const Citizen a,const Citizen b);
bool operator<(const Citizen a,const Citizen b);
bool operator>(const Citizen a,const Citizen b);

namespace LuaPopulation
{
	void Setup(void);
};

#endif