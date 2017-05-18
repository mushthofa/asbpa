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
 * @file   PrintVisitor.cpp
 * @author Thomas Krennwallner, Mushthofa
 * 
 * @brief Provides various pretty-printers using the visitor pattern
 *
 * Implements the common printing methods. For instance, if a Rule
 * object calls visitRule(this), visitRule will iterate through the
 * components of the Rule and outputs it to #stream.
 * 
 */



#include "Program.h"
#include "AtomSet.h"
#include "PrintVisitor.h"

#include <iostream>


PrintVisitor::PrintVisitor(std::ostream& s)
	:stream(s)
{
}

void PrintVisitor::visitAtom(const Atom* a)
{
	//std::cout<<"Visiting atom "<<*a<<std::endl;
			
	if (a->isStronglyNegated())
		stream << '-';

	stream << a->getArgument(0);

	if (a->getArity() > 0)
	{
		stream << '(';

		for (unsigned i = 1; i < a->getArity(); i++)
		{
			stream << a->getArgument(i) << ',';
		}

		stream << a->getArgument(a->getArity()) << ')';
	}
}


void PrintVisitor::visitAtomSet(const AtomSet* as)
{
	stream << '{';
	unsigned i = 0;
	AtomSet::const_iterator a ;
	if (!as->empty())
	{
		for (a = as->begin(); a != as->end(); ++a)
		{
			a->accept(*this);
			if(i++<as->size()-1)
				stream << ", ";
		}
	}

	stream << '}';
}

void PrintVisitor::visitInfixAtom(const InfixAtom* a)
{
	//std::cout<<"Visiting infix atom : "<<a->getArgument(1)<<" "<<a->getArgument(0)<<" "<<a->getArgument(2)<<std::endl;
	stream<<a->getArgument(1);
	stream<<a->getArgument(0);
	stream<<a->getArgument(2);
}

void PrintVisitor::visitLiteral(const Literal* l)
{
	if(l->isNAF())
		stream<<"not ";
	l->getAtom()->accept(*this);

}

void PrintVisitor::visitRule(const Rule* r)
{
	if(!r->getHead().empty())
	{
		unsigned n(0);
		for (RuleHead_t::const_iterator hl = r->getHead().begin();
				   hl != r->getHead().end();
				   ++hl)
		{
			(*hl)->accept(*this);
			if (++n < r->getHead().size())
				stream << " v ";
		}
	}
	if (!r->getBody().empty()) // output body with leading " :- "
	{
		stream << " :- ";

		unsigned n(0);
		for (RuleBody_t::const_iterator l = r->getBody().begin();
				   l != r->getBody().end();
				   ++l)
		{
			(*l)->accept(*this);
			if (++n < r->getBody().size())
				stream << ", ";
		}
	}
	if(!r->getHead().empty() || !r->getBody().empty())
		stream <<".";
}

/*
void PrintVisitor::visitPredicateSign(const struct PredicateSign* ps)
{
	stream<<ps->name;
	stream<<"/";
	stream<<ps->arity;
	stream<<", ";
	stream<<"m_NEG_";
	stream<<ps->name;
	stream<<"/";
	stream<<ps->arity;
}

void PrintVisitor::visitPredicates(const std::set<struct PredicateSign> preds)
{
	stream<<":- dynamic ";
	std::set<struct PredicateSign>::iterator ps_it;
	unsigned n=0;
	
	for(ps_it=preds.begin(); ps_it!=preds.end(); ++ps_it)
	{
		ps_it->accept(*this);
		if(n<preds.size()-1)
			stream<<", ";
		else
			stream<<".";
		n++;
	}
	stream<<std::endl;
}
*/

DLVPrintVisitor::DLVPrintVisitor(std::ostream& os)
	:PrintVisitor(os)
{
	
}

void DLVPrintVisitor::visitAtomSet(const AtomSet* as)
{
	if (!as->empty())
	{
		AtomSet::const_iterator a_it ;
		for (a_it = as->begin(); a_it != as->end(); ++a_it)
		{
			a_it->accept(*this);
			stream << '.' << std::endl;
		}
	}
}

void DLVPrintVisitor::visitAtom(const Atom* a)
{
	PrintVisitor::visitAtom(a);
}

void DLVPrintVisitor::visitInfixAtom(const InfixAtom* a)
{
	PrintVisitor::visitInfixAtom(a);
}

void DLVPrintVisitor::visitLiteral(const Literal* l)
{
	PrintVisitor::visitLiteral(l);
}

void DLVPrintVisitor::visitRule(const Rule* r)
{
	PrintVisitor::visitRule(r);
	stream<<std::endl;
}




PositivePrintVisitor::PositivePrintVisitor(std::ostream& os)
	:PrintVisitor(os)
{

}

void PositivePrintVisitor::visitInfixAtom(const InfixAtom* a)
{
	PrintVisitor::visitInfixAtom(a);
}


