/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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


/** @file
 * @brief the class for computing G2 scores
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE
    IndepTestG2::IndepTestG2(const DBRowGeneratorParser&                                 parser,
                             const Prior&                                                prior,
                             const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                             const Bijection< NodeId, std::size_t >& nodeId2columns) :
        IndependenceTest(parser, prior, ranges, nodeId2columns),
        _domain_sizes_(parser.database().domainSizes()), _chi2_(_domain_sizes_) {
      GUM_CONSTRUCTOR(IndepTestG2);
    }

    /// default constructor
    INLINE IndepTestG2::IndepTestG2(const DBRowGeneratorParser&             parser,
                                    const Prior&                            prior,
                                    const Bijection< NodeId, std::size_t >& nodeId2columns) :
        IndependenceTest(parser, prior, nodeId2columns),
        _domain_sizes_(parser.database().domainSizes()), _chi2_(_domain_sizes_) {
      GUM_CONSTRUCTOR(IndepTestG2);
    }

    /// copy constructor
    INLINE IndepTestG2::IndepTestG2(const IndepTestG2& from) :
        IndependenceTest(from), _chi2_(_domain_sizes_) {
      GUM_CONS_CPY(IndepTestG2);
    }

    /// move constructor
    INLINE IndepTestG2::IndepTestG2(IndepTestG2&& from) :
        IndependenceTest(std::move(from)), _domain_sizes_(from._domain_sizes_),
        _chi2_(_domain_sizes_) {
      GUM_CONS_MOV(IndepTestG2);
    }

    /// virtual copy constructor
    INLINE IndepTestG2* IndepTestG2::clone() const { return new IndepTestG2(*this); }

    /// destructor
    INLINE IndepTestG2::~IndepTestG2() { GUM_DESTRUCTOR(IndepTestG2); }

    /// returns the pair <statistics,pvalue> corresponding to a given IdCondSet
    INLINE std::pair< double, double >
           IndepTestG2::statistics(NodeId var1, NodeId var2, const std::vector< NodeId >& rhs_ids) {
      return statistics_(IdCondSet(var1, var2, rhs_ids, false));
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
