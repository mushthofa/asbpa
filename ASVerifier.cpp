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
 * @file   ASVerifier.cpp
 * @author Mushthofa
 *
 * 
 * @brief  Verifying an answer set of a program 
 * 
 * 
 */

#include "ASVerifier.h"

ASVerifier::ASVerifier(const AtomSet& m, ASPSolver* se)
	:model(m.getPrimed()), solver(se)
{
	/* Building constraints :- a', not a for each a\in model */
	AtomSet::const_iterator a_it;
	for(a_it = m.begin(); a_it!=m.end(); ++a_it)
	{
		AtomPtr Aptr (new Atom(*a_it));
		LiteralPtr A (new Literal(Aptr, true));
		Atom primed_atom = a_it->getPrimed();
		AtomPtr primed_atomptr (new Atom(primed_atom));
		LiteralPtr primedA (new Literal(primed_atomptr));
		RuleBody_t cons_body;
		cons_body.insert(A);
		cons_body.insert(primedA);
		RulePtr cons (new HardConstraint(cons_body));
		constraints.addRule(cons);
	}
}

ASVerifier::~ASVerifier()
{
	//delete solver;
}

bool ASVerifier::isAnswerSetOf(const Program& IDB, const AtomSet& EDB)
{
	try
	{
		solver->callSolver(IDB.getPrimed() + constraints, model + EDB);
	}
	catch(GeneralError& e)
	{
		std::cout << "Error verifying model "<<EDB<<std::endl;
		delete solver;
		throw e;
	}
	
	return solver->answerSetsLeft();
	
}



//End

