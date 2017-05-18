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
 * @file ProgramParser.h
 * @author Mushthofa
 * @date Tue 24 03 2009
 *
 * @brief Base class for program parsers
 *
 *
 */

#ifndef _PROGRAMPARSER_H
#define _PROGRAMPARSER_H

#include <iostream>
#include <fstream>
#include "Program.h"
#include "Globals.h"
#include "AtomSet.h"

class ProgramParser
{
	public:
		ProgramParser()
		{
			filename = Globals::Instance()->getFileName();
			
			if(!Globals::Instance()->boolOption("fromSTDIN"))
			{
				in_fs.open(filename.c_str(), std::ifstream::in);
				if(!in_fs.is_open())
				{
					std::ostringstream oserr;
					oserr<<"Cannot open file "<<filename<<std::endl;
					throw FatalError(oserr.str());
				}
		
				buf = in_fs.rdbuf();
				std::cin.rdbuf(buf);
			}
			else
				std::cout<<"Reading from <STDIN>:"<<std::endl<<std::endl;
		}
		~ProgramParser() 
		{
			if(in_fs.is_open())
				in_fs.close();
			
		}
		
		Program getIDB() 
		{
			return IDB;
		}
		AtomSet getEDB()
		{
			return EDB;
		}
	protected:
		std::string filename;
		std::ifstream in_fs;
		std::streambuf *buf;
		
		Program IDB;
		AtomSet EDB;
		
};


#endif
//End
