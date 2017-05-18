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
 * @file   EvalComp.cpp
 * @author Mushthofa
 *
 * 
 * @brief  Evaluation of one component
 * 
 * 
 */

#include "EvalComp.h"
/* We use the Prolog way of generating program subsets */
#include "PrologProgramSubsetGenerator.h"
#include "ASVerifier.h"


unsigned c = 0;
EvalEmpty::EvalEmpty()
	:EvalComp(1, false)
{
}

EvalEmpty::~EvalEmpty()
{
}

void EvalEmpty::setInput(const Program& p, const AtomSet& f)
{
	if(f.isConsistent())
	{
		currentAnswerSet = f;
		answersetsleft = true;
	}
	else
		answersetsleft = false;
}

void EvalEmpty::doEval()
{
	answersetsleft = false;
}

void EvalEmpty::clearInput()
{
	answersetsleft = true;
}



EvalDirect::EvalDirect(ASPSolverEngine* se, ProgramBuilder* pb, unsigned long long n, bool limAS )
	:EvalComp(n, limAS)
{
	solver = se->createSolver(pb);
}

EvalDirect::~EvalDirect()
{
	//std::cout<<"deleting solver"<<std::endl;
	//delete solver;
}

void EvalDirect::setInput(const Program& p, const AtomSet& f)
{
	IDB = p;
	EDB = f;
	try
	{
		solver->callSolver(IDB, EDB);
	}
	catch(GeneralError& e)
	{
		throw e;
	}
	doEval();
}

void EvalDirect::doEval()
{
	if(solver->answerSetsLeft() && (numAS < maxNumAS || !limitAS))
	{
		answersetsleft = true;
		currentAnswerSet = solver->getNextAnswerSet();
		numAS++;
	}
	else
		answersetsleft = false;
}

void EvalDirect::clearInput()
{
	answersetsleft = false;
}

EvalMethod1::EvalMethod1(ASPSolverEngine* se, ProgramBuilder* pb, PrologEngine* pe, 
					unsigned long long n, bool limAS)
	:EvalComp(n, limAS), solverEngine(se), currentSubsetDone(true), 
			pt(pe->createThread()), programBuilder(pb)
{
	checkSolver = solverEngine->createSolver(programBuilder);
	evalSolver = solverEngine->createSolver(programBuilder);
}

EvalMethod1::~EvalMethod1()
{
	clearInput();
	delete checkSolver;
	delete evalSolver;
	//delete pt;
}

void EvalMethod1::setInput(const Program& p, const AtomSet& f)
{
	IDB = p.doShift();
	EDB = f;
	
	/* Compute positive program */
	ProgramBuilder* pb_pos = new PositiveProgramBuilder();
	/*
	std::cout<<"Positive program :"<<std::endl;
	pb_pos->buildProgram(IDB);
	std::cout<<pb_pos->getString()<<std::endl;
	pb_pos->clearString();
	*/
	
	ASPSolver* posSolver = solverEngine->createSolver(pb_pos);
	
	try
	{
		posSolver->callSolver(IDB, EDB);
	}
	catch(GeneralError& e)
	{
		throw e;
	}
	
	/* Get the PT  */
	AtomSet PT = posSolver->getNextAnswerSet();
	
	/* Clean up temporary stuffs */
	delete pb_pos;
	delete posSolver;
	
	/* Set up subset generator */
	subsetGenerator = new PrologProgramSubsetGenerator(IDB, EDB, pt);
	
	/* Initialize it to PT-EDB */
	try
	{
		subsetGenerator->initialize(PT.difference(EDB));
	}
	catch(GeneralError& e)
	{
		throw e;
	}
	
	/* Set up the model checker */
	modelChecker = new ModelCheck(IDB, checkSolver);
	
	/* Perform evaluation */
	doEval();
}

void EvalMethod1::clearInput()
{
	
	delete modelChecker;
	delete subsetGenerator;
	answersetsleft = false;
}

