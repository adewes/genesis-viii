#include "program.h"
#include "tree.h"
#include "leaf.h"
#include "branch.h"
#include "../utils/statistics.h"
#include "../utils/nr/nr.h"
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;

#include "../utils/matrix.h"

Program::Program()
{
	Trunk=NULL;
	FitnessValue=0.0f;
	FitnessCalculated=false;
}

float Program::Eval()
{
	return Trunk->Eval();
}

Program *Program::Crossover(Program *p)
{
	Program *R=Copy();

	Tree *NewTree=p->Trunk->Copy();

	Node *From,*To,*Current;

	Current=NewTree->Root;

	unsigned int c=0;
	unsigned int from_prob=(unsigned int)NewTree->Size();
	unsigned int to_prob=(unsigned int)Size();
	if (Current->Children.size()!=0 && rand()%from_prob)
		do {Current=Current->Children.at(rand()%Current->Children.size());c++;} while(rand()%from_prob && Current->Children.size()!=0);
//	cout << "Selected at " << c << " of " << NewTree->Size() << "\n";
	From=Current;
	Current=Trunk->Root;
	if (Current->Children.size()!=0 && rand()%to_prob)
		do Current=Current->Children.at(rand()%Current->Children.size()); while(rand()%to_prob && Current->Children.size()!=0);	
	To=Current;
//	cout << "From " << From->Print() << " to " << To->Print() << "\n";
	if (From==NewTree->Root)
	{
		delete NewTree->Root;
		NewTree->Root=To->Copy();
		NewTree->Root->Parent=NULL;
	}
	else
		NewTree->Replace(NewTree->Root,From,To);
	R->SetTrunk(NewTree);
//	cout << "Program copy...\n";
//	cout << "Template:" << Print() << "\n";
//	cout << "Original:" << p->Print() << "\n";
//	cout << "New     :" << R->Print() << "\n";
	R->FitnessCalculated=false;
	return R;
}

Program *Program::Copy()
{
	Program *p=new Program;
	p->Trunk=Trunk->Copy();
	return p;
}

unsigned int Program::Size()
{
	return Trunk->Size();
}

void Program::Mutate()
{
	std::vector<Node *> Nodes;
	unsigned int r=0;
	FitnessCalculated=false;
	if (rand()%2)
	{
//		cout << "Before mutation:" << Print() << "\n";
		Node *b=RandomNode(1,BRANCH);
		r=rand()%b->Children.size();
		b->ReplaceChild(b->Children[r],Trunk->GetRoot());
		Trunk->SetRoot(b);
//		cout << "After mutation :" << Print() << "\n";
		return;
	}
	Nodes=Trunk->Root->GetNodes();
	r=rand()%Nodes.size();
	Node *Mutator=Nodes[r];
	Node *NewNode;
	if (Mutator->Parent==NULL)
		return;
//	cout << "Before mutation:" << Print() << ", " << Size() << "\n";
	if (rand()%2)
	{
		if (Mutator->IsLeaf())
		{
			Mutator->MutateAttributes();
//			cout << "After mutation-:" << Print() << "\n";
			return;
		}
		NewNode=new Leaf();
		NewNode->Random(0);
	}
	else
	{
		if (Mutator->IsBranch())
		{
			Mutator->MutateAttributes();
			while (Mutator->Children.size()>Mutator->RequiredChildren())
				Mutator->PopChild();
			while (Mutator->Children.size()<Mutator->RequiredChildren())
				Mutator->Attach(RandomNode(0,LEAF));
			//cout << "After mutatio--:" << Print() << "\n";
			return;
		}
		NewNode=RandomNode(1,BRANCH);
	}
	Node *p=Mutator->Parent;
	p->ReplaceChild(Mutator,NewNode);
//	cout << "After mutation :" << Print() << "\n";
}

Node *Program::RandomNode(int depth,unsigned int type)
{
	Node *n;
	if (type==BRANCH)
		n=new Branch();
	else if (type==LEAF)
		n=new Leaf();
	else if (rand()%2 && depth>0)
		n=new Leaf();
	else
		n=new Branch();
	n->Random(depth);
	if (n->IsBranch())
		for(unsigned int i=0;i<n->RequiredChildren();i++)
			n->Attach(RandomNode(depth-1,depth>0?NODE:LEAF));
	return n;
}

void Program::Simplex(int steps)
{

}

void Program::Shrink()
{
	unsigned int r,rk;
	unsigned int cnt=0;
	FitnessCalculated=false;
	std::vector<Node *> Nodes=Trunk->GetRoot()->GetBranches();
	if (Nodes.size()<=1)
		return;
	r=rand()%Nodes.size();
	while(Nodes[r++%Nodes.size()]->Parent==NULL && cnt++<Nodes.size());
	r%=Nodes.size();
	if (Nodes[r]->Parent==NULL)
		return;
	if (rand()%2 || Nodes[r]->Parent->Parent==NULL)
	{
		Node *l=new Leaf();
		l->Random(0);
		Nodes[r]->Parent->ReplaceChild(Nodes[r],l);
	}
	else
	{
		Node *n=Nodes[r]->Copy();
//		cout << "Before shrink:" << Print() << "\n";
		Nodes[r]->Parent->Parent->ReplaceChild(Nodes[r]->Parent,n);
//		cout << "After shrink :" << Print() << "\n";
	}
}	


void Program::Write(std::string directory,unsigned int generation)
{
	cout << "doing nothing...\n";
}

void Program::Setup(std::string directory,vector<std::string> parameters)
{

}

float Program::Fitness()
{
	cerr << "You try to calculate the fitness of the program template class! Something went wrong with your algorithm (probably you used a wrong constructor)...\n";
	exit(-1);
	return -99.9;
}


std::string Program::Print()
{
	return Trunk->Print();
}

void Program::Random(int depth)
{
	if (Trunk!=NULL)
		delete Trunk;
	Trunk=new Tree();
	Trunk->SetRoot(RandomNode(depth,NODE));
}


Program::~Program()
{
	if (Trunk==NULL)
		return;
	delete Trunk;
}
