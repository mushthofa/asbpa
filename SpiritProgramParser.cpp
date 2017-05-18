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



#include "SpiritProgramParser.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>

//#include <boost/spirit/tree/tree_to_xml.hpp>

SpiritProgramParser::SpiritProgramParser()
	:ProgramParser()
{
	unsigned line_id = 1;
	std::string line_str;
	
	// read line from std input until a dot 
	
	while(std::getline(std::cin, line_str, '.'))
	{
		boost::trim(line_str);
		line_str = ignore_comments(line_str);
		boost::trim(line_str);
		if(line_str.size()==0)
			continue;
		
		logic_grammar lg;
		tree_parse_info<> parse_tree = ast_parse(line_str.c_str(), lg, space_p);
		
		if(parse_tree.full)
		{
			bool safe = true;
			tree_it tree_begin = parse_tree.trees.begin();
			boost::spirit::parser_id node_id = tree_begin->value.id();
			if(node_id == logic_grammar::literalID
						|| node_id == logic_grammar::predicateID)
			{
				AtomPtr fact (new Atom (parseAtom(tree_begin)));
				SafetyChecker* sc = new SafetyChecker(*fact);
				if(!sc->checkSafe())
					safe = false;
				else
					EDB.insert(fact);
				delete sc;
			}
			else if(node_id == logic_grammar::disjunctID)
			{
				curVarID = 0;
				varID.clear();
				//tree_it head_it = tree_begin->children.begin();
				RuleHead_t head = parseHead(tree_begin);
				if(head.size() > 1)
				{
					RulePtr prule (new Rule(head, RuleBody_t(), filename, line_id) );
					
					SafetyChecker* sc = new SafetyChecker(*prule);
					if(!sc->checkSafe())
					{
						safe = false;
					}
					delete sc;
					
					IDB.addRule(prule);
				}
				else
				{
					/* Only one head literal and no body literals so it is actually a fact */
					AtomPtr fact = *(head.begin());
					
					SafetyChecker* sc = new SafetyChecker(*fact);
					if(!sc->checkSafe())
						safe = false;
					else
						EDB.insert(fact);
					delete sc;
					
					
				}
				line_id++;
			}
			else if(node_id == logic_grammar::constraintID)
			{
				curVarID = 0;
				varID.clear();
				tree_it body_it = tree_begin->children.begin();
				RulePtr prule(new HardConstraint(parseBody(body_it), 
						    filename, line_id)
						   );
				SafetyChecker* sc = new SafetyChecker(*prule);
				if(!sc->checkSafe())
				{
					safe = false;
				}
				delete sc;
				
				IDB.addRule(prule);
				line_id++;
			}
			else if(node_id == logic_grammar::ruleID)
			{
				curVarID = 0;
				varID.clear();
				tree_it child_it = tree_begin->children.begin();
				RuleHead_t head;
				head = parseHead(child_it);
				++ child_it;
				
				RuleBody_t body;
				body = parseBody(child_it);
				
				RulePtr prule(new Rule(head, body, 
						    filename, line_id));
				SafetyChecker* sc = new SafetyChecker(*prule);
				if(!sc->checkSafe())
				{
					safe = false;
					/*
					std::cerr<<"Rule not safe, line "<<line_id<<"."<<std::endl;
					std::cerr<<*prule<<std::endl;
					assert(0);
					*/
				}
				delete sc;
				IDB.addRule(prule);
				line_id++;
			}
			else
			{
				throw SyntaxError("Unknown node_id: ", line_id, filename);
			}
			
			
			if(!safe)
				throw SyntaxError("Rule/fact not safe", line_id, filename);
			/*
			std::map<parser_id, std::string> rule_names;
			rule_names[logic_grammar::constantID] = "constant";
			rule_names[logic_grammar::anonID] = "anonymous";
			rule_names[logic_grammar::variableID] = "variable";
			rule_names[logic_grammar::termID] = "term";
			rule_names[logic_grammar::infix_predID] = "infix predicate";
			rule_names[logic_grammar::predicateID] = "predicate";
			rule_names[logic_grammar::atomID] = "atom";
			rule_names[logic_grammar::literalID] = "literal";
			rule_names[logic_grammar::body_literalID] = "body literals";
			rule_names[logic_grammar::headID] = "head";
			rule_names[logic_grammar::bodyID] = "body";
			rule_names[logic_grammar::constraintID] = "constraint";
			rule_names[logic_grammar::factID] = "fact";
			rule_names[logic_grammar::ruleID] = "rule";
			rule_names[logic_grammar::disjunctID] = "disjunction";
			rule_names[logic_grammar::symbolID] = "symbol";
			rule_names[logic_grammar::stringconstID] = "stringconst";
			rule_names[logic_grammar::integerID] = "integer";
			rule_names[logic_grammar::lineID] = "line";
			tree_to_xml(std::cout, parse_tree.trees, line_str.c_str(), rule_names);
			*/
		}
		else
		{
			std::ostringstream oserr;
			oserr<<"\n>Here ==> "<<parse_tree.stop<<std::endl;
			throw SyntaxError(oserr.str(), line_id, filename);
		}
	}
}