void EvalMethod1::doEval()
{
	/* Stop if have enough answer set */
	if(numAS >= maxNumAS && limitAS)
	{
		answersetsleft = false;
		return;
	}
	/* IF current program subset has no more answer sets, we generate the next subset */
	if(currentSubsetDone)
	{
		bool found = false;
		while(subsetGenerator->subsetsLeft())
		{
			currentSubset = subsetGenerator->getNextSubset();
			if(Globals::Instance()->intOption("verboseLevel") > 0)
			{
				std::cout<<"________________________________________________________________"<<std::endl;
				std::cout<<"Current program subset generated : "<<std::endl;
				programBuilder->clearString();
				programBuilder->buildProgram(currentSubset);
				programBuilder->buildFacts(EDB);
				std::string currentSubsetStr = programBuilder->getString();
				std::cout<<currentSubsetStr<<std::endl;
			}
			
			evalSolver->callSolver(currentSubset, EDB);
			
			while(evalSolver->answerSetsLeft())
			{
				AtomSet currentModel = evalSolver->getNextAnswerSet();
				if(Globals::Instance()->intOption("verboseLevel") > 0)
				{
					std::cout<<"Found candidate answer set = "<<currentModel<<std::endl;
				}
				
				if(modelChecker->checkModel(currentModel))
				{
					answersetsleft = true;
					currentAnswerSet = currentModel;
					found = true;
					numAS++;
					if(Globals::Instance()->intOption("verboseLevel") > 0)
					{
						std::cout<<"Found an answer set: "<<currentAnswerSet<<std::endl;
					}
					
					break;
				}
			}// end while solver->answersetsleft()
			if(found)
			{
				if(evalSolver->answerSetsLeft())
					currentSubsetDone = false;
				else
					currentSubsetDone = true;
				break;
			}
		}// end while subgen->subsetsLeft()
		if(found)
			answersetsleft = true;
		else 
			answersetsleft = false;
	}// end if currentSubsetDone
	/* ELSE, we get the next answer sets from the current subset */
	else
	{
		
		
		bool found = false;
		while(evalSolver->answerSetsLeft())
		{
			AtomSet currentModel = evalSolver->getNextAnswerSet();
			
			if(modelChecker->checkModel(currentModel))
			{
				
				found = true;
				answersetsleft = true;
				currentAnswerSet = currentModel;
				numAS++;
				if(Globals::Instance()->intOption("verboseLevel") > 0)
				{
					std::cout<<"Found an answer set: "<<currentAnswerSet<<std::endl;
				}
				
				break;
			}
		}// end while solver->answersetsleft()
		if(!found)
		{
			if(subsetGenerator->subsetsLeft())
			{
				//currentSubset = subgen->getNextSubset();
				currentSubsetDone = true;
				doEval();
			}
			else
				answersetsleft = false;
		}
	}
}



EvalMethod2::EvalMethod2(ASPSolverEngine* se, ProgramBuilder* pb, PrologEngine* pe, 
					unsigned long long n, bool limAS )
	:EvalComp(n, limAS), solverEngine(se), programBuilder(pb), prologThread(pe->createThread())
{
}

EvalMethod2::~EvalMethod2()
{
	delete modelGenerator;
	//delete prologThread;
	delete subsetGenerator;

	answersetsleft = false;
}

void EvalMethod2::setInput(const Program& p, const AtomSet& f)
{
	IDB = p.doShift();
	EDB = f;


	//std::cout<<"Constructing model generator"<<std::endl;
	modelGenerator = new DisjunctiveModelGenerator(IDB, EDB, solverEngine, programBuilder);
	
	//std::cout<<"Constructing subset generator"<<std::endl;
	subsetGenerator = new PrologProgramSubsetGenerator(IDB, EDB, prologThread, true);
	doEval();
} 	

void EvalMethod2::clearInput()
{
	delete modelGenerator;
	//delete prologThread;
	delete subsetGenerator;
	answersetsleft = false;
	//IDB.clear();
	//EDB.clear();
}

