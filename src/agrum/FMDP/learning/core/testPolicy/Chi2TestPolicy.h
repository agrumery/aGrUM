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
 * @brief Headers of the Chi2TestPolicy
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_CHI2_TEST_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_CHI2_TEST_POLICY_H

// ============================================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/core/contingencyTable.h>
#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>
// ============================================================================

namespace gum {

  /**
   * @class Chi2TestPolicy Chi2TestPolicy.h
   * <agrum/multidim/core/testPolicy/Chi2TestPolicy.h>
   *
   * @brief Chi2TestPolicy implements a test policy that follows the Chi2
   * statistic
   *
   * @ingroup fmdp_group
   */
  template <typename GUM_SCALAR>
  class Chi2TestPolicy : public ITestPolicy<GUM_SCALAR> {

    public:
    Chi2TestPolicy()
        : ITestPolicy<GUM_SCALAR>()
        , __conTab()
        , __chi2Score( 0 ) {
      GUM_CONSTRUCTOR( Chi2TestPolicy )
    }

    virtual ~Chi2TestPolicy() { GUM_DESTRUCTOR( Chi2TestPolicy ) }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( Chi2TestPolicy ) );
    }

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
    bool isTestRelevant() const {
      return ( this->nbObservation() > 20 &&
               this->nbObservation() > __conTab.attrASize() * 5 );
    }

    /// @}


    // ############################################################################
    /// @name Test result
    // ############################################################################
    /// @{

    // ============================================================================
    /// Recomputes the statistic from the beginning
    // ============================================================================
    void computeScore() const;

    // ============================================================================
    /// Returns the performance of current variable according to the test
    // ============================================================================
    double score() const;

    // ============================================================================
    /// Returns a second criterion to severe ties
    // ============================================================================
    virtual double secondaryscore() const;

    /// @}

    const ContingencyTable<Idx, GUM_SCALAR>& ct() const { return __conTab; }

    void add( const Chi2TestPolicy<GUM_SCALAR>& src );

    std::string toString() const {
      std::stringstream ss;
      ss << ITestPolicy<GUM_SCALAR>::toString()
         << "\t\t\tContingency Table : " << std::endl
         << __conTab.toString() << std::endl
         << "\t\t\tGStat : " << this->score() << std::endl
         << "\t\t\tGStat : " << this->secondaryscore() << std::endl;
      return ss.str();
    }

    private:
    /// The contingency table used to keeps records of all observation
    ContingencyTable<Idx, GUM_SCALAR> __conTab;

    mutable double __chi2Score;
  };

}  // End of namespace gum

#include <agrum/FMDP/learning/core/testPolicy/Chi2TestPolicy_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_CHI2_TEST_POLICY_H */
