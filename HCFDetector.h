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
 *	@brief HCFDetector.h
 * 	Provides a class to perform HCF test to input programs (contained in a Component object)
 *
 *	@author Mushthofa
 */

#ifndef _HCF_DETECTOR_H
#define _HCF_DETECTOR_H

#include "Component.h"


class HCFDetector
{
	public:
		
		HCFDetector()
		:ishcf(true)
		{}
		
		virtual bool isHCF(Component* ) = 0;
		
		
	protected:
		/**
	 * Single Vertex Type.
		 */
		typedef unsigned Vertex;

    		/**
		 * List of Vertices.
		 */
		typedef std::vector<Vertex> Vertices;

    		/**
		 * An Edge is a pair of Vertices.
		 */
		typedef std::pair<Vertex, Vertex> Edge;

    		/**
		 * List of Edges.
		 */
		typedef std::vector<Edge> Edges;
		bool ishcf;

};

#endif
// End

