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
 *	@brief BoostHCFDetector.h
 * 	Performing HCF detection using  BOOST Graph Lib
 *
 *	@author Mushthofa
 */


#ifndef BOOST_HCF_DETECTOR_H
#define BOOST_HCF_DETECTOR_H

#include "HCFDetector.h"

class BoostHCFDetector: public HCFDetector
{
	public:
		BoostHCFDetector();
		
		virtual bool isHCF(Component* comp);
		
	protected:
		
		void makeEdges(const std::vector<AtomNodePtr>&, Edges&) const;

		void selectNodes(const Vertices&, 
					  const std::vector<AtomNodePtr>&,
	 				  std::vector<AtomNodePtr>&) const;
		
		void findCycles(const std::vector<AtomNodePtr>& nodes,
					 std::vector<std::vector<AtomNodePtr> >& sccs);
		
		bool findHeadCycle(std::vector<AtomNodePtr>&) const;
};


#endif
//End

