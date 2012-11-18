#include "matrix.h"
#include "scriptmanager.h"

#include <sstream>
#include <fstream>


const char Matrix::className[] = "Matrix";
const Luna<Matrix>::RegType Matrix::Register[] = {
     {"get",  &Matrix::lua_get},
     {"set",  &Matrix::lua_set},
     {"write",  &Matrix::lua_write},
     {"load",  &Matrix::lua_load},
     {"clear",  &Matrix::lua_clear},
     {"dim",  &Matrix::lua_dim},
     {0}
};

int Matrix::lua_set(lua_State* L)
{
	int n=lua_gettop(L);
	if (n<4)
		return 0;
	unsigned int i,j;
	Value value;
	i=(unsigned int)lua_tonumber(L,-n+1);
	j=(unsigned int)lua_tonumber(L,-n+2);
	if (lua_isnumber(L,-n+3))
		value=(float)lua_tonumber(L,-n+3);
	else if (lua_isstring(L,-n+3))
		value=(std::string)lua_tostring(L,-n+3);
	Set(i,j,value);
	cout << "m[" << i << "," << j << "] = " << (std::string)value << "\n";
	return 0;
}

int Matrix::lua_get(lua_State *L)
{
	int n=lua_gettop(L);
	if (n<3)
		return 0;
	unsigned int i,j;
	i=(unsigned int)lua_tonumber(L,-n+1);
	j=(unsigned int)lua_tonumber(L,-n+2);
	if (Get(i)[j].IsFloat())
		lua_pushnumber(L,(lua_Number)Get(i)[j]);
	else
		lua_pushstring(L,(const char *)((std::string)(Get(i)[j])).data());
	return 1;
}

int Matrix::lua_write(lua_State *L)
{
	int n=lua_gettop(L);
	if (n<2)
		return 0;
	std::string filename=lua_tostring(L,-n+1);
	Write(filename);
	return 0;

}

int Matrix::lua_clear(lua_State *L)
{
	Clear();
	return 0;
}

int Matrix::lua_dim(lua_State *L)
{
	lua_pushnumber(L,(lua_Number)Dim());
	return 1;
}

int Matrix::lua_load(lua_State *L)
{
	int n=lua_gettop(L);
	if (n<2)
		return 0;
	std::string filename,seperator;
	seperator="\t";
	if (n>2)
	{
		filename=lua_tostring(L,-n+1);
		seperator=lua_tostring(L,-n+2);
	}
	else
	{
		filename=lua_tostring(L,-n+1);
	}
	Load(filename,seperator,NULL);
	return 0;
}

Matrix::Matrix(lua_State *L)
{
	
}


Matrix::Matrix()
{

}

Matrix::~Matrix()
{

}

namespace LuaMatrix
{

	void Setup(void)
	{
		ScriptManager *SM=SM->i();
		Luna<Matrix> ML;
		ML.Register(SM->Lua);
	}
};


std::string read_file(std::string filename)
{
	std::ifstream ff;
	std::string content;
	ff.open(filename.c_str(),ifstream::in);
	if (ff.fail())
	{
	  cout << "Can't open file " << filename << "\n";
	  return content;
	}
	std::stringstream stream;
	while(ff.good())stream << (char)ff.get();
	content=stream.str();
	ff.close();
	return content;
}

bool Matrix::Load(std::string filename)
{
	return Load(filename,"\t",NULL);
}
float Matrix::Max(unsigned int col)
{
	float max=(*this)[0][col];
	for(unsigned int i=1;i<Dim();i++)
	{
		if ((float)(*this)[i][col]>max)
			max=(*this)[i][col];
	}
	return max;
}

float Matrix::Min(unsigned int col)
{
	float min=(*this)[0][col];
	for(unsigned int i=1;i<Dim();i++)
	{
		if ((float)(*this)[i][col]<min)
			min=(*this)[i][col];
	}
	return min;
}

void Matrix::SetHeader(std::string header)
{
	Header=header;
}

