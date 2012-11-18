#ifndef _SYMBOLIC_PROGRAM
#define _SYMBOLIC_PROGRAM

#include "program.h"

class SymbolicProgram : public Program
{
public:
	SymbolicProgram();
	virtual float Fitness();
	virtual Program *Copy(){Program *p=new SymbolicProgram;p->SetTrunk(Trunk->Copy());return p;};
	virtual float Function(float x);
	virtual void Setup(std::string directory,vector<std::string> parameters);
	~SymbolicProgram();
	static Matrix m,n;
	virtual void Write(std::string directory,unsigned int generation);
	static float X;
private:
protected:
};


#endif