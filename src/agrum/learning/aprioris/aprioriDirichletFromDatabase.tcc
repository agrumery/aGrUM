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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>::
        AprioriDirichletFromDatabase(const RowFilter &filter,
                                     const std::vector<unsigned int> &var_modalities)
        : Counter<IdSetAlloc, CountAlloc>(filter, var_modalities) {
      GUM_CONSTRUCTOR(AprioriDirichletFromDatabase);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>::
        AprioriDirichletFromDatabase(
            const AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> &from)
        : Apriori<IdSetAlloc, CountAlloc>(from),
          Counter<IdSetAlloc, CountAlloc>(from) {
      GUM_CONS_CPY(AprioriDirichletFromDatabase);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>::
        AprioriDirichletFromDatabase(
            AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> &&from)
        : Apriori<IdSetAlloc, CountAlloc>(std::move(from)),
          Counter<IdSetAlloc, CountAlloc>(std::move(from)) {
      GUM_CONS_MOV(AprioriDirichletFromDatabase);
    }

    /// virtual copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc> *
    AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE AprioriDirichletFromDatabase<
        IdSetAlloc, CountAlloc>::~AprioriDirichletFromDatabase() {
      GUM_DESTRUCTOR(AprioriDirichletFromDatabase);
    }

    /// include the apriori into a given set of counts
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>::compute() {
      if (this->_weight != 0) {
        // perform the countings
        Counter<IdSetAlloc, CountAlloc>::clear();
        const unsigned int size =
            Apriori<IdSetAlloc, CountAlloc>::_target_nodesets->size();
        for (unsigned int i = 0; i < size; ++i) {
          if (Apriori<IdSetAlloc, CountAlloc>::_target_nodesets->operator[](i) !=
              nullptr) {
            if (Apriori<IdSetAlloc, CountAlloc>::_conditioning_nodesets->operator[](
                    i) != nullptr) {
              Counter<IdSetAlloc, CountAlloc>::addNodeSet(
                  Apriori<IdSetAlloc, CountAlloc>::_target_nodesets->operator[](i)
                      ->first.back(),
                  Apriori<IdSetAlloc, CountAlloc>::_conditioning_nodesets->
                  operator[](i)->first);
            } else {
              Counter<IdSetAlloc, CountAlloc>::addNodeSet(
                  Apriori<IdSetAlloc, CountAlloc>::_target_nodesets->operator[](i)
                      ->first.back());
            }
          } else {
            Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet();
          }
        }

        // save the countings
        std::vector<std::vector<float, CountAlloc>> &counts =
            Counter<IdSetAlloc, CountAlloc>::_getCounts();
        std::swap(this->_apriori_counts, counts);

        Counter<IdSetAlloc, CountAlloc>::clear();
      }
    }

    /// indicates whether an apriori is of a certain type
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>::isOfType(
        const std::string &type) {
      return AprioriDirichletType::isOfType(type);
    }

    /// returns the type of the apriori
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::string &
    AprioriDirichletFromDatabase<IdSetAlloc, CountAlloc>::getType() const noexcept {
      return AprioriDirichletType::type;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
