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
 * @file   DLVASPSolver.h
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  ASP solver class implemented using dlv.
 * 
 */

#ifndef _DLVASPSOLVER_H
#define _DLVASPSOLVER_H

#include "ASPSolver.h"
#include "ProcessBuff.h"
#include <sstream>
#include <set>

class DLVASPSolver : public ASPSolver
{
	public:
		DLVASPSolver(ProgramBuilder*, ASPResultParser*, std::set<std::string>* filter = 0);
		~DLVASPSolver();
		void callSolver(const Program&, const AtomSet& IDB, bool checkOnly = 0);
		AtomSet getNextAnswerSet();
		//unsigned numAnswerSets();
		bool answerSetsLeft() const;
		
	private:
		ProcessBuf* pb;
		bool checkOnly;
		bool checkSatisfy;

};

class DLVASPSolverEngine : public ASPSolverEngine
{

	public: 
		DLVASPSolverEngine();
		virtual DLVASPSolver* createSolver(ProgramBuilder*, std::set<std::string>* = 0);
	
};

#endif
//End

