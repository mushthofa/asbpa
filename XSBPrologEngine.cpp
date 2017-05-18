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
 * @file XSBPrologEngine.cpp
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief XSB Prolog Engine interface class
 *
 *
 */

#include "XSBPrologEngine.h"
#include <sstream>
#include <cstring>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

typedef boost::tokenizer<boost::char_separator<char> > dotTokenizer_t;

/* Initialize static member */

th_context* XSBPrologEngine::main_ctxt = 0;


XSBPrologThread::XSBPrologThread(th_context* ctxt)
	:PrologThread(), th_ctxt(ctxt)
{
}

XSBPrologThread::~XSBPrologThread()
{
	//std::cout<<"Killing my self " << std::endl;
	xsb_kill_thread(th_ctxt);
}

void XSBPrologThread::doAssert(const std::string& argsterm)
{
	std::string term(argsterm);
	boost::trim(term);
	if(term.size()==0)
		throw FatalError("Trying to assert an empty term into Prolog");
	std::string assert_command = "assert((" + term + ")).";
	
	//std::cout<<assert_command<<std::endl;
	
	char* command = new char[assert_command.size() + 1];
	strcpy(command, assert_command.c_str());
	
	int retQ = xsb_command_string(th_ctxt, command );
	if(retQ == XSB_ERROR || retQ == XSB_FAILURE)
	{
		std::ostringstream os;
		os << "XSB Error in asserting: " <<term<<":"<<xsb_get_error_message(th_ctxt)<<std::endl;
		throw FatalError(os.str());
	}
	delete [] command;
}

void XSBPrologThread::doRetract(const std::string& term)
{
	
	if(term.size()==0)
		throw FatalError("Trying to retract an empty term into Prolog");
	
	std::string assert_command = "retract((" + term + ")).";
	
	//std::cout<<assert_command<<std::endl;
	
	char* command = new char[assert_command.size() + 1];
	strcpy(command, assert_command.c_str());
		
	
	int retQ = xsb_command_string(th_ctxt, command );
	if(retQ == XSB_ERROR ) //|| retQ == XSB_FAILURE)
	{
		std::ostringstream os;
		os << "XSB Error in retracting: " <<term<<":"<<xsb_get_error_message(th_ctxt)<<std::endl;
		throw FatalError(os.str());
	}
	delete [] command;
}

void XSBPrologThread::consult(const std::string& clauses)
{
	//std::cout<<"Consulting "<<std::endl<<clauses<<std::endl;
	boost::char_separator<char> dotSep (".\n");
	dotTokenizer_t tokens(clauses, dotSep);
	dotTokenizer_t::iterator t_it;
	for (t_it = tokens.begin(); t_it != tokens.end(); ++t_it)
	{
		doAssert(*t_it);
	}
	//std::cout<<"Done"<<std::endl;
}

void XSBPrologThread::addFacts(const std::string& facts)
{
	consult(facts);
}

void XSBPrologThread::delFacts(const std::string& facts)
{
	boost::char_separator<char> dotSep (".\n");
	dotTokenizer_t tokens(facts, dotSep);
	dotTokenizer_t::iterator t_it;
	for (t_it = tokens.begin(); t_it != tokens.end(); ++t_it)
	{
		doRetract(*t_it);
	}
}

void XSBPrologThread::declareDynamic(const std::set<PredicateSign>& ps)
{
	//std::cout<<"Declaring dynamics "<<std::endl;
	
	std::set<PredicateSign>::const_iterator p_it;
	for(p_it = ps.begin(); p_it != ps.end(); ++p_it)
	{
		std::ostringstream dynamic_cmd;
		dynamic_cmd <<"dynamic(" << p_it->name << "/" << p_it->arity << "), ";
		dynamic_cmd <<"dynamic(m_NEG_"<<p_it->name<<"/"<<p_it->arity<<"). ";
// 		dynamic_cmd <<"dynamic(" << p_it->name << "_m_PRIME " << "/" <<p_it->arity <<"), ";
// 		dynamic_cmd <<"dynamic(m_NEG_" << p_it->name << "_m_PRIME " << "/" <<p_it->arity <<"). ";
		char* command = new char[dynamic_cmd.str().size() + 1];
		strcpy(command, dynamic_cmd.str().c_str());
		
		//std::cout<<p_it->name<<"/"<<p_it->arity<<", " <<std::endl;
		
		if(xsb_command_string(th_ctxt, command) == XSB_ERROR)
		{
			std::ostringstream os;
			os << "XSB Error in declaring dynamic: " <<p_it->name<<"/"<<p_it->arity
					<<":"<<xsb_get_error_message(th_ctxt)<<std::endl;
			throw FatalError(os.str());
		}
		delete [] command;	
	}
	
	//std::cout<<" "<<std::endl;
}

