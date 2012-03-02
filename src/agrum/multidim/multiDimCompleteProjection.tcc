/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief A generic class to project efficiently a MultiDim table over all
 * of its variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <limits>
#include <agrum/config.h>


namespace gum {


  /// constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCompleteProjection<T_DATA,TABLE>::MultiDimCompleteProjection
  ( T_DATA ( *proj )( const TABLE<T_DATA>&, Instantiation* ) ) :
  _proj( proj ) {
    /// for debugging purposes
    GUM_CONSTRUCTOR( MultiDimCompleteProjection );
  }


  /// copy constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCompleteProjection<T_DATA,TABLE>::MultiDimCompleteProjection
  ( const MultiDimCompleteProjection<T_DATA,TABLE>& from ) :
    _proj( from._proj ) {
    /// for debugging purposes
    GUM_CONS_CPY( MultiDimCompleteProjection );
  }

  
  /// destructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCompleteProjection<T_DATA,TABLE>::~MultiDimCompleteProjection() {
    /// for debugging purposes
    GUM_DESTRUCTOR( MultiDimCompleteProjection );
  }
  

  /// virtual constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimCompleteProjection<T_DATA,TABLE>*
  MultiDimCompleteProjection<T_DATA,TABLE>::newFactory() const {
    return new MultiDimCompleteProjection<T_DATA,TABLE> ( *this );
  }


  /// creates and returns the projection of the table over a subset of its vars
  template< typename T_DATA, template<typename> class TABLE >
  INLINE T_DATA
  MultiDimCompleteProjection<T_DATA,TABLE>::project
  ( const TABLE<T_DATA>& table, Instantiation* inst ) {
    return _proj( table, inst );
  }


  /// changes the function used for projecting TABLES
  template< typename T_DATA, template<typename> class TABLE >
  void MultiDimCompleteProjection<T_DATA,TABLE>::setProjectFunction
  ( T_DATA ( *proj ) ( const TABLE<T_DATA>&, Instantiation* ) ) {
    _proj = proj;
  }


  /// returns the projection function currently used by the projector
  template< typename T_DATA, template<typename> class TABLE >
  INLINE T_DATA
  ( * MultiDimCompleteProjection<T_DATA,TABLE>::projectFunction() )
  ( const TABLE<T_DATA>&, Instantiation* ) const {
    return _proj;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
