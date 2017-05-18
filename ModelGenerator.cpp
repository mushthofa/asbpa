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
 * @file ModelGenerator.cpp
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Model generator generates all models of a program, by enumerating subsets of possibly true atoms and 
 * @brief	checking for satisfiability
 *
  */
 
#include "ModelGenerator.h"

SubsetModelGenerator::SubsetModelGenerator(const Program& prog, const AtomSet& facts, ASPSolverEngine* s, ProgramBuilder* pb)
	:ModelGenerator(prog.doShift(), facts)
{
	
	// perform shifting to normalize 
	
	ProgramBuilder* pbpos = new PositiveProgramBuilder();
	ASPSolver* possolver = s->createSolver(pbpos);
	
	try
	{
		possolver->callSolver(IDB, EDB);
	}
	catch( GeneralError& e)	
	{
		throw e;
	}
	
	AtomSet posAS;
	posAS = possolver->getNextAnswerSet();
	
	/* Done with the solver and the positive programbuilder */
	delete possolver;
	delete pbpos;
	
	/* Get the definite part P_D of the program, and compute the answer set D of P_D\union EDB */
	
	//Program def = IDB.getDefinite();
	//pb->buildProgram(def);
	//std::cout<<pb->getString()<<std::endl;
	ASPSolver* solver = s->createSolver(pb);
	try
	{
		solver->callSolver(IDB.getDefinite(), EDB);
	}
	catch(GeneralError& e)
	{
		throw e;
	}
	
	Definite = solver->getNextAnswerSet();
	//std::cout<<"Definite = "<<Definite<<std::endl;
	delete solver;
	
	
	/* Go through all subsets PT of posAS such that D \subseteq PT */
	AtomSet PT = posAS.difference(Definite);
	//std::cout<<"PT = "<<PT<<std::endl;
	/* Create the modelchecker */
	ASPSolver* checkSolver = s->createSolver(pb);
	mc = new ModelCheck(IDB, checkSolver);
	
	vectorPT = PT.asVector();
	
	/* We need to handle the case where PT.size == 0 */
	/* bitmap does not handle this case nicely.. saying no subset is left */
	
	if(vectorPT.size() == 0)
	{
		
		if(Globals::Instance()->intOption("verboseLevel") > 0)
		{
			std::cout<<"Guessed subset: "<<Definite<<std::endl;
		}
		if(mc->checkModel(Definite))
		{
			currentModel = Definite;
			modelsleft = true;
		}
		
		else
			modelsleft = false;
	}
	else
	{
		bitmap = new BitMap (PT.size());
		bool *mask;
		bool found = false;
		while(!found && bitmap->subsetsLeft())
		{
			
			AtomSet copyD = Definite;
			mask = bitmap->getNextMask();
			for(unsigned i=0;i<vectorPT.size();i++)
			{
				if(mask[i])
				{
					copyD.insert(vectorPT[i]);
				}
			}
			
			
			if(Globals::Instance()->intOption("verboseLevel") > 0)
			{
				std::cout<<"Guessed subset: "<<copyD<<std::endl;
			}
			
			//std::cout<<"Checking model "<<std::endl;
			if(mc->checkModel(copyD))
			{
				currentModel = copyD;
				modelsleft = true;
				found = true;
			}
		}
		if(!found)
			modelsleft = false;
	}// End else if vectorPT size == 0;
}

SubsetModelGenerator::~SubsetModelGenerator()
{
	//delete checkSolver;
	delete mc;
	delete bitmap;
}

AtomSet SubsetModelGenerator::getNextModel()
{
	if(!modelsleft || (vectorPT.size()> 0 && !bitmap->subsetsLeft()))
		throw FatalError("No more models left");
	AtomSet resultModel = currentModel;
	
	/* If vectorPT is empty, we only have at most one model, so stop here */
	
	if(vectorPT.size() == 0)
	{
		modelsleft = false;
	}
	else
	{
		bool *mask;
		
		bool found = false;
		while(!found && bitmap->subsetsLeft())
		{
			AtomSet copyD = Definite;
			
			mask = bitmap->getNextMask();
			for(unsigned i=0;i<vectorPT.size();i++)
			{
				if(mask[i])
				{
					copyD.insert(vectorPT[i]);
				}
			}
			
			if(Globals::Instance()->intOption("verboseLevel") > 0)
			{
				std::cout<<"Guessed subset: "<<copyD<<std::endl;
			}
			
			if(mc->checkModel(copyD))
			{
				currentModel = copyD;
				modelsleft = true;
				found = true;
			}
		}
		if(!found)
			modelsleft = false;
	}
	return resultModel;
}

