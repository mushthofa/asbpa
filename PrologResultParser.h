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
 * @file PrologResultParser.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Parser for Prolog output
 *
 *
 */


#ifndef _PROLOGRESULTPARSER_H
#define _PROLOGRESULTPARSER_H

#include <vector>


#include <boost/algorithm/string/trim.hpp>
#include "Program.h"
#include "Error.h"



class PrologResultParser
{
	private:
		Program ground;
		
		std::vector<std::string>
				tokenize(const std::string& inString, const std::string& separator)
				{
					std::vector<std::string> returnVector;
			
					std::string::size_type start = 0;
					std::string::size_type end = 0;

			
					while ((end = inString.find(separator, start)) != std::string::npos)
					{
						returnVector.push_back(inString.substr(start, end - start));
						start = end + separator.size();
					}
			
					if (inString.size() > 0)
						returnVector.push_back(inString.substr(start));
					return returnVector;
				}
		
				std::string getArgs(const std::string& input)
				{
					std::string::size_type start = input.find_first_of("()", 0);
					std::string::size_type end = input.find_first_of("()", start + 1);
					return input.substr(start + 1, end-start-1);
				}
				
		
	public:
		PrologResultParser(std::string&, const Program& );
		Program getGroundProg() const
		{
			return ground;
		}
		
};




#endif

//