bool XSBPrologThread::checkQuery(const std::string& qstr)
{
	if(query_opened)
		xsb_close_query(th_ctxt);
	
	XSB_StrDefine(return_str);
	int retQuery;
	char *queryStr = new char[qstr.size()+1];
	strcpy(queryStr, qstr.c_str());
	retQuery = xsb_query_string_string(th_ctxt, queryStr, &return_str, (char*)"|");
	delete[] queryStr;
	bool result = false;
	if(retQuery == XSB_SUCCESS)
	{
		result = true;
		xsb_close_query(th_ctxt);
		query_opened = false;
	}
	else if(retQuery == XSB_FAILURE)
	{
		result = false;
	}
	else if(retQuery == XSB_ERROR)
	{
		std::ostringstream os;
		os<<"XSB Error: cannot send query "<<qstr<<std::endl<<": "<<xsb_get_error_message(th_ctxt)<<std::endl;
		throw FatalError(os.str());
	}
	return result;
}

void XSBPrologThread::openQuery(const std::string& qstr)
{
	xsb_close_query(th_ctxt);
	
	XSB_StrDefine(return_str);
	int retQuery;
	char *queryStr = new char[qstr.size()+1];
	strcpy(queryStr, qstr.c_str());
	
	retQuery = xsb_query_string_string(th_ctxt, queryStr, &return_str, (char*)"|");
	
	delete[] queryStr;
	if(retQuery == XSB_SUCCESS)
	{
		answersleft = true;
		curr_answer = (std::string) return_str.string;
	}
	else if(retQuery == XSB_FAILURE)
	{
		answersleft = false;
	}
	else if(retQuery == XSB_ERROR)
	{
		answersleft = false;
		std::ostringstream os;
		os<<"XSB Error: cannot send query "<<qstr<<std::endl<<": "<<xsb_get_error_message(th_ctxt)<<std::endl;
		throw FatalError(os.str());
	}
	
	query_opened = true;
}

void XSBPrologThread::getNextAnswer(std::string& answer)
{
	if(!query_opened || !answersleft)
		throw FatalError("XSB Error: no more answers available!");
	XSB_StrDefine(return_str);
	answer = curr_answer;
	int retQuery;
	retQuery = xsb_next_string(th_ctxt, &return_str, (char*)"|");
	if(retQuery == XSB_SUCCESS)
	{
		curr_answer = (std::string) return_str.string;
		answersleft = true;
	}
	else
	{
		answersleft = false;
		query_opened = false;
	}

}

void XSBPrologThread::closeQuery()
{
	if(query_opened)
		xsb_close_query(th_ctxt);
}

XSBPrologEngine::XSBPrologEngine(const std::string& xsbpath)
	:PrologEngine()
{
	int myargc = 4;
	char* myargv[4];
	
	myargv[0] = new char [xsbpath.size()+1];
	strcpy(myargv[0], xsbpath.c_str());
	
	myargv[1] = (char*)"-n";
	myargv[2] = (char*)"--nobanner";
	myargv[3] = (char*)"--quietload";
	
	int retInit = xsb_init(myargc, myargv);
	
	if(retInit == XSB_ERROR)
	{
		std::ostringstream os;
		os<<"Error initialising XSB : "<<xsb_get_init_error_message()<<std::endl;
		throw FatalError(os.str());
	}
	else
		initialised = true;
	delete [] myargv[0];
	
	main_ctxt = xsb_get_main_thread();
}

XSBPrologEngine::~XSBPrologEngine()
{
	//std::cout <<"Closing XSB ... " <<std::endl;
	xsb_close(main_ctxt);
}

XSBPrologThread* XSBPrologEngine::createThread() const
{
	if(main_ctxt)
	{
		th_context* new_ctxt;
		xsb_ccall_thread_create(main_ctxt, &new_ctxt);
		return new XSBPrologThread(new_ctxt);
		//return new_thread;
	}
	else
		throw FatalError("XSB Prolog Engine is not initialised yet, cannot create a new Prolog thread!");
}

//End
