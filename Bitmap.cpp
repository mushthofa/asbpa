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
 * @file Bitmap.cpp
 * @author Mushthofa
 * @date Tue 24 06 2009
 *
 * @brief Generating subsets of a set represented using bit map (boolean array) 
 *
 *
 */
 
#include "Bitmap.h"

BitMap::BitMap(unsigned number)
	:n(number), overflow(false)
{
	bit = new bool[n];
	for(unsigned i=0;i<n;i++)
		bit[i]=false;
	if(n==0)
		overflow = true;
}

BitMap::~BitMap()
{
	delete [] bit;
}


bool BitMap::add_one()
{
	int idx = n-1;
	while(idx>=0 && bit[idx])
	{
		bit[idx] = false;
		idx--;
	}
	if(idx<0)
		return true;
	bit[idx] = true;
	return false;
}

bool* BitMap::getNextMask()
{
	if(!overflow)
	{
		bool* result = bit;
		overflow = add_one();
		return result;
	}
	else
	{
		throw GeneralError("getNextMask called while no more subsets left");
	}
}

//

