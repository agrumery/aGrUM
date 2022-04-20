/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the internal apriori for the K2 score = Laplace Apriori
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/aprioris/aprioriK2.h>

namespace gum {

  namespace learning {


    /// default constructors
    INLINE AprioriK2::AprioriK2(const DatabaseTable&                    database,
                                const Bijection< NodeId, std::size_t >& nodeId2columns) :
        AprioriSmoothing(database, nodeId2columns) {
      GUM_CONSTRUCTOR(AprioriK2);
    }


    /// copy constructor
    INLINE AprioriK2::AprioriK2(const AprioriK2& from) : AprioriSmoothing(from) {
      GUM_CONS_CPY(AprioriK2);
    }


    /// move constructor
    INLINE AprioriK2::AprioriK2(AprioriK2&& from) : AprioriSmoothing(std::move(from)) {
      GUM_CONS_MOV(AprioriK2);
    }


    /// virtual copy constructor
    INLINE AprioriK2* AprioriK2::clone() const { return new AprioriK2(*this); }


    /// destructor
    INLINE AprioriK2::~AprioriK2() { GUM_DESTRUCTOR(AprioriK2); }


    /// copy operator
    INLINE AprioriK2& AprioriK2::operator=(const AprioriK2& from) {
      AprioriSmoothing::operator=(from);
      return *this;
    }


    /// move operator
    INLINE AprioriK2& AprioriK2::operator=(AprioriK2&& from) {
      AprioriSmoothing::operator=(std::move(from));
      return *this;
    }


    /// dummy set weight function: in K2, weights are always equal to 1
    INLINE void AprioriK2::setWeight(const double weight) {}


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
