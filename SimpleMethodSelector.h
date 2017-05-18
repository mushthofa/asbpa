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
 * @brief SimpleMethodSelector.h
 * @author Mushthofa
 *
 * CLass providing a generic heuristic for selecting evaluation method, and a simple 
 * concrete implementation 
 *
 */


#ifndef SIMPLE_METHOD_SELECTOR_H
#define SIMPLE_METHOD_SELECTOR_H

#include "MethodSelector.h"




class SimpleMethodSelector: public MethodSelector
{
	public:
		SimpleMethodSelector(HCFDetector*);
		
		virtual Method_t getMethod(Component* );
		
	protected:
		

};

#endif
//End

