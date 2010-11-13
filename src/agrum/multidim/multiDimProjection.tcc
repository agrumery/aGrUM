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
 * @brief A generic interface to project efficiently a MultiDim table over a subset
 * of its variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>


namespace gum {

  
  /// constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimProjection<T_DATA,TABLE>::MultiDimProjection
  (TABLE<T_DATA>* (*proj) ( const TABLE<T_DATA>&,
                            const Set<const DiscreteVariable*>& ) ) :
    _proj ( proj ) {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( MultiDimProjection );
  }

  
  /// copy constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimProjection<T_DATA,TABLE>::MultiDimProjection
  ( const MultiDimProjection<T_DATA,TABLE>& from ) {
    /// for debugging purposes
    GUM_CONS_CPY ( MultiDimProjection );
  }

  
  /// destructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimProjection<T_DATA,TABLE>::~MultiDimProjection () {
    /// for debugging purposes
    GUM_DESTRUCTOR ( MultiDimProjection );
  }


  /// creates and returns the projection of the table over a subset of its vars
  template< typename T_DATA, template<typename> class TABLE >
  INLINE TABLE<T_DATA>*
  MultiDimProjection<T_DATA,TABLE>::project
  ( const TABLE<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars ) {
    return _proj ( table, del_vars );
  }


  /// creates and returns the projection of the table over a subset of its vars
  template< typename T_DATA, template<typename> class TABLE >
  INLINE void
  MultiDimProjection<T_DATA,TABLE>::project
  ( TABLE<T_DATA>& container,
    const TABLE<T_DATA>& table,
    const Set<const TABLE<T_DATA>*>& del_vars ) {
    TABLE<T_DATA>*  res = project ( table, del_vars );
    container = *res;
    delete res;
  }


  /// changes the function used for projecting TABLES
  template< typename T_DATA, template<typename> class TABLE >
  void MultiDimProjection<T_DATA,TABLE>::setProjector
  ( TABLE<T_DATA>*
    (*proj)
    ( const TABLE<T_DATA>&,
      const Set<const DiscreteVariable*>& ) ) {
    _proj = proj;
  }
    

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template< typename T_DATA, template<typename> class TABLE >
  float MultiDimProjection<T_DATA,TABLE>::nbOperations
  ( const TABLE<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars ) {
    return table.domainSize ();
  }


  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
