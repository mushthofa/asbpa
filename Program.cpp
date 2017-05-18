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
 * @file Program.cpp
 * @author Roman Schindlauer, Mushthofa
 *
 * @brief Program class.
 *
 */

#include "Program.h"
#include <sstream>

Program::Program()
{
}

RulePtr Program::operator[](unsigned int idx) const
{
	rulemap_t::const_iterator it = rule_map.find(idx);
	if(it==rule_map.end())
	{
		std::ostringstream oserr;
		oserr<<"Rule index "<<idx<<" is not found."<<std::endl;
		throw GeneralError(oserr.str());
	}
	else
		return it->second;
}

void Program::addRule(RulePtr r, bool withPredicate)
{
	rules.insert(r);
	rule_map.insert(std::pair<unsigned, RulePtr>(r->getLine(), r));
	/* Update the predicate set */
	
	if(!withPredicate)
		return;
	RuleHead_t currentHead = r->getHead();
	RuleBody_t currentBody = r->getBody();
		
	RuleHead_t::iterator h_it;
	for(h_it = currentHead.begin(); h_it!=currentHead.end(); ++h_it)
	{
		AtomPtr currAtom = *h_it;
		if(typeid(*currAtom) != typeid(InfixAtom))
		{
			struct PredicateSign currPredicate;
			currPredicate.name = currAtom->getPredicateName();
			currPredicate.arity = currAtom->getArity();
			preds.insert(currPredicate);
		}
	}
		
	RuleBody_t::iterator b_it;
	for(b_it = currentBody.begin(); b_it!=currentBody.end(); ++b_it)
	{
		LiteralPtr currLiteral = *b_it;
		AtomPtr currAtom = currLiteral->getAtom();
		if(typeid(*currAtom) != typeid(InfixAtom))
		{
			struct PredicateSign currPredicate;
			currPredicate.name = currAtom->getPredicateName();
			currPredicate.arity = currAtom->getArity();
			preds.insert(currPredicate);
		}
	}
}

void Program::deleteRule(iterator i)
{
	
	RulePtr r = *(i.it);
	unsigned idx = r->getLine();
	rulemap_t::iterator it = rule_map.find(idx);
	rule_map.erase(it);
	rules.erase(i.it);
}

void Program::clear()
{
	rule_map.clear();
	rules.clear();
}

bool Program::isHorn() const
{
	/* Program is normal positive, no negative body literals appear on a body of a rule 
	and no disjunction appear in the head */
	
	Program::iterator it;
	for(it=this->begin(); it!=this->end(); ++it)
	{
		RulePtr currentRule = (*it);
		RuleHead_t currentHead = currentRule->getHead();
		if(currentHead.size() > 1)
			return false;
		RuleBody_t currentBody = currentRule->getNegativeBody();
		if(currentBody.size() > 0)
			return false;
	}
	return true;
}

unsigned Program::maxVarNumber() const
{
	unsigned maxNumber = 0;
	Program::iterator it;
	for(it=this->begin(); it!=this->end(); ++it)
	{
		RulePtr currentRule = (*it);
		unsigned numVars = currentRule->numberOfVars();
		if(numVars > maxNumber)
			maxNumber = numVars;
	}
	return maxNumber;
}


Program Program::doShift() const
{
	Program result;
	Program::iterator it;
	unsigned newID = 0;
	for (it = this->begin(); it!=this->end(); ++it)
	{
		RulePtr currentRule = (*it);
		RuleHead_t currentHead = currentRule->getHead();
		RuleBody_t currentBody = currentRule->getBody();
		std::string filename = currentRule->getFile();
		
		if(currentHead.size()>1) // rule is disjunctive -> perform shifting 
		{
			RuleHead_t::iterator h_it, h_it2;
			for(h_it = currentHead.begin(); h_it!=currentHead.end(); ++h_it)
			{
				RuleHead_t newHead;
				newHead.insert(*h_it);
				RuleBody_t newBody;
				newBody = currentBody;
				for(h_it2 = currentHead.begin(); h_it2!=currentHead.end(); ++h_it2)
				{
					if(h_it2!=h_it)
					{
						LiteralPtr addedNegBody (new Literal(*h_it2, true));
						newBody.insert(addedNegBody);
					}
				}
				RulePtr newRule (new Rule(newHead, newBody, filename, newID++));
				result.addRule(newRule);
			}
		}
		else		// rule is normal -> just copy it
		{
			RulePtr currentRule = (*it);
			RuleHead_t currentHead = currentRule->getHead();
			RuleBody_t currentBody = currentRule->getBody();
			std::string filename = currentRule->getFile();
			RulePtr newRule (new Rule(currentHead, currentBody, filename, newID++));
			result.addRule(newRule);
		}
	}
	return result;
}

Program Program::getCons() const 
{
	Program result;
	Program::iterator it;
	unsigned newID = 0;
	for (it = this->begin(); it!=this->end(); ++it)
	{
		RulePtr currentRule = (*it);
		RuleHead_t currentHead = currentRule->getHead();
		RuleBody_t currentBody = currentRule->getBody();
		std::string filename = currentRule->getFile();
		
		RuleHead_t::iterator h_it;
		for(h_it = currentHead.begin(); h_it!=currentHead.end(); ++h_it)
		{
			LiteralPtr addedNegBody (new Literal(*h_it, true));
			currentBody.insert(addedNegBody);
		}
		RulePtr cons_r (new HardConstraint(currentBody, filename, newID++));
		result.addRule(cons_r);
	}
	return result;
}

Program Program::getPrimed() const
{
	Program result;
	Program::iterator it;
	unsigned newID = 0;
	for (it = this->begin(); it!=this->end(); ++it)
	{
		RulePtr currentRule = (*it);
		RuleHead_t currentHead = currentRule->getHead();
		RuleBody_t currentBody = currentRule->getBody();
		std::string filename = currentRule->getFile();
		
		RuleBody_t newBody;
		RuleBody_t::iterator b_it;
		for(b_it = currentBody.begin(); b_it != currentBody.end(); ++b_it)
		{
			LiteralPtr addedBody (new Literal((*b_it)->getPrimed()));
			newBody.insert(addedBody);
		}
		RulePtr primed_r (new Rule(currentHead, newBody, filename, newID++));
		result.addRule(primed_r);
	}
	return result;
}

std::set<struct PredicateSign> Program::getPredicates() const
{	
	return preds;
}

std::ostream& operator<< (std::ostream& os, const Program& p)
{
	for (Program::const_iterator it=p.begin(); it!=p.end(); ++it)
	{
		os<<(*(*it));//<<std::endl;
	}
}

/*bool operator< (struct PredicateSign a, struct PredicateSign b)
{
	if(a.arity < b.arity)
		return true;
	if(a.arity > b.arity)
		return false;
	if(a.name < b.name)
		return true;
	if(a.name > b.name)
		return false;
	return false;
}
*/

//End
