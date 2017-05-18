/***************************************************************************
 *   Copyright (C) 2009 by Thomas Krenwallner, Mushthofa   				*
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


/* Modified from dlvhex's BaseVisitor */

/**
 * @file   BaseVisitor.h
 * @author Thomas Krennwallner, Mushthofa
 * 
 * @brief  The baseclass for all Visitors.
 * 
 * When calling the accept(BaseVisitor&) method of an object, the
 * object knows its own type and calls the corresponding visiting
 * method of BaseVisitor.
 * 
 */


#ifndef _BASEVISITOR_H
#define _BASEVISITOR_H

//
// forward declarations
//
class AtomSet;
class Rule;
class Literal;
class Atom;
class InfixAtom;
class PredicateSign;

class BaseVisitor
{
	public:
		virtual ~BaseVisitor()
		{ }
		virtual void visitAtomSet(const AtomSet*) = 0;
		
		virtual void visitAtomSet(const AtomSet*, bool) = 0;

		virtual void visitRule(const Rule*) = 0;
		
		virtual void visitLiteral(const Literal*) = 0;

		virtual void visitAtom(const Atom*) = 0;
		
		virtual void visitInfixAtom(const InfixAtom*) = 0;
		
		//virtual void visitPredicateSign(const PredicateSign*) = 0;

};

#endif

// End

