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
 * @file logic_grammar.h
 * @author Mushthofa
 * @date Tue 24 03 2009
 *
 * @brief Grammar spesification for logic program in Boost::Spirit format
 *
 *
 */

#ifndef _LOGIC_GRAMMAR_H
#define _LOGIC_GRAMMAR_H

using namespace boost::spirit;

struct logic_grammar : public grammar<logic_grammar>
{
	
	static const int constantID = 1;
	static const int anonID= 2;
	static const int variableID = 3;
	static const int termID = 4;
	static const int predicateID = 5;
	static const int infix_predID = 6;
	static const int atomID = 7;
	static const int literalID = 8;
	static const int body_literalID = 9;
	static const int headID = 10;
	static const int bodyID = 11;
	static const int constraintID = 12;
	static const int factID = 13;
	static const int ruleID = 14;
	static const int lineID = 15;
	static const int disjunctID = 16;
	static const int symbolID = 17;
	static const int stringconstID = 18;
	static const int integerID = 19;
	
	
	template <typename ScannerT>
			struct definition
	{
		definition(logic_grammar const& )
		{
			//Starting the definition of the grammar 
			
			symbol = leaf_node_d[lower_p >> *(alnum_p|ch_p('_'))];
			// symbol --> [a-z].([A-Z]|[a-z]|[0-9]|_)*
			stringconst = leaf_node_d[ch_p('"')>>*(alnum_p|ch_p('_')|space_p)>>ch_p('"')];
			// stringconst --> ".([a-z]|[A-Z]|[0-9]|_)"
			integer = leaf_node_d[+digit_p];
			// integer --> [0-9]*
			constant = symbol|stringconst|integer;
			// constant --> [0-9]+ | [a-z].{[a-z A-Z]|[0-9]|'_'}*
			anon = leaf_node_d[ch_p('_')];
			// anon --> '_'
			variable =  leaf_node_d[upper_p >> *(alnum_p|ch_p('_'))];
			// variable --> [A-Z].{[a-zA-Z]|[0-9]|'_'}*
			term = constant | variable | anon;
			// term --> constant | variable | anon
			predicate = leaf_node_d[lower_p >> *(alnum_p|ch_p('_'))];
			// predicate --> [a-z].{[a-zA-Z]|[0-9]|'_'}*
			infix_pred = str_p(">=") | str_p("<=")|
					ch_p('<') | ch_p('>') | 
					ch_p('=') | str_p("!=");
			// infix_pred = '<'|'>'|'>='|'<='|'='|'!='
			atom = root_node_d[predicate] >> 
					inner_node_d
					[
					ch_p('(') >> infix_node_d
					[
						term  >> *(ch_p(',') >> term)
					] >> ch_p(')')
					]
					| term>>root_node_d[infix_pred]>>term
					| predicate;
			
			// atom --> predicate | predicate . '('. term.(term.',')* ')' 
			
			/* Actually not a good term for this ... literal should be used for body */
			literal = (root_node_d[ch_p('-')]>>atom) | 
					atom ;
			// literal --> - atom | atom
			
			
			body_literal = (root_node_d[str_p("not")]>>literal) |
					literal;
			// body_literal --> not literal | literal
			disjunct = str_p('v') | ch_p('|');
			head = literal >> *(root_node_d[disjunct]>>literal);
			// head --> literal.('v'.literal)*
			body = infix_node_d[body_literal >> *(ch_p(',')>>body_literal)];
			// body --> body_literal.(','. body_literal)*
			constraint = root_node_d[str_p(":-")] >> body;
			// constraint = ':-'.body
			fact = literal;
			// fact --> literal
			prule = (infix_node_d[head >> str_p(":-") >> body]) | 
					literal >> +(root_node_d[disjunct]>>literal);
			// rule --> head.":-".body | head . 
			/* Only head for the case of propositional disjunctive rules, eg: a v b. */
			line = prule | root_node_d[fact] | constraint;
			// line --> fact | constraint | rule
		}
		rule<ScannerT, parser_context<>, parser_tag<constantID> > constant;
		rule<ScannerT, parser_context<>, parser_tag<anonID> > anon;
		rule<ScannerT, parser_context<>, parser_tag<variableID> > variable;
		rule<ScannerT, parser_context<>, parser_tag<termID> > term;
		rule<ScannerT, parser_context<>, parser_tag<infix_predID> > infix_pred;
		rule<ScannerT, parser_context<>, parser_tag<predicateID> > predicate;
		rule<ScannerT, parser_context<>, parser_tag<atomID> > atom;
		rule<ScannerT, parser_context<>, parser_tag<literalID> > literal;
		rule<ScannerT, parser_context<>, parser_tag<body_literalID> > body_literal;
		rule<ScannerT, parser_context<>, parser_tag<headID> > head;
		rule<ScannerT, parser_context<>, parser_tag<bodyID> > body;
		rule<ScannerT, parser_context<>, parser_tag<constraintID> > constraint;
		rule<ScannerT, parser_context<>, parser_tag<factID> > fact;
		rule<ScannerT, parser_context<>, parser_tag<ruleID> > prule;
		rule<ScannerT, parser_context<>, parser_tag<disjunctID> > disjunct;
		rule<ScannerT, parser_context<>, parser_tag<symbolID> > symbol;
		rule<ScannerT, parser_context<>, parser_tag<stringconstID> > stringconst;
		rule<ScannerT, parser_context<>, parser_tag<integerID> > integer;
		rule<ScannerT, parser_context<>, parser_tag<lineID> > line;
		rule<ScannerT, parser_context<>, parser_tag<lineID> > const&
				start() const { return line; }
	};

};

#endif 
//End

