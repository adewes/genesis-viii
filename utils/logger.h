#ifndef _LOGGER
#define _LOGGER 1

#include "singleton.h"
#include <string>
#include <ostream>
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

class Logger : public Singleton <Logger>
{
public:
	~Logger();
	void Setup();
	std::string LogTime();
	void Reopen(std::string directory);
	void Error(std::string msg);
	void Event(std::string msg);
	void RedirectOutput(std::string filename);
	void Game(std::string msg);
	void Script(std::string msg);
	void PlainScript(std::string msg);
private:
	ofstream ErrorHandle,EventHandle,GameHandle,ScriptHandle;
};
namespace LuaLogger
{
	void Setup();
};

#endif
