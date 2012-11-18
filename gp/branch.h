#ifndef _BRANCH
#define _BRANCH

#include "node.h"

#include <string>

using namespace std;

#define PLUS 0
#define MINUS 1
#define MUL 2
#define DIV 3
#define SQUARE 4
#define SQRT 5
#define X_TO_Y 6
#define LOG 7
#define EXP 8
#define MIN 9
#define MAX 10
#define AVG 11
#define MIX 12
#define LESS 13
#define LESS_EQUAL 14
#define GREATER 15
#define GREATER_EQUAL 16
#define EQUAL 17
#define NOT_EQUAL 18
#define IF_THEN_ELSE 19

//Trigonometric functions
#define SIN 20
#define COS 21
#define TAN 22
#define ASIN 23
#define ACOS 24
#define ATAN 25

#define NOPS 13

class Branch : public Node
{
public:
	virtual bool IsLeaf();
	virtual unsigned int RequiredChildren();
	virtual bool IsBranch();
	virtual Node *CopyAttributes();
	virtual void MutateAttributes();
	virtual void Random(unsigned int depth);
	virtual float Eval();
	virtual std::string Print();
	Branch();
	Branch(unsigned int op);
	~Branch();
private:
	unsigned int Op;
protected:
};

#endif