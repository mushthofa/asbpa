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



/** 
 *	@brief GraphProcessor.h
 * 		Provides a class to perform evaluation along the components dep. graph
 * 		Assuming that the components is already given in a topological ordering
 *
 *	@author Mushthofa
 */


#ifndef _GRAPH_PROCESSOR_H
#define _GRAPH_PROCESSOR_H

#include <map>

#include "Component.h"
#include "AtomSet.h"
#include "ASPSolver.h"
#include "PrologEngine.h"
#include "DependencyGraph.h"
#include "MethodSelector.h"
#include "OutputBuilder.h"


class GraphProcessor
{
	public: 
		GraphProcessor(DependencyGraph*, ASPSolverEngine*, ProgramBuilder*, 
					PrologEngine*, MethodSelector*, OutputBuilder*);
		
		~GraphProcessor();
		void run(AtomSet& input)
		{
			initialInput = input;
			eval(0);
		}
	private:
		void eval(unsigned);
		
		/* Get all input needed for a component */
		AtomSet getInput(unsigned);
		
		/* map to store globalAS data */
		std::map<Component*, AtomSet> globalAS;
		
		/* the components */
		std::vector<Component*> components;
		
		/* Initial input EDB */
		AtomSet initialInput;
		ASPSolverEngine* solver_engine;
		ProgramBuilder* prgbuilder;
		PrologEngine* prolog_engine;
		
		MethodSelector* method_selector;
		
		OutputBuilder* outputBuilder;
		static unsigned long long numAS;
};

#endif
//end 

