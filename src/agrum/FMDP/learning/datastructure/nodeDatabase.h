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
 * @brief Headers of the NodeDatabase class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_NODE_DATABASE_H
#define GUM_NODE_DATABASE_H
// =========================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/core/testPolicy/Chi2TestPolicy.h>
#include <agrum/FMDP/learning/core/testPolicy/GTestPolicy.h>
#include <agrum/FMDP/learning/core/testPolicy/leastSquareTestPolicy.h>
#include <agrum/FMDP/learning/observation.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class NodeDatabase nodeDatabase.h
   * <agrum/FMDP/learning/datastructure/nodeDatabase.h>
   * @brief
   * @ingroup fmdp_group
   *
   */


  template <TESTNAME AttributeSelection, bool isScalar>
  class NodeDatabase {

    typedef typename ValueSelect<isScalar, double, Idx>::type ValueType;

    template <typename GUM_SCALAR>
    using TestPolicy =
        typename TestSelect<AttributeSelection,
                            GTestPolicy<GUM_SCALAR>,
                            Chi2TestPolicy<GUM_SCALAR>,
                            LeastSquareTestPolicy<GUM_SCALAR>>::type;

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    NodeDatabase( const Set<const DiscreteVariable*>*,
                  const DiscreteVariable* = nullptr );

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~NodeDatabase();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( NodeDatabase ) );
    }

    /// @}

    // ==========================================================================
    /// @name Observation handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    /** Updates database with new observation
     *
     * Calls either @fn __addObservation( const Observation*, Int2Type<true>)
     * or @fn __addObservation( const Observation*, Int2Type<false>)
     * depending on if we're learning reward function or transition probability
     **/
    // ###################################################################
    void addObservation( const Observation* );

    private:
    void __addObservation( const Observation*, Int2Type<true> );
    void __addObservation( const Observation*, Int2Type<false> );

    public:
    // ###################################################################
    /// Nb observation taken into account by this instance
    // ###################################################################
    INLINE Idx nbObservation() const { return __nbObservation; }

    /// @}

    // ==========================================================================
    /// @name Variable Test Methods
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Indicates wether or not, node has sufficient observation so that
    /// any statistic is relevant
    // ###################################################################
    INLINE bool isTestRelevant( const DiscreteVariable* var ) const {
      return __attrTable[var]->isTestRelevant();
    }

    // ###################################################################
    /// Returns the performance of given variables according to selection
    /// criterion
    // ###################################################################
    INLINE double testValue( const DiscreteVariable* var ) const {
      return __attrTable[var]->score();
    }

    // ###################################################################
    /// Returns the performance of given variables according to selection
    /// secondary criterion (to break ties)
    // ###################################################################
    INLINE double testOtherCriterion( const DiscreteVariable* var ) const {
      return __attrTable[var]->secondaryscore();
    }

    /// @}

    // ==========================================================================
    /// @name Aggregation Methods
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Merges given NodeDatabase informations into current nDB.
    // ###################################################################
    NodeDatabase<AttributeSelection, isScalar>&
    operator+=( const NodeDatabase<AttributeSelection, isScalar>& src );

    // ###################################################################
    /// Returns a reference to nDB test policy for given variable
    /// (so that test policy information can be merged too)
    // ###################################################################
    const TestPolicy<ValueType>* testPolicy( const DiscreteVariable* var ) const {
      return __attrTable[var];
    }

    // ###################################################################
    /// Iterators on value count to recopy correctly its content
    // ###################################################################
    const HashTableConstIteratorSafe<ValueType, Idx> cbeginValues() const {
      return __valueCount.cbeginSafe();
    }
    const HashTableConstIteratorSafe<ValueType, Idx> cendValues() const {
      return __valueCount.cendSafe();
    }

    /// @}

    // ###################################################################
    ///
    // ###################################################################

    Idx effectif( Idx moda ) const {
      return __valueCount.exists( moda ) ? __valueCount[moda] : 0;
    }

    Idx valueDomain() const { return __valueDomain( Int2Type<isScalar>() ); }

    private:
    Idx __valueDomain( Int2Type<true> ) const { return __valueCount.size(); }
    Idx __valueDomain( Int2Type<false> ) const { return __value->domainSize(); }

    std::string toString() const;

    private:
    /// Table giving for every variables its instantiation
    HashTable<const DiscreteVariable*, TestPolicy<ValueType>*> __attrTable;

    /// So does this reference on the value observed
    const DiscreteVariable* __value;

    ///
    Idx __nbObservation;

    ///
    HashTable<ValueType, Idx> __valueCount;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/nodeDatabase_tpl.h>

#endif  // GUM_NODE_DATABASE_H
