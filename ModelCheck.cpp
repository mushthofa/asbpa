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
 * @file ModelCheck.cpp
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Model checker checks if an interpretation is a model of a program
 *
 *
 */
 
#include "ModelCheck.h"
 
 
 
ModelCheck::ModelCheck(const Program& p, const AtomSet& i, ASPSolver* s)
	:program(p.getCons()), interp(i), solver(s)
{
}

ModelCheck::ModelCheck(const Program& p, ASPSolver* s)
	:program(p.getCons()), solver(s)
{
}

ModelCheck::~ModelCheck()
{
	
}

bool ModelCheck::checkModel() const
{
	//std::cout<<"Checking model"<<interp<<std::endl;
	try
	{
		solver->callSolver(program, interp, true);
	}
	catch( GeneralError& e)	
	{
		std::string errMsg = e.getErrorMsg() + "\n";
		errMsg += "* Error calling ASP Solver while checking models. \n";
		throw FatalError(errMsg);
	}
	
	if(solver->answerSetsLeft())
		return true;
	else
		return false;
}

//

