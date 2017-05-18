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
 * @file ProgramSubsetGenerator.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Class to generate program subsets 
 *
 *
 */

#ifndef _PROGRAMSUBSETGENERATOR_H
#define _PROGRAMSUBSETGENERATOR_H

#include "Program.h"
#include "AtomSet.h"


class ProgramSubsetGenerator
{
	public:
		ProgramSubsetGenerator(const Program& p, const AtomSet& f, bool withNeg = 0)
			:IDB(p), EDB(f), withNegative(withNeg), subsetsleft(false)
		{}
		virtual ~ProgramSubsetGenerator(){}
		
		virtual void initialize(const AtomSet&) = 0;
		virtual void reset()
		{
			subsetsleft = false;
		}
		virtual Program getNextSubset() = 0;
		bool subsetsLeft() const
		{
			return subsetsleft;
		}
	protected:
		Program IDB;
		AtomSet EDB;
		AtomSet PT;
		Program currentSubset;
		bool subsetsleft;
		bool withNegative;
		
};

#endif
//End

