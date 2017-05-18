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
 * @file   ProgramBuilder.cpp
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  Builders for logic program representations.
 * 
 * 
 */

#include "ProgramBuilder.h"

ProgramBuilder::ProgramBuilder()
{
	
}
ProgramBuilder::~ProgramBuilder()
{
}

std::string ProgramBuilder::getString() const
{
	return stream.str();
}

void ProgramBuilder::clearString()
{
	stream.str("");
	stream.clear();
}

DLVProgramBuilder::DLVProgramBuilder()
	:ProgramBuilder(), pv(new DLVPrintVisitor(stream))
{ }

DLVProgramBuilder::~DLVProgramBuilder()
{
	delete pv;
}

void DLVProgramBuilder::buildFacts(const AtomSet& as)
{
	as.accept(*pv);
}

void DLVProgramBuilder::buildRule(const Rule* r)
{
	r->accept(*pv);
}

void DLVProgramBuilder::buildProgram(const Program& program)
{
	for (Program::const_iterator r = program.begin();
		    r != program.end();
		    ++r)
	{
		(*r)->accept(*pv);
	}
}


PositiveProgramBuilder::PositiveProgramBuilder()
	:ProgramBuilder(), pv(new PositivePrintVisitor(stream))
{ }

PositiveProgramBuilder::~PositiveProgramBuilder()
{
	delete pv;
}

void PositiveProgramBuilder::buildFacts(const AtomSet& as)
{
	as.accept(*pv);
}

void PositiveProgramBuilder::buildRule(const Rule* r)
{
	r->accept(*pv);
}

void PositiveProgramBuilder::buildProgram(const Program& program)
{
	for (Program::const_iterator r = program.begin();
		    r != program.end();
		    ++r)
	{
		(*r)->accept(*pv);
	}
}


PrologProgramBuilder::PrologProgramBuilder()
	:ProgramBuilder(), pv(new PrologPrintVisitor(stream))
{ }

PrologProgramBuilder::~PrologProgramBuilder()
{
	delete pv;
}

void PrologProgramBuilder::buildFacts(const AtomSet& as)
{
	as.accept(*pv);
}

void PrologProgramBuilder::buildRule(const Rule* r)
{
	r->accept(*pv);
}

void PrologProgramBuilder::buildProgram(const Program& program)
{
	for (Program::const_iterator r = program.begin();
		    r != program.end();
		    ++r)
	{
		(*r)->accept(*pv);
	}
}


PrologGrProgramBuilder::PrologGrProgramBuilder(bool withNeg)
	:ProgramBuilder(), pv(new PrologGrPrintVisitor(stream, withNeg))
{
	
}

PrologGrProgramBuilder::~PrologGrProgramBuilder()
{
	delete pv;
}

void PrologGrProgramBuilder::buildFacts(const AtomSet& as)
{
	as.accept(*pv);
}

void PrologGrProgramBuilder::buildRule(const Rule* r)
{
	r->accept(*pv);
}

void PrologGrProgramBuilder::buildProgram(const Program& program)
{
	/*
	predicates = program.getPredicates();
	pv->visitPredicates(predicates);
	*/
	
	for (Program::const_iterator r = program.begin();
		    r != program.end();
		    ++r)
	{
		(*r)->accept(*pv);
	}
}

void PrologGrProgramBuilder::buildQuery(const AtomSet& as)
{
	stream.str("");
	stream.clear();
	
	if (!as.empty())
	{
		unsigned n(0);
		AtomSet::const_iterator a_it ;
		for (a_it = as.begin(); a_it != as.end(); ++a_it)
		{
			stream<<"m_GROUND"<<"(";
			
			pv->visitAtom(&(*a_it));
			
			stream <<", ";
			stream <<"R"<<n<<", ";
			stream <<"V"<<n<<")";
			if(n<as.size()-1)
				stream<<", ";
			else
				stream<<".";
			n++;
		}
	}

}


PrologMinCheckProgramBuilder::PrologMinCheckProgramBuilder()
	:ProgramBuilder(), pv(new PrologMinCheckPrintVisitor(stream))
{
	
}

PrologMinCheckProgramBuilder::~PrologMinCheckProgramBuilder()
{
	delete pv;
}

void PrologMinCheckProgramBuilder::buildFacts(const AtomSet& as)
{
	/*
	predicates = as.getPredicateSigns();
	pv->visitPredicates(predicates);
	*/
	
	pv->set_list_predicate("m_DEFINITE");
	as.accept(*pv);
}

