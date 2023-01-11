/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief the type of algorithm to use to perform relevant reasoning in
 * Bayes net inference
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_RELEVANT_POTENTIAL_FINDER_TYPE_H
#define GUM_RELEVANT_POTENTIAL_FINDER_TYPE_H

namespace gum {


  /** @brief type of algorithm for determining the relevant potentials for
   * combinations using some d-separation analysis
   *
   * When constructing messages from one clique to its neighbor, we can
   * exploit d-separation to determine that some potentials are irrelevant for
   * the message computation. So we can discard them and, thereby, speed-up
   * the computations.
   */
  enum class RelevantPotentialsFinderType {
    FIND_ALL,                    // do not perform d-separation analysis
    DSEP_BAYESBALL_NODES,        // BayesBall requisite nodes -> potentials
    DSEP_BAYESBALL_POTENTIALS,   // BayesBall requisite potentials (directly)
    DSEP_KOLLER_FRIEDMAN_2009    // Koller & Friedman 2009 requisite potentials
  };

}   // namespace gum


#endif   // GUM_RELEVANT_POTENTIAL_FINDER_TYPE_H
