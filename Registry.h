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
 * 	@brief The Registry class is a sort of mediator that inserts objects into factory classes.
 * 	@author Roman Schindlauer, Mushthofa		
 */


/* This code is mostly unchanged from DLVHEX */

#ifndef _REGISTRY_H
#define _REGISTRY_H


#include "Atom.h"
#include "AtomFactory.h"


class Registry
{
	public:

    		/**
	 * @brief Get (unique) instance of the static registry class.
		 */
		static Registry* Instance();

    		/**
		 * @brief Stores an Atom.
		 *
		 * Using boost::shared_ptr, the ownership over a is transferred to the
		 * shared pointer. The pointer Atom* must not be deleted after this call.
		 * This method is supposed to be used for storing ground atoms.
		 * The Atom is stored in the singleton instance of AtomFactory, which
		 * maintains a set of Atoms, taking care of uniqueness of its members and
		 * therefore optimal memory management. 
		 */
		AtomPtr storeAtom(Atom*);

	protected:

		Registry()
		{ };

	private:

		static Registry* _instance;
};

#endif /* _REGISTRY_H */

// end

