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
 * @file Rule.cpp
 * @author Roman Schindlauer, Mushthofa
 *
 * @brief Rule class.
 *
 */


#include "Rule.h"

Rule::Rule(const RuleHead_t& h, const RuleBody_t& b,
		 const std::string& f, unsigned l, bool noVars)
	:head(h), body(b), filename(f), lineid(l)
{
	if(!noVars)
		
		/* Store the rule's variables */
	{
		RuleBody_t::const_iterator b_it;
		for(b_it = body.begin(); b_it!=body.end(); ++b_it)
		{
			std::set<std::string> tmpvars;
			if(!(*b_it)->isNAF() && !(*b_it)->isInfix())
			{
				tmpvars = (*b_it)->getVariables();
				std::set_union(posVars.begin(), posVars.end(),
							tmpvars.begin(), tmpvars.end(),
									inserter(posVars, posVars.begin()));
			}
		}
	}
}

Rule::~Rule()
{
}

const RuleHead_t& Rule::getHead() const
{
	return head;
}

const RuleBody_t& Rule::getBody() const
{
	return body;
}

const RuleBody_t Rule::getPositiveBody() const
{
	RuleBody_t result;
	RuleBody_t::const_iterator it;
	for(it=body.begin(); it!=body.end(); ++it)
	{
		if(!(*it)->isNAF())
			result.insert(*it);
	}
	return result;
}

const RuleBody_t Rule::getNegativeBody() const
{
	RuleBody_t result;
	RuleBody_t::const_iterator it;
	for(it=body.begin(); it!=body.end(); ++it)
	{
		if((*it)->isNAF())
			result.insert(*it);
	}
	return result;
}


std::string Rule::getFile() const
{
	return filename;
}

unsigned Rule::getLine() const
{
	return lineid;
}

void Rule::setHead(const RuleHead_t& h)
{
	head = h;
}

void Rule::setBody(const RuleBody_t& b)
{
	body = b;
}

void Rule::addHead(AtomPtr a)
{
	head.insert(a);
}

void Rule::addBody(LiteralPtr l)
{
	body.insert(l);
}

std::set<std::string> Rule::getVariables() const
{
	std::set<std::string> vars;
	RuleHead_t::const_iterator h_it;
	for(h_it = head.begin(); h_it!=head.end(); ++h_it)
	{
		std::set<std::string> tmpvars;
		tmpvars = (*h_it)->getVariables();
		std::set_union(vars.begin(), vars.end(),
					tmpvars.begin(), tmpvars.end(),
							    inserter(vars, vars.begin()));
	}
	
	RuleBody_t::const_iterator b_it;
	for(b_it = body.begin(); b_it!=body.end(); ++b_it)
	{
		std::set<std::string> tmpvars;
		tmpvars = (*b_it)->getVariables();
		std::set_union(vars.begin(), vars.end(),
					tmpvars.begin(), tmpvars.end(),
							    inserter(vars, vars.begin()));
	}
	return vars;
}

std::set<std::string> Rule::getPosVariables() const
{
	return posVars;
}

std::set<std::string> Rule::getNegVariables() const
{
	std::set<std::string> vars;
	RuleHead_t::const_iterator h_it;
	for(h_it = head.begin(); h_it!=head.end(); ++h_it)
	{
		std::set<std::string> tmpvars;
		tmpvars = (*h_it)->getVariables();
		std::set_union(vars.begin(), vars.end(),
					tmpvars.begin(), tmpvars.end(),
							    inserter(vars, vars.begin()));
	}
	
	RuleBody_t::const_iterator b_it;
	for(b_it = body.begin(); b_it!=body.end(); ++b_it)
	{
		std::set<std::string> tmpvars;
		if((*b_it)->isNAF() || (*b_it)->isInfix()) // Also take the infix predicates for safety checks
		{
			tmpvars = (*b_it)->getVariables();
			std::set_union(vars.begin(), vars.end(),
						tmpvars.begin(), tmpvars.end(),
								    inserter(vars, vars.begin()));
		}
	}
	return vars;
}

unsigned Rule::numberOfVars() const
{
	return posVars.size();
}


