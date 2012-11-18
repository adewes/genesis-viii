#ifndef _DISTRIBUTION_PROGRAM
#define _DISTRIBUTION_PROGRAM

#include "copula_program.h"

class DistributionProgram : public CopulaProgram
{
public:
	DistributionProgram();
	virtual Program *Copy(){Program *p=new DistributionProgram;p->SetTrunk(Trunk->Copy());return p;};
	virtual void Setup(std::string directory,vector<std::string> parameters);
	virtual void Write(std::string directory,unsigned int generation);
	virtual float Fitness();
	~DistributionProgram();
	static Matrix bins,d;
private:
protected:
};


#endif