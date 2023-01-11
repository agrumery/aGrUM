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


#ifndef GUM_UTILS_PRM_H
#define GUM_UTILS_PRM_H

#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/tools/multidim/aggregators/multiDimAggregator.h>
#include <agrum/tools/multidim/implementations/multiDimBijArray.h>
#include <agrum/tools/multidim/implementations/multiDimBucket.h>
#include <agrum/tools/multidim/implementations/multiDimSparse.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/tools/multidim/utils/operators/projections4MultiDim.h>

#include <agrum/PRM/elements/PRMObject.h>

namespace gum {
  namespace prm {

    /// PRMType for real numbers
    using prm_float = float;

    /// Decompose a string in a vector of strings using "." as separators.
    void decomposePath(const std::string& path, std::vector< std::string >& v);

    /**
     * @brief Returns a copy of a Potential after applying a bijection over the
     *variables in source.
     * This copies the Potential source in a new Potential by permuting all
     *variables in source with respect to bij.
     *
     * @warning This method in most case creates the new Potential using a
     *gum::MultiDimBijArray, this means that the created Potential holds a
     *reference over source, so do not delete source if you still need the created
     *potential.
     *
     * @param bij A Bijection of DiscreteVariable where firsts are variables in
     *source and seconds variables added in the returned Potential.
     * @param source The copied Potential.
     * @return a pointer over a Potential which is a copy of source.
     * @throw FatalError raised if an unknown MultiDimImplementation is
     *encountered.
     */
    template < typename GUM_SCALAR >
    Potential< GUM_SCALAR >*
       copyPotential(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
                     const Potential< GUM_SCALAR >&                                       source);

    template < typename GUM_SCALAR >
    Potential< GUM_SCALAR > multPotential(const Potential< GUM_SCALAR >& t1,
                                          const Potential< GUM_SCALAR >& t2);
    /**
     * @brief Proceeds with the elimination of var in pool.
     * @param var The variable eliminated from every potentials in pool.
     * @param pool A pool of potentials in which the elimination of var is done.
     * @param trash All create potentials are inserted in this set, useful to
     *              delete later.
     */
    template < typename GUM_SCALAR >
    void eliminateNode(const DiscreteVariable*          var,
                       Set< Potential< GUM_SCALAR >* >& pool,
                       Set< Potential< GUM_SCALAR >* >& trash);

    template < typename GUM_SCALAR >
    void eliminateNodes(const std::vector< const DiscreteVariable* >& elim_order,
                        Set< Potential< GUM_SCALAR >* >&              pool,
                        Set< Potential< GUM_SCALAR >* >&              trash);

    /**
     * @brief Returns the next value of an unique counter for PRM's node id.
     * @return Returns the next value of an unique counter for PRM's node id.
     */
    NodeId nextNodeId();

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/utils_prm_tpl.h>

#endif   // GUM_UTILS_PRM_H