void PrologMinCheckProgramBuilder::buildSubset(const AtomSet& as)
{
	pv->set_list_predicate("m_SUBSET");
	as.accept(*pv);
}
void PrologMinCheckProgramBuilder::buildModel(const AtomSet& as)
{
	pv->set_list_predicate("m_MODEL");
	as.accept(*pv);
	
	/* Get primed atomset  */
	AtomSet as_primed = as.getPrimed();
	
	/* Print it as facts */
	AtomSet::const_iterator a ;
	if (!as_primed.empty())
	{
		for (a = as_primed.begin(); a != as_primed.end(); ++a)
		{
			pv->visitAtom(& (*a));
			stream<<"."<<std::endl;
		}
	}
	
	/* When using Disjunctive/ASP model generator, we need to include also
		the rules  m_NEG_p(args,...) :- not p(args,...). foreach p(args,..) in model
		This is
	*/
	
	/*
	for(a = as.begin(); a!= as.end(); ++a)
	{
		
		RuleBody_t currBody;
		
		AtomPtr Pptr (new Atom(*a));
		Pptr->negate();
		Pptr->accept(*pv);
		stream <<" :- ";
		Pptr->negate();
		LiteralPtr LPtr (new Literal(Pptr, true));
		LPtr->accept(*pv);
		stream<<"."<<std::endl;
	}
	*/
}

void PrologMinCheckProgramBuilder::buildQuery(const AtomSet& as)
{
	stream.str("");
	stream.clear();
	//stream <<"trace, ";
	pv->set_list_predicate("m_MIN");
	as.accept(*pv);
	//stream <<"m_MIN([a, b]).";
}

void PrologMinCheckProgramBuilder::buildRule(const Rule* r)
{
	r->accept(*pv);
}

void PrologMinCheckProgramBuilder::buildProgram(const Program& program)
{
	/*
	predicates = program.getPredicates();
	pv->visitPredicates(predicates);
	*/
	
	
	for (Program::const_iterator r = program.begin();
		    r != program.end();
		    ++r)
	{
		(*r)->accept(*pv);
	}
}

void PrologMinCheckProgramBuilder::buildMinCheck(bool withDebug)
{
	/* Generate proper subsets */
	stream << "m_MEMBER([X|_], X)." <<std::endl;
	stream << "m_MEMBER([_|R], X) :- m_MEMBER(R,X)."<<std::endl;
	stream << "m_GENSUB([],[])."<<std::endl;
	stream << "m_GENSUB([H|T], [H|M]) :- m_GENSUB(T, M)."<<std::endl;
	stream << "m_GENSUB([_|T], S) :- m_GENSUB(T, S)."<<std::endl;
	
	stream << "m_GENPROPER([H|T], [H|M]) :- m_GENPROPER(T,M)." <<std::endl;
	stream << "m_GENPROPER([_|T], S) :- m_GENSUB(T,S)."<<std::endl;
	
	/* candidate smaller models must contain the definite part */
	
	stream << "m_APPEND([], X, X)." <<std::endl;
	stream << "m_APPEND([H|T], Y, [H|Z]) :- m_APPEND(T, Y, Z)." <<std::endl;
	
	stream <<"m_CANDSUBSET(M, A) :- m_MODEL(M), m_SUBSET(PT), "
 			<<"m_GENPROPER(PT, S), m_DEFINITE(D), m_APPEND(S, D, A)." 
			<<std::endl;
	
	/* Asserting and retracting list of facts */
	stream << "m_ASSERTALL([H | T]) :- assert(H), m_ASSERTALL(T)."<<std::endl;
	stream << "m_ASSERTALL([])."<<std::endl;
	//stream << "m_RETRACTALL([H | T]) :- call(H), retract(H), m_RETRACTALL([H|T]), !."<<std::endl;
	stream << "m_RETRACTALL([H | T]) :- retract(H), m_RETRACTALL(T), !."<<std::endl;
	stream << "m_RETRACTALL([_ | T]) :- m_RETRACTALL(T)."<<std::endl;
	stream << "m_RETRACTALL([])."<<std::endl;
	
	/* Satisfiability check */
	stream << "m_SATISFY(S) :- m_ASSERTALL(S), not(m_VIOLATES), m_RETRACTALL(S)."<<std::endl;
	
	/* Minimality check itself */
	std::string debugStr;
	if(withDebug)
		debugStr = ", write(S).";
	else
		debugStr = ".";
	stream << "m_NOTMIN(I) :- m_CANDSUBSET(I, S), m_RETRACTALL(I), m_SATISFY(S)"<<debugStr<<std::endl;
	stream << "m_MIN(I) :- m_MODEL(I), not m_NOTMIN(I)."<<std::endl;
}


//End

