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
 * @file Bitmap.h
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Generating subsets of a set represented using bit map (boolean array) 
 *
 *
 */
 
#ifndef BITMAP_H
#define BITMAP_H

#include "Error.h"

class BitMap
{
	public:
		BitMap(unsigned n);
		~BitMap() ;
		bool* getNextMask();
		bool subsetsLeft() const
		{
			return !overflow;
		}
	private:
		unsigned n;
		bool* bit;
		bool add_one();
		bool overflow;
};

			
#endif
// End


