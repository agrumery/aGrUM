/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
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
/** @file
 * @brief Outlined implementation of directed and undirected edges
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <iostream>
#include <sstream>
#include <agrum/core/exceptions.h>
#include <agrum/graphs/graphElements.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/graphElements.inl>
#endif /* GUM_NO_INLINE */


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  // ==============================================================================
  /// A \c << operator for edges
  // ==============================================================================
  std::ostream& operator<< ( std::ostream& stream, const Edge& edge ) {
    return ( stream << edge.first()<<"--"<<edge.second() );
  }

  // ==============================================================================
  /// A \c << operator for arcs
  // ==============================================================================
  std::ostream& operator<< ( std::ostream& stream, const Arc& arc ) {
    return ( stream << arc.first()<<"->"<<arc.second() );
  }

  // ==============================================================================
  /// A \c << operator for basic edges
  // ==============================================================================
  std::ostream& operator<< ( std::ostream& stream, const Link& edge ) {
    return ( stream << edge.first()<<"~~"<<edge.second() );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