bool Rule::isGround() const
{
	RuleHead_t::const_iterator h_it;
	for(h_it = head.begin(); h_it!=head.end(); ++h_it)
	{
		if(!(*h_it)->isGround())
			return false;
	}
	RuleBody_t::const_iterator b_it;
	for(b_it = body.begin(); b_it!=body.end(); ++b_it)
	{
		if(!(*b_it)->isGround())
			return false;
	}
	return true;
}

Rule Rule::bindVariables(const std::vector<std::string>& vals)
{
	/* create the subtitutions list first */
	
	Substitutions_t sub;
	std::set<std::string>::iterator var_it;
	std::vector<std::string>::const_iterator val_it;
	
	if(vals.size() != posVars.size())
	{
		std::ostringstream oserr;
		oserr<<*this<<std::endl;
		oserr<<"with values: "<<std::endl;
		for(val_it = vals.begin(); val_it!=vals.end(); ++val_it)
			oserr<<*val_it<<", ";
		oserr<<std::endl;
		throw GeneralError("Incomplete values for binding: " + oserr.str());
	}
	
	for(var_it = posVars.begin(), val_it=vals.begin(); var_it!= posVars.end(); ++var_it, ++val_it)
	{
		sub.insert(std::pair<std::string, std::string>(*var_it, *val_it));
	}
	
	
	RuleHead_t bindHead;
	RuleHead_t::const_iterator h_it;
	for(h_it = head.begin(); h_it!=head.end(); ++h_it)
	{
		AtomPtr bindAtom(new Atom((*h_it)->bindVariables(sub)));
		bindHead.insert(bindAtom);
	}
	
	RuleBody_t bindBody;
	RuleBody_t::const_iterator b_it;
	for(b_it = body.begin(); b_it!=body.end(); ++b_it)
	{
		LiteralPtr bindLiteral(new Literal((*b_it)->bindVariables(sub)));
		bindBody.insert(bindLiteral);
	}
	return Rule(bindHead, bindBody, "", 0, true);
}

void Rule::accept(BaseVisitor& v) const
{
	v.visitRule(this);
}

bool Rule::operator== (const Rule& rule2) const
{
	const RuleHead_t& head2 = rule2.getHead();

	if (head.size() != head2.size())
		return false;

	const RuleBody_t& body2 = rule2.getBody();
	if (body.size() != body2.size())
		return false;
	return std::equal(head.begin(), head.end(), head2.begin()) &&
			std::equal(body.begin(), body.end(), body2.begin());
}

bool Rule::operator!=(const Rule& rule2) const
{
	return !(*this==rule2);
}

bool operator< (const RuleHead_t& head1, const RuleHead_t& head2)
{
	return std::lexicographical_compare(head1.begin(), head1.end(),
								 head2.begin(), head2.end());
}

bool operator< (const RuleBody_t& b1, const RuleBody_t& b2)
{
	return std::lexicographical_compare(b1.begin(), b1.end(),
								 b2.begin(), b2.end());
}

bool Rule::operator< (const Rule& rule2) const
{
	if (this->head < rule2.head)
	{
		return true;
	}
	if (rule2.head < this->head)
	{
		return false;
	}

	if (this->body < rule2.body)
	{
		return true;
	
	}
	return false;
}

std::ostream& operator<<(std::ostream& os, const Rule& r)
{
	RuleHead_t h = r.getHead();
	unsigned i = 0;
	for(RuleHead_t::const_iterator h_it = h.begin(); h_it!=h.end(); ++h_it)
	{
		os<<*(*h_it);
		if(++i<h.size())
			os<<" v ";
	}
	RuleBody_t b = r.getBody();
	if(b.size()>0)
	{
		os<<" :- ";
		os<<b;
	}
	if(h.size()>0 || b.size()>0)
		os<<"."<<std::endl;
	return os;
}

HardConstraint::HardConstraint(const RuleBody_t& b, const std::string& f, unsigned l)
	:Rule (RuleHead_t(), b, f, l)
{
}

bool HardConstraint::operator==(const HardConstraint& h2) const
{

	const RuleBody_t& body2 = h2.getBody();
	if (body.size() != body2.size())
		return false;

	return std::equal(body.begin(), body.end(), body2.begin());
}

bool HardConstraint::operator<(const HardConstraint& h2) const
{
	if (this->body < h2.body)
		return true;
	else 
		return false;
}

void HardConstraint::setHead(const RuleHead_t& )
{
	//Disallow
}

void HardConstraint::addHead(AtomPtr)
{
	//Disallow
}
//End
