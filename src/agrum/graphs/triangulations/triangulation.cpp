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
 * @brief Abstract base class for computing triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <cmath>

#include <agrum/config.h>
#include <agrum/graphs/triangulations/triangulation.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/triangulations/triangulation_inl.h>
#endif  // GUM_NO_INLINE

namespace gum {

  // constructor
  Triangulation::Triangulation() {
    // for debugging purposes
    GUM_CONSTRUCTOR( Triangulation );
  }

  // constructor with a domain size specified
  Triangulation::Triangulation( const NodeProperty<Size>* domsizes )
      : _domain_sizes( domsizes ) {
    GUM_CONSTRUCTOR( Triangulation );
  }

  // destructor
  Triangulation::~Triangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR( Triangulation );
  }

  // copy constructor
  Triangulation::Triangulation( const Triangulation& from )
      : _domain_sizes( from._domain_sizes ) {
    GUM_CONS_CPY( Triangulation );
  }

  // move constructor
  Triangulation::Triangulation( Triangulation&& from )
      : _domain_sizes( from._domain_sizes ) {
    GUM_CONS_MOV( Triangulation );
  }

  // returns the max of log10DomainSize of cliques in the junction tree
  double Triangulation::maxLog10CliqueDomainSize() {
    double              res = 0.0;
    double              dSize;
    const JunctionTree& jt = junctionTree();  // here, the fact that we get
    // a junction tree ensures that _domain_sizes is different from nullptr

    for ( const NodeId cl : jt ) {
      dSize = 0.0;

      for ( const auto node : jt.clique( cl ) )
        dSize += std::log10( ( *_domain_sizes )[node] );

      if ( res < dSize ) res = dSize;
    }

    return res;
  }

} /* namespace gum */
