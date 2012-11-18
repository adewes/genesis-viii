#include "logger.h"
#include "scriptmanager.h"

using namespace std;

namespace LuaLogger
{

	int reopen(lua_State *L)
	{
		int n=lua_gettop(L);
		Logger *l=l->i();
		if (n==1 && lua_isstring(L,-n))
		{
			std::string str=lua_tostring(L,-n);
			l->Reopen(str);
		}
		return 0;
	}

	int redirect_output(lua_State *L)
	{
		int n=lua_gettop(L);
		Logger *l=l->i();
		if (n==1 && lua_isstring(L,-n))
		{
			std::string str=lua_tostring(L,-n);
			l->RedirectOutput(str);
		}
		return 0;
	}

	void Setup(void)
	{
		ScriptManager *SM=SM->i();
		SM->RegisterFunction("reopen","logger",LuaLogger::reopen);
		SM->RegisterFunction("redirect_output","logger",LuaLogger::redirect_output);
	}
};

void Logger::Setup()
{
	ErrorHandle.open ( "log/error.log",ios::trunc );
	EventHandle.open ( "log/event.log",ios::trunc );
	ScriptHandle.open ( "log/lua.log",ios::trunc );
	Error("Starting error log.");
	Event("Starting event log.");
	Script("Starting scripting log.");
}

void Logger::Reopen(std::string directory)
{
	ErrorHandle.close();
	EventHandle.close();
	ScriptHandle.close();
	std::string filename=directory;
	filename.append("/error.log");
	ErrorHandle.open(filename.data(),ios::trunc );
	filename=directory;
	filename.append("/event.log");
	EventHandle.open(filename.data(),ios::trunc );
	filename=directory;
	filename.append("/lua.log");
	ScriptHandle.open(filename.data(),ios::trunc );	
}

void Logger::RedirectOutput(std::string filename)
{
    cout << "Redirecting stdout/stderr to file \"" << filename << "\"\n";
    fflush(stdout);
    fclose(stderr);
    fclose(stdin);
    freopen(filename.data(),"w+",stdout);
}

std::string Logger::LogTime()
{
	char *timestring;
	time_t currentTime;
	time (&currentTime);
	timestring=ctime(&currentTime);
	timestring[strlen(timestring)-1]=0x0;
	return timestring;
}

Logger::~Logger()
{
	ErrorHandle.close();
	EventHandle.close();
	ScriptHandle.close();
}

void Logger::Script(std::string msg)
{
	ScriptHandle << LogTime() << ": " << msg << "\n";
	ScriptHandle.flush();
}

void Logger::PlainScript(std::string msg)
{
	ScriptHandle << msg ;
	ScriptHandle.flush();
}

void Logger::Error(std::string msg)
{
	ErrorHandle << LogTime() << ": " << msg << "\n";
	ErrorHandle.flush();
}

void Logger::Event(std::string msg)
{
	EventHandle << LogTime() << ": " << msg << "\n";
	EventHandle.flush();
}

void Logger::Game(std::string msg)
{
	GameHandle << LogTime() << ": " << msg << "\n";
	GameHandle.flush();
}