DisjunctiveModelGenerator::DisjunctiveModelGenerator(const Program& p, const AtomSet& f, ASPSolverEngine* se, ProgramBuilder* pb)
	:ModelGenerator(p, f), solverEngine(se)
{
	
	/* First compute the Definite Part */
	
	ASPSolver* solver = solverEngine->createSolver(pb);
	try
	{
		solver->callSolver(IDB.getDefinite(), EDB);
	}
	catch(GeneralError& e)
	{
		throw e;
	}
	
	Definite = solver->getNextAnswerSet();
	//delete solver;

	/* compute PT */
	
	ProgramBuilder* pbpos = new PositiveProgramBuilder();
	ASPSolver* possolver = solverEngine->createSolver(pbpos);

	try
	{
		possolver->callSolver(IDB.doShift(), Definite);
	}
	catch(GeneralError& e)
	{
		throw e;
	}
	PT = possolver->getNextAnswerSet();
	//std::cout <<" PT = "<<PT<< std::endl;
	/* done with solver */
	delete possolver;
	delete solver;
	/* get M = PT - Definite */
	AtomSet M = PT.difference(Definite);

	/* Create the guessing program = Definite + {p v -p | p\in M} + small rule constraints */
	AtomSet::const_iterator m_it;
	Program classicalGuess;
	for(m_it = M.begin(); m_it != M.end(); ++m_it)
	{
		AtomPtr PAptr(new Atom(*m_it));
		AtomPtr NAptr(new Atom(*m_it));
		NAptr->negate();
		RuleHead_t currHead;
		currHead.insert(PAptr);
		currHead.insert(NAptr);
		RulePtr currRule (new Rule(currHead, RuleBody_t(), "", 0));
		classicalGuess.addRule(currRule);
	}

	
	Program smallRule = IDB.getSmall(Globals::Instance()->intOption("smallRuleMaxVar"));
	generatingProg = classicalGuess + smallRule.getCons();
	
	/*
	std::cout <<"%small rule "<< Globals::Instance()->intOption("smallRuleMaxVar") <<std::endl;
	std::cout<<"%Generating program:"<<std::endl;
	pb->clearString();
	pb->buildProgram(generatingProg);
	pb->buildFacts(Definite);
	std::cout<<pb->getString();*/
	
	
	
	/* Get AS of the guessing program */
	pb->clearString();
	genSolver = solverEngine->createSolver(pb);

	try
	{
		genSolver->callSolver(generatingProg, Definite);
	}
	catch(GeneralError& e)
	{
		throw e;
	}
	/* Check the AS against the big rule constraints. */
	
	
	bigConstraints = IDB.getBig(Globals::Instance()->intOption("smallRuleMaxVar"));
	checkSolver = solverEngine->createSolver(pb);


	modelsleft = false;
	AtomSet candModel;

	
	while(genSolver->answerSetsLeft())
	{
		/* Get candidate model */
		candModel = genSolver->getNextAnswerSet();
// 
// 		
// 		std::cout<<"%big constraints"<<std::endl;
// 		pb->clearString();
// 		pb->buildProgram(bigConstraints.getCons());
// 		pb->buildFacts(candModel);
// 		std::cout<<pb->getString()<<std::endl;
	
		try
		{
			/* check against the big constraints */
			checkSolver->callSolver(bigConstraints.getCons(), candModel, true);
		}
		catch(GeneralError& e)
		{
			throw e;
		}
		
		/* If it is consistent, we obtain a model */
		if(checkSolver->answerSetsLeft())
		{
			//std::cout<<"Initially model " <<candModel <<std::endl;
			candModel.intersectWith(PT);
			//std::cout<<" Filtered model after " <<candModel <<std::endl;
			currentModel = candModel;
			modelsleft = true;
			break;
		}
	}
	
	
	
}

DisjunctiveModelGenerator::~DisjunctiveModelGenerator()
{
	
	delete genSolver;
	delete checkSolver;
}

AtomSet DisjunctiveModelGenerator::getNextModel()
{
	
	
	if(!modelsleft)
		throw GeneralError("DisjunctiveModelGenerator: No more models obtained");
	AtomSet result = currentModel;
	modelsleft = false;
	while(genSolver->answerSetsLeft())
	{
		/* Get candidate model */
		AtomSet candModel = genSolver->getNextAnswerSet();

// 		ProgramBuilder* pb = new DLVProgramBuilder();
// 		std::cout<<"%big constraints"<<std::endl;
// 		pb->clearString();
// 		pb->buildProgram(bigConstraints.getCons());
// 		pb->buildFacts(candModel);
// 		std::cout<<pb->getString()<<std::endl;
	
		try
		{
			/* check against the big constraints */
			checkSolver->callSolver(bigConstraints.getCons(), candModel, true);
		}
		catch(GeneralError& e)
		{
			throw e;
		}
		
		/* If it is consistent, we obtain a model */
		if(checkSolver->answerSetsLeft())
		{
			candModel.intersectWith(PT);
			currentModel = candModel;
			modelsleft = true;
			break;
		}
	}
	
	return result;
}

//End

