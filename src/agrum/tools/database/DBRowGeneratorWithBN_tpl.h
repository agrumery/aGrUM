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


/** @file
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/tools/database/DBRowGeneratorIdentity.h>

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
        DBRowGenerator(column_types, goal),
        bn_(&bn), nodeId2columns_(nodeId2columns) {
      GUM_CONSTRUCTOR(DBRowGeneratorWithBN);
    }


    /// copy constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >::DBRowGeneratorWithBN(
       const DBRowGeneratorWithBN< GUM_SCALAR >& from) :
        DBRowGenerator(from),
        bn_(from.bn_), nodeId2columns_(from.nodeId2columns_) {
      GUM_CONS_CPY(DBRowGeneratorWithBN);
    }


    /// move constructor
    template < typename GUM_SCALAR >
    DBRowGeneratorWithBN< GUM_SCALAR >::DBRowGeneratorWithBN(
       DBRowGeneratorWithBN< GUM_SCALAR >&& from) :
        DBRowGenerator(std::move(from)),
        bn_(from.bn_), nodeId2columns_(std::move(from.nodeId2columns_)) {
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
