#ifndef _PROGRAM
#define _PROGRAM

#include "tree.h"
#include "../utils/matrix.h"

class Program
{
public:
	Program();
	virtual std::string Print();
	~Program();

	//Overwrite this function to create custom nodes!
	virtual Node *RandomNode(int depth,unsigned int type);
	
	//Overwrite this function to create custom random programs.
	virtual void Random(int depth);
	virtual float Eval();
	unsigned int Size();
	virtual void Simplex(int steps);

	//Overwrite the Setup() function to customize the program.
	virtual void Setup(std::string directory,vector<std::string> parameters);

	virtual void Mutate();
	virtual void Shrink();

	//Overwrite this function to store the output in some folder...
	virtual void Write(std::string directory,unsigned int generation);
	virtual Program *Crossover(Program *p);
	
	//Overwrite this function to calculate the program fitness!
	virtual float Fitness();
	virtual Program *Copy();

	void SetTrunk(Tree *t){if (Trunk!=NULL)delete Trunk;Trunk=t;};


	Tree *Trunk;
protected:
	float FitnessValue;
	bool FitnessCalculated;
private:
	//These are private, no access by inherited programs!
};


#endif