#ifndef _MATRIX
#define _MATRIX

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "scriptmanager.h"

using namespace std;

//Value class. Can contain string and floats. Automatic type conversion possible.
class Value
{
public:
	void operator=(float a){is_float=1;fvalue=a;};
	void operator=(std::string s){is_float=0;svalue=s;};
	operator int(){return (int)fvalue;};
	operator float(){return fvalue;};
	operator double(){return (double)fvalue;};
	operator std::string(){if (is_float){std::ostringstream oss;oss << fvalue;std::string output(oss.str());return output;}return svalue;};
	Value(){is_float=1;fvalue=0.0;};
	Value(float f){is_float=1;fvalue=f;};
	Value(const char *str){is_float=0;svalue=str;};
	bool IsFloat(){return is_float;};
	Value(std::string s){is_float=0;svalue=s;};
	~Value(){};
private:
	bool is_float;
	float fvalue;
	std::string svalue;
};

class Row
{
public:
	Row();
	~Row();
	void Clear();
	virtual bool Set(unsigned int col,Value value);
	virtual Value operator[](unsigned int col);
	unsigned int Dim();
private:
	std::vector<Value> Columns;
protected:
};

class Matrix
{
public:
	Matrix();
	Matrix(lua_State *L);
	
	/*Lua glue functions...*/
	int lua_set(lua_State *L);
	int lua_get(lua_State *L);
	int lua_dim(lua_State *L);
	int lua_write(lua_State *L);
	int lua_load(lua_State *L);
	int lua_clear(lua_State *L);

	float Max(unsigned int col);
	float Min(unsigned int col);
	virtual bool Load(std::string filename);
	virtual bool Load(std::string filename,std::string seperator,Value (*parser)(char *str,unsigned int col));
	virtual bool Write(std::string filename);
	virtual bool Write(std::string filename,std::string seperator,std::string (*formatter)(float value,unsigned int col));
	virtual Row operator[](unsigned int col);
	virtual Row Get(unsigned int col);
	virtual void Set(unsigned int row,Row r);
	virtual void Set(unsigned int row,unsigned int col,Value value);
	virtual void Clear();
	virtual void SetHeader(std::string header);
	unsigned int Dim();
	~Matrix();
	static const char className[];
    static const Luna<Matrix>::RegType Register[];
private:
	std::string Header;
	std::vector<Row> Rows;
protected:
};

namespace LuaMatrix
{
	void Setup(void);	
};

#endif