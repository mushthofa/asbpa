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
 * @file   Globals.h
 * @author Roman Schindlauer, Mushthofa
 * 
 * @brief  Global parameter class
 * 
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <boost/algorithm/string/trim.hpp>
#include "Globals.h"


Globals* Globals::_instance = 0;

Globals::Globals()
{
	//put default options here
	
	//
	strOpt["DLVPath"] = DLVPATH;
	
	intOpt["EvalMethod"] = 4;
	
	intOpt["maxNumGround"] = 10000;
	
	intOpt["verboseLevel"] = 0;
	
	intOpt["minRuleVar"] = 4;
	
	intOpt["smallConstraintMaxVar"] = 10000;
	
	intOpt["smallRuleMaxVar"] = 10000;
	//strOpt["DLVArgs"] = "-silent --";
}

Globals* Globals::Instance()
{
	if(_instance == 0)
		_instance = new Globals;
	
	return _instance;
}

void Globals::processArgs(GetPot cl)
{
	/* Get the options from the command line */
	bool showHelp = cl.search("--help") || cl.options_contain("h");
	
	std::string dlvpath =  cl("--dlv", DLVPATH);
	
	const int evalmethod = cl("-m", 4);
	const int maxNumAS = cl("-n", 0);
	const int maxNumGround = cl("-gr", 10);
	
	const int smallConstraintMaxVar = cl("-scv", 10);
	
	const int smallRuleMaxVar = cl("-grmax", 10);

	const int minRuleVar = cl("-vmax", 4);
	
	bool noscc = cl.search("-no-scc");
	
	bool ruleml = cl.search("-ruleml");
	
	const bool verbose_f = cl.options_contain("vV");
	
	std::string filter = cl("-filter", "");
	
	
	/* Save the options in the internal records */
	
	strOpt["DLVPath"] = dlvpath;
	intOpt["EvalMethod"] = evalmethod;
	intOpt["maxNumAS"] = maxNumAS;
	//std::cout << maxNumAS;
	intOpt["maxNumGround"] = maxNumGround * 1000;
	intOpt["smallConstraintMaxVar"] = smallConstraintMaxVar * 1000;
	intOpt["smallRuleMaxVar"] = smallRuleMaxVar  * 1000;
	intOpt["minRuleVar"] = minRuleVar;
	
	if(intOpt["maxNumAS"] > 0)
	{
		boolOpt["limitAS"] = true;
	}
	else
		boolOpt["limitAS"] = false;
	
	
	
	boolOpt["no-scc"] = noscc;
	
	boolOpt["ruleml"] = ruleml;
	
	if(verbose_f)
		intOpt["verboseLevel"] = 1;
	
	strOpt["filter"] = filter;
	
	/* Determine the input file */
	
	std::string f (cl.next_nominus());
	boost::trim(f);
	if(f.size()>0)
	{
		filename = f;
		boolOpt["fromSTDIN"] = false;
	}
	else
	{
		filename = "<STDIN>"; 
		boolOpt["fromSTDIN"] = true;
	}
	
}

bool Globals::boolOption(const std::string& o) 
{
	return boolOpt[o];
}

unsigned long Globals::intOption(const std::string& o) 
{
	return intOpt[o]; 
}

std::string Globals::strOption(const std::string& o) 
{
	return strOpt[o];
}

void Globals::setOption(const std::string& key, bool val)
{
	boolOpt[key] = val;
}

void Globals::setOption(const std::string& key, unsigned val)
{
	intOpt[key] = val;
}

void Globals::setOption(const std::string& key, const std::string& val)
{
	strOpt[key] = val;
}

const std::string& Globals::getFileName() 
{
	return filename;
}

void Globals::setFileName(const std::string& f)
{
	filename = f;
}
// End

