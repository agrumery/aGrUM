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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/database/DBRowGeneratorIdentity.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >::DBRowGeneratorWithBN(
        const std::vector< DBTranslatedValueType >& column_types,
        const BayesNet< GUM_SCALAR >&               bn,
        const DBRowGeneratorGoal                    goal,
        const Bijection< NodeId, std::size_t >&     nodeId2columns) :
        DBRowGenerator(column_types, goal), bn_(&bn), nodeId2columns_(nodeId2columns) {
      GUM_CONSTRUCTOR(DBRowGeneratorWithBN);
    }

    /// copy constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >::DBRowGeneratorWithBN(
        const DBRowGeneratorWithBN< GUM_SCALAR >& from) :
        DBRowGenerator(from), bn_(from.bn_), nodeId2columns_(from.nodeId2columns_) {
      GUM_CONS_CPY(DBRowGeneratorWithBN);
    }

    /// move constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >::DBRowGeneratorWithBN(
        DBRowGeneratorWithBN< GUM_SCALAR >&& from) :
        DBRowGenerator(std::move(from)), bn_(from.bn_),
        nodeId2columns_(std::move(from.nodeId2columns_)) {
      GUM_CONS_MOV(DBRowGeneratorWithBN);
    }

    /// destructor
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >::~DBRowGeneratorWithBN() {
      GUM_DESTRUCTOR(DBRowGeneratorWithBN);
    }

    /// copy operator
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >& DBRowGeneratorWithBN< GUM_SCALAR >::operator=(
        const DBRowGeneratorWithBN< GUM_SCALAR >& from) {
      if (this != &from) {
        DBRowGenerator::operator=(from);
        bn_             = from.bn_;
        nodeId2columns_ = from.nodeId2columns_;
      }

      return *this;
    }

    /// move operator
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >&
        DBRowGeneratorWithBN< GUM_SCALAR >::operator=(DBRowGeneratorWithBN< GUM_SCALAR >&& from) {
      if (this != &from) {
        DBRowGenerator::operator=(std::move(from));
        bn_             = from.bn_;
        nodeId2columns_ = std::move(from.nodeId2columns_);
      }

      return *this;
    }

    /// assign a new Bayes net to the generator
    template < typename GUM_SCALAR >
    INLINE void
        DBRowGeneratorWithBN< GUM_SCALAR >::setBayesNet(const BayesNet< GUM_SCALAR >& new_bn) {
      bn_ = &new_bn;
    }

    /// returns the Bayes net used by the generator
    template < typename GUM_SCALAR >
    INLINE const BayesNet< GUM_SCALAR >& DBRowGeneratorWithBN< GUM_SCALAR >::getBayesNet() const {
      return *bn_;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
