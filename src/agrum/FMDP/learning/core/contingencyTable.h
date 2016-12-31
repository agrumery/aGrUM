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
 * @brief Headers of the ContingencyTable class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


#ifndef GUM_CONTINGENCY_TABLE_H
#define GUM_CONTINGENCY_TABLE_H

#include <cmath>

#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>

#include <agrum/FMDP/learning/observation.h>

#include <agrum/variables/discreteVariable.h>


namespace gum {

  /**
   * @class ContingencyTable contingencyTable.h
   * <agrum/FMDP/learning/core/contingencyTable.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template <typename GUM_SCALAR_A, typename GUM_SCALAR_B>
  class ContingencyTable {

    public:
    // ##########################################################################
    /// @name Constructor & destructor.
    // ##########################################################################
    /// @{


    /// Default constructor

    ContingencyTable();


    /// Default destructor

    ~ContingencyTable();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( ContingencyTable ) );
    }

    /// @}


    /// @name

    /// @{


    /// Increments the number of sample for case( iattr, ivalue )

    void add( GUM_SCALAR_A valueA, GUM_SCALAR_B valueB );


    /// Returns the number of samples for case (iattr, ivalue)

    Idx joint( GUM_SCALAR_A valueA, GUM_SCALAR_B valueB ) const {
      return __jointTable.exists(
                 std::pair<GUM_SCALAR_A, GUM_SCALAR_B>( valueA, valueB ) )
                 ? __jointTable[std::pair<GUM_SCALAR_A, GUM_SCALAR_B>( valueA,
                                                                       valueB )]
                 : 0;
    }


    /// Returns the number of samples for case (iattr, ivalue)

    Idx attrAMarginal( GUM_SCALAR_A valueA ) const {
      return __attrAMarginalTable.exists( valueA ) ? __attrAMarginalTable[valueA]
                                                   : 0;
    }


    /// Returns the number of samples for case (iattr, ivalue)

    Idx attrBMarginal( GUM_SCALAR_B valueB ) const {
      return __attrAMarginalTable.exists( valueB ) ? __attrAMarginalTable[valueB]
                                                   : 0;
    }


    /// Returns the number of samples for line iattr

    //        Idx aMarginal( GUM_SCALAR_A iattr ) { return
    //        __attrMarginalTable[iattr]; }
    HashTableConstIteratorSafe<GUM_SCALAR_A, Idx> attrABeginSafe() const {
      return __attrAMarginalTable.cbeginSafe();
    }
    HashTableConstIteratorSafe<GUM_SCALAR_A, Idx> attrAEndSafe() const {
      return __attrAMarginalTable.cendSafe();
    }


    /// Returns the number of samples for column ivalue

    //        Idx vMarginal( GUM_SCALAR_B ivalue ) { return
    //        __valueMarginalTable[ivalue]; }
    HashTableConstIteratorSafe<GUM_SCALAR_B, Idx> attrBBeginSafe() const {
      return __attrBMarginalTable.cbeginSafe();
    }
    HashTableConstIteratorSafe<GUM_SCALAR_B, Idx> attrBEndSafe() const {
      return __attrBMarginalTable.cendSafe();
    }


    /// Returns the number of samples for line iattr

    Idx attrASize() const { return __attrAMarginalTable.size(); }


    /// Returns the number of samples for column ivalue

    Idx attrBSize() const { return __attrBMarginalTable.size(); }

    /// @}

    ContingencyTable<GUM_SCALAR_A, GUM_SCALAR_B>&
    operator+=( const ContingencyTable<GUM_SCALAR_A, GUM_SCALAR_B>& src );

    std::string toString() const {
      std::stringstream ss;
      ss << "\t\t\t\t" << __attrAMarginalTable << std::endl
         << "\t\t\t\t" << __attrBMarginalTable << std::endl
         << "\t\t\t\t" << __jointTable << std::endl;
      return ss.str();
    }

    private:
    /**
     * The contingency table used to compute the GStat
     * Left Idx is for the attribute
     * Right Idx for the value
     *
     * NB: This is a silly and in a hurry implementation of contingency table
     * If someone ever use this class and has time to correctly implements
     * a efficient contingency table, you're welcome
     */
    HashTable<std::pair<GUM_SCALAR_A, GUM_SCALAR_B>, Idx> __jointTable;
    HashTable<GUM_SCALAR_A, Idx> __attrAMarginalTable;
    HashTable<GUM_SCALAR_B, Idx> __attrBMarginalTable;
  };

} /* namespace gum */

#include <agrum/FMDP/learning/core/contingencyTable_tpl.h>
#endif  // GUM_CONTINGENCY_TABLE_H
