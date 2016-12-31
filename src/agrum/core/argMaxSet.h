/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES *
 *   {prenom.nom}_at_lip6.fr *
 *                                                                               *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 2 of the License, or *
 *   (at your option) any later version. *
 *                                                                               *
 *   This program is distributed in the hope that it will be useful, *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the *
 *   GNU General Public License for more details. *
 *                                                                               *
 *   You should have received a copy of the GNU General Public License *
 *   along with this program; if not, write to the *
 *   Free Software Foundation, Inc., *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
 ********************************************************************************/
/**
* @file
* @brief This files contains several function objects that are not (yet) defined
*        in the STL
*
* Generically, function objects are instances of a class with member function
* operator() defined.
* This member function allows the object to be used with the same syntax as a
* function call.
*
* @author Jean-Christophe MAGNAN
*/

// =========================================================================
#ifndef GUM_ARG_MAX_SET_H
#define GUM_ARG_MAX_SET_H
// =========================================================================
#include <cstdlib>
// =========================================================================
#include <agrum/core/sequence.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================

namespace gum {

  /**
   * @struct ArgMaxSet argMaxSet.h <agrum/multidim/patterns/argMaxSet.h>
   * @brief A class containing the set of object assoicated to a maximum
   * @ingroup core
   *
   * This set contains the ids of the obj
   *
   */
  /// Class to handle efficiently argMaxSet
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  class ArgMaxSet {

    public:
    // ###########################################################################
    /// @name CNL
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    ArgMaxSet();

    // ============================================================================
    /// Constructor
    // ============================================================================
    ArgMaxSet( const GUM_SCALAR_VAL& val, const GUM_SCALAR_SEQ& elem );

    // ============================================================================
    /// Copy Constructor
    // ============================================================================
    ArgMaxSet( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& src );

    ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>&
    operator=( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& src );

    // ============================================================================
    /// Destructor
    // ============================================================================
    ~ArgMaxSet();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate(
          p, sizeof( ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ> ) );
    }

    /// @}

    // ###########################################################################
    /// @name Iterators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Iterator beginning
    // ============================================================================
    SequenceIteratorSafe<GUM_SCALAR_SEQ> beginSafe() const {
      return __argMaxSeq->beginSafe();
    }

    // ============================================================================
    /// Iterator end
    // ============================================================================
    SequenceIteratorSafe<GUM_SCALAR_SEQ> endSafe() const {
      return __argMaxSeq->endSafe();
    }

    /// @}

    // ###########################################################################
    /// @name Operators
    // ###########################################################################
    /// @{

    // ============================================================================
    /// Ajout d'un élément
    // ============================================================================
    ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>&
    operator+=( const GUM_SCALAR_SEQ& elem );

    // ============================================================================
    /// Use to insert the content of another set inside this one
    // ============================================================================
    ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>&
    operator+=( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& src );

    // ============================================================================
    /// Gives the ith element
    // ============================================================================
    const GUM_SCALAR_SEQ& operator[]( const Idx i ) const {
      return __argMaxSeq->atPos( i );
    }

    // ============================================================================
    /// Compares two ArgMaxSet to check if they are equals
    // ============================================================================
    bool
    operator==( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& compared ) const;
    bool
    operator!=( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& compared ) const {
      return !( *this == compared );
    }

    // ============================================================================
    /// Checks if val is lower or higher from the compared ArgMaxSet val
    // ============================================================================
    bool
    operator<( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& compared ) const {
      return __val < compared.value() ? true : false;
    }
    bool
    operator>( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& compared ) const {
      return compared < *this;
    }
    bool
    operator<=( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& compared ) const {
      return !( *this > compared );
    }
    bool
    operator>=( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& compared ) const {
      return !( *this < compared );
    }

    /// @}

    // ============================================================================
    /// Gives the size
    // ============================================================================
    Idx size() const { return __argMaxSeq->size(); }

    // ============================================================================
    /// Returns the value on which comparison are made
    // ============================================================================
    const GUM_SCALAR_VAL& value() const { return __val; }

    bool exists( const GUM_SCALAR_SEQ& elem ) const {
      return __argMaxSeq->exists( elem );
    }

    private:
    /// The very bone of the ArgMaxSet
    Sequence<GUM_SCALAR_SEQ>* __argMaxSeq;
    GUM_SCALAR_VAL            __val;

    public:
    friend std::ostream& operator<<( std::ostream&    streamy,
                                     const ArgMaxSet& objy ) {
      streamy << "Value : " << objy.value()
              << " - Set : " << objy.__argMaxSeq->toString();
      return streamy;
    }
  };

}  // End of namespace gum

#include <agrum/core/argMaxSet_tpl.h>

#endif /* GUM_ARG_MAX_SET_H */
