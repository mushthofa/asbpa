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
 * @file ModelGenerator.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Model generator generates some models of a program, by enumerating subsets of possibly true atoms and 
 * @brief	checking for satisfiability
 *
 */
 
 
#ifndef MODEL_GENERATOR_H
#define MODEL_GENERATOR_H


#include "ASPSolver.h"
#include "ProgramBuilder.h"
#include "ModelCheck.h"
#include "Bitmap.h"

class ModelGenerator
{
	public:
		ModelGenerator()
		{}
		ModelGenerator(const Program& p, const AtomSet& f)
		:IDB(p), EDB(f)
		{}
		
		//~ModelGenerator();

		virtual AtomSet getNextModel() = 0;
		bool modelsLeft() const
		{
			return modelsleft;
		}
	protected:
		Program IDB;
		AtomSet EDB;
		AtomSet Definite;
 		AtomSet currentModel;
		bool modelsleft;
};

class SubsetModelGenerator: public ModelGenerator
/* Generating models by visiting subsets of the potentially true literals/atoms */
		/* It is innefficient hence not used */
{
	public:
		SubsetModelGenerator(const Program&, const AtomSet&, ASPSolverEngine*, ProgramBuilder*);
		~SubsetModelGenerator();
		virtual AtomSet getNextModel();
		
	protected:
		BitMap* bitmap;
		ASPSolver* checkSolver;
		ModelCheck* mc;
		std::vector<AtomPtr> vectorPT;
		
};


class DisjunctiveModelGenerator: public ModelGenerator
/* Generating models by submitting classical guesses p v -p for each possibly true literals, plus some small constraints.*/
/* each candidate can then be check for satisfaction against the big constraints */
{
	public:
		DisjunctiveModelGenerator(const Program&, const AtomSet&, ASPSolverEngine*, ProgramBuilder*);
		~DisjunctiveModelGenerator();
		virtual AtomSet getNextModel();
	protected:
		ASPSolverEngine* solverEngine;
		ASPSolver* genSolver;
		ASPSolver* checkSolver;
		Program generatingProg;
		Program bigConstraints;
		/* The original possibly-true atoms */
		AtomSet PT;
};

#endif

//

