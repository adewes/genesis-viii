#include "branch.h"
#include "leaf.h"

#include <math.h>

Branch::Branch() : Node()
{
	Op=PLUS;
}

void Branch::MutateAttributes()
{
	Random(0);
}

bool Branch::IsLeaf()
{
	return false;
}
bool Branch::IsBranch()
{
	return true;
}

Node *Branch::CopyAttributes()
{
	Branch *NewBranch=new Branch;
	*NewBranch=*this;
	return (Node *)NewBranch;
}


Branch::Branch(unsigned int op) : Node()
{
	Op=op;
}

Branch::~Branch()
{

}

std::string Branch::Print()
{
	unsigned int i=0;
	std::string output=" ";
	if (Children.size()==0)
		return output;
	switch(Op)
	{
	case MIN:
		output+="min(";
		for(i=0;i<Children.size();i++)
		{
			output+=Children[i]->Print();
			if (i<Children.size()-1)
				output+=",";
		}
		output+=")";
		break;
	case MAX:
		output+="max(";
		for(i=0;i<Children.size();i++)
		{
			output+=Children[i]->Print();
			if (i<Children.size()-1)
				output+=",";
		}
		output+=")";
		break;
	case EQUAL:
		output+= Children[0]->Print()+" == "+Children[1]->Print();
		break;
	case NOT_EQUAL:
		output+= Children[0]->Print()+" != "+Children[1]->Print();
		break;
	case LESS:
		output+= Children[0]->Print()+" < "+Children[1]->Print();
		break;
	case LESS_EQUAL:
		output+= Children[0]->Print()+" <= "+Children[1]->Print();
		break;
	case GREATER:
		output+= Children[0]->Print()+" > "+Children[1]->Print();
		break;
	case GREATER_EQUAL:
		output+= Children[0]->Print()+" >= "+Children[1]->Print();
		break;
	case IF_THEN_ELSE:
		output+="if ( "+Children[0]->Print()+" ) { "+Children[1]->Print()+" } else { "+Children[2]->Print()+" } ";
		break;
	case AVG:
		output+="avg(";
		for(i=0;i<Children.size();i++)
		{
			output+=Children[i]->Print();
			if (i<Children.size()-1)
				output+=",";
		}
		output+=")";
		break;
	case MIX:
		output+="mix("+Children.at(0)->Print()+"|"+Children.at(1)->Print()+","+Children.at(2)->Print()+")";
		break;
	case EXP:
		output+="exp("+Children.at(0)->Print()+")";
		break;
	case LOG:
		output+="log("+Children.at(0)->Print()+")";
		break;
	case SIN:
		output+="sin("+Children.at(0)->Print()+")";
		break;
	case COS:
		output+="cos("+Children.at(0)->Print()+")";
		break;
	case TAN:
		output+="tan("+Children.at(0)->Print()+")";
		break;
	case ATAN:
		output+="atan("+Children.at(0)->Print()+")";
		break;
	case ACOS:
		output+="acos("+Children.at(0)->Print()+")";
		break;
	case ASIN:
		output+="asin("+Children.at(0)->Print()+")";
		break;
	case SQUARE:
		output+=Children.at(0)->Print()+"^2";
		break;
	case SQRT:
		output+="sqrt("+Children.at(0)->Print()+")";
		break;
	case X_TO_Y:
		if (Children.size()>=2)
			output+=Children.at(0)->Print()+"^"+Children.at(1)->Print();
		break;
	case PLUS:
		output+=Children.at(0)->Print();
		for(i=1;i<Children.size();i++)
			output+="+"+Children.at(i)->Print();
		break;
	case MUL:
		output+=Children.at(0)->Print();
		for(i=1;i<Children.size();i++)
			output+="*"+Children.at(i)->Print();
		break;
	case DIV:
		output+=Children.at(0)->Print();
		for(i=1;i<Children.size();i++)
			output+="/"+Children.at(i)->Print();
		break;
	case MINUS:
		output+=Children.at(0)->Print();
		for(i=1;i<Children.size();i++)
			output+="-"+Children.at(i)->Print();
		break;
	default:break;
	}
	output+=" ";
	return output;
}

unsigned int Branch::RequiredChildren()
{
	unsigned int n_childs=0;
	switch(Op)
	{
	case X_TO_Y:
		n_childs=2;
		break;
	case PLUS:
		n_childs=2;
		break;
	case MUL:
		n_childs=2;
		break;
	case DIV:
		n_childs=2;
		break;
	case MINUS:
		n_childs=2;
		break;
	case MIX:
		n_childs=3;
		break;
	case MAX:
		n_childs=2;
		break;
	case MIN:
		n_childs=2;
		break;
	case AVG:
		n_childs=2;
		break;
	case IF_THEN_ELSE:
		n_childs=3;
		break;
	case EQUAL:
		n_childs=2;
		break;
	case NOT_EQUAL:
		n_childs=2;
		break;
	case GREATER:
		n_childs=2;
		break;
	case GREATER_EQUAL:
		n_childs=2;
		break;
	case LESS:
		n_childs=2;
		break;
	case LESS_EQUAL:
		n_childs=2;
		break;
	default:n_childs=1;break;
	}	
	return n_childs;
}


