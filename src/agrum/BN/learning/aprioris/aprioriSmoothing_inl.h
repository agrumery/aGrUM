/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the smooth a priori: adds a weight w to all the countings
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE
       AprioriSmoothing::AprioriSmoothing(const DatabaseTable&                    database,
                                          const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Apriori(database, nodeId2columns) {
      GUM_CONSTRUCTOR(AprioriSmoothing);
    }


    /// copy constructor
    INLINE AprioriSmoothing::AprioriSmoothing(const AprioriSmoothing& from) : Apriori(from) {
      GUM_CONS_CPY(AprioriSmoothing);
    }


    /// move constructor
    INLINE AprioriSmoothing::AprioriSmoothing(AprioriSmoothing&& from) : Apriori(std::move(from)) {
      GUM_CONS_MOV(AprioriSmoothing);
    }


    /// virtual copy constructor
    INLINE AprioriSmoothing* AprioriSmoothing::clone() const { return new AprioriSmoothing(*this); }


    /// destructor
    INLINE AprioriSmoothing::~AprioriSmoothing() { GUM_DESTRUCTOR(AprioriSmoothing); }


    /// copy operator
    INLINE AprioriSmoothing& AprioriSmoothing::operator=(const AprioriSmoothing& from) {
      Apriori::operator=(from);
      return *this;
    }


    /// move operator
    INLINE AprioriSmoothing& AprioriSmoothing::operator=(AprioriSmoothing&& from) {
      Apriori::operator=(std::move(from));
      return *this;
    }


    /// indicates whether an apriori is of a certain type
    INLINE bool AprioriSmoothing::isOfType(const std::string& type) {
      return AprioriSmoothingType::isOfType(type);
    }


    /// returns the type of the apriori
    INLINE const std::string& AprioriSmoothing::getType() const {
      return AprioriSmoothingType::type;
    }


    /// indicates whether the apriori is potentially informative
    INLINE bool AprioriSmoothing::isInformative() const { return this->weight_ != 0.0; }


    /// returns the apriori vector all the variables in the idset
    INLINE void AprioriSmoothing::addAllApriori(const IdCondSet&       idset,
                                                std::vector< double >& counts) {
      // if the idset is empty or the weight is zero, the apriori is also empty
      if (idset.empty() || (this->weight_ == 0.0)) return;

      // otherwise, add the weight to all the cells in the counting vector
      for (auto& count: counts)
        count += this->weight_;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
