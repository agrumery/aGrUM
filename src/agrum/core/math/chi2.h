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
 * @brief The class that represents the chi2 distribution
 *
 * The Chi2 class allows to easily compute critical values for the Chi2
 * distribution.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_CHI2_H
#define GUM_LEARNING_CHI2_H

#include <cmath>

#include <agrum/config.h>
#include <agrum/core/hashTable.h>

#define GUM_LEARNING_CONFIDENCE_PROBA 0.05

namespace gum {

  // =========================================================================
  // ===                             CHI2 CLASS                            ===
  // =========================================================================

  /**
   * @class Chi2 chi2.h <agrum/core/math/chi2.h>
   * @brief Represent the chi2 distribution.
   * @ingroup math_group
   *
   * The class constructor receives a std::vector of unsigned integers. This
   * std::vector represents the variables modalities. The variables indexes in
   * the std::vector will be used as the variables identifiers.
   */
  class Chi2 {
    public:
    // ==========================================================================
    /// @name Constructors / Destructors
    // ==========================================================================
    /// @{

    /**
     * @brief Default constructor.
     * @param var_modalities The variables modalities.
     * @param confidence_proba The confidence probability.
     */
    Chi2( const std::vector<Idx>& var_modalities,
          double confidence_proba = GUM_LEARNING_CONFIDENCE_PROBA );

    /**
     * @brief Class destructor.
     */
    ~Chi2();

    /// @}
    // ==========================================================================
    /// @name Accessors / Modifiers
    // ==========================================================================
    /// @{

    /**
     * @brief Sets the conditioning nodes (useful for computing degrees of
     * freedom).
     *
     * @param db_conditioning_ids The conditioning nodes id.
     */
    void setConditioningNodes( const std::vector<Idx>& db_conditioning_ids );

    /**
    * @brief Computes the critical value according to the number of degrees of
    * freedom.
    * @param pair A pair of variables ids.
    * @return Returns the critical values.
    */
    double criticalValue( const std::pair<Idx, Idx>& pair );

    /**
     * @brief Computes the critical value according to the number of degrees of
     * freedom.
     * @param var1 The first variable id.
     * @param var2 The second variable id.
     * @return Returns the critical value.
     */
    double criticalValue( Idx var1, Idx var2 );

    /**
     * @brief Returns the number of degrees of freedom.
     * @param pair A pair of variables ids.
     * @return Returns the number of degrees of freedom.
     */
    Size degreesOfFreedom( const std::pair<Idx, Idx>& pair );

    /**
     * @brief Returns the number of degrees of freedom.
     * @param var1 The first variable id.
     * @param var2 The second variable id.
     * @return Returns the number of degrees of freedom.
     */
    Size degreesOfFreedom( Idx var1, Idx var2 );

    /**
     * @brief Modifies the confidence probability.
     * @param new_proba The new confidence probability
     */
    void setConfidenceProba( double new_proba );

    /// @}

    private:
    /// The modalities of the random variables.
    const std::vector<Idx>& __modalities;

    /// The confidence probability used for critical values.
    double __confidence_proba;

    /// The domain size of the conditioning nodes.
    Size __conditioning_size;

    /// A set of already computed critical values.
    HashTable<Idx, double> __critical_values;

    /**
     * @brief Computes the critical value of a given chi2 test (used by the
     * cache).
     *
     * This code has been written by Gary Perlman.
     *
     * @param proba The probability value.
     * @param df The number of degrees of freedom.
     * @return Returns the critical value of a given chi2 test.
     */
    static double __criticalValue( double proba, Size df );

    /**
     * @brief Computes the probability of chi2 value.
     *
     * This code has been written by Gary Perlman.
     *
     * ALGORITHM Compute probability of chi square value.
     * Adapted from:
     * Hill, I. D. and Pike, M. C.  Algorithm 299
     * Collected Algorithms for the CACM 1967 p. 243
     * Updated for rounding errors based on remark in
     * ACM TOMS June 1985, page 185
     *
     * @param x The chi2 value.
     * @param df The number of degrees of freedom.
     * @return The probability of x given df degrees of freedom.
     */
    static double __probaChi2( double x, Size df );

    /**
     * @brief Computes the probability of normal z value.
     *
     * This code has been written by Gary Perlman.
     *
     * ALGORITHM Adapted from a polynomial approximation in:
     * Ibbetson D, Algorithm 209
     * Collected Algorithms of the CACM 1963 p. 616
     *
     * This routine has six digit accuracy, so it is only useful for absolute z
     * values < 6.  For z values >= to 6.0, __probaZValue() returns 0.0.
     *
     * @param z A value.
     * @return The probability of z.
     */
    static double __probaZValue( double z );

    /// Forbid use of the copy constructor.
    Chi2( const Chi2& ) = delete;

    /// Forbid used of the copy operator.
    Chi2& operator=( const Chi2& ) = delete;
  };

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/math/chi2_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CHI2_H */
