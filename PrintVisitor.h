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
 * @file   PrintVisitor.h
 * @author Thomas Krennwallner, Mushthofa
 * 
 * @brief Provides various pretty-printers using the visitor pattern
 *
 * Implements the common printing methods. For instance, if a Rule
 * object calls visitRule(this), visitRule will iterate through the
 * components of the Rule and outputs it to #stream.
 * 
 */


#ifndef _PRINTVISITOR_H
#define _PRINTVISITOR_H

#include "BaseVisitor.h"

#include <iosfwd>
#include <set>

class PrintVisitor : public BaseVisitor
{
	protected: 
		std::ostream& stream;
		std::string list_predicate;

	public: 
		PrintVisitor(std::ostream&);
		std::ostream& getStream();
		virtual void visitRule(const Rule*);
		virtual void visitAtomSet(const AtomSet*);
		virtual void visitAtomSet(const AtomSet* as, bool flag)
		{
			visitAtomSet(as);
		}
		virtual void visitLiteral(const Literal*);
		virtual void visitAtom(const Atom*);
		virtual void visitInfixAtom(const InfixAtom*);
		/*
		virtual void visitPredicateSign(const struct PredicateSign*);
		virtual void visitPredicates(const std::set<struct PredicateSign>);
		*/
		void set_list_predicate(const std::string& str)
		{
			list_predicate = str;
		}
};

class DLVPrintVisitor : public PrintVisitor
{
	public:
		DLVPrintVisitor(std::ostream&);
		virtual void visitRule(const Rule*);
		virtual void visitAtomSet(const AtomSet*);
		virtual void visitLiteral(const Literal*);
		virtual void visitAtom(const Atom*);
		virtual void visitInfixAtom(const InfixAtom*);

};


class PositivePrintVisitor : public PrintVisitor
{
	public:
		PositivePrintVisitor(std::ostream&);
		virtual void visitRule(const Rule*);
		virtual void visitAtomSet(const AtomSet*);
		virtual void visitLiteral(const Literal*);
		virtual void visitAtom(const Atom*);
		virtual void visitInfixAtom(const InfixAtom*);
};

class PrologPrintVisitor : public PrintVisitor
{
	public:
		PrologPrintVisitor(std::ostream&);
		virtual void visitRule(const Rule*);
		virtual void visitAtomSet(const AtomSet*);
		virtual void visitLiteral(const Literal*);
		virtual void visitAtom(const Atom*);
		virtual void visitInfixAtom(const InfixAtom*);
};


class PrologGrPrintVisitor: public PrintVisitor
{
	public:
		PrologGrPrintVisitor(std::ostream&, bool withNeg = 0);
		virtual void visitRule(const Rule*);
		virtual void visitAtomSet(const AtomSet*);
		virtual void visitLiteral(const Literal*);
		virtual void visitAtom(const Atom*);
		virtual void visitInfixAtom(const InfixAtom*);
	protected:
		bool includeNegative;
};

class PrologMinCheckPrintVisitor: public PrintVisitor
{
	public:
		PrologMinCheckPrintVisitor(std::ostream&);
		virtual void visitRule(const Rule*);
		virtual void visitAtomSet(const AtomSet*);
		virtual void visitLiteral(const Literal*);
		virtual void visitAtom(const Atom*);
		virtual void visitInfixAtom(const InfixAtom*);
		
};

#endif
//End