void PositivePrintVisitor::visitAtom(const Atom* a)
{
	/* TODO: ugly hack to encode classical negation */
	/* Makes parsing in DLVresultparser ugly as well */
	
	if (a->isStronglyNegated())
		stream << "m_NEG_";

	stream << a->getArgument(0);

	if (a->getArity() > 0)
	{
		stream << '(';

		for (unsigned i = 1; i < a->getArity(); i++)
		{
			stream << a->getArgument(i) << ',';
		}

		stream << a->getArgument(a->getArity()) << ')';
	}
}


void PositivePrintVisitor::visitLiteral(const Literal* l)
{
	if(l->isNAF())
		return;
	else
		l->getAtom()->accept(*this);
}

void PositivePrintVisitor::visitAtomSet(const AtomSet* as)
{
	if (!as->empty())
	{
		AtomSet::const_iterator a_it ;
		for (a_it = as->begin(); a_it != as->end(); ++a_it)
		{
			a_it->accept(*this);
			stream << '.' << std::endl;
		}
	}
}

void PositivePrintVisitor::visitRule(const Rule* r)
{
	/* If it is a constraint, we skip it, because it does not contribute to generation of models */
	if(r->getHead().empty())
		return;
	if(!r->getHead().empty())
	{
		unsigned n(0);
		for (RuleHead_t::const_iterator hl = r->getHead().begin();
				   hl != r->getHead().end();
				   ++hl)
		{
			(*hl)->accept(*this);
			if (++n < r->getHead().size())
				stream << " v ";
		}
	}
	if (!r->getPositiveBody().empty()) // output body with leading " :- "
	{
		stream << " :- ";

		unsigned n(0);
		for (RuleBody_t::const_iterator l = r->getBody().begin();
				   l != r->getBody().end();
				   ++l)
		{
			if(!(*l)->isNAF())
			{
				(*l)->accept(*this);
				if (++n < r->getPositiveBody().size())
					stream << ", ";
			}
		}
	}
	if(!r->getPositiveBody().empty() || !r->getHead().empty())
		stream <<"."<<std::endl;
}

PrologPrintVisitor::PrologPrintVisitor(std::ostream& os)
	:PrintVisitor(os)
{
	
}


void PrologPrintVisitor::visitInfixAtom(const InfixAtom* a)
{
	PrintVisitor::visitInfixAtom(a);
}


void PrologPrintVisitor::visitAtom(const Atom* a)
{
	/* TODO: We represent classical negation as  */
	
	
	if (a->isStronglyNegated())
		stream << "m_NEG_";

	stream << a->getArgument(0);

	if (a->getArity() > 0)
	{
		stream << '(';

		for (unsigned i = 1; i < a->getArity(); i++)
		{
			stream << a->getArgument(i) << ',';
		}

		stream << a->getArgument(a->getArity()) << ')';
	}
}

void PrologPrintVisitor::visitAtomSet(const AtomSet* as)
{
	if (!as->empty())
	{
		AtomSet::const_iterator a_it ;
		for (a_it = as->begin(); a_it != as->end(); ++a_it)
		{
			a_it->accept(*this);
			stream << '.' << std::endl;
		}
	}
}



void PrologPrintVisitor::visitLiteral(const Literal* l)
{
	PrintVisitor::visitLiteral(l);
}

void PrologPrintVisitor::visitRule(const Rule* r)
{
	PrintVisitor::visitRule(r);
	stream<<std::endl;
}


PrologGrPrintVisitor::PrologGrPrintVisitor(std::ostream& os, bool withNeg)
	:PrintVisitor(os), includeNegative(withNeg)
{
	
}


void PrologGrPrintVisitor::visitInfixAtom(const InfixAtom* a)
{
	Term op = a->getArgument(0);
	//std::cout<<op.getName()<<std::endl;
	std::string opstr = op.getName();
	if(opstr.compare("!=") == 0)
	{
		stream<<"not ";
		stream<<a->getArgument(1);
		stream<<" = ";
		stream<<a->getArgument(2);
	}
	else
		PrintVisitor::visitInfixAtom(a);
}


void PrologGrPrintVisitor::visitAtom(const Atom* a)
{
	/* TODO: We represent classical negation as  m_NEG_*/
	
	if (a->isStronglyNegated())
		stream << "m_NEG_";

	stream << a->getArgument(0);

	if (a->getArity() > 0)
	{
		stream << '(';

		for (unsigned i = 1; i < a->getArity(); i++)
		{
			stream << a->getArgument(i) << ',';
		}

		stream << a->getArgument(a->getArity()) << ')';
	}
}

void PrologGrPrintVisitor::visitAtomSet(const AtomSet* as)
{
	if (!as->empty())
	{
		AtomSet::const_iterator a_it ;
		for (a_it = as->begin(); a_it != as->end(); ++a_it)
		{
			a_it->accept(*this);
			stream << '.' << std::endl;
		}
	}
}



void PrologGrPrintVisitor::visitLiteral(const Literal* l)
{
	PrintVisitor::visitLiteral(l);
}

