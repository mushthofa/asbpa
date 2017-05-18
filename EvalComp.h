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
 * @file   EvalComp.h
 * @author Mushthofa
 *
 * 
 * @brief  Evaluation of one component
 * 
 * 
 */

#ifndef _EVALCOMP_H
#define _EVALCOMP_H

#include "ASPSolver.h"
#include "PrologEngine.h"
#include "ProgramSubsetGenerator.h"
#include "ModelCheck.h"
#include "ModelGenerator.h"
#include "MinChecker.h"

/* Abstract base class */
class EvalComp
{
	public:
		EvalComp(unsigned long long n, bool limAS = false )
		:maxNumAS(n), numAS(0), limitAS(limAS)
		{}
		virtual ~EvalComp()
		{}
		virtual void setInput(const Program& , const AtomSet& f) = 0;
		virtual void clearInput() = 0;
		
		bool answerSetsLeft() const
		{
			return answersetsleft;
		}
		
		AtomSet getNextAnswerSet()
		{
			AtomSet result = currentAnswerSet;
			doEval();
			return result;	
		}
		
		
	protected:
		virtual void doEval() = 0;
		
		Program IDB;
		AtomSet EDB;
		AtomSet currentAnswerSet;
		bool answersetsleft;
		
		/* Limiting the number of answer sets to be computed */
		bool limitAS;
		unsigned long long numAS;
		unsigned long long maxNumAS;
};

/* Empty component evaluation class */
/* Dummy evaluation to simulate other non-empty component's evaluation */

class EvalEmpty : public EvalComp
{
	public:
		EvalEmpty();
		~EvalEmpty();
		
		virtual void setInput(const Program&, const AtomSet& f);
		
		virtual void clearInput();
		
		virtual void doEval();
		
	protected:
		
};


/* Direct Evaluation by ASP Solver */

class EvalDirect : public EvalComp
{
	public:
		EvalDirect(ASPSolverEngine*, ProgramBuilder* pb, unsigned long long n, bool limAS=false);
		~EvalDirect();
		
		virtual void setInput(const Program&, const AtomSet& f);
		
		virtual void clearInput();
		
		
		
	protected:
		virtual void doEval();
		
		ASPSolver* solver;
		
};

/* Evaluation Method 1 */
class EvalMethod1: public EvalComp
{
	public:
		EvalMethod1(ASPSolverEngine*, ProgramBuilder*, PrologEngine*, unsigned long long n, bool limAS=false );
		~EvalMethod1();
				
		virtual void setInput(const Program&, const AtomSet& f);
		
		virtual void clearInput();
		
	protected:
			
		virtual void doEval();
		
		ASPSolverEngine* solverEngine;
		ProgramBuilder* programBuilder;
		ASPSolver* checkSolver;
		ASPSolver* evalSolver;
		
		ProgramSubsetGenerator* subsetGenerator;
		
		Program currentSubset;
		bool currentSubsetDone;
		
		PrologThread* pt;
		
		ModelCheck* modelChecker;
		
};

/* Evaluation Method 2 */

class EvalMethod2: public EvalComp
{
	public:
		EvalMethod2(ASPSolverEngine*, ProgramBuilder*, PrologEngine*, unsigned long long n, bool limAS = false);
		~EvalMethod2();
		
		virtual void setInput(const Program&, const AtomSet& f);
		
		virtual void clearInput();
		
	protected:
			
		virtual void doEval();
		
		
		ASPSolverEngine* solverEngine;
		ProgramBuilder* programBuilder; 
		PrologThread* prologThread;
		
		Program IDB;
		AtomSet EDB;
		
		ModelGenerator* modelGenerator;
		ProgramSubsetGenerator* subsetGenerator;
		PrologThread* pt;
};

/* Evaluation Method 3: Non-HCF disjunctive programs */

class EvalDisjunctive: public EvalComp
{
	public:
		EvalDisjunctive(ASPSolverEngine*, ProgramBuilder*, PrologEngine*, 
					 unsigned long long n, bool limAS=false);
		~EvalDisjunctive();
		virtual void setInput(const Program&, const AtomSet& f);
		
		virtual void clearInput();
		
	protected:
			
		virtual void doEval();
		Program IDB;
		AtomSet EDB;
		
		ASPSolverEngine* solverEngine;
		PrologEngine* prologEngine;
		ProgramBuilder* programBuilder;
		
		ModelGenerator* modelGenerator;
		
		MinChecker* minChecker;
		
};

#endif
// End