Term SpiritProgramParser::parseTerm(const tree_it& node)
{
	boost::spirit::parser_id node_id = node->value.id();
	std::string termstr (node->value.begin(), node->value.end());
	boost::trim(termstr);
	if(node_id==logic_grammar::symbolID)
	{
		return Term(termstr, Term::SYMBOL);
	}
	if(node_id==logic_grammar::stringconstID)
	{
		return Term(termstr, Term::STRING);
	}
	if(node_id==logic_grammar::integerID)
	{
		return Term(termstr, Term::INTEGER);
	}
	if(node_id==logic_grammar::variableID)
	{
		/* Standardize apart */
		std::ostringstream varName;
		std::map<std::string, unsigned>::iterator it = varID.find(termstr);
		if(it == varID.end())
		{
			//std::cout<<"creating a new variable ";
			varID[termstr] = curVarID++;
		}
		
		varName << "VAR_" << varID[termstr];
		
		return Term(varName.str(), Term::VARIABLE);
	}
	if(node_id==logic_grammar::anonID)
	{
		return Term();
	}

}

Atom SpiritProgramParser::parseAtom(const tree_it& node)
{
	boost::spirit::parser_id node_id = node->value.id();
	bool strongneg = false;
	tree_it args_node, args_it;
	if(node_id==logic_grammar::literalID)
	{
		strongneg = true;
		args_node = node->children.begin();
	}
	else
		args_node = node;
	std::string predname (args_node->value.begin(), args_node->value.end());
	boost::trim(predname);
	Tuple args;
	
	/* Predicate name must be a symbol */
	args.push_back(Term(predname, Term::SYMBOL));
	
	/* Go through all argument nodes */
	
	for(args_it = args_node->children.begin(); 
		   args_it!=args_node->children.end(); 
		   ++args_it)
	{
		args.push_back(parseTerm(args_it));
	}
	
	return Atom(args, strongneg);
}

InfixAtom SpiritProgramParser::parseInfixAtom(const tree_it& node)
{
	boost::spirit::parser_id node_id = node->value.id();
	std::string opname (node->value.begin(), node->value.end());
	boost::trim(opname);
	
	tree_it child_it = node->children.begin();
	Term t1 = parseTerm(child_it);
	++ child_it;
	Term t2 = parseTerm(child_it);
	return InfixAtom(opname, t1, t2);
}

RuleHead_t SpiritProgramParser::parseHead(const tree_it& node)
{
	boost::spirit::parser_id node_id = node->value.id();
	if(node_id == logic_grammar::predicateID 
		  || node_id == logic_grammar::literalID)
	{
		RuleHead_t head;
		AtomPtr head_atom (new Atom (parseAtom(node)));
		head.insert(head_atom);
		return head;
	}
	else if(node_id == logic_grammar::disjunctID)
	{
		tree_it child_it = node->children.begin();
		RuleHead_t head1 = parseHead(child_it);
		++child_it;
		RuleHead_t head2 = parseHead(child_it);
		RuleHead_t head;
		std::set_union(head1.begin(), head1.end(), 
					head2.begin(), head2.end(),
							  inserter(head, head.begin()));
		return head;
		
	}
}

RuleBody_t SpiritProgramParser::parseBody(const tree_it& node)
{
	boost::spirit::parser_id node_id = node->value.id();
	if(node_id == logic_grammar::predicateID
		  || node_id == logic_grammar::literalID)
	{
		RuleBody_t body;
		AtomPtr atom (new Atom(parseAtom(node)));
		LiteralPtr literal (new Literal(atom));
		body.insert(literal);
		return body;
	}
	else if(node_id == logic_grammar::infix_predID)
	{
		RuleBody_t body;
		AtomPtr atom (new InfixAtom(parseInfixAtom(node)));
		LiteralPtr literal (new Literal(atom));
		body.insert(literal);
		return body;
	}
	else if(node_id == logic_grammar::body_literalID)
	{
		RuleBody_t body;
		tree_it child_it = node->children.begin();
		AtomPtr atom (new Atom(parseAtom(child_it)));
		LiteralPtr literal (new Literal(atom, true));
		body.insert(literal);
		return body;	
	}
	else if(node_id == logic_grammar::bodyID)
	{
		RuleBody_t body;
		tree_it child_it;
		for(child_it=node->children.begin(); 
				  child_it!=node->children.end(); ++child_it)
		{
			RuleBody_t tempbody = parseBody(child_it);
			std::set_union(body.begin(), body.end(),
						tempbody.begin(), tempbody.end(),
									inserter(body, body.begin())
					    );
		}
		return body;
	}
}

AtomSet SpiritProgramParser::getEDB()
{
	return ProgramParser::getEDB();
}

Program SpiritProgramParser::getIDB()
{
	return ProgramParser::getIDB();
}


std::string ignore_comments(const std::string& input)
{
	std::string::size_type index1, index2, length;
	std::string temp;
	index1 = input.find('%', 0);
	if(index1==std::string::npos)
		return input;
	else
	{
		index2 = input.find('\n',index1);
		if(index2==std::string::npos)
			return input.substr(0, index1);
		else
		{
			length = index2 - index1;
			temp = input;
			temp.erase(index1, length);
			return ignore_comments(temp);
		}
	}
}

// End

