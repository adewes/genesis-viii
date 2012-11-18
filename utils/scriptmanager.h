#ifndef _SCRIPTMANAGER
#define _SCRIPTMANAGER 1

#include "manager.h"

#include <vector>
#include <string>
#include "luna.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

using namespace std;

typedef struct
{
	string script;
} ScriptEntry;

class ScriptManager : public Manager<ScriptEntry,ScriptManager>
{
public:
 ~ScriptManager();
 void Setup();
 int Run(std::string name);
 int Eval(std::string code);
 float Getfloat(std::string name);
 int Getint(std::string name);
 int RegisterFunction(std::string name,lua_CFunction f);
 int RegisterFunction(std::string name,std::string package,lua_CFunction f);
 int RegisterUserdata(std::string name,std::string package,void *userdata);
 ScriptEntry Load(std::string FileHandle);
 lua_State* Lua;
private:
};

#endif
