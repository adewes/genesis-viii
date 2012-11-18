#include "scriptmanager.h"
#include "logger.h"

#include <fstream>

namespace LuaScript
{
	int exit(lua_State* L)
	{
		::exit(0);
		return 0;
	}
	int print(lua_State* L)
	{
		Logger *ML=ML->i();
		int n=lua_gettop(L);
		for(int i=-n;i<0;i++)
		{
			if (lua_isstring(L,i))
			{
				std::string msg=lua_tostring(L,i);
				cout << msg;
				ML->PlainScript(msg);
			}
		}
		return 0;
	}
};


void ScriptManager::Setup(void)
{
	Lua = lua_open();

	luaopen_math(Lua);
	luaopen_debug(Lua);
	luaopen_string(Lua);
	//We overwrite the print function and redirect the output to a log file.

	lua_pushcfunction(Lua,LuaScript::print);
	lua_setglobal(Lua,"print");
	lua_register(Lua,(const char *)"exit",LuaScript::exit);
}

ScriptManager::~ScriptManager()
{
	
}
int ScriptManager::Getint(std::string name)
{
	lua_getglobal(Lua,(const char *)name.data());
	int n=lua_tointeger(Lua,0);
	return n;
}


float ScriptManager::Getfloat(std::string name)
{
	lua_getglobal(Lua,(const char *)name.data());
	lua_Number n=lua_tonumber(Lua,0);
	return (float)n;
}

int ScriptManager::Run(std::string name)
{
	Entry MyEntry=Search(name);
	if (MyEntry.valid)
	{
		luaL_dostring(Lua,(const char *)MyEntry.data.script.data());
		return 0;
	}
	return -1;
}

#undef luaL_dostring
#define luaL_dostring(L,s) (luaL_loadstring(L, s) || lua_pcall(L, 0, LUA_MULTRET, 0))
int ScriptManager::Eval(std::string code)
{
	luaL_dostring(Lua,(const char *)code.data());
	return 0;
}

int ScriptManager::RegisterFunction(std::string name,lua_CFunction f)
{
	//Isn't that easy ;-)
	lua_register(Lua,(const char *)name.data(),f);
	return 0;
}

int ScriptManager::RegisterFunction(std::string name,std::string package,lua_CFunction f)
{
	lua_getglobal(Lua,(const char *)package.data());
	if (lua_isnil(Lua,-1))
	{
		lua_newtable(Lua);//We create a new table...
		lua_setglobal(Lua,(const char *)package.data());//We set the table name...
	}

	lua_getglobal(Lua,(const char *)package.data());
	lua_pushstring(Lua,(const char *)name.data());//We set the name of the new function

	lua_pushcfunction(Lua , f);//We push the function onto the stack
	lua_settable(Lua,-3);//We set the table value to the function.
	lua_pop(Lua,-1);
	return 0;
}

int ScriptManager::RegisterUserdata(std::string name,std::string package,void *userdata)
{
	lua_getglobal(Lua,(const char *)package.data());
	if (lua_isnil(Lua,-1))
	{
		lua_newtable(Lua);//We create a new table...
		lua_setglobal(Lua,(const char *)package.data());//We set the table name...
	}

	lua_getglobal(Lua,(const char *)package.data());
	lua_pushstring(Lua,(const char *)name.data());//We set the name of the new function

	lua_pushlightuserdata(Lua , userdata);//We push the function onto the stack
	lua_settable(Lua,-3);//We set the table value to the function.
	lua_pop(Lua,-1);
	return 0;
}


ScriptEntry ScriptManager::Load(std::string Filename)
{
	ScriptEntry MyEntry;
	ifstream file((const char *)Filename.data(),ios::in);
	std::string buffer;
	char string[2];
	string[1]=0;
	while(file.read(string,1)){MyEntry.script.append(string);}
	return MyEntry;
}