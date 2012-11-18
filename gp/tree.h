#ifndef _TREE
#define _TREE

#include <vector>

#include "node.h"

class Tree
{
public:
	Tree();
	void SetRoot(Node *n);
	Node *GetRoot();
	virtual Tree *Copy();
	unsigned int Size();
	virtual int Replace(Node *Root,Node *A,Node *B);
	virtual std::string Print();
	~Tree();
	float Eval();
	Node *Root;
private:
protected:
};

#endif