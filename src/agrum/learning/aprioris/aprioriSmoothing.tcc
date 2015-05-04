/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief  the smooth a priori: adds a weight w to all the countings
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriSmoothing<IdSetAlloc, CountAlloc>::AprioriSmoothing() {
      GUM_CONSTRUCTOR(AprioriSmoothing);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriSmoothing<IdSetAlloc, CountAlloc>::AprioriSmoothing(
        const AprioriSmoothing<IdSetAlloc, CountAlloc> &from)
        : Apriori<IdSetAlloc, CountAlloc>(from) {
      GUM_CONS_CPY(AprioriSmoothing);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriSmoothing<IdSetAlloc, CountAlloc>::AprioriSmoothing(
        AprioriSmoothing<IdSetAlloc, CountAlloc> &&from)
        : Apriori<IdSetAlloc, CountAlloc>(std::move(from)) {
      GUM_CONS_MOV(AprioriSmoothing);
    }

    /// virtual copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriSmoothing<IdSetAlloc, CountAlloc> *
    AprioriSmoothing<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new AprioriSmoothing<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriSmoothing<IdSetAlloc, CountAlloc>::~AprioriSmoothing() {
      GUM_DESTRUCTOR(AprioriSmoothing);
    }

    /// include the apriori into a given set of counts
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void AprioriSmoothing<IdSetAlloc, CountAlloc>::compute() {
      if (this->_weight != 0) {
        // put the weight into the countings for the targets
        // and the sum of the weight times the target for the conditioning nodes
        const unsigned int size = this->_target_nodesets->size();
        for (unsigned int i = 0; i < size; ++i) {
          if (this->_target_nodesets->operator[](i) != nullptr) {
            std::vector<float, CountAlloc> &apriori =
                this->_apriori_counts[this->_target_nodesets->operator[](i)->second];
            for (auto &count : apriori) {
              count = this->_weight;
            }
          }

          if (this->_conditioning_nodesets->operator[](i) != nullptr) {
            const float weight =
                this->_weight *
                (*(this->_modalities))[(*(this->_target_nodesets))[i]->first.back()];
            std::vector<float, CountAlloc> &apriori =
                this->_apriori_counts[this->_conditioning_nodesets->operator[](i)
                                          ->second];
            for (auto &count : apriori) {
              count = weight;
            }
          }
        }
      }
    }

    /// indicates whether an apriori is of a certain type
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool
    AprioriSmoothing<IdSetAlloc, CountAlloc>::isOfType(const std::string &type) {
      return AprioriSmoothingType::isOfType(type);
    }

    /// returns the type of the apriori
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::string &
    AprioriSmoothing<IdSetAlloc, CountAlloc>::getType() const noexcept {
      return AprioriSmoothingType::type;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
