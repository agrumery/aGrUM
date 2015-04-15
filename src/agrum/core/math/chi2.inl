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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// sets the conditioning nodes (useful for computing degrees of freedom)
  INLINE void
  Chi2::setConditioningNodes(const std::vector<unsigned int> &db_conditioning_ids) {
    __conditioning_size = 1;
    for (unsigned int i = 0; i < db_conditioning_ids.size(); ++i) {
      __conditioning_size *= __modalities[db_conditioning_ids[i]];
    }
  }

  /// returns the number of degrees of freedom
  INLINE unsigned long
  Chi2::degreesOfFreedom(const std::pair<unsigned int, unsigned int> &pair) {
    return (__conditioning_size * (__modalities[pair.first] - 1) *
            (__modalities[pair.second] - 1));
  }

  /// returns the number of degrees of freedom
  INLINE unsigned long Chi2::degreesOfFreedom(unsigned int var1, unsigned int var2) {
    return (__conditioning_size * (__modalities[var1] - 1) *
            (__modalities[var2] - 1));
  }

  /// computes the critical value according to the number of degrees of freedom
  ALWAYS_INLINE float
  Chi2::criticalValue(const std::pair<unsigned int, unsigned int> &pair) {
    unsigned long DF = degreesOfFreedom(pair);

    // try to see if the threshold is not already in cache
    try {
      return __critical_values[DF];
    } catch (const Exception &) {
      // here we have to compute the threshold of the chi2
      // we use Gary Perlman's algorithm
      float value = __criticalValue(__confidence_proba, DF);
      __critical_values.insert(DF, value);
      return value;
    }
  }

  /// computes the critical value according to the number of degrees of freedom
  ALWAYS_INLINE float Chi2::criticalValue(unsigned int var1, unsigned int var2) {
    unsigned long DF = degreesOfFreedom(var1, var2);

    // try to see if the threshold is not already in cache
    try {
      return __critical_values[DF];
    } catch (const Exception &) {
      // here we have to compute the threshold of the chi2
      // we use Gary Perlman's algorithm
      float value = __criticalValue(__confidence_proba, DF);
      __critical_values.insert(DF, value);
      return value;
    }
  }

  /// modifies the confidence proba
  INLINE void Chi2::setConfidenceProba(float new_proba) {
    // if we did not change the confidence proba, do nothing
    if (__confidence_proba == new_proba)
      return;

    __confidence_proba = new_proba;

    // remove the currently stored critical values
    __critical_values.clear();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
