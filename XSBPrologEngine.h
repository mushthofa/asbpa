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
 * @file XSBPrologEngine.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief XSB Prolog Engine interface class
 *
 *
 */

#ifndef _XSBPROLOGENGINE_H
#define _XSBPROLOGENGINE_H



#include "PrologEngine.h"

/* Include from XSB's library */
extern "C"
{
#include "cinterf.h"
#include "context.h"
#include "thread_xsb.h"
}


class XSBPrologThread : public PrologThread
{
	public:
		
		XSBPrologThread(th_context* );
		virtual ~XSBPrologThread();
		
		/* Do assert */
		virtual void doAssert(const std::string&);
		
		/* Do retract */
		virtual void doRetract(const std::string&);
				
		/* Load a program */
		virtual void consult(const std::string&);
			
		/* Open a query to Prolog */
		virtual void openQuery(const std::string& );
			
		/* Is query open? */
		virtual bool isQueryOpen() const
		{
			return query_opened;
		}
			
		/* Get the next answer from the last query of the thread */
		virtual void getNextAnswer(std::string& );
		
		
		/* Asserts the facts to the context thread */
		virtual void addFacts(const std::string& );
			
		/* Retract the thread from the context thread */
		virtual void delFacts(const std::string& );
		
		/* Declare some predicates as dynamic */
		virtual void declareDynamic(const std::set<PredicateSign>& );
		
		/* check whether a query is true */
		virtual bool checkQuery(const std::string& q);
		
		/* Close the currently open query */
		
		virtual void closeQuery();
		
	protected:
		th_context* th_ctxt;
		
		
};


class XSBPrologEngine: public PrologEngine
{
	public:
		XSBPrologEngine(const std::string& xsbpath);
		virtual ~XSBPrologEngine();
		
		/* Create XSB thread */
		XSBPrologThread* createThread() const;
		
	protected:
		
		static th_context* main_ctxt;

};

#endif
// end
