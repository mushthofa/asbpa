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

/* Modified from dlvhex's ASPSolver.h */
/* ASPSolver is now an abstract class */


/**
 * @file   ASPSolver.h
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  ASP solver base class.
 * 
 */

#ifndef _ASPSOLVER_H
#define _ASPSOLVER_H

#include <iostream>
#include <string>
#include <vector>

#include "ProcessBuff.h"
#include "Error.h"
#include "Globals.h"
#include "AtomSet.h"
#include "Program.h"
#include "ProgramBuilder.h"
#include "ASPResultParser.h"




/**
 * @brief ASP solver class.
 */
class ASPSolver
{
	public:

		
		ASPSolver(ProgramBuilder* pb, ASPResultParser* rp)
			:programBuilder(pb), resultParser(rp)
		{}
		virtual ~ASPSolver()
		{
			delete resultParser;
		}
		/**
		* @brief Calls the answer set solver with a program.
		* 
		* 
		*
		*/
		virtual void callSolver(const Program& prog, const AtomSet& edb, bool checkonly = 0) = 0;
		
    		/**
		 * @brief Retrieves an Answer set, incrementing the internal result pointer.
		 *
		 *
		 */
		virtual AtomSet getNextAnswerSet() = 0;

    		/**
		 * @brief Returns the number of answer sets of the last result.
		 */
		//virtual unsigned numAnswerSets() = 0;
		
		virtual bool answerSetsLeft() const = 0;
		

	protected:
    
    		/**
	 * @brief System command to call the external asp reasoner.
		 */
		std::string lpcommand;
		
		/**
		 * @brief List of arguments supplied 
		 */
		
		std::vector<std::string> lpargs;

		/**
		 * @brief Internal result
		 */
		
		AtomSet currentAnswer;
    
    		/* Is there any answers left */		
		bool answersetsleft;
		
		/* Program serializer */
		
		ProgramBuilder* programBuilder;
		
		/* Result Parser */
		
		ASPResultParser* resultParser;
		
		
};

/* The class to store ASP Solver factory */

class ASPSolverEngine
{
	public:
		ASPSolverEngine()
		{}
		/* Create one instance of the solver */
		
		virtual ASPSolver* createSolver(ProgramBuilder*, std::set<std::string>* = 0) = 0;
		
	protected:
};


#endif // _ASPSOLVER_H

//End
