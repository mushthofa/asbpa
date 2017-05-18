/***************************************************************************
 *   Copyright (C) 2009 by Mushthofa   								*
 *   unintendedchoice@gmail.com   					   *
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
 * @file Rule.h
 * @author Roman Schindlauer, Mushthofa
 *
 * @brief Rule class.
 *
 */


#ifndef _RULE_H
#define _RULE_H

#include <boost/ptr_container/indirect_fun.hpp>
#include <sstream>
#include <cmath>
#include "Literal.h"
#include "Globals.h"
typedef std::set<AtomPtr, boost::indirect_fun<std::less<Atom> > > RuleHead_t;



class Rule
{
	public:
		Rule(){}
		Rule(const RuleHead_t&, const RuleBody_t&, 
   			const std::string& = "",
   			unsigned = 0, bool noVars=false);
		virtual ~Rule();
		
		virtual const RuleHead_t& getHead() const;
		virtual const RuleBody_t& getBody() const;
		virtual const RuleBody_t getPositiveBody() const;
		virtual const RuleBody_t getNegativeBody() const;
		
		virtual void setHead(const RuleHead_t&);
		virtual void setBody(const RuleBody_t&);
		
		virtual void addHead(AtomPtr);
		virtual void addBody(LiteralPtr);
		
		std::set<std::string> getVariables() const;
		std::set<std::string> getPosVariables() const;
		std::set<std::string> getNegVariables() const;
		unsigned numberOfVars() const;
		
		bool isGround() const;
		
		Rule bindVariables(const std::vector<std::string>&);
		
		std::string getFile() const;
		unsigned getLine() const;
		
		bool operator==(const Rule&) const;
		bool operator!=(const Rule&) const;
		bool operator<(const Rule&) const;
		bool operator>(const Rule& r2) const
		{
			return !(*this<r2 || *this==r2);
		}
		
		virtual void accept(BaseVisitor&) const;
		
		bool isDefinite()
		{
			if(head.size() != 1)
				return false;
			RuleBody_t::iterator b_it;
			for(b_it = body.begin(); b_it!=body.end(); ++b_it)
			{
				if((*b_it)->isNAF())
					return false;
			}
			return true;
		}
		
		bool isSmallConstraint(unsigned long maxGr) const
		{
			if(head.size() == 0 && isSmall(maxGr))
				return true;
			return false;
		}
		
		bool isSmall(unsigned long maxGr) const
		{
			int minRuleVar = Globals::Instance()->intOption("minRuleVar");
			if(posVars.size() <= minRuleVar) 
				return true;
			unsigned long numConstants = Atom::constantsTable.size();
			double cv = (double)log(maxGr)/ (double)log(posVars.size());
			//std::cout << "cv " << cv <<std::endl;
			if(numConstants < cv)
				return true;
			return false;
		}
	protected:
		RuleHead_t head;
		RuleBody_t body;
		std::string filename;
		unsigned lineid;
		std::set<std::string> posVars;
};

std::ostream& operator<<(std::ostream&, const Rule&);

bool operator< (const RuleHead_t& head1, const RuleHead_t& head2);
bool operator< (const RuleBody_t& b1, const RuleBody_t& b2);

class HardConstraint : public Rule
{
	public:
		HardConstraint(const RuleBody_t&,
					const std::string& = "",
	unsigned = 0);
		bool operator==(const HardConstraint&) const;
		bool operator<(const HardConstraint&) const;
		void setHead(const RuleHead_t&);
		void addHead(AtomPtr);
		
};



#endif

//End

