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
 * @brief Inlined implementation of the hash functions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
// ================================================================================
/// returns a hashed key for hash tables the keys of which are represented
/// by a gumEdge
// ================================================================================
INLINE gumSize
gumHashFunc<gumEdge>::operator()( const gumEdge& key ) const {
	gumNodeId first=key.first();gumNodeId second=key.second();
	bool b=first<second;
	pair.first=b?first:second;
	pair.second=b?second:first;
	return real_func.operator()( pair );
}

// ================================================================================
/// update the hash function to take into account a resize of the hash table
// ================================================================================
INLINE void
gumHashFunc<gumEdge>::resize( gumSize new_size ) {
	gumHashFuncBase<gumEdge>::resize( new_size );
	real_func.resize( new_size );
}

// ================================================================================
/// returns a hashed key for hash tables the keys of which are represented
/// by a gumArc
// ================================================================================
INLINE gumSize
gumHashFunc<gumArc>::operator()( const gumArc& key ) const {
	pair.first  = key.first();
	pair.second = key.second();
	return real_func.operator()( pair );
}

// ================================================================================
/// update the hash function to take into account a resize of the hash table
// ================================================================================
INLINE void
gumHashFunc<gumArc>::resize( gumSize new_size ) {
	gumHashFuncBase<gumArc>::resize( new_size );
	real_func.resize( new_size );
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
