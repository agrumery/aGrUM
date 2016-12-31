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
 * @brief Headers of the LeastSquareTestPolicy
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_LEAST_SQUARE_TEST_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_LEAST_SQUARE_TEST_POLICY_H

// ============================================================================
#include <agrum/FMDP/learning/core/contingencyTable.h>
#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>
// ============================================================================
#include <agrum/multidim/FunctionGraphUtilities/link.h>
// ============================================================================

namespace gum {

  /**
   * @class LeastSquareTestPolicy leastSquareTestPolicy.h
   * <agrum/multidim/core/testPolicy/leastSquareTestPolicy.h>
   *
   * @brief LeastSquareTestPolicy implements a test policy that follows the
   * Least Square statistic
   *
   * @ingroup fmdp_group
   */
  template <typename GUM_SCALAR>
  class LeastSquareTestPolicy : public ITestPolicy<GUM_SCALAR> {

    public:
    // ############################################################################
    /// @name Constructor/Destrcutor
    // ############################################################################
    /// @{

    // ============================================================================
    /// Constructor
    // ============================================================================
    LeastSquareTestPolicy()
        : ITestPolicy<GUM_SCALAR>()
        , __sumO( 0.0 )
        , __score( 0 ) {
      GUM_CONSTRUCTOR( LeastSquareTestPolicy )
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate(
          p, sizeof( LeastSquareTestPolicy ) );
    }

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~LeastSquareTestPolicy();

    // ############################################################################
    /// @name Observation insertion
    // ############################################################################
    /// @{

    // ============================================================================
    /// Comptabilizes the new observation
    // ============================================================================
    void addObservation( Idx attr, GUM_SCALAR value );

    /// @}


    // ############################################################################
    /// @name Test relevance
    // ############################################################################
    /// @{

    // ============================================================================
    /// Returns true if enough observation were made so that the test can be
    /// relevant
    // ============================================================================
    bool isTestRelevant() const { return ( this->nbObservation() > 20 ); }

    /// @}


    // ############################################################################
    /// @name Test result
    // ############################################################################
    /// @{

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    void computeScore() const;

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    double score() const;

    // ============================================================================
    /// Returns a second criterion to severe ties
    // ============================================================================
    double secondaryscore() const;

    /// @}


    // ############################################################################
    /// @name Fusion Methods
    // ############################################################################
    /// @{

    // ============================================================================
    /// Performs the merging of current LeastSquareTestPolicy instance with
    /// given instance
    // ============================================================================
    void add( const LeastSquareTestPolicy<GUM_SCALAR>& src );


    // ============================================================================
    /// Returns global sum (needed for the merging)
    // ============================================================================
    double sumValue() const { return __sumO; }

    // ============================================================================
    /// Returns nbobs per modality table (needed for the merging)
    // ============================================================================
    const HashTable<Idx, Idx>& nbObsTable() const { return __nbObsTable; }

    // ============================================================================
    /// Returns sum per modality table (needed for the merging)
    // ============================================================================
    const HashTable<Idx, double>& sumAttrTable() const { return __sumAttrTable; }

    // ============================================================================
    /// Returns global sum (needed for the merging)
    // ============================================================================
    const HashTable<Idx, LinkedList<double>*>& obsTable() const {
      return __obsTable;
    }

    private:
    /// Global sum
    double __sumO;

    /// Nb Observation for each modality assumed by variable
    HashTable<Idx, Idx> __nbObsTable;

    /// Sum for each modality assumed by variable
    HashTable<Idx, double> __sumAttrTable;

    /// Not sure if needed
    HashTable<Idx, LinkedList<double>*> __obsTable;

    /// Keeping computed score
    double __score;
  };

}  // End of namespace gum

#include <agrum/FMDP/learning/core/testPolicy/leastSquareTestPolicy_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_LEAST_SQUARE_TEST_POLICY_H */
