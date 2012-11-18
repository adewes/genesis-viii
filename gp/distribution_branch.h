#ifndef _DISTRIBUTION_BRANCH
#define _DISTRIBUTION_BRANCH

#include "branch.h"

#include <string>

using namespace std;

#define OP_MIX 0 
#define OP_DISTRI 1

class Distribution
{
public:
	std::string name,description;
	unsigned int parameters;
	float (*f)(float *x,unsigned int d,float *p);
};

class DistributionBranch : public Branch
{
public:
	void AddDistribution(std::string name,unsigned int parameters,float (*f)(float *x,unsigned int d,float *p));
	virtual unsigned int RequiredChildren();
	virtual void MutateAttributes();
	virtual void Random(unsigned int depth);
	virtual float Eval();
	virtual Node *CopyAttributes();
	virtual int RequiredChildTypeAt(unsigned int i);
	virtual std::string Print();
	~DistributionBranch(){};
	DistributionBranch();
private:
	static vector<Distribution> Distributions;
	unsigned int Op;
	int Distri;
protected:
};

#endif