void EvalMethod2::doEval()
{
	/* Stop if have enough answer set */
	if(limitAS && (numAS >= maxNumAS))
	{
		answersetsleft = false;
		return;
	}

	/*
	answersetsleft = true;
	currentAnswerSet = modelGenerator->getNextModel();
	answersetsleft = true;
	numAS++;
	return;
	*/

	
	answersetsleft = false;
	while(modelGenerator->modelsLeft())
	{
		AtomSet currentModel = modelGenerator->getNextModel();
		
		if(Globals::Instance()->intOption("verboseLevel") > 0)
		{
			std::cout<<"%current model: "<<currentModel<<std::endl;
			std::cout<<"%________________________________________________________________"<<std::endl;
		}
		
		
		/* For each model, generate a supporting subset of the ground program */
		
		/* We use Prolog to compute supporting subset */
		try	
		{
			
			
			
			subsetGenerator->initialize(currentModel.difference(EDB));
			ASVerifier* av = new ASVerifier(currentModel, solverEngine->createSolver(programBuilder));
			
			//std::cout<<"Getting the subsets "<<std::endl;
			while(subsetGenerator->subsetsLeft())
			{
				
				Program currentSubset = subsetGenerator->getNextSubset();
				
				if(Globals::Instance()->intOption("verboseLevel") > 0)
				{
					programBuilder->clearString();
					programBuilder->buildProgram(currentSubset);
					
					std::cout<<"%current program subset "<<std::endl;
					std::cout<< "" << programBuilder->getString()<<std::endl;
					
				}
				
				
				/* Check whether the gl reduct program has an answer set */
				
				
				if(av->isAnswerSetOf(currentSubset, EDB))
				{
					/* Model is a global answer set, output it */
					/*
					if(Globals::Instance()->intOption("verboseLevel") > 0)
					{
						std::cout<<"Found an answer set: "<<currentModel<<std::endl;
					}
					*/
		
					currentAnswerSet = currentModel;
					answersetsleft = true;
					numAS++;
					break;
				}
			}// end while subsetGenerator->subsetsLeft()
			
			subsetGenerator->reset();

			delete av;
			//delete subsetGenerator;
		}
		catch(GeneralError& e)
		{
			throw e;
		}
		
		 //std::cout<<"Finished with model "<<currentModel<<std::endl;
		if(answersetsleft)
			break;
	}// End While modelGenerator->answerSetsLeft()
}

EvalDisjunctive::EvalDisjunctive(ASPSolverEngine* se, ProgramBuilder* pb, PrologEngine* pe,
						    unsigned long long n, bool limAS )
	:EvalComp(n, limAS), solverEngine(se), programBuilder(pb), prologEngine(pe)
{
}

EvalDisjunctive::~EvalDisjunctive()
{
	clearInput();
}

void EvalDisjunctive::setInput(const Program& p, const AtomSet& f)
{
	IDB = p;
	EDB = f;
	
	/* Get the definite part first */
	
	
	
	modelGenerator = new DisjunctiveModelGenerator(IDB, EDB, solverEngine, programBuilder);
	//modelGenerator = new SubsetModelGenerator(IDB, EDB, solverEngine, programBuilder);
	//minChecker = new PrologMinChecker(IDB, Definite, prologEngine);
	minChecker = new DisjunctiveMinChecker(IDB, EDB, solverEngine, programBuilder);
	doEval();
}

void EvalDisjunctive::doEval()
{
	/* Stop if have enough answer set */
	if(numAS >= maxNumAS && limitAS)
	{
		answersetsleft = false;
		return;
	}
	
	answersetsleft = false;
	while(modelGenerator->modelsLeft())
	{
		
		AtomSet currentModel = modelGenerator->getNextModel();
		
		//std::cout << "current model "<<currentModel <<std::endl;
		try
		{
			if(minChecker->checkMinModel(currentModel))
			{
				answersetsleft = true;
				currentAnswerSet = currentModel;
				numAS++;
			}
			else
				answersetsleft = false;
			if(answersetsleft)
				break;
		}
		catch(GeneralError& e)
		{
			std::cout << "Error checking min " <<std::endl;
			throw e;
		}
	}
}

void EvalDisjunctive::clearInput()
{
	answersetsleft = false;
	delete modelGenerator;
	delete minChecker;
}




//End

