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
 * @file MinChecker.h
 * @author Mushthofa
 * @date Tue 01 07 2009
 *
 * @brief Minimality checking component
 *
 *
 */
 
#ifndef MINCHECKER_H
#define MINCHECKER_H

#include "Program.h"
#include "AtomSet.h"
#include "PrologEngine.h"
#include "ProgramBuilder.h"
#include "ASPSolver.h"

/* Base class for abstraction */

class MinChecker
{
	public:
		MinChecker(const Program& p, const AtomSet& f, const AtomSet& i)
		:IDB(p.getPrimed().getCons()), EDB(f), model(i)
		{
		}
		
		MinChecker(const Program& p, const AtomSet& f)
		:IDB(p.getPrimed().getCons()), EDB(f)
		{
		}
		
		bool checkMinModel(const AtomSet& i)
		{
			model = i;
			return checkMinModel();
		}
		virtual bool checkMinModel() = 0;
		
	protected:
		MinChecker()
		{
		}
		
		
		Program IDB;
		AtomSet EDB;
		AtomSet model;
		bool isMinModel;
};

class PrologMinChecker : public MinChecker
{
	public:
		//PrologMinChecker(const Program& p, const AtomSet& f, const AtomSet& i, PrologEngine* pl);
		PrologMinChecker(const Program& p, const AtomSet& f, PrologEngine* pl);
		
		~PrologMinChecker();
		
		virtual bool checkMinModel();
	protected:
		PrologThread* pt;
};


class DisjunctiveMinChecker: public MinChecker
{

	public:
		 DisjunctiveMinChecker(const Program&p, const AtomSet&f, ASPSolverEngine* se, ProgramBuilder* pb);
		 ~DisjunctiveMinChecker();
		 
		 virtual bool checkMinModel();
		 
	protected:
		
		ASPSolver* genSolver;
		ASPSolver* checkSolver;
		 
		Program genProgram;
		Program checkProgram;
};

#endif
// End
