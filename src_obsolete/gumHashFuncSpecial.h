/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
 *   test $Id: $                                                           *
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
/** @file
 * @brief Classes providing hash functions for some useful keys
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_HASH_FUNC_SPECIAL_H
#define GUM_HASH_FUNC_SPECIAL_H
#include <agrum/gumHashFunc.h>
#include <agrum/gumGraphElements.h>

template <> class gumHashFunc<gumEdge> : public gumHashFuncBase<gumEdge> {
	public:
		/**
		* @throw gumHashSize
		*/
		void resize( gumSize );
		gumSize operator()( const gumEdge& key ) const ;
	private:
		mutable std::pair<gumNodeId,gumNodeId> pair;
		gumHashFunc< std::pair<gumNodeId,gumNodeId> > real_func;
};

template <> class gumHashFunc<gumArc> : public gumHashFuncBase<gumArc> {
	public:
		/**
		* @throw gumHashSize
		*/
		void resize( gumSize );
		gumSize operator()( const gumArc& key ) const ;
	private:
		mutable std::pair<gumNodeId,gumNodeId> pair;
		gumHashFunc< std::pair<gumNodeId,gumNodeId> > real_func;
};

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include "gumHashFuncSpecial.inl"
#endif /* GUM_NO_INLINE */
#endif /* GUM_HASH_FUNC_SPECIAL_H */
