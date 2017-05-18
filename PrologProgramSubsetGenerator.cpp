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
 * @file PrologProgramSubsetGenerator.cpp
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Generating program subset implemented using Prolog
 *
 *
 */

#include "PrologProgramSubsetGenerator.h"
#include "PrologResultParser.h"
#include "Globals.h"

PrologProgramSubsetGenerator::PrologProgramSubsetGenerator
		(const Program& IDB, const AtomSet& EDB, PrologThread* pro_th, bool withNeg )
		:ProgramSubsetGenerator(IDB, EDB, withNeg), 
		prologThread(pro_th), programBuilder(new PrologGrProgramBuilder(withNeg))
{
	smallConstraints = IDB.getSmallConstraints(Globals::Instance()->intOption("smallConstraintMaxVar"));
	programBuilder->clearString();
	programBuilder->buildProgram(IDB);
	programBuilder->buildFacts(EDB);

	
	prologThread->consult(programBuilder->getString());
	std::set<struct PredicateSign> preds = IDB.getPredicates();
	prologThread->declareDynamic(preds);
}

PrologProgramSubsetGenerator::~PrologProgramSubsetGenerator()
{
	
	
	if(withNegative)
	{
		programBuilder->clearString();
		programBuilder->buildProgram(IDB);
		programBuilder->buildFacts(EDB);
		programBuilder->buildFacts(PT);
		prologThread->delFacts(programBuilder->getString());

	}

	delete programBuilder;
	
	

}

void PrologProgramSubsetGenerator::reset() 
{
	prologThread->closeQuery();
	std::string facts;
	programBuilder->clearString();
	programBuilder->buildFacts(PT);
	facts = programBuilder->getString();
	
	prologThread->delFacts(facts);
	subsetsleft = false;
}

void PrologProgramSubsetGenerator::initialize(const AtomSet& input)
{
	
	PT = input;
	
	/* If PT is empty, we just get EDB  since IDB does not contribute anything */
	
	if(PT.size() == 0)
	{
		Program empty;
		currentSubset = empty;
		subsetsleft = true;
	}
	
	else
	{
		
		std::string grprogram, grquery;
		
		programBuilder->clearString();
		programBuilder->buildFacts(PT);
		grprogram = programBuilder->getString();
		programBuilder->clearString();
		programBuilder->buildQuery(PT);
		grquery = programBuilder->getString();
		
	//	std::cout<<"Query :"<<grquery<<std::endl;
		
		try
		{
			
// 			std::cout<<"Gr program : "<<std::endl;
// 			std::cout<<grprogram<<std::endl;
// 			
// 			std::cout<<"Gr query : "<<std::endl;
// 			std::cout<<grquery<<std::endl;
			
			
			
			
			prologThread->consult(grprogram);
			
			prologThread->openQuery(grquery);
			
			/* If we have an answer, then we have program subset from this initialization */
			
			if(prologThread->answersLeft())
			{
				std::string ans;
				prologThread->getNextAnswer(ans);
				PrologResultParser resultParser (ans, IDB);
				
				/*
				std::cout<<"Got a program subset from prolog : "<<std::endl;
				std::cout<<"with answer: "<<ans<<std::endl;
				*/
				
				currentSubset = resultParser.getGroundProg();
				/* Adding small constraints to get early pruning */
				currentSubset = currentSubset + smallConstraints;
				subsetsleft = true;
			}
			
			/* No answer is returned. Don't forget to delete 
			the input from the  Prolog database */
			else 
			{
				subsetsleft = false;
				
				//std::cout<<"Deleting facts "<<std::endl;
				programBuilder->clearString();
				programBuilder->buildFacts(PT);
				std::string input_facts = programBuilder->getString();
				prologThread->delFacts(input_facts);
			}
		}
		catch(GeneralError& e)
		{
			throw e;
		}
	} //Else if PT empty
		
}

Program PrologProgramSubsetGenerator::getNextSubset()
{
	if(!subsetsleft)
		throw GeneralError("No more subsets is available to be returned!");
	Program nextSubset = currentSubset;
	
	/* If PT is empty, we have only empty as program subset, so we stop here */
	if(PT.size() == 0)
	{
		subsetsleft = false;
	}
	else
	{
		try
		{
			if(prologThread->answersLeft())
			{
				std::string ans;
				prologThread->getNextAnswer(ans);
				
				/*
				std::cout<<"Got a program subset from prolog : "<<std::endl;
				std::cout<<"with answer: "<<ans<<std::endl;
				*/
				
				PrologResultParser resultParser (ans, IDB);
				currentSubset = resultParser.getGroundProg();
				currentSubset = currentSubset + smallConstraints;
				subsetsleft = true;
			}
			else
			{
				//std::cout<<"Deleting facts"<<std::endl;
				subsetsleft = false;
				
				programBuilder->clearString();
				programBuilder->buildFacts(PT);
				std::string input_facts = programBuilder->getString();
				prologThread->delFacts(input_facts);
			}
		}
		catch(GeneralError& e)
		{
			throw e;
		}
	}// End else if PT empty
	return nextSubset;
}

//End
