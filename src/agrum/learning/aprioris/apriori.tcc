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
 * @brief the base class for all a prioris
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Apriori<IdSetAlloc, CountAlloc>::Apriori() {
      GUM_CONSTRUCTOR(Apriori);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Apriori<IdSetAlloc, CountAlloc>::Apriori(
        const Apriori<IdSetAlloc, CountAlloc> &from)
        : _weight(from._weight), _modalities(from._modalities),
          _unapriori_counts(from._unapriori_counts),
          _target_nodesets(from._target_nodesets),
          _conditioning_nodesets(from._conditioning_nodesets),
          _apriori_counts(from._apriori_counts) {
      GUM_CONS_CPY(Apriori);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE
    Apriori<IdSetAlloc, CountAlloc>::Apriori(Apriori<IdSetAlloc, CountAlloc> &&from)
        : _weight(std::move(from._weight)), _modalities(std::move(from._modalities)),
          _unapriori_counts(std::move(from._unapriori_counts)),
          _target_nodesets(std::move(from._target_nodesets)),
          _conditioning_nodesets(std::move(from._conditioning_nodesets)),
          _apriori_counts(std::move(from._apriori_counts)) {
      GUM_CONS_MOV(Apriori);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Apriori<IdSetAlloc, CountAlloc>::~Apriori() {
      GUM_DESTRUCTOR(Apriori);
    }

    /// sets the parameters for the apriori
    template <typename IdSetAlloc, typename CountAlloc>
    void Apriori<IdSetAlloc, CountAlloc>::setParameters(
        const std::vector<unsigned int> &modalities,
        std::vector<std::vector<float, CountAlloc>> &counts,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &target_nodesets,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &conditioning_nodesets) {
      if (_weight != 0) {
        _modalities = &modalities;
        _unapriori_counts = &counts;
        _target_nodesets = &target_nodesets;
        _conditioning_nodesets = &conditioning_nodesets;

        // reserve the _apriori_counts
        unsigned int apriori_size = _apriori_counts.size();
        const unsigned int count_size = counts.size();

        while (apriori_size > count_size) {
          _apriori_counts.pop_back();
          --apriori_size;
        }

        for (unsigned int i = 0; i < apriori_size; ++i) {
          _apriori_counts[i].resize(counts[i].size());
        }

        if (apriori_size < count_size) {
          _apriori_counts.reserve(count_size);
          do {
            _apriori_counts.push_back(
                std::vector<float, CountAlloc>(counts[apriori_size].size()));
            ++apriori_size;
          } while (apriori_size < count_size);
        }
      }
    }

    /// sets the weight of the a priori (kind of effective sample size)
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void Apriori<IdSetAlloc, CountAlloc>::setWeight(float weight) {
      if (weight < 0) {
        GUM_ERROR(OutOfBounds, "the weight of the apriori must be positive");
      }
      _weight = weight;
    }

    /// returns the weight assigned to the apriori
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE float Apriori<IdSetAlloc, CountAlloc>::weight() const noexcept {
      return _weight;
    }

    /// returns the apriori vector for a given (conditioned) target set
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<float, CountAlloc> &
    Apriori<IdSetAlloc, CountAlloc>::getAllApriori(unsigned int index) {
      if (_weight) {
        return _apriori_counts[_target_nodesets->operator[](index)->second];
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "Scores and Parameter estimators should "
                  "not get a priori countings when the apriori weight is zero");
      }
    }

    /// returns the apriori vector for a conditioning set
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<float, CountAlloc> &
    Apriori<IdSetAlloc, CountAlloc>::getConditioningApriori(unsigned int index) {
      if (_weight) {
        return _apriori_counts[_conditioning_nodesets->operator[](index)->second];
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "Scores and Parameter estimators should "
                  "not get a priori countings when the apriori weight is zero");
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