bool Matrix::Load(std::string filename,std::string seperator,Value (*parser)(char *str,unsigned int value))
{
	Clear();
	char *content;
	std::string file;
	char *endptr;
	char *ptr,*pjotr;
	double *values=NULL;
	int n_values=0;
	double x,y,z;
	cout << "Loading matrix from \"" << filename << "\"\n";
	file=read_file(filename);
	if (file.empty())
	{
	  cout << "Error while reading file.\n";
	  return false;
	}
	string::size_type pos=file.find("\n");
	std::string line;
	std::string header;
	bool is_header=true;
	while (file.size()>0)
	{
		if (pos==file.npos)
		{
			file.clear();
			break;
		}
		else
		{
			line=file.substr(0,pos);
			file.erase(0,pos+strlen("\n"));
			pos=file.find("\n");
		}
		if (is_header && line.find("#")==0)
		{
			header.append(line);
			header.append("\n");
		}
		else
			is_header=false;
		Row r;
		Value v;
		n_values=0;
		string::size_type token_pos=line.find(seperator);
		if (is_header==false)
		{
		while(line.size()>0)
		{
			std::string token;
			if (token_pos==line.npos)
			{
				token=line;
				line.clear();
			}
			else
			{
				token=line.substr(0,token_pos);
				line.erase(0,token.size()+seperator.size());
				token_pos=line.find(seperator);
			}
			if (parser==NULL)
			{
				std::istringstream inpStream(token);
				if (token.find_first_not_of("01234567890.defEFD+-^,")==std::string::npos)
				{
					inpStream >> x;
					v=x;
				}
				else
					v=token;
			}
			else
				v=parser((char *)token.data(),n_values);
			r.Set(n_values++,v);
		}
		Rows.push_back(r);
		}	
		cout << "Lines read:" << Dim() << "\r";
	}
	SetHeader(header);
	cout << "\nDone\n";
	return true;
}

Row Matrix::Get(unsigned int row)
{
	Row r;
	if (row<Rows.size())
		return Rows[row];
	return r;
}

Row Matrix::operator[](unsigned int row)
{
	return Get(row);
}

bool Matrix::Write(std::string filename)
{
	return Write(filename,"\t",NULL);
}

void Matrix::Set(unsigned int row,Row r)
{
	if (row>=Rows.size())
		Rows.resize(row+1);
	Rows[row]=r;
}

void Matrix::Set(unsigned int row,unsigned int col,Value value)
{
	if (row>=Rows.size())
		Rows.resize(row+1);
	Rows[row].Set(col,value);
}



bool Matrix::Write(std::string filename,std::string seperator,std::string (*formatter)(float value,unsigned int col))
{

	int series=1;
	std::ofstream ff;
	std::stringstream stream;
	ff.open((const char *)filename.data());
	cout << "Writing matrix to \"" << filename << "\"\n";
	if (ff.fail())
	{
		cout << "Error in writing matrix: Can't open file:" << filename << "\n";
		return false;
	}
	ff << Header;
	for(int x=0;x<Dim();x++)
	{
		Row r=Rows[x];
		for(unsigned int j=0;j<r.Dim();j++)
		{
			std::string value="";
			if (formatter!=NULL)
				value=formatter(r[j],j);
			else
			{
				value.append((std::string)r[j]);
			}
			if (j<r.Dim()-1)
				stream << value << seperator;
			else
				stream << value;
		}
		stream << "\n";
	}
	ff << stream.str();
	ff.close();
	return true;
}

void Matrix::Clear()
{
	for(unsigned int i=0;i<Rows.size();i++)
		Rows[i].Clear();
	Rows.clear();
}

unsigned int Matrix::Dim()
{
	return Rows.size();
}

Row::Row()
{

}

Row::~Row()
{

}

unsigned int Row::Dim()
{
	return Columns.size();
}

bool Row::Set(unsigned int col,Value value)
{
	if (col>=Columns.size())
		Columns.resize(col+1);
	Columns[col]=value;
	return true;
}

Value Row::operator[](unsigned int col)
{
	if (col<Columns.size())
		return Columns[col];
	return 0.0;
}

void Row::Clear()
{
	Columns.clear();
}