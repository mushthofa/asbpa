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

#include "MinChecker.h"

/*
PrologMinChecker::PrologMinChecker(const Program& p, const AtomSet& f, const AtomSet& i, PrologEngine* pe)
	:MinChecker(p, f, i)
{
	
	pt = pe->createThread();
	ProgramBuilder* mincheckProg = new PrologMinCheckProgramBuilder();
	mincheckProg->buildProgram(IDB);
	mincheckProg->buildFacts(EDB);
	mincheckProg->buildMinCheck();
	
	pt = pe->createThread();
	
	std::set<struct PredicateSign> preds = IDB.getPredicates();
	pt->declareDynamic(preds);
	preds = EDB.getPredicates();
	pt->declareDynamic(preds);
	
	pt->consult(mincheckProg->getString());
	
	delete mincheckProg;
}

*/


PrologMinChecker::PrologMinChecker(const Program& p, const AtomSet& f, PrologEngine* pe)
	:MinChecker(p, f)
{
	pt = pe->createThread();
	ProgramBuilder* mincheckProg = new PrologMinCheckProgramBuilder();
	mincheckProg->buildProgram(IDB);
	mincheckProg->buildFacts(EDB);
	mincheckProg->buildMinCheck();
	
	/*
	std::cout<<"Prolog mincheck: "<<std::endl<<mincheckProg->getString()<<std::endl;
	std::cout<<"++++++"<<std::endl;
	pt = pe->createThread();
	*/
	
	std::set<struct PredicateSign> preds = IDB.getPredicates();
	pt->declareDynamic(preds);
	preds = EDB.getPredicates();
	pt->declareDynamic(preds);

	pt->consult(mincheckProg->getString());
	
	
	delete mincheckProg;
}

PrologMinChecker::~PrologMinChecker()
{
	delete pt;
}

bool PrologMinChecker::checkMinModel()
{
	AtomSet PT = model.difference(EDB);
	//std::cout<<"Checking model "<<model<<std::endl;
	ProgramBuilder* mincheckProg = new PrologMinCheckProgramBuilder();
	mincheckProg->buildModel(model);
	mincheckProg->buildSubset(PT);
	
	//std::cout<<mincheckProg->getString()<<std::endl;
	
	std::string currFacts = mincheckProg->getString();
	pt->addFacts(currFacts);
	mincheckProg->clearString();
	mincheckProg->buildQuery(model);
	
	//std::cout<<"Query = "<<mincheckProg->getString()<<std::endl;
	bool minimal = pt->checkQuery(mincheckProg->getString());
	
	/* Dont forget to delete the asserted facts */
	pt->delFacts(currFacts);
	
	delete mincheckProg;
	return minimal;	
}


DisjunctiveMinChecker::DisjunctiveMinChecker(const Program& P, const AtomSet& f, ASPSolverEngine* se, ProgramBuilder* pb)
	:genSolver(se->createSolver(pb)), checkSolver(se->createSolver(pb))
{
	IDB = P.getPrimed().getCons();
	ASPSolver* defSolver = se->createSolver(pb);
	try
	{
		defSolver->callSolver(P.getDefinite(), f);
	}
	catch(...)
	{
		delete defSolver;
		throw;
	}
	
	EDB = defSolver->getNextAnswerSet();
	delete defSolver;
}

DisjunctiveMinChecker::~DisjunctiveMinChecker()
{
	delete genSolver;
	delete checkSolver;
}


bool DisjunctiveMinChecker::checkMinModel()
{
	
	/* Get the range for guessing subsets */
	AtomSet PT = model.difference(EDB); 
	
	
	/* Create the guess p v -p for every p\in PT = model - EDB */
	AtomSet::const_iterator m_it;
	Program classicalGuess;
	for(m_it = PT.begin(); m_it != PT.end(); ++m_it)
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
	
	
	/* Create the constraint :- a1, .., an where {a1,...,an} = PT */
	/* To eliminate the subset which is not a proper subset of the model */
	

	RuleBody_t consBody;
	for(m_it = PT.begin(); m_it != PT.end(); ++m_it)
	{
		AtomPtr APtr(new Atom(*m_it));
		LiteralPtr LPtr(new Literal(APtr));
		consBody.insert(LPtr);
	}
	RulePtr consRule (new HardConstraint(consBody,"", 0));
	
	classicalGuess.addRule(consRule);
	
	/* Now add the small rules */
	
	genProgram = classicalGuess + IDB.getSmall(Globals::Instance()->intOption("smallRuleMaxVar"));
	//.getPrimed().getCons();
	//genProgram = classicalGuess + IDB.getPrimed().getCons();
	
	
// 	ProgramBuilder* pb = new DLVProgramBuilder();
// 	pb->buildProgram(genProgram);
// 	pb->buildFacts(EDB + PT.getPrimed());
// 	std::cout<<"gen program "<<std::endl<<pb->getString()<<std::endl;
// 	
	
	try
	{
		genSolver->callSolver(genProgram, EDB + PT.getPrimed());
	}
	catch(...)
	{
		throw;
	}
	
	/*
	if(genSolver->answerSetsLeft())
	{
		//std::cout<<"Smaller model = " << genSolver->getNextAnswerSet() <<std::endl;
		return false;
	}
	else
		return true;
	*/
	
	/* Now for every candidate model generated... we just need to check against the big rules */
	
	
	checkProgram = IDB.getBig(Globals::Instance()->intOption("smallRuleMaxVar"));
	//.getPrimed().getCons();
	while(genSolver->answerSetsLeft())
	{
		AtomSet candModel = genSolver->getNextAnswerSet();
		
// 		pb->clearString();
// 		pb->buildProgram(checkProgram);
// 		pb->buildFacts(candModel);
// 		std::cout << " got smaller " << candModel << std::endl;
// 		std::cout <<"check program "<<std::endl<<pb->getString()<<std::endl;
// 		
		try
		{
			checkSolver->callSolver(checkProgram, candModel, true);
		}
		catch(...)
		{
			throw;
		}
	
		/* If the candidate model satisfies the constraints, we have found a smaller model */
		/* return false */
		
		if(checkSolver->answerSetsLeft())
		{
			return false;
		}
	}
	
	return true;
}

// End

