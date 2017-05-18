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
 * @file   OutputBuilder.h
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  Builders for solver result.
 * 
 * 
 */

#ifndef _OUTPUT_BUILDER_H
#define _OUTPUT_BUILDER_H

#include <string>
#include <sstream>
#include <vector>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "AtomSet.h"


/**
 * @brief Base Builder for building solver output.
 */

typedef boost::tokenizer<boost::char_separator<char> > sepTokenizer_t;

class OutputBuilder
{
	protected:
    		std::ostringstream stream;
		/// Ctor
		OutputBuilder(std::string& filters) 
		{
			
			if(!filters.empty())
			{
				boost::char_separator<char> commaSep (",");
				sepTokenizer_t tokens(filters, commaSep);
				sepTokenizer_t::iterator t_it;
				for (t_it = tokens.begin(); t_it != tokens.end(); ++t_it)
				{
					std::string currFilter = *t_it;
					boost::trim(currFilter);
					if(!currFilter.empty())
						filter.push_back(currFilter);
				}
				
			}
		}
		
		std::vector<std::string> filter;

	public:
		/// Dtor
		virtual ~OutputBuilder() {};

		virtual void buildPre() {};

		virtual void buildPost() {};

    		/**
		 * @brief Build answer set.
		 */
		virtual void buildAnswerSet(const AtomSet&) = 0;

		virtual inline std::string getString()
		{
			const std::string& str = stream.str();

			stream.str("");
			stream.clear();
			return str;
		}
		
		virtual void buildOutput(const AtomSet& f)
		{
			AtomSet output = f;
			
			if(!filter.empty())
				output.keep(filter);
			
			//buildPre();
			
			buildAnswerSet(output);
			
			//buildPost();
		}

		
};

	
/**
* @brief Simple textual output.
*/

class OutputTextBuilder : public OutputBuilder
{
	public:
    		/// Dtor
		virtual ~OutputTextBuilder() {};

   	 	/// Ctor 
		OutputTextBuilder(std::string& filters) 
		: OutputBuilder(filters){};

    		/**
		 * @brief Build answer set.
		 */
		virtual void buildAnswerSet(const AtomSet&);
};


/**
 * @brief XML output.
 */

class OutputXMLBuilder : public OutputBuilder
{
	public:
    		/// Dtor
		virtual ~OutputXMLBuilder() {};

    		/// Ctor 
		OutputXMLBuilder(std::string& filters) 
		: OutputBuilder(filters){};

		virtual void buildPre();

		virtual void buildPost();
		/**
		* @brief Build answer set.
		*/
		virtual void buildAnswerSet(const AtomSet&);

};



#endif /* _OUTPUTBUILDER_H */