void Branch::Random(unsigned int depth)
{
	Op=rand()%NOPS;
}

float Branch::Eval()
{
	unsigned int i=0;
	float r=0.0;
	float alpha,avg,min,max;
	if (Children.size()==0)
		return r;
	switch(Op)
	{
	case EXP:
		r=exp(Children.at(0)->Eval());
		break;
	case LOG:
		if (Children.at(0)->Eval()>0)
			r=log(Children.at(0)->Eval());
		break;
	case SIN:
		r=sin(Children.at(0)->Eval());
		break;
	case COS:
		r=cos(Children.at(0)->Eval());
		break;
	case TAN:
		r=tan(Children.at(0)->Eval());
		break;
	case ATAN:
		r=atan(Children.at(0)->Eval());
		break;
	case ACOS:
		r=acos(Children.at(0)->Eval());
		break;
	case ASIN:
		r=asin(Children.at(0)->Eval());
		break;
	case SQUARE:
		r=pow(Children.at(0)->Eval(),2);
		break;
	case SQRT:
		if (Children.at(0)->Eval()>0)
			r=sqrt(Children.at(0)->Eval());
		break;
	case X_TO_Y:
		if (Children.size()>1)
			r=pow(Children.at(0)->Eval(),Children.at(1)->Eval());
		break;
	case PLUS:
		if (Children.size()==2)
			r=Children.at(0)->Eval()+Children.at(1)->Eval();
		break;
	case MUL:
		if (Children.size()==2)
			r=Children.at(0)->Eval()*Children.at(1)->Eval();
		break;
	case DIV:
		if (Children.size()==2)
			r=Children.at(0)->Eval()/Children.at(1)->Eval();
		break;
	case MIN:
		if (Children.size()<2)
			return 0.f;
		r=Children.at(0)->Eval();
		for(i=1;i<Children.size();i++)
			if (Children[i]->Eval()<r)
				r=Children[i]->Eval();
		break;
	case MAX:
		if (Children.size()<2)
			return 0.f;
		r=Children.at(0)->Eval();
		for(i=1;i<Children.size();i++)
			if (Children[i]->Eval()>r)
				r=Children[i]->Eval();
		break;
	case AVG:
		if (Children.size()<2)
			return 0.f;
		avg=0.0;
		for(i=0;i<Children.size();i++)
			avg+=Children[i]->Eval();
		r=avg/(float)Children.size();
		break;
	case MIX:
		if (Children.size()<3)
			return 0.f;
		alpha=Children[0]->Eval();
		r=Children[1]->Eval()*alpha+Children[2]->Eval()*(1.f-alpha);
		break;
	case MINUS:
		if (Children.size()==2)
			r=Children.at(0)->Eval()-Children.at(1)->Eval();
		break;
	case EQUAL:
		if (Children.size()<2)
			return 0.f;
		if (Children[0]->Eval()==Children[1]->Eval())
			r=1.0;
		else
			r=0.0;
		break;
	case NOT_EQUAL:
		if (Children.size()<2)
			return 0.f;
		if (Children[0]->Eval()!=Children[1]->Eval())
			r=1.0;
		else
			r=0.0;
		break;
	case LESS:
		if (Children.size()<2)
			return 0.f;
		if (Children[0]->Eval()<Children[1]->Eval())
			r=1.0;
		else
			r=0.0;
		break;
	case LESS_EQUAL:
		if (Children.size()<2)
			return 0.f;
		if (Children[0]->Eval()<=Children[1]->Eval())
			r=1.0;
		else
			r=0.0;
		break;
	case GREATER:
		if (Children.size()<2)
			return 0.f;
		if (Children[0]->Eval()>Children[1]->Eval())
			r=1.0;
		else
			r=0.0;
		break;
	case GREATER_EQUAL:
		if (Children.size()<2)
			return 0.f;
		if (Children[0]->Eval()>=Children[1]->Eval())
			r=1.0;
		else
			r=0.0;
		break;
	case IF_THEN_ELSE:
		if (Children.size()<3)
			return 0.f;
		if (Children[0]->Eval())
			r=Children[1]->Eval();
		else
			r=Children[2]->Eval();
		break;
	default:break;
	};
	return r;
}
