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
 * @brief the no a priorclass: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/priors/noPrior.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE
    NoPrior::NoPrior(const DatabaseTable&                    database,
                     const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Prior(database, nodeId2columns) {
      Prior::setWeight(0.0);
      GUM_CONSTRUCTOR(NoPrior);
    }


    /// copy constructor
    INLINE NoPrior::NoPrior(const NoPrior& from) : Prior(from) { GUM_CONS_CPY(NoPrior); }


    /// move constructor
    INLINE NoPrior::NoPrior(NoPrior&& from) : Prior(std::move(from)) { GUM_CONS_MOV(NoPrior); }


    /// virtual copy constructor
    INLINE NoPrior* NoPrior::clone() const { return new NoPrior(*this); }


    /// destructor
    INLINE NoPrior::~NoPrior() { GUM_DESTRUCTOR(NoPrior); }


    /// copy operator
    INLINE NoPrior& NoPrior::operator=(const NoPrior& from) {
      Prior::operator=(from);
      return *this;
    }


    /// move operator
    INLINE NoPrior& NoPrior::operator=(NoPrior&& from) {
      Prior::operator=(std::move(from));
      return *this;
    }


    /// sets the weight of the a priori
    INLINE void NoPrior::setWeight(const double) {}

    /// returns the type of the prior
    INLINE PriorType NoPrior::getType() const { return PriorType::NoPriorType; }


    /// indicates whether the prior is potentially informative
    INLINE bool NoPrior::isInformative() const { return false; }


    /// returns the prior vector all the variables in the idset
    INLINE void NoPrior::addJointPseudoCount(const IdCondSet&       idset,
                                             std::vector< double >& counts) {}


    /// returns the prior vector over only the conditioning set of an idset
    INLINE void NoPrior::addConditioningPseudoCount(const IdCondSet&       idset,
                                                    std::vector< double >& counts) {}


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
