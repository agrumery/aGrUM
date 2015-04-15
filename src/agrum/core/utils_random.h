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
#ifndef GUM_UTILS_RANDOM_H
#define GUM_UTILS_RANDOM_H

#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <chrono>

namespace gum {

  /// returns a random number between 0 and 1 included (i.e. a proba)
  double randomProba();

  /// return a random discrete distribution
  /// @param n is the number of modalities for the ditribution
  template <typename GUM_SCALAR> std::vector<GUM_SCALAR> randomDistribution(Size n);

  /// initialize random generator seed
  void initRandom(unsigned int init = 0);

  /// returns the aGrUM's seed used by the std::generators
  unsigned int &randomGeneratorSeed(unsigned int seed = 0);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/core/utils_random.inl>
#endif // GUM_NO_INLINE

#include <agrum/core/utils_random.tcc>

#endif /* GUM_UTILS_RANDOM_H */
