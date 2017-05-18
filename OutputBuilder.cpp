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
 * @file   OutputBuilder.h
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  Builders for solver result.
 * 
 * 
 */


#include "OutputBuilder.h"
#include "PrintVisitor.h"
#include "Globals.h"



void OutputTextBuilder::buildAnswerSet(const AtomSet& facts)
{

	PrintVisitor rpv(stream);
	facts.accept(rpv);
	stream << std::endl;
}


void OutputXMLBuilder::buildPre()
{
	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

	stream << "<RuleML xmlns=\"http://www.ruleml.org/0.9/xsd\"\n"
			<< "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
			<< "xsi:schemaLocation=\"http://www.ruleml.org/0.9/xsd "
			<< "http://www.ruleml.org/0.9/xsd/datalog.xsd\">\n";

	stream << "<Assert mapClosure=\"universal\">\n";

	stream << "<Or>" << std::endl;
}


void OutputXMLBuilder::buildPost()
{
	stream << "</Or>" << std::endl;
	stream << "</Assert>" << std::endl;
	stream << "</RuleML>" << std::endl;
}


void OutputXMLBuilder::buildAnswerSet(const AtomSet& facts)
{
	stream << "<And>" << std::endl;

	for (AtomSet::const_iterator f = facts.begin();
		    f != facts.end();
		    ++f)
	{
		if ((*f).isStronglyNegated())
			stream << "<Neg>";

		stream << "<Atom>";

		stream << "<Rel>";
		stream << "<![CDATA[" << (*f).getArgument(0) << "]]>";
		stream << "</Rel>";

		for (unsigned i = 1; i <= (*f).getArity(); i++)
		{
			stream << "<Ind>";
			stream << "<![CDATA[" << (*f).getArgument(i) << "]]>";
			stream << "</Ind>";
		}

		stream << "</Atom>";

		if ((*f).isStronglyNegated())
			stream << "</Neg>";

		stream << std::endl;
	}

	stream << "</And>" << std::endl;
}
//End

