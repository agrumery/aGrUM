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
 * @brief the no a priori class: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/aprioris/aprioriNoApriori.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE
    AprioriNoApriori::AprioriNoApriori(const DatabaseTable&                    database,
                                       const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Apriori(database, nodeId2columns) {
      Apriori::setWeight(0.0);
      GUM_CONSTRUCTOR(AprioriNoApriori);
    }


    /// copy constructor
    INLINE AprioriNoApriori::AprioriNoApriori(const AprioriNoApriori& from) : Apriori(from) {
      GUM_CONS_CPY(AprioriNoApriori);
    }


    /// move constructor
    INLINE AprioriNoApriori::AprioriNoApriori(AprioriNoApriori&& from) : Apriori(std::move(from)) {
      GUM_CONS_MOV(AprioriNoApriori);
    }


    /// virtual copy constructor
    INLINE AprioriNoApriori* AprioriNoApriori::clone() const { return new AprioriNoApriori(*this); }


    /// destructor
    INLINE AprioriNoApriori::~AprioriNoApriori() { GUM_DESTRUCTOR(AprioriNoApriori); }


    /// copy operator
    INLINE AprioriNoApriori& AprioriNoApriori::operator=(const AprioriNoApriori& from) {
      Apriori::operator=(from);
      return *this;
    }


    /// move operator
    INLINE AprioriNoApriori& AprioriNoApriori::operator=(AprioriNoApriori&& from) {
      Apriori::operator=(std::move(from));
      return *this;
    }


    /// sets the weight of the a priori
    INLINE void AprioriNoApriori::setWeight(const double) {}


    /// indicates whether an apriori is of a certain type
    INLINE bool AprioriNoApriori::isOfType(const std::string& type) {
      return AprioriNoAprioriType::isOfType(type);
    }


    /// returns the type of the apriori
    INLINE const std::string& AprioriNoApriori::getType() const {
      return AprioriNoAprioriType::type;
    }


    /// indicates whether the apriori is potentially informative
    INLINE bool AprioriNoApriori::isInformative() const { return false; }


    /// returns the apriori vector all the variables in the idset
    INLINE void AprioriNoApriori::addAllApriori(const IdCondSet&       idset,
                                                std::vector< double >& counts) {}


    /// returns the apriori vector over only the conditioning set of an idset
    INLINE void AprioriNoApriori::addConditioningApriori(const IdCondSet&       idset,
                                                         std::vector< double >& counts) {}


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
