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
 * @brief MethodSelector.h
 * @author Mushthofa
 *
 * CLass providing a generic heuristic for selecting evaluation method, and a simple 
 * concrete implementation 
 *
 */

#ifndef _METHOD_SELECTOR_H
#define _METHOD_SELECTOR_H

#include "Component.h"
#include "HCFDetector.h"

class MethodSelector
{
	public:
		typedef enum 
		{EMPTY, DIRECT, METHOD_NORMAL1, METHOD_NORMAL2, METHOD_DISJ}
		Method_t;
		
		MethodSelector(HCFDetector* h)
		:hcfd(h)
		{}
		
		static std::string getMethodName(Method_t m)
		/* Verbose print */
		{	
			std::string out;
			switch(m)
			{
				case EMPTY:
					out = "Empty program evaluation ";
					break;
				case DIRECT:
					out = "Direct evaluation by external ASP solver";
					break;
				
				case METHOD_NORMAL1:
					out = "Method 1 for hcf/normal program";
					break;
				
				case METHOD_NORMAL2:
					out = "Method 2 for hcf/normal program";
					break;
				
				case METHOD_DISJ:
					out = "Method for Disjunctive/non-HCF program";
					break;
				default:
				 	out = "Unknown method";
			}
			return out;
		}
		
		virtual Method_t getMethod(Component* ) = 0;
		
	protected:
		HCFDetector* hcfd;
		Component* component;
	
};

#endif
//End

