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
 * @file   DLVASPSolver.h
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  ASP solver class implemented using dlv.
 * 
 */


#include "DLVASPSolver.h"
#include "DLVResultParser.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

DLVASPSolver::DLVASPSolver(ProgramBuilder* progbuilder, ASPResultParser* rp, std::set<std::string>* filter)
	:ASPSolver(progbuilder, rp), checkOnly(false), checkSatisfy(false)
{
	/* Get dlv's path and arguments from command line */
	
	lpcommand = Globals::Instance()->strOption("DLVPath");
	boost::tokenizer<> args (Globals::Instance()->strOption("DLVArgs"));
	lpargs.push_back(lpcommand);
	lpargs.push_back("-silent");
	lpargs.push_back("--");
	std::string filter_arg = "";
	if(filter != 0 && filter->size()>0)
	{
		filter_arg += "-filter=";
		std::set<std::string>::iterator f_it;
		unsigned n(0);
		for(f_it = filter->begin(); f_it !=filter->end(); ++f_it)
		{
			filter_arg += *f_it;
			if(n<filter->size() - 1)
				filter_arg += ",";
		}
		lpargs.push_back(filter_arg);
	}
	
	for(boost::tokenizer<>::iterator args_it = args.begin(); args_it!=args.end(); ++args_it)
	{
		lpargs.push_back(*args_it);
	}
	
	pb = new ProcessBuf();
	/*
	answerSets.clear();
	answerSetIndex = answerSets.end();
	*/
}

DLVASPSolver::~DLVASPSolver()
{
	delete pb;
}

void DLVASPSolver::callSolver(const Program& prog, const AtomSet& facts, bool checkonly)
{
	//answerSets.clear();
	checkOnly = checkonly;
	
	int retcode = 0;
	if(pb->isOpen())
		retcode = pb->close();
	
	try
	{
		pb->open(lpargs);

		std::iostream iopipe(pb);
		iopipe.exceptions(std::ios_base::badbit);
		std::string prg;
		programBuilder->clearString();
		programBuilder->buildProgram(prog);
		programBuilder->buildFacts(facts);
		prg = programBuilder->getString();
		try
		{
			/*
			if(checkonly)
			{
				std::cout<<"Calling dlv with program "<<std::endl;
				std::cout<<prg<<std::endl;
			}
			*/
			
			iopipe<<prg<<std::endl;	
		}
		catch(std::ios_base::failure e)
		{
			std::ostringstream oserr;
			oserr << "Error executing command ";
			for(std::vector<std::string>::iterator it = lpargs.begin(); it!=lpargs.end(); ++it)
				oserr<<*it<<" ";
			std::cout<<"I/O stream failure: "<<e.what()<<std::endl;
			oserr<<"Program is "<<std::endl<<prg<<std::endl;
			throw FatalError(oserr.str());
		}
		
		pb->endoffile();
		
		/*std::cout<<"DLV Result: "<<std::endl;
		*/
		std::string outputline;
		
		if(checkonly)
		{
			
			std::getline(iopipe, outputline);
			if(outputline.size() >= 2) // atleast {}
				checkSatisfy = true;
			else
				checkSatisfy = false;
		}
		else
		{
			//std::cout<<"parsing dlv result"<<outputline<<std::endl;
			if(std::getline(iopipe, outputline))
			{
			
				try
				{
					resultParser->parseLine(outputline);
				}
				catch(GeneralError& e)
				{
					std::ostringstream err;
					err << e.getErrorMsg() << std::endl;
					programBuilder->clearString();
					programBuilder->buildProgram(prog);
					programBuilder->buildFacts(facts);
					err << " Program was : " <<std::endl << programBuilder->getString() <<std::endl;
		
					throw FatalError(err.str());
				}
				currentAnswer= resultParser->getAnswerSet();
			
				answersetsleft = true;
			}
			else
				answersetsleft = false;
			
		}
		//retcode = pb.close();
	
	}
	catch(FatalError& e)
	{
		throw e;
	}
	catch(GeneralError& e)
	{
		throw FatalError(e.getErrorMsg());
	}
	catch (std::exception& e)
	{
		throw FatalError(e.what());
	}
	if(retcode==127)
	{
		throw FatalError("DLV is not found in the specified path");
	}

}

//
AtomSet DLVASPSolver::getNextAnswerSet()
{
	if(checkOnly)
		throw FatalError("Cannot get next answer set: Solver was called with 'check only' flag!");
	
	AtomSet resultAS = currentAnswer;
	std::string outputline;
	std::iostream iopipe(pb);
	iopipe.exceptions(std::ios_base::badbit);
	if(std::getline(iopipe, outputline))
	{
		resultParser->parseLine(outputline);
		currentAnswer = resultParser->getAnswerSet();
		
		answersetsleft = true;
	}
	else
		answersetsleft = false;
	return resultAS;
	
}



bool DLVASPSolver::answerSetsLeft() const
{
	if(checkOnly)
	{
		return checkSatisfy;
	}
	return answersetsleft;
}


DLVASPSolverEngine::DLVASPSolverEngine()
{
}

DLVASPSolver* DLVASPSolverEngine::createSolver(ProgramBuilder* pb, std::set<std::string>* filter)
{
	ASPResultParser* rp = new DLVResultParser();
	
	DLVASPSolver* new_solver = new DLVASPSolver(pb, rp, filter);
	return new_solver;
}
//End

