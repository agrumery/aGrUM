/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef GUM_UTILS_PRM_H
#define GUM_UTILS_PRM_H

#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/multidim/aggregators/multiDimAggregator.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/base/multidim/implementations/multiDimBijArray.h>
#include <agrum/base/multidim/implementations/multiDimBucket.h>
#include <agrum/base/multidim/implementations/multiDimSparse.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/base/multidim/utils/operators/projections4MultiDim.h>
#include <agrum/PRM/elements/PRMObject.h>

namespace gum {
  namespace prm {

    /// PRMType for real numbers
    using prm_float = float;

    /// Decompose a string in a vector of strings using "." as separators.
    void decomposePath(const std::string& path, std::vector< std::string >& v);

    /**
     * @brief Returns a copy of a Tensor after applying a bijection over the
     *variables in source.
     * This copies the Tensor source in a new Tensor by permuting all
     *variables in source with respect to bij.
     *
     * @warning This method in most case creates the new Tensor using a
     *gum::MultiDimBijArray, this means that the created Tensor holds a
     *reference over source, so do not delete source if you still need the created
     *tensor.
     *
     * @param bij A Bijection of DiscreteVariable where firsts are variables in
     *source and seconds variables added in the returned Tensor.
     * @param source The copied Tensor.
     * @return a pointer over a Tensor which is a copy of source.
     * @throw FatalError raised if an unknown MultiDimImplementation is
     *encountered.
     */
    template < typename GUM_SCALAR >
    Tensor< GUM_SCALAR >*
        copyTensor(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
                      const Tensor< GUM_SCALAR >&                                       source);

    template < typename GUM_SCALAR >
    Tensor< GUM_SCALAR > multTensor(const Tensor< GUM_SCALAR >& t1,
                                          const Tensor< GUM_SCALAR >& t2);
    /**
     * @brief Proceeds with the elimination of var in pool.
     * @param var The variable eliminated from every tensors in pool.
     * @param pool A pool of tensors in which the elimination of var is done.
     * @param trash All create tensors are inserted in this set, useful to
     *              delete later.
     */
    template < typename GUM_SCALAR >
    void eliminateNode(const DiscreteVariable*          var,
                       Set< Tensor< GUM_SCALAR >* >& pool,
                       Set< Tensor< GUM_SCALAR >* >& trash);

    template < typename GUM_SCALAR >
    void eliminateNodes(const std::vector< const DiscreteVariable* >& elim_order,
                        Set< Tensor< GUM_SCALAR >* >&              pool,
                        Set< Tensor< GUM_SCALAR >* >&              trash);

    /**
     * @brief Returns the next value of an unique counter for PRM's node id.
     * @return Returns the next value of an unique counter for PRM's node id.
     */
    NodeId nextNodeId();

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/utils_prm_tpl.h>

#endif   // GUM_UTILS_PRM_H
