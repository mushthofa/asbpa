/***************************************************************************
 *   Copyright (C) 2009 by Mushthofa   								*
 *   unintendedchoice@gmail.com  									*
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

/** 
 *	@brief GraphProcessor.cpp
 * 		Provides a class to perform evaluation along the components dep. graph
 * 		Assuming that the components is already given in a topological ordering
 *
 *	@author Mushthofa
 */


#include "GraphProcessor.h"
#include "EvalComp.h"

unsigned long long GraphProcessor::numAS = 0;

GraphProcessor::GraphProcessor(DependencyGraph* dg, ASPSolverEngine* se, ProgramBuilder* pb,
						 PrologEngine* pe, MethodSelector* ms, OutputBuilder* ob)
	:solver_engine(se), prgbuilder(pb), 
	prolog_engine(pe), method_selector(ms), outputBuilder(ob)
{
	components = dg->getComponents();
	
	/* Set up array for storing individual AS */
	
	std::vector<Component*>::const_iterator cit;
	unsigned i=0;
	for(cit = components.begin(); cit!= components.end(); ++cit)
	{
		globalAS.insert(std::pair<Component*, AtomSet> (components[i++], AtomSet()));
	}
}

GraphProcessor::~GraphProcessor()
{
}

AtomSet GraphProcessor::getInput(unsigned idx)
{
	AtomSet result;
	
	/* Concatenate all the input from the previous components */
	/* If index is zero, then we take from the initial input (EDB) */
	
	if(idx == 0)
	{
		return initialInput;
	}
	for(unsigned i=0; i<idx; i++)
	{
		result.insert(globalAS[components[i]]);
	}
	return result;
}


void GraphProcessor::eval(unsigned idx)
{
	if(Globals::Instance()->boolOption("limitAS"))
	{
		/* Have we found enough answer sets? */
		if(numAS >= Globals::Instance()->intOption("maxNumAS"))
			return;
	}
	
	EvalComp* compeval;
	
	/* Get the input */
	AtomSet input = getInput(idx);
	
	/* Decide what evaluation method we should call */
	MethodSelector::Method_t selMethod = method_selector->getMethod(components[idx]);
	
	Program currentComp = components[idx]->getBottom();

	if(Globals::Instance()->intOption("verboseLevel") > 0 && !currentComp.empty())
	{
		std::cout <<std::endl;
		std::cout<<"Evaluating component "<<idx<<": " << std::endl;
		prgbuilder->clearString();
		prgbuilder->buildProgram(currentComp);
		std::cout <<prgbuilder->getString() <<std::endl;
		std::cout <<"with input "<< std::endl << input << std::endl;
		std::cout <<"using " << MethodSelector::getMethodName(selMethod) << std::endl;
	}

	
	switch(selMethod)
	{
		case MethodSelector::EMPTY :
			compeval = new EvalEmpty();
			break;
		
		case MethodSelector::DIRECT :
			compeval = new EvalDirect(solver_engine, prgbuilder, 0, false);
			break;
			
		case MethodSelector::METHOD_NORMAL1 :
			compeval = new EvalMethod1(solver_engine, prgbuilder, prolog_engine, 0, false);
			break;
		
		case MethodSelector::METHOD_NORMAL2 :
			compeval = new EvalMethod2(solver_engine, prgbuilder, prolog_engine, 0, false);
			break;
			
		case MethodSelector::METHOD_DISJ :
			compeval = new EvalDisjunctive(solver_engine, prgbuilder, prolog_engine, 0, false);
			break;
			
		default:
			throw GeneralError("Method Selector did not select an evaluation method");
	}
	compeval->setInput(currentComp, input);
	
	AtomSet currentAS;
	while(compeval->answerSetsLeft())
	{
		if(idx == components.size()-1)
		{
			/* Last component to be evaluated */
			/* Just stream it out */
			currentAS = compeval->getNextAnswerSet();
			outputBuilder->buildOutput(currentAS);
			std::cout << outputBuilder->getString() <<std::endl;
			numAS++;
			
		}
		else
		{
			/* Call eval recursively for the next components */
			currentAS = compeval->getNextAnswerSet();
			if(Globals::Instance()->intOption("verboseLevel") > 0 && !currentComp.empty())
			{
				std::cout <<" Answer set of component " <<idx<<std::endl<<currentAS<<std::endl;
				std::cout <<std::endl;
			}
			globalAS[components[idx]] = currentAS;
			eval(idx+1);
		}
		if(Globals::Instance()->boolOption("limitAS") && numAS >= Globals::Instance()->intOption("maxNumAS"))
				break;
	}
	if(Globals::Instance()->intOption("verboseLevel") > 0 && !currentComp.empty())
	{
		std::cout <<" Finished with component " <<idx<<std::endl;
		std::cout <<std::endl;
	}
	
	delete compeval;
	
}

//end
