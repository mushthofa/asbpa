/***************************************************************************
 *   Copyright (C) 2009 by Mushthofa   								*
 *   unintendedchoice@gmail.com   									*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>

#include "SpiritProgramParser.h"
#include "DLVASPSolver.h"
#include "XSBPrologEngine.h"
#include "EvalComp.h"
#include "BoostComponentFinder.h"
#include "GraphBuilder.h"
#include "GraphProcessor.h"
#include "BoostHCFDetector.h"
#include "SimpleMethodSelector.h"
#include "OutputBuilder.h"
#include <ctime>

void showUsage(char* prog)
{
	std::cout <<"BPA"<<std::endl;
	std::cout <<"Answer set semantics for programs with Bounded Predicate Arities." <<std::endl;
	std::cout <<"USAGE  : ";
	std::cout <<prog<<" [OPTIONS] [FILE] "<<std::endl<<std::endl;
	std::cout << "Reads program from file FILE and evaluate the program under answer set semantics" <<std::endl;
	std::cout << "If no filename is given, reads from standard input instead "<<std::endl;
	std::cout << "Supported options are the following: "<<std::endl;
	std::cout << "-m=M\t selects evaluation method, where M can be 1, 2 or 3 "<<std::endl;
	std::cout << "\t note that if (a component of) the program is non-HCF, "<<std::endl
			<< "\t method 3 will be used to evaluate the component regardless "<<std::endl
			<< "\t of the method selected " <<std::endl
			<< "\t if not given, an internal method selection heuristic will determine" <<std::endl
			<< "\t the methods used "<<std::endl;
	
	std::cout << "-n=N\t limits the number of answer set printed to N "<<std::endl;
	std::cout << "-filter=p1[,p2[...]]\n\t filters the answer set to contain only " <<std::endl
			<< "\t extensions of predicates p1, p2,..." <<std::endl;
	
	std::cout << "-ruleml\t output the answer set in RuleML format " <<std::endl;
	//std::cout << "-no-scc\t disables SCC decomposition (can make evaluation very slow) " <<std::endl;//
	std::cout << "-grmax=N\t sets the maximum number of ground rules/1000 a rule may have "<<std::endl;
	std::cout << "\t to be considered 'small'. this option affects the model generations and " <<std::endl;
	std::cout << "\t minimality checking. defaults to 10 " <<std::endl;
	std::cout << "-vmax=N\t sets the minimum number of variables a rule may have "<<std::endl;
	std::cout << "\t to be considered 'big'. this option affects the model generations and " <<std::endl;
	std::cout << "\t minimality checking. defaults to 4 " <<std::endl;
	std::cout << "-v, -V\t be verbose (prepare for a long output)." <<std::endl;
	std::cout << "-h, --help\t display this help text "<<std::endl;
	
	
	std::cout << std::endl;
}

int main(int argc, char *argv[])
{
	GetPot cl(argc, argv);
	Globals::Instance()->processArgs(cl);
	
	if(Globals::Instance()->boolOption("help"))
	{
		showUsage(argv[0]);
		return EXIT_SUCCESS;
	}
	
	AtomSet EDB;
	Program IDB;

	/* Read and parse input */
	try
	{
		ProgramParser* parser = new SpiritProgramParser();
		EDB = parser->getEDB();
		IDB = parser->getIDB();
		delete parser;
	}
	catch(GeneralError& e)
	{
		std::cerr << e.getErrorMsg() << std::endl;
		
		return EXIT_FAILURE;
	}
	
	time_t start = time(NULL);
	
	OutputBuilder* ob;
	
	std::string filters = Globals::Instance()->strOption("filter");
	
	if(Globals::Instance()->boolOption("ruleml"))
		ob = new OutputXMLBuilder(filters);
	else
		ob = new OutputTextBuilder(filters);
	
	/* Program is empty, just check consistency of the facts */
	if(IDB.empty())
	{
		if(EDB.isConsistent())
		{
			ob->buildPre();
			ob->buildOutput(EDB);
			ob->buildPost();
			std::cout << ob->getString();
		}
		delete ob;
		return EXIT_SUCCESS;
	}
	
	
	
	/* Framework component classes for evaluation */
	
	/* Here it is possible to customize bpa to use different ASP solvers, Prolog engine etc. */
	ProgramBuilder* pb  = new DLVProgramBuilder();
	
	ASPSolverEngine* se = new DLVASPSolverEngine();
	
	PrologEngine* pe = new XSBPrologEngine(XSBHOME);
	
	
	/* This is currently experimental, some dependency preprocessing is needed in the evaluation steps
		so using no-scc might give incorrect result */
	
	if(Globals::Instance()->boolOption("no-scc"))
	{
		EvalComp* evalcomp;
		unsigned method = Globals::Instance()->intOption("EvalMethod");
		bool limitAS = Globals::Instance()->boolOption("limitAS");
		unsigned long long maxNumAS = Globals::Instance()->intOption("maxNumAS");
		unsigned foundAS = 0;
		switch(method)
		{
			case 0:
				evalcomp = new EvalDirect(se, pb, maxNumAS, limitAS);
				break;
			case 1:
				evalcomp = new EvalMethod1(se, pb, pe, maxNumAS, limitAS);
				break;
			case 2:
				evalcomp = new EvalMethod2(se, pb, pe, maxNumAS, limitAS);
				break;
			case 3:
				evalcomp = new EvalDisjunctive(se, pb, pe, maxNumAS, limitAS);
				break;
			default:
				evalcomp = new EvalDirect(se, pb, maxNumAS, limitAS);
				break;
		}
		
		try
		{
			evalcomp->setInput(IDB, EDB);
			ob->buildPre();
			while(evalcomp->answerSetsLeft())
			{
				foundAS++;
				AtomSet currentAS = evalcomp->getNextAnswerSet();
				ob->buildOutput(currentAS);
				std::cout << ob->getString() <<std::endl;
				if( Globals::Instance()->boolOption("limitAS") &&
					(foundAS >= Globals::Instance()->intOption("maxNumAS") ))
					break;
			}
			ob->buildPost();
			std::cout << ob->getString();
		}
		catch(GeneralError& e)
		{
			std::cerr <<e.getErrorMsg()<<std::endl;
			delete evalcomp;
			delete pb; 
			delete se; 
			delete pe;
			delete ob;
			return EXIT_FAILURE;
		}
		delete evalcomp;
		delete pb; 
		delete se;
		delete pe;
		delete ob;
		return EXIT_SUCCESS;
	}
	
	/* Strategy classes for dependency analysis/SCC */
	HCFDetector* hcfd = new BoostHCFDetector();
	
	MethodSelector* ms = new SimpleMethodSelector(hcfd);
	
	GraphBuilder gb;

	NodeGraph nodegraph;

	try
	{
		/* Build the graph of dependency between atoms */
		gb.run(IDB, nodegraph);
	}
	catch (GeneralError& e)
	{
		std::cerr << e.getErrorMsg() << std::endl << std::endl;
		delete ms;
		delete hcfd;
		delete pb; 
		delete se; 
		delete pe;
		delete ob;
		return EXIT_FAILURE;
	}
	
	/*
	gb.dumpGraph(nodegraph, std::cout);
	return EXIT_SUCCESS;
	*/
	
	ComponentFinder* cf;

	//
	// The DependencyGraph identifies and creates the graph components that will
	// be processed by the GraphProcessor.
	//
	DependencyGraph* dg;

	try
	{
		cf = new BoostComponentFinder();

		//
		// Initializing the DependencyGraph. Its constructor uses the
		// ComponentFinder to find relevant graph
		// properties for the subsequent processing stage.
		//
		dg = new DependencyGraph(nodegraph, cf);
	}
	catch (GeneralError &e)
	{
		std::cerr << e.getErrorMsg() << std::endl << std::endl;
		
		delete cf;
		delete dg;
		delete ms;
		delete hcfd;
		delete pb; 
		delete se; 
		delete pe;
		delete ob;
		
		return EXIT_FAILURE;
	}
	
	/* Create the graph processors to perform evaluation along SCCs */
	
	GraphProcessor* gp = new GraphProcessor(dg, se, pb, pe, ms, ob);
	
	/* Run it */
	
	ob->buildPre();
	try
	{
		gp->run(EDB);
	}
	catch(GeneralError& e)
	{
		std::cerr << e.getErrorMsg() << std::endl;
		delete gp;
		delete cf;
		delete dg;
		delete ms;
		delete hcfd;
		delete pb; 
		delete se; 
		delete pe;
		delete ob;
		return EXIT_FAILURE;
	}
	ob->buildPost();
	std::cout << ob->getString();
	
	/* computation ends */
	time_t end = time(NULL);
	
	std::cout << "BPA done.."<<std::endl;
	std::cout << "Time: " <<(end-start) << "s" << std::endl;
	
	

	delete gp;
	delete cf;
	delete dg;
	delete ms;
	delete hcfd;
	delete pb; 
	delete se; 
	delete pe;
	delete ob;
	
	/* We are done */
	
  	return EXIT_SUCCESS;
}
