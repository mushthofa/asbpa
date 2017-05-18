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
 * @file ModelCheck.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Model checker checks if an interpretation is a model of a program
 *
 *
 */
 
#ifndef MODEL_CHECK_H
#define MODEL_CHECK_H

#include "Error.h"
#include "Globals.h"

#include "Program.h"
#include "ASPSolver.h"



class ModelCheck
{
	public:
		ModelCheck(const Program& prog, const AtomSet& as, ASPSolver* solv);
		ModelCheck(const Program& prog, ASPSolver* solv);
		~ModelCheck();
		
		void setProgram(const Program& prog)
		{
			program = prog.getCons();
		}
		
		void setInterp(const AtomSet& i)
		{
			interp = i;
		}
		
		bool checkModel() const;
		bool checkModel(const AtomSet& i)
		{
			//std::cout<<"checking "<<i<<std::endl;
			interp = i;
			return checkModel();
		}
		
	protected:
		Program program;
		AtomSet interp;
		ASPSolver *solver;
};


#endif
//
