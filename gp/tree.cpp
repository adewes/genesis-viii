#include "tree.h"
#include "branch.h"

#include <iostream>
#include <math.h>
using namespace std;

Tree::Tree()
{
	Root=NULL;
}

unsigned int Tree::Size()
{
	return Root->Size();
}

int Tree::Replace(Node *Root,Node *A,Node *B)
{
	if (Root==A)
		if (Root->Parent->Children.size()!=0)
		{
			std::vector<Node *>::iterator Child=Root->Parent->Children.begin();
			while(*Child!=Root)
				Child++;
			Root->Parent->ReplaceChild(*Child,B->Copy());
			return 1;
		}
		else
			Root->Parent->Attach(B->Copy());
	else
		for(unsigned int i=0;i<Root->Children.size();i++)
			if (Replace(Root->Children.at(i),A,B)==1)return 1;
	return 0;
}


Tree *Tree::Copy()
{
	Tree *NewTree=new Tree;
	NewTree->Root=Root->Copy();
	return NewTree;
}

Tree::~Tree()
{
	if (Root==NULL)
		return;
	delete Root;
}
std::string Tree::Print()
{
	return Root->Print();
}

float Tree::Eval()
{
	return Root->Eval();
}

void Tree::SetRoot(Node *n)
{
	Root=n;
	Root->Parent=NULL;
}

Node *Tree::GetRoot()
{
	return Root;
}
