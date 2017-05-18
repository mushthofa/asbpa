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

#include "SimpleMethodSelector.h"
#include "Globals.h"
#include <cmath>

SimpleMethodSelector::SimpleMethodSelector(HCFDetector* h)
	:MethodSelector(h)
{
	
}

MethodSelector::Method_t SimpleMethodSelector::getMethod(Component* c)
{
	
	Program IDB = c->getBottom();
	/* If it is empty, then we must use empty evaluation */
	if(IDB.empty())
	{
		//std::cout << "empty" <<std::endl;
		return EMPTY;
	}
	
	/* Non=HCF programs must be evaluated using disjunctive method */
	if(!hcfd->isHCF(c))
	{
		//std::cout << "non-hcf" << std::endl;
		return METHOD_DISJ;
	}
	
	/* If the program is positive/stratified, then we use the direct evaluation using
		ASP solver */
	
	if(IDB.isHorn() || IDB.size() == 1)
	{
		//std::cout << "stratified " <<std::endl;
		return DIRECT;
	}
	
	/* If the program is sufficiently small, indicated by small number (possible)
		ground rules, then we use direct evaluation as well */
	
	/* number of possible ground rules is computed as maxVar^C, where maxVar
	 is the maximal number variables appearing in a rule of the program and C
	 is the number of constant symbols */
	
	unsigned maxVar = IDB.maxVarNumber();
	unsigned numConst = Atom::constantsTable.size();
	
	/* If the max number of variables is small, just use the direct evaluation */
	if(maxVar <= Globals::Instance()->intOption("minRuleVar"))
		return DIRECT;
	
	unsigned long maxGr = Globals::Instance()->intOption("maxNumGround");
	double cv = (double)log(maxGr) / (double)log(maxVar);
	if(numConst <= cv)
		return DIRECT;
	
	/* We now respect the settings given in the commandline argument -m */
	unsigned method = Globals::Instance()->intOption("EvalMethod");
	switch(method)
	{
		case 0:
			return DIRECT;
			
		case 1:
			return METHOD_NORMAL1;
			
		case 2:
			return METHOD_NORMAL2;
			
		case 3:
			return METHOD_DISJ;
			
		default:
			
			/* At this point, we are free to choose between Method 1 and  Method 2 
			Our choice for now is just to decide whether we are computing all answer set
			or just one/several answer set. 
			For the former, it seems that Method 2 is better, while for the latter, 
			Method 1 is better 
			
			*/
			
			if(Globals::Instance()->boolOption("limitAS"))
			{
				return METHOD_NORMAL1;
			}
			else
				return METHOD_NORMAL2;
	}
	
	/* Should not reach here */
}
//End