void PrologGrPrintVisitor::visitRule(const Rule* r)
{
	
	
	/*
	Printing a rule a :- b1,...bn, not c1,..not cm.
	into
	m_GROUND(a, m_RULEN, m_VAL(X1,...Xk)) :- b1,....not cm.
	if includeNegative is true, or
	m_GROUND(a, m_RULEN, m_VAL(X1,...Xk)) :- b1,....bn.
	otherwise,
	where X1..Xk are the variables occuring in the rule.
		
	*/
	//assert(r->getHead().size() == 1);
	if(r->getHead().size()!= 1)
	{
		return;
	}
	stream << "m_GROUND"<<"(";
	
	AtomPtr a = *(r->getHead().begin());
	a->accept(*this);
	stream<<", "<<"m_RULE"<<"("<<r->getLine()<<"), ";
	std::set<std::string> ruleVars = r->getPosVariables(); 
	// the rule is already safe, so just take the vars from PBL
	
	if(ruleVars.empty())
	{
		stream<<"_) ";
	}
	else
	{
		unsigned i = 0;
		stream<<"m_VAL"<<"(";
		for(std::set<std::string>::iterator v_it = ruleVars.begin();
				  v_it!= ruleVars.end(); ++v_it)
		{
			stream<<*v_it;
			if(i++<ruleVars.size()-1)
				stream<<", ";
		}
		stream<<")) ";
	}
	RuleBody_t myBody;
	
	/* Do we include negative literals to print? */
	if(includeNegative)
		myBody = r->getBody();
	else
		myBody = r->getPositiveBody()	;
	
	/* Remove all infix atom literals in the body */
	/* They are irrelevant to finding ground instantiation, since variables
		appearing in infix body literals must appear in an ordinary predicates
		in the positive body literals 
	
	RuleBody_t::iterator l;
	for(l=myBody.begin(); l!= myBody.end(); ++l)
	{
		AtomPtr a = (*l)->getAtom();
		if(typeid(*a) == typeid(InfixAtom))
			myBody.erase(l);
	}
	*/
	
	if (!myBody.empty()) // output body with leading " :- "
	{
		stream << " :- ";

		unsigned n(0);
		for (RuleBody_t::const_iterator l = myBody.begin();
				   l != myBody.end();
				   ++l)
		{
			(*l)->accept(*this);
			if (++n < myBody.size())
				stream << ", ";
		
		}
		
	}
	if(!myBody.empty() || !r->getHead().empty())
		stream <<".";
	stream<<std::endl;
}




PrologMinCheckPrintVisitor::PrologMinCheckPrintVisitor(std::ostream& os)
	:PrintVisitor(os)
{
}


void PrologMinCheckPrintVisitor::visitInfixAtom(const InfixAtom* a)
{
	Term op = a->getArgument(0);
	//std::cout<<op.getName()<<std::endl;
	std::string opstr = op.getName();
	if(opstr.compare("!=") == 0)
	{
		stream<<"not ";
		stream<<a->getArgument(1);
		stream<<" = ";
		stream<<a->getArgument(2);
	}
	else
		PrintVisitor::visitInfixAtom(a);
}


void PrologMinCheckPrintVisitor::visitAtom(const Atom* a)
{
	/* TODO: We represent classical negation as m_NEG */
	
	
	if (a->isStronglyNegated())
		stream << "m_NEG_";

	stream << a->getArgument(0);

	if (a->getArity() > 0)
	{
		stream << '(';

		for (unsigned i = 1; i < a->getArity(); i++)
		{
			stream << a->getArgument(i) << ',';
		}

		stream << a->getArgument(a->getArity()) << ')';
	}
}

void PrologMinCheckPrintVisitor::visitAtomSet(const AtomSet* as)
{
	/* Printing an atomset as a Prolog list */
	
	stream << list_predicate <<"([";
	unsigned i = 0;
	AtomSet::const_iterator a ;
	if (!as->empty())
	{
		for (a = as->begin(); a != as->end(); ++a)
		{
			a->accept(*this);
			if(i++<as->size()-1)
				stream << " , ";
		}
	}

	stream << "])."<<std::endl;
}



void PrologMinCheckPrintVisitor::visitLiteral(const Literal* l)
{
	PrintVisitor::visitLiteral(l);
}

void PrologMinCheckPrintVisitor::visitRule(const Rule* r)
{
	/*
	Printing a rule :- b1,...bn, not c1,..not cm.
	into
	m_VIOLATES :- b1,....not cm.
	*/
	
	//assert(r->getHead().size() == 1);
	
	
	if(r->getHead().size()>0)
	{
		throw GeneralError("Cannot print rule with head in minimality checking");
	}
	RuleBody_t myBody;
	
	/* If body is empty, we return */
	myBody = r->getBody();
	if(myBody.size() == 0)
		return;
	
	stream<<"m_VIOLATES ";
		
	
	if (!myBody.empty()) // output body with leading " :- "
	{
		stream << " :- ";

		unsigned n(0);
		for (RuleBody_t::const_iterator l = myBody.begin();
				   l != myBody.end();
				   ++l)
		{
			(*l)->accept(*this);
			if (++n < myBody.size())
				stream << ", ";
		
		}
		
	}
	if(!myBody.empty() || !r->getHead().empty())
		stream <<".";
	stream<<std::endl;
}


// End

