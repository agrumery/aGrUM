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
/**
 * @file
 * @brief Headers of the MDDOperatorStrategy planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_ACTION_SET_H
#define GUM_ACTION_SET_H
// =========================================================================
#include <thread>
// =========================================================================
#include <agrum/core/argMaxSet.h>
#include <agrum/core/functors.h>
#include <agrum/core/inline.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================
#include <agrum/FMDP/fmdp.h>
// =========================================================================

namespace gum {

  /**
   * @struct ArgumentMaximisesAction actionSet.h
   * <agrum/FMDP/planning/actionSet.h>
   * @brief Argument Maximization function object class
   * @ingroup core
   *
   * Returns the set that has the maximal value between its two arguments sets
   */
  template <typename GUM_SCALAR>
  struct ArgumentMaximisesAction {

    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    const GUM_SCALAR& operator()( const GUM_SCALAR& x,
                                  const GUM_SCALAR& y ) const {

      if ( x > y ) {
        return x;
      }
      if ( x < y ) {
        return y;
      }

      __temp = x;
      __temp += y;
      return __temp;
    }

    private:
    mutable GUM_SCALAR __temp;
  };


  /**
   * @class ActionSet actionSet.h <agrum/FMDP/planning/actionSet.h>
   * @brief A class to store the optimal actions.
   * @ingroup fmdp_group
   *
   * Stores the ids of optimal actions. To be used as leaves on optimal policy
   * tree or function graph
   *
   */
  class ActionSet {

    public:
    // ###########################################################################
    /// @name CNL
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    ActionSet() {
      GUM_CONSTRUCTOR( ActionSet )
      __actionSeq = new Sequence<Idx>();
    }

    ActionSet( const ActionSet& src ) {
      GUM_CONSTRUCTOR( ActionSet )
      __actionSeq = new Sequence<Idx>();
      for ( auto idi = src.beginSafe(); idi != src.endSafe(); ++idi )
        __actionSeq->insert( *idi );
    }

    ActionSet& operator=( const ActionSet& src ) {
      __actionSeq = new Sequence<Idx>();
      for ( auto idi = src.beginSafe(); idi != src.endSafe(); ++idi )
        __actionSeq->insert( *idi );
      return *this;
    }

    // ============================================================================
    /// Destructor
    // ============================================================================
    ~ActionSet() {
      GUM_DESTRUCTOR( ActionSet )
      delete __actionSeq;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( ActionSet ) );
    }

    /// @}

    // ###########################################################################
    /// @name Iterators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Iterator beginning
    // ============================================================================
    SequenceIteratorSafe<Idx> beginSafe() const {
      return __actionSeq->beginSafe();
    }

    // ============================================================================
    /// Iterator end
    // ============================================================================
    SequenceIteratorSafe<Idx> endSafe() const { return __actionSeq->endSafe(); }

    /// @}

    // ###########################################################################
    /// @name Operators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Ajout d'un élément
    // ============================================================================
    ActionSet& operator+=( const Idx& elem ) {
      __actionSeq->insert( elem );
      return *this;
    }

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ActionSet& operator+=( const ActionSet& src ) {
      for ( auto iter = src.beginSafe(); iter != src.endSafe(); ++iter )
        if ( !__actionSeq->exists( *iter ) ) __actionSeq->insert( *iter );
      return *this;
    }

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ActionSet& operator-=( const ActionSet& src ) {
      for ( auto iter = src.beginSafe(); iter != src.endSafe(); ++iter )
        if ( __actionSeq->exists( *iter ) ) __actionSeq->erase( *iter );
      return *this;
    }

    // ============================================================================
    /// Gives the ith element
    // ============================================================================
    const Idx& operator[]( const Idx i ) const { return __actionSeq->atPos( i ); }

    // ============================================================================
    /// Compares two ActionSet to check if they are equals
    // ============================================================================
    bool operator==( const ActionSet& compared ) const {
      for ( auto iter = compared.beginSafe(); iter != compared.endSafe(); ++iter )
        if ( !__actionSeq->exists( *iter ) ) return false;
      for ( auto iter = this->beginSafe(); iter != this->endSafe(); ++iter )
        if ( !compared.exists( *iter ) ) return false;
      return true;
    }
    bool operator!=( const ActionSet& compared ) const {
      return !( *this == compared );
    }

    /// @}

    // ============================================================================
    /// Gives the size
    // ============================================================================
    Size size() const { return __actionSeq->size(); }

    bool exists( const Idx& elem ) const { return __actionSeq->exists( elem ); }

    private:
    /// The very bone of the ActionSet
    Sequence<Idx>* __actionSeq;

    friend std::ostream& operator<<( std::ostream&    streamy,
                                     const ActionSet& objy ) {
      streamy << objy.__actionSeq->toString();
      return streamy;
    }
  };
}
#endif  // GUM_ACTION_SET_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
