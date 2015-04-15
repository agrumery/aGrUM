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
 * @brief the base class for estimating parameters of CPTs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE ParamEstimator<IdSetAlloc, CountAlloc>::ParamEstimator(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities,
        Apriori<IdSetAlloc, CountAlloc> &apriori,
        const ScoreInternalApriori<IdSetAlloc, CountAlloc> &score_internal_apriori)
        : Counter<IdSetAlloc, CountAlloc>(filter, var_modalities),
          _apriori(&apriori),
          _score_internal_apriori(score_internal_apriori.copyFactory()) {
      GUM_CONSTRUCTOR(ParamEstimator);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ParamEstimator<IdSetAlloc, CountAlloc>::ParamEstimator(
        const ParamEstimator<IdSetAlloc, CountAlloc> &from)
        : Counter<IdSetAlloc, CountAlloc>(from), _apriori(from._apriori),
          _score_internal_apriori(from._score_internal_apriori->copyFactory()),
          _is_normalized(from._is_normalized),
          __apriori_computed(from.__apriori_computed),
          __score_apriori_inserted(from.__score_apriori_inserted) {
      GUM_CONS_CPY(ParamEstimator);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ParamEstimator<IdSetAlloc, CountAlloc>::ParamEstimator(
        ParamEstimator<IdSetAlloc, CountAlloc> &&from)
        : Counter<IdSetAlloc, CountAlloc>(std::move(from)),
          _apriori(std::move(from._apriori)),
          _score_internal_apriori(std::move(from._score_internal_apriori)),
          _is_normalized(std::move(from._is_normalized)),
          __apriori_computed(std::move(from.__apriori_computed)),
          __score_apriori_inserted(std::move(from.__score_apriori_inserted)) {
      from._score_internal_apriori = nullptr;
      GUM_CONS_MOV(ParamEstimator);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ParamEstimator<IdSetAlloc, CountAlloc>::~ParamEstimator() {
      if (_score_internal_apriori != nullptr)
        delete _score_internal_apriori;
      GUM_DESTRUCTOR(ParamEstimator);
    }

    /// add a new CPT with a single variable to be estimated
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int
    ParamEstimator<IdSetAlloc, CountAlloc>::addNodeSet(unsigned int var) {
      _is_normalized.push_back(false);
      return Counter<IdSetAlloc, CountAlloc>::addNodeSet(var);
    }

    /// add a new target variable plus some conditioning vars
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int ParamEstimator<IdSetAlloc, CountAlloc>::addNodeSet(
        unsigned int var, const std::vector<unsigned int> &conditioning_ids) {
      _is_normalized.push_back(false);
      return Counter<IdSetAlloc, CountAlloc>::addNodeSet(var, conditioning_ids);
    }

    /// clears all the data structures from memory
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void ParamEstimator<IdSetAlloc, CountAlloc>::clear() {
      Counter<IdSetAlloc, CountAlloc>::clear();
      _is_normalized.clear();
      __apriori_computed = false;
      __score_apriori_inserted = false;
    }

    /// sets the CPT's parameters corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void
    ParamEstimator<IdSetAlloc, CountAlloc>::setParameters(unsigned int nodeset_index,
                                                          Potential<float> &pot) {
      const std::vector<float, CountAlloc> &normalized_counts =
          parameters(nodeset_index);
      pot.fillWith(normalized_counts);
    }

    /// returns the apriori vector for a given (conditioned) target set
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<float, CountAlloc> &
    ParamEstimator<IdSetAlloc, CountAlloc>::_getAllApriori(unsigned int index) {
      if (!__apriori_computed) {
        _apriori->setParameters(
            this->_modalities, Counter<IdSetAlloc, CountAlloc>::_getCounts(),
            this->_target_nodesets, this->_conditioning_nodesets);
        _apriori->compute();
        __apriori_computed = true;
      }

      return _apriori->getAllApriori(index);
    }

    /// returns the apriori vector for a conditioning set
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<float, CountAlloc> &
    ParamEstimator<IdSetAlloc, CountAlloc>::_getConditioningApriori(
        unsigned int index) {
      if (!__apriori_computed) {
        _apriori->setParameters(
            this->_modalities, Counter<IdSetAlloc, CountAlloc>::_getCounts(),
            this->_target_nodesets, this->_conditioning_nodesets);
        _apriori->compute();
        __apriori_computed = true;
      }

      return _apriori->getConditioningApriori(index);
    }

    /// if needed insert the score apriori into the countings
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void ParamEstimator<IdSetAlloc, CountAlloc>::_insertScoreApriori() {
      if (!__score_apriori_inserted) {
        _score_internal_apriori->insertScoreApriori(
            Counter<IdSetAlloc, CountAlloc>::_modalities,
            Counter<IdSetAlloc, CountAlloc>::_getCounts(),
            Counter<IdSetAlloc, CountAlloc>::_target_nodesets,
            Counter<IdSetAlloc, CountAlloc>::_conditioning_nodesets);

        __score_apriori_inserted = true;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
