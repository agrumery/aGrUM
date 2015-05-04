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
 * @brief the class for estimating parameters of CPTs using Maximum Likelihood
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE ParamEstimatorML<IdSetAlloc, CountAlloc>::ParamEstimatorML(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities,
        Apriori<IdSetAlloc, CountAlloc> &apriori,
        const ScoreInternalApriori<IdSetAlloc, CountAlloc> &score_internal_apriori)
        : ParamEstimator<IdSetAlloc, CountAlloc>(filter, var_modalities, apriori,
                                                 score_internal_apriori) {
      // for debugging purposes
      GUM_CONSTRUCTOR(ParamEstimatorML);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ParamEstimatorML<IdSetAlloc, CountAlloc>::ParamEstimatorML(
        const ParamEstimatorML<IdSetAlloc, CountAlloc> &from)
        : ParamEstimator<IdSetAlloc, CountAlloc>(from) {
      // for debugging purposes
      GUM_CONS_CPY(ParamEstimatorML);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ParamEstimatorML<IdSetAlloc, CountAlloc>::ParamEstimatorML(
        ParamEstimatorML<IdSetAlloc, CountAlloc> &&from)
        : ParamEstimator<IdSetAlloc, CountAlloc>(std::move(from)) {
      // for debugging purposes
      GUM_CONS_MOV(ParamEstimatorML);
    }

    /// virtual copy factory
    template <typename IdSetAlloc, typename CountAlloc>
    ParamEstimatorML<IdSetAlloc, CountAlloc> *
    ParamEstimatorML<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ParamEstimatorML<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ParamEstimatorML<IdSetAlloc, CountAlloc>::~ParamEstimatorML() {
      // for debugging purposes
      GUM_DESTRUCTOR(ParamEstimatorML);
    }

    /// returns the CPT's parameters corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    const std::vector<float, CountAlloc> &
    ParamEstimatorML<IdSetAlloc, CountAlloc>::parameters(
        unsigned int nodeset_index) {
      // if all_counts is already normalized, just return it
      if (this->_is_normalized[nodeset_index]) {
        return this->_getAllCounts(nodeset_index);
      }

      // now, normalize all_counts

      // get the nodes involved in the score as well as their modalities
      const std::vector<unsigned int, IdSetAlloc> &all_nodes =
          this->_getAllNodes(nodeset_index);
      const std::vector<unsigned int, IdSetAlloc> *conditioning_nodes =
          this->_getConditioningNodes(nodeset_index);
      const unsigned int target_modal =
          this->modalities()[all_nodes[all_nodes.size() - 1]];
      std::vector<float, CountAlloc> &N_ijk =
          const_cast<std::vector<float, CountAlloc> &>(
              this->_getAllCounts(nodeset_index));

      // if we did not count yet the internal apriori of the score, do it
      this->_insertScoreApriori();

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (conditioning_nodes) {
        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float, CountAlloc> &N_ij =
            this->_getConditioningCounts(nodeset_index);
        const unsigned int conditioning_size = N_ij.size();

        if (this->_apriori->weight()) {
          const std::vector<float, CountAlloc> &N_prime_ijk =
              this->_getAllApriori(nodeset_index);
          const std::vector<float, CountAlloc> &N_prime_ij =
              this->_getConditioningApriori(nodeset_index);

          // check that all conditioning nodes have strictly positive counts
          for (unsigned int j = 0; j < conditioning_size; ++j) {
            if (N_ij[j] + N_prime_ij[j] == 0) {
              GUM_ERROR(CPTError, "A conditioning set has a value that never "
                                  "appears in the database");
            }
          }

          // normalize the counts
          for (unsigned int i = 0, k = 0; i < target_modal; ++i) {
            for (unsigned int j = 0; j < conditioning_size; ++k, ++j) {
              N_ijk[k] = (N_ijk[k] + N_prime_ijk[k]) / (N_ij[j] + N_prime_ij[j]);
            }
          }
        } else {
          // check that all conditioning nodes have strictly positive counts
          for (unsigned int j = 0; j < conditioning_size; ++j) {
            if (!N_ij[j]) {
              GUM_ERROR(CPTError, "A conditioning set has a value that never "
                                  "appears in the database");
            }
          }

          // normalize the counts
          for (unsigned int i = 0, k = 0; i < target_modal; ++i) {
            for (unsigned int j = 0; j < conditioning_size; ++k, ++j) {
              N_ijk[k] /= N_ij[j];
            }
          }
        }
      } else {
        float sum = 0;

        if (this->_apriori->weight()) {
          const std::vector<float, CountAlloc> &N_prime_ijk =
              this->_getAllApriori(nodeset_index);

          // here, there are no conditioning nodes
          for (unsigned int k = 0; k < target_modal; ++k) {
            sum += N_ijk[k] + N_prime_ijk[k];
          }

          if (sum) {
            for (unsigned int k = 0; k < target_modal; ++k) {
              N_ijk[k] = (N_ijk[k] + N_prime_ijk[k]) / sum;
            }
          } else {
            GUM_ERROR(CPTError, "The database being empty, it is impossible "
                                "to estimate the parameters by maximum likelihood");
          }
        } else {
          // here, there are no conditioning nodes
          for (unsigned int k = 0; k < target_modal; ++k) {
            sum += N_ijk[k];
          }

          if (sum) {
            for (unsigned int k = 0; k < target_modal; ++k) {
              N_ijk[k] /= sum;
            }
          } else {
            GUM_ERROR(CPTError, "The database being empty, it is impossible "
                                "to estimate the parameters by maximum likelihood");
          }
        }
      }

      // counts are normalized
      this->_is_normalized[nodeset_index] = true;
      return N_ijk;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
