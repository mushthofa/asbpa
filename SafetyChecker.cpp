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
 * @file SafetyChecker.cpp
 * @author Mushthofa
 * @date Tue 24 03 2009
 *
 * @brief Safety checker class
 *
 *
 */

#include "SafetyChecker.h"

SafetyChecker::SafetyChecker(const Atom& a)
	:atom(a)
{
	type = FACT;
}

SafetyChecker::SafetyChecker(const Rule& r)
	:rule(r)
{
	type = RULE;
}

bool SafetyChecker::checkSafe() const
{
	if(type == FACT)
	{
		if(atom.isGround())
			return true;
		else
			return false;
	}
	else if(type == RULE)
	{
		std::set<std::string> posvars, negvars;
		posvars = rule.getPosVariables();
		negvars = rule.getNegVariables();
		std::set<std::string> diff;
		std::set_difference(negvars.begin(), negvars.end(),
						posvars.begin(), posvars.end(),
								    inserter(diff, diff.begin()));
		if(diff.size()>0)
			return false;
		else
			return true;
	}
	else
	{
		throw GeneralError("Safety checker called for unknown type object!");
	}
}

//End

