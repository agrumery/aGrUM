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
 * @brief the no a priori class: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriNoApriori<IdSetAlloc, CountAlloc>::AprioriNoApriori() {
      setWeight(0);
      GUM_CONSTRUCTOR(AprioriNoApriori);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriNoApriori<IdSetAlloc, CountAlloc>::AprioriNoApriori(
        const AprioriNoApriori<IdSetAlloc, CountAlloc> &from)
        : Apriori<IdSetAlloc, CountAlloc>(from) {
      GUM_CONS_CPY(AprioriNoApriori);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriNoApriori<IdSetAlloc, CountAlloc>::AprioriNoApriori(
        AprioriNoApriori<IdSetAlloc, CountAlloc> &&from)
        : Apriori<IdSetAlloc, CountAlloc>(std::move(from)) {
      GUM_CONS_MOV(AprioriNoApriori);
    }

    /// virtual copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriNoApriori<IdSetAlloc, CountAlloc> *
    AprioriNoApriori<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new AprioriNoApriori<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriNoApriori<IdSetAlloc, CountAlloc>::~AprioriNoApriori() {
      GUM_DESTRUCTOR(AprioriNoApriori);
    }

    /// include the apriori into a given set of counts
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void AprioriNoApriori<IdSetAlloc, CountAlloc>::compute() {}

    /// sets the weight of the a priori
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void AprioriNoApriori<IdSetAlloc, CountAlloc>::setWeight(float) {}

    /// indicates whether an apriori is of a certain type
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool
    AprioriNoApriori<IdSetAlloc, CountAlloc>::isOfType(const std::string &type) {
      return AprioriNoAprioriType::isOfType(type);
    }

    /// returns the type of the apriori
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::string &
    AprioriNoApriori<IdSetAlloc, CountAlloc>::getType() const noexcept {
      return AprioriNoAprioriType::type;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
