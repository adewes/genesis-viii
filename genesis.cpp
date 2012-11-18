// Genesis VIII.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <stdio.h>
#include <iostream>
#include <math.h>
#include "utils/logger.h"
#include "utils/scriptmanager.h"
#include "utils/matrix.h"
#include "utils/statistics.h"

#include "gp/program.h"
#include "gp/var_program.h"
#include "gp/terrorism_program.h"
#include "gp/symbolic_program.h"
#include "gp/distribution_program.h"
#include "gp/copula_program.h"
#include "gp/tree.h"
#include "gp/branch.h"
#include "gp/leaf.h"
#include "gp/population.h"
#include "gp/es_program.h"

#ifndef VISUAL_CPP
#include<unistd.h>
#endif


int silent=0;//Controlled by the -s switch. Shuts down input,output and error channels.
int daemonize=0;//Daemonizes the program, i.e. makes it run in the background. Controlled by the -d switch.
int interpret_file=0;//Interpret a file???
int redirect=0;//Redirect stdout and stderr to a logfile???
char *filename=NULL;
char *redirect_file=NULL;
int do_eval=0;
char *eval_string=NULL;

void process_flag(char flag,char *optarg)
{
		switch (flag)
		{
			case 's':
				silent=1;
				break;
			case 'e':
				if (optarg!=NULL)
				{
					do_eval=1;
					eval_string=optarg;
				}
				break;
			case 'd':
				daemonize=1;
				break;
			case 'r':
				if (optarg!=NULL)
				{
					redirect=1;
					redirect_file=optarg;
				}
				break;
			case 'f':
				if (optarg!=NULL)
				{
					filename=optarg;
					interpret_file=1;
				}
				break;
			default:
				
				break;
		}
}

//Makes just what it says.
void parse_command_line(int argc,char *argv[])
{
	int c=0;
	extern char *optarg;
	extern int optind, optopt;
#ifndef VISUAL_CPP
	while ((c=getopt(argc,argv,"e:sdf:r:"))!=-1)
	{
		process_flag(c,optarg);
	}
#else
	int x;
	char flag[1024];
	for(x=0;x<argc;x++)
	{
		if(sscanf(argv[x],"-%s",flag))
		{
			process_flag(flag[0],x<argc-1?argv[x+1]:NULL);
		}
	}
#endif
}

void Setup(void)
{
	
	srand(time(NULL));
	Logger *ML=ML->i();
	ScriptManager *SM=SM->i();
	//We set up the scripting and logging facilities...
	ML->Setup();
	SM->Setup();
	LuaMatrix::Setup();
	LuaLogger::Setup();
	LuaStat::Setup();
	LuaPopulation::Setup();
}

void console(void)
{
	ScriptManager *SM=SM->i();
    char buffer[2048];
    char *tmp;
    /*signal(SIGABRT,new_flux_handler);
    signal(SIGINT,handler);*/
    fflush(stdin);
	buffer[0]=0;
    while(1)
    {
	  
	    cout << "\ngenesis>";
	    fflush(stdout);
		if (cin.getline(buffer,2048))
		{
			tmp=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
			strcpy(tmp,buffer);
			SM->Eval(tmp);
		}
    }
}

Program *my_var_generator()
{
	Program *p=new VaRProgram();
	return p;
}

Program *my_terrorism_generator()
{
	Program *p=new TerrorismProgram();
	return p;
}

Program *my_symbolic_generator()
{
	Program *p=new SymbolicProgram();
	return p;
}

Program *my_copula_generator()
{
	Program *p=new CopulaProgram();
	return p;
}

Program *my_es_generator()
{
	Program *p=new ESProgram();
	return p;
}

Program *my_distribution_generator()
{
	Program *p=new DistributionProgram();
	return p;
}

int main(int argc, char *argv[])
{
	Setup();
	ScriptManager *SM=SM->i();
	SM->RegisterUserdata("var","program",(void *)my_var_generator);
	SM->RegisterUserdata("es","program",(void *)my_es_generator);
	SM->RegisterUserdata("terror","program",(void *)my_terrorism_generator);
	SM->RegisterUserdata("symbolic","program",(void *)my_symbolic_generator);
	SM->RegisterUserdata("copula","program",(void *)my_copula_generator);
	SM->RegisterUserdata("distribution","program",(void *)my_distribution_generator);
	parse_command_line(argc,argv);
	Leaf MyLeaf;
	unsigned int i,j;
/*	Population *Pop=new Population;
	Pop->Create(10000,10,my_generator);
	while(1)
	{
		Pop->Evaluate();
		Pop->Evolve();
	}
	Pop->Evaluate();
	cout << "Best program:" << Pop->Best->Print() << "(" << Pop->Best->Eval() << ")\n";
*/
	if(silent)
    {
	    cout << "Going to silent mode...\n";
	    fclose(stdout);
	    fclose(stdin);
	    fclose(stderr);
    }
    
	if (redirect)
    {
	    cout << "Redirecting stdout/stderr to file \"" << redirect_file << "\"\n";
	    fflush(stdout);
	    fclose(stderr);
	    fclose(stdin);
	    freopen(redirect_file,"w+",stdout);
    }

	if (do_eval)
	{
		SM->Eval(eval_string);
	}
	if (interpret_file)
	{
		SM->Require("startup",filename);
		SM->Run("startup");
	}
	console();
	return 0;
}

