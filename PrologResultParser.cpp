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
 * @file PrologResultParser.cpp
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Parser for Prolog output
 *
 *
 */



#include "PrologResultParser.h"
//#include <boost/algorithm/string/trim.hpp>
#include <cstdlib>

PrologResultParser::PrologResultParser(std::string& str, const Program& prog)
{
	boost::trim(str);
	if(str.size()>0)
	{
		std::string::size_type nextpos = str.find_first_of("|", 0);
		std::string::size_type firstpos = 0;
		if(nextpos == std::string::npos)
			throw FatalError("Error in XSB Result " + str);
		while(nextpos != std::string::npos && firstpos!=std::string::npos)
		{
			/* Find the rule id: m_RULEN */
			std::string strCurrID = str.substr(firstpos, nextpos-firstpos);
			strCurrID = getArgs(strCurrID);
			
			
			firstpos = nextpos;
			nextpos = str.find_first_of("|", firstpos + 1);
			
			/* Get the valuations : m_VAL(v1, ...,vn)*/
			
			std::string strCurrVals = str.substr(firstpos, nextpos-firstpos);
			strCurrVals = getArgs(strCurrVals);
			
			firstpos = nextpos;
			nextpos = str.find_first_of("|", firstpos + 1);
			
			RulePtr currRule;
			/* Chop off m_RULE */
			try
			{
				
				unsigned currID = (unsigned)strtoul(strCurrID.c_str(), 0, 10);
				currRule = prog[currID];
			}
			catch(GeneralError& e)
			{
				std::cout<<"Error in Prolog result "<<str<<std::endl<<e.getErrorMsg()<<std::endl;
				exit(1);
			}
			
			
			if(currRule->getPosVariables().size()>0)
			{
				
				/* tokenize valuations */
				
				std::vector<std::string> vals = tokenize(strCurrVals, ",");
				RulePtr groundRule(new Rule(currRule->bindVariables(vals)));
				ground.addRule(groundRule, false);
			}
			else
				ground.addRule(currRule, false);
			
		}// End while
	}
	else
	{
		// What to do with empty answer ?? 
	}
	
}
//End
