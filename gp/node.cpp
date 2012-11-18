#include "node.h"

#include <iostream>
#include <iterator>
using namespace std;

static unsigned int nodes=0;

Node::Node()
{
	Parent=NULL;
	nodes++;
//	cout <<  "Nodes created:" << nodes << "\n";
}

unsigned int Node::RequiredChildren()
{
	return 0;
}

std::vector<Node *> Node::GetNodes()
{
	return GetNodesOfType(NODE);
}

std::vector<Node *> Node::GetBranches()
{
	return GetNodesOfType(BRANCH);
}

std::vector<Node *> Node::GetLeafs()
{
	return GetNodesOfType(LEAF);
}

std::vector<Node *> Node::GetNodesOfType(unsigned int type)
{
	std::vector<Node *> Nodes,NewNodes;
	if (type==BRANCH)
	{
		if (IsBranch())
			Nodes.push_back(this);
	}
	else if (type==LEAF)
	{
		if (IsLeaf())
			Nodes.push_back(this);
	}
	else
		Nodes.push_back(this);
	for(unsigned int i=0;i<Children.size();i++)
	{
		NewNodes=Children[i]->GetNodesOfType(type);
		std::copy( NewNodes.begin(), NewNodes.end(), std::back_inserter(Nodes));
	}
	return Nodes;
}

bool Node::IsBranch()
{
	return false;
}

bool Node::IsLeaf()
{
	return false;
}

unsigned int Node::Size()
{
	unsigned int size=1;
	for(unsigned int i=0;i<Children.size();i++)
		size+=Children[i]->Size();
	return size;
}

void Node::MutateAttributes()
{

}

void Node::EraseChild(Node *a)
{
	for(unsigned int i=0;i<Children.size();i++)
		if (Children[i]==a)
		{
			delete Children[i];
			Children.erase(Children.begin()+i);
			return;
		}
}

void Node::ReplaceChild(Node *a,Node *b)
{
	for(unsigned int i=0;i<Children.size();i++)
		if (Children[i]==a)
		{
			delete Children[i];
			b->Parent=this;
			Children[i]=b;
			return;
		}
}

Node *Node::CopyAttributes()
{
	Node *NewNode=new Node;
	return NewNode;
}

Node *Node::Copy()
{
	Node *NewNode;
	NewNode=CopyAttributes();
	vector<Node *> NewChildren;
	for(unsigned int i=0;i<Children.size();i++)
	{
		Node *Child=Children.at(i)->Copy();
		Child->Parent=NewNode;
		NewChildren.push_back(Child);
	}
	NewNode->Children=NewChildren;
	return NewNode;
}

void Node::PopChild()
{
	if (Children.size()==0)
		return;
	delete Children[Children.size()-1];
	Children.pop_back();
}

void Node::Random(unsigned int depth)
{

}

void Node::ClearChildren(void)
{
	for(unsigned int i=0;i<Children.size();i++)
	{
		delete Children[i];
	}
	Children.clear();
}

Node::~Node()
{
	nodes--;
	for(unsigned int i=0;i<Children.size();i++)
		delete Children.at(i);
}
std::string Node::Print()
{
	std::string output;
	return output;
}

void Node::Attach(Node *Child)
{
	Child->Parent=this;
	Children.push_back(Child);
}

float Node::Eval()
{
	return 0.0;
}
