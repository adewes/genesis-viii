#ifndef _TERRORISM_PROGRAM
#define _TERRORISM_PROGRAM

#include "program.h"

class TerrorismProgram : public Program
{
public:
	TerrorismProgram();
	virtual Program *Copy(){Program *p=new TerrorismProgram;p->SetTrunk(Trunk->Copy());return p;};
	virtual void Setup(std::string directory,vector<std::string> parameters);
	~TerrorismProgram();
	virtual float Fitness();
	virtual void Write(std::string directory,unsigned int generation);
	static Matrix values,categories,cleaned;
	static float latitude,longitude,date,country,region,suicide_attack,attack_type,target_type,target_nationality,perp_gname,num_perps,weapon_type,num_fatalities,num_injured,damage,successful;
private:
protected:
};


#endif