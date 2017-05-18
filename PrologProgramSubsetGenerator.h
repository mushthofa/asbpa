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
 * @file PrologProgramSubsetGenerator.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Generating program subsets implemented using Prolog
 *
 *
 */

#ifndef _PROLOGPROGRAMSUBSETGEN_H
#define _PROLOGPROGRAMSUBSETGEN_H

#include "ProgramSubsetGenerator.h"
#include "PrologEngine.h"
#include "ProgramBuilder.h"


class PrologProgramSubsetGenerator : public ProgramSubsetGenerator
{
	public:
		PrologProgramSubsetGenerator(const Program&, const AtomSet&,
							    PrologThread*, bool withNeg = 0);
		virtual ~PrologProgramSubsetGenerator();
		
		virtual void reset();
		virtual Program getNextSubset();
		virtual void initialize(const AtomSet&);
	private:
		PrologThread* prologThread;
		ProgramBuilder* programBuilder;
		Program smallConstraints;
};

#endif
//End

