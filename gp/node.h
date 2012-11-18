#ifndef _NODE
#define _NODE

#include <vector>
#include <string>

#define NODE 0
#define BRANCH 1
#define LEAF 2

class Node
{
public:
	virtual bool IsLeaf();
	virtual bool IsBranch();
	virtual float Eval();
	virtual void ReplaceChild(Node *a,Node *b);
	virtual void EraseChild(Node *a);
	virtual void PopChild();
	virtual void MutateAttributes();
	virtual unsigned int RequiredChildren();
	unsigned int Size();
	std::vector<Node *> GetNodesOfType(unsigned int type);
	std::vector<Node *> GetNodes();
	std::vector<Node *> GetBranches();
	std::vector<Node *> GetLeafs();
	virtual std::string Print();
	Node();
	~Node();
	void ClearChildren(void);
	virtual Node *CopyAttributes();
	virtual Node *Copy();
	virtual void Random(unsigned int depth);
	void Attach(Node *Child);

	Node *Parent;
	std::vector<Node *> Children;

private:

protected:

};

#endif