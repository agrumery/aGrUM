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
 * @brief The class that represents the chi2 distribution
 *
 * The Chi2 class allows to easily compute critical values for the Chi2
 * distribution
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_CHI2_H
#define GUM_LEARNING_CHI2_H

#include <agrum/config.h>
#include <agrum/core/hashTable.h>

#define GUM_LEARNING_CONFIDENCE_PROBA 0.05

namespace gum {

  /* ========================================================================= */
  /* ===                             CHI2 CLASS                            === */
  /* ========================================================================= */
  class Chi2 {
    public:
    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    /// default constructor
    Chi2(const std::vector<unsigned int> &var_modalities,
         float confidence_proba = GUM_LEARNING_CONFIDENCE_PROBA);

    /// destructor
    ~Chi2();

    /// @}

    // ##########################################################################
    /// @name Accessors / Modifiers
    // ##########################################################################
    /// @{

    /// sets the conditioning nodes (useful for computing degrees of freedom)
    void setConditioningNodes(const std::vector<unsigned int> &db_conditioning_ids);

    /// computes the critical value according to the number of degrees of freedom
    float criticalValue(const std::pair<unsigned int, unsigned int> &pair);

    /// computes the critical value according to the number of degrees of freedom
    float criticalValue(unsigned int var1, unsigned int var2);

    /// returns the number of degrees of freedom
    unsigned long
    degreesOfFreedom(const std::pair<unsigned int, unsigned int> &pair);

    /// returns the number of degrees of freedom
    unsigned long degreesOfFreedom(unsigned int var1, unsigned int var2);

    /// modifies the confidence proba
    void setConfidenceProba(float new_proba);

    /// @}

    private:
    /// the modalities of the random variables
    const std::vector<unsigned int> &__modalities;

    /// the confidence probability used for critical values
    float __confidence_proba;

    /// the domain size of the conditioning nodes
    unsigned long __conditioning_size;

    /// a set of already computed critical values
    HashTable<unsigned int, float> __critical_values;

    /// computes the critical value of a given chi2 test (used by the cache)
    /** This code has been written by Gary Perlman */
    static double __criticalValue(double proba, unsigned long df);

    /// computes the probability of chi2 value
    /** This code has been written by Gary Perlman.
     * ALGORITHM Compute probability of chi square value.
     * Adapted from:
     * Hill, I. D. and Pike, M. C.  Algorithm 299
     * Collected Algorithms for the CACM 1967 p. 243
     * Updated for rounding errors based on remark in
     * ACM TOMS June 1985, page 185 */
    static double __probaChi2(double x, unsigned long df);

    /// computes the probability of normal z value
    /** This code has been written by Gary Perlman.
     * ALGORITHM Adapted from a polynomial approximation in:
     * Ibbetson D, Algorithm 209
     * Collected Algorithms of the CACM 1963 p. 616
     * Note:
     * This routine has six digit accuracy, so it is only useful for absolute
     * z values < 6.  For z values >= to 6.0, __probaZValue() returns 0.0. */
    static double __probaZValue(double z);

    /// prevent copy constructor
    Chi2(const Chi2 &) = delete;

    /// prevent copy operator
    Chi2 &operator=(const Chi2 &) = delete;
  };

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/math/chi2.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CHI2_H */
