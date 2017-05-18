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
 * @file PrologEngine.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Prolog Engine base interface class
 *
 *
 */

#ifndef _PROLOGENGINE_H
#define _PROLOGENGINE_H

#include <vector>
#include <string>
#include "Program.h"

class PrologThread
{
	public:
		
		PrologThread()
		:answersleft(false), query_opened(false)
		{}
		
		virtual ~PrologThread ()
		{
		}
		
		/* Do assert */
		virtual void doAssert(const std::string&) = 0;
		
		/* Do retract */
		virtual void doRetract(const std::string&) = 0;
		
		
		/* Load a program */
		virtual void consult(const std::string&) = 0;
		
		/* Open a query to Prolog */
		virtual void openQuery(const std::string& ) = 0;
		
		/* Is query open? */
		virtual bool isQueryOpen() const = 0;
		
		/* Get the next answer from the last query of the thread */
		virtual void getNextAnswer(std::string& ) = 0;
		
		/* Still more answers remaining ? */
		bool answersLeft() const
		{
			return answersleft;
		}
		
		/* Asserts the facts to the context thread */
		virtual void addFacts(const std::string& ) = 0;
		
		/* Retract the thread from the context thread */
		virtual void delFacts(const std::string& ) = 0;
		
		/* Declare predicates as dynamic predicates */
		
		virtual void declareDynamic(const std::set<PredicateSign>& ) = 0;
		
		/* Check whether a ground query is true in the current database */
		
		virtual bool checkQuery(const std::string&) = 0;
		
		/* Close the currently open query */
		
		virtual void closeQuery() = 0;
		
	protected:
		bool answersleft;
		std::string curr_answer;
		std::string next_answer;
		bool ready;
		bool query_opened;
};


class PrologEngine
{
	public:
		PrologEngine()
		:initialised(false)
		{}
		
		virtual ~PrologEngine() {};
		/* Create a new thread */
		virtual PrologThread* createThread() const = 0;
		
		
	protected:
		bool initialised;
		
};

#endif
//End

