/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it udnder the terms of the GNU General Public License as published by *
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
 * @brief Contains usefull methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_UTILS_RANDOM_H
#define GUM_UTILS_RANDOM_H

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <agrum/config.h>

namespace gum {

  /// @ingroup utilities_group
  /// @{

  /**
   * @brief Returns a random number between 0 and 1 included (i.e. a proba).
   * @return Returns a random number between 0 and 1 included (i.e. a proba).
   */
  double randomProba();

  /**
   * @brief Return a random discrete distribution.
   * @param n The number of modalities for the ditribution.
   * @return Return a random discrete distribution.
   * @tparam GUM_SCALAR The type used for representing probabilities.
   */
  template <typename GUM_SCALAR>
  std::vector<GUM_SCALAR> randomDistribution( Size n );

  /**
   * @brief Returns the aGrUM's seed used by the std::generators.
   * @return Returns the aGrUM's seed used by the std::generators.
   */
  unsigned int randomGeneratorSeed();

  /**
   * @brief Initialize random generator seed.
   */
  void initRandom( unsigned int seed = 0 );

  /**
   * define a random_engine with correct seed
   */
  std::default_random_engine getRandomGenerator( unsigned int seed = 0 );

  /// @}
} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/core/utils_random_inl.h>
#endif  // GUM_NO_INLINE

// always include the template implementations
#include <agrum/core/utils_random_tpl.h>

#endif /* GUM_UTILS_RANDOM_H */
