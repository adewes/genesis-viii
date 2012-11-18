#include "leaf.h"

#include <iostream>

using namespace std;


bool Leaf::IsLeaf()
{
	return true;
}

bool Leaf::IsBranch()
{
	return false;
}

Leaf::Leaf() : Node()
{
	Min=-10.0f;
	Max=10.0f;
	CustomTerminal=-1;
}

void Leaf::MutateAttributes()
{
	Random(0);
}

Node *Leaf::CopyAttributes()
{
	Leaf *NewLeaf=new Leaf;
	*NewLeaf=*this;
	return NewLeaf;
}

void TerminalManager::Add(std::string name,float *ptr)
{
	Terminal t;
	t.Name=name;
	t.Ptr=ptr;
	t.IsPtr=true;
	Push_Back(t);
}

void TerminalManager::Add(std::string name)
{
	Terminal t;
	t.Name=name;
	t.Value=0;
	t.IsPtr=false;
	Push_Back(t);
}

void TerminalManager::Set(std::string name,float value)
{

}


void Leaf::Random(unsigned int depth)
{
	TerminalManager *TM=TM->i();
	CustomTerminal=-1;
	if (rand()%4==0 || TM->Size()==0)
		Value=((float)(rand()%10000))/10000.0f*(Max-Min)+Min;
	else
		CustomTerminal=rand()%TM->Size();
}

std::string Leaf::Print()
{
	char number[1024];
	TerminalManager *TM=TM->i();
	if (CustomTerminal!=-1)
		return TM->Get(CustomTerminal).Name;
	else
	{
		sprintf(number,"%g",Value);
		std::string output;
		output.append(number);
		return output;
	}
}

Leaf::Leaf(float value) : Node()
{
	Value=value;
	Min=-10.0f;
	Max=10.0f;
}

void Leaf::Limits(float min,float max)
{
	Min=min;
	Max=max;
}

Leaf::~Leaf()
{

}	

void Leaf::Set(float value)
{
	CustomTerminal=-1;
	Value=value;
}	

float Leaf::Eval()
{
	TerminalManager *TM=TM->i();
	if (CustomTerminal!=-1)
	{
		if (TM->Get(CustomTerminal).IsPtr)
		{
//			cout << TM->Get(CustomTerminal).Name << " = " << *(TM->Get(CustomTerminal).Ptr) << "\n";
			return *(TM->Get(CustomTerminal).Ptr);
		}
		else
			return TM->Get(CustomTerminal).Value;
	}
	else
		return Value;
}