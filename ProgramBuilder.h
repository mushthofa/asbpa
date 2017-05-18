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
 * @file   ProgramBuilder.h
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  Builders for logic program representations.
 * 
 * 
 */

#ifndef _PROGRAMBUILDER_H
#define _PROGRAMBUILDER_H

#include "Program.h"
#include "PrintVisitor.h"
#include "AtomSet.h"
#include <string>
#include <sstream>



class ProgramBuilder
{
	protected:
		std::ostringstream stream;

		
	public:
		ProgramBuilder();
		
		virtual ~ProgramBuilder();
		
		
		virtual void buildRule(const Rule* rule) = 0;
		
		virtual void buildFacts(const AtomSet&) = 0;
		
		virtual void buildProgram(const Program&) = 0;
		
		virtual void buildQuery(const AtomSet&) {}
	
		virtual void buildMinCheck(bool withDebug = false){};
		
		virtual void buildModel(const AtomSet&){};
		
		virtual void buildSubset(const AtomSet&){};
		
		std::string getString() const;

		void clearString();
		

};

class DLVProgramBuilder : public ProgramBuilder
{
	public: 
		DLVProgramBuilder();
		~DLVProgramBuilder();
		virtual void buildRule(const Rule*);
		virtual void buildFacts(const AtomSet&);
		virtual void buildProgram(const Program&);
	protected:
		PrintVisitor* pv;
		
};

class PositiveProgramBuilder : public ProgramBuilder
{
	public: 
		PositiveProgramBuilder();
		~PositiveProgramBuilder();
		virtual void buildRule(const Rule*);
		virtual void buildFacts(const AtomSet&);
		virtual void buildProgram(const Program&);
	protected:
		PrintVisitor* pv;

};

class PrologProgramBuilder: public ProgramBuilder
{
	public: 
		PrologProgramBuilder();
		~PrologProgramBuilder();
		virtual void buildRule(const Rule*);
		virtual void buildFacts(const AtomSet&);
		virtual void buildProgram(const Program&);
	protected:
		PrintVisitor* pv;
};

class PrologGrProgramBuilder : public ProgramBuilder
{
	public: 
		PrologGrProgramBuilder(bool withNeg = 0);
		~PrologGrProgramBuilder();
		virtual void buildRule(const Rule*);
		virtual void buildFacts(const AtomSet&);
		virtual void buildProgram(const Program&);
		virtual void buildQuery(const AtomSet&);
		
	protected:
		PrintVisitor* pv;
		std::set<struct PredicateSign> predicates;

};


class PrologMinCheckProgramBuilder : public ProgramBuilder
{
	public: 
		PrologMinCheckProgramBuilder();
		~PrologMinCheckProgramBuilder();
		virtual void buildRule(const Rule*);
		virtual void buildFacts(const AtomSet&);
		virtual void buildProgram(const Program&);
		
		virtual void buildModel(const AtomSet&);
		virtual void buildSubset(const AtomSet&);
		virtual void buildQuery(const AtomSet&);
		
		virtual void buildMinCheck(bool withDebug = false);
		
	protected:
		PrintVisitor* pv;
		std::set<struct PredicateSign> predicates;

};


#endif
//End

