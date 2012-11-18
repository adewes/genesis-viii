#ifndef _COPULA_PROGRAM
#define _COPULA_PROGRAM

#include "program.h"

class CopulaProgram : public Program
{
public:
	CopulaProgram();
	virtual Node *RandomNode(int depth,unsigned int type);
	virtual Program *Copy(){Program *p=new CopulaProgram;p->SetTrunk(Trunk->Copy());return p;};
	virtual void Setup(std::string directory,vector<std::string> parameters);
	virtual float Fitness();
	virtual void Shrink();
	virtual void Mutate();
	virtual Program *Crossover(Program *p);
	virtual void Write(std::string directory,unsigned int generation);
	~CopulaProgram();
	static Matrix c,d,cdf_a,cdf_b,stocks;
	static float px[100];
	static unsigned int dim;
private:
protected:
};


#endif