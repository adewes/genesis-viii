#ifndef _LEAF
#define _LEAF

#include "node.h"
#include "../utils/manager.h"

class Terminal
{
public:
	std::string Name;
	float Value;
	float *Ptr;
	bool IsPtr;
};


class TerminalManager : public Manager<Terminal,TerminalManager>
{
public:
	void Add(std::string name);
	void Add(std::string name,float *ptr);
	void Set(std::string name,float value);
private:
};


class Leaf : public Node
{
public:
	virtual void Set(float value);
	virtual bool IsLeaf();
	virtual bool IsBranch();
	virtual void Limits(float min,float max);
	virtual Node *CopyAttributes();
	virtual void MutateAttributes();
	virtual void Random(unsigned int depth);
	virtual float Eval();
	Leaf();
	std::string Print();
	Leaf(float value);
	~Leaf();
private:
	unsigned int CustomTerminal;
	float Min,Max;
	float Value;
protected:
};

#endif