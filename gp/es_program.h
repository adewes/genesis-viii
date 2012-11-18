#include "var_program.h"

class ESProgram : public VaRProgram
{
public:
	ESProgram();
	~ESProgram();
	virtual void Write(std::string directory,unsigned int generation);
	virtual float Fitness();
	virtual void Setup(std::string directory,std::vector<std::string> parameters);
	virtual Program *Copy(){ESProgram *p=new ESProgram;p->SetTrunk(Trunk->Copy());return (Program *)p;};
	static Matrix es;
private:

protected:
};	