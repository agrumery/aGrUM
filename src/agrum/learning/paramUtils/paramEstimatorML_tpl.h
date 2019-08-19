/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief the class for estimating parameters of CPTs using Maximum Likelihood
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// if this define is active, the estimator will not accept to assess numbers for a
// distribution with no case at all (N_ij=0). if this define is not active, the
// estimator will use an uniform distribution for this case.
//#  define GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML< ALLOC >::ParamEstimatorML(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              external_apriori,
       const Apriori< ALLOC >&              score_internal_apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                                 nodeId2columns,
       const typename ParamEstimatorML< ALLOC >::allocator_type& alloc) :
        ParamEstimator< ALLOC >(parser,
                                external_apriori,
                                score_internal_apriori,
                                ranges,
                                nodeId2columns,
                                alloc) {
      GUM_CONSTRUCTOR(ParamEstimatorML);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML< ALLOC >::ParamEstimatorML(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              external_apriori,
       const Apriori< ALLOC >&              score_internal_apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                                 nodeId2columns,
       const typename ParamEstimatorML< ALLOC >::allocator_type& alloc) :
        ParamEstimator< ALLOC >(parser,
                                external_apriori,
                                score_internal_apriori,
                                nodeId2columns,
                                alloc) {
      GUM_CONSTRUCTOR(ParamEstimatorML);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML< ALLOC >::ParamEstimatorML(
       const ParamEstimatorML< ALLOC >&                          from,
       const typename ParamEstimatorML< ALLOC >::allocator_type& alloc) :
        ParamEstimator< ALLOC >(from, alloc) {
      GUM_CONS_CPY(ParamEstimatorML);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML< ALLOC >::ParamEstimatorML(
       const ParamEstimatorML< ALLOC >& from) :
        ParamEstimatorML< ALLOC >(from, this->getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML< ALLOC >::ParamEstimatorML(
       ParamEstimatorML< ALLOC >&&                               from,
       const typename ParamEstimatorML< ALLOC >::allocator_type& alloc) :
        ParamEstimator< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(ParamEstimatorML);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML< ALLOC >::ParamEstimatorML(
       ParamEstimatorML< ALLOC >&& from) :
        ParamEstimatorML< ALLOC >(std::move(from), this->getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ParamEstimatorML< ALLOC >* ParamEstimatorML< ALLOC >::clone(
       const typename ParamEstimatorML< ALLOC >::allocator_type& alloc) const {
      ALLOC< ParamEstimatorML< ALLOC > > allocator(alloc);
      ParamEstimatorML< ALLOC >*         new_score = allocator.allocate(1);
      try {
        allocator.construct(new_score, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_score, 1);
        throw;
      }

      return new_score;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    ParamEstimatorML< ALLOC >* ParamEstimatorML< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    ParamEstimatorML< ALLOC >::~ParamEstimatorML() {
      GUM_DESTRUCTOR(ParamEstimatorML);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ParamEstimatorML< ALLOC >& ParamEstimatorML< ALLOC >::
                               operator=(const ParamEstimatorML< ALLOC >& from) {
      ParamEstimator< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ParamEstimatorML< ALLOC >& ParamEstimatorML< ALLOC >::
                               operator=(ParamEstimatorML< ALLOC >&& from) {
      ParamEstimator< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// returns the CPT's parameters corresponding to a given set of nodes
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > > ParamEstimatorML< ALLOC >::parameters(
       const NodeId                                  target_node,
       const std::vector< NodeId, ALLOC< NodeId > >& conditioning_nodes) {
      // create an idset that contains all the nodes in the following order:
      // first, the target node, then all the conditioning nodes
      IdSet< ALLOC > idset(target_node, conditioning_nodes, true);

      // get the counts for all the nodes in the idset and add the external and
      // score internal aprioris
      std::vector< double, ALLOC< double > > N_ijk(
         this->_counter.counts(idset, true));
      const bool informative_external_apriori =
         this->_external_apriori->isInformative();
      const bool informative_score_internal_apriori =
         this->_score_internal_apriori->isInformative();
      if (informative_external_apriori)
        this->_external_apriori->addAllApriori(idset, N_ijk);
      if (informative_score_internal_apriori)
        this->_score_internal_apriori->addAllApriori(idset, N_ijk);


      // now, normalize N_ijk

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (!conditioning_nodes.empty()) {
        // get the counts for all the conditioning nodes, and add them the
        // external and score internal aprioris
        std::vector< double, ALLOC< double > > N_ij(
           this->_counter.counts(idset.conditionalIdSet(), false));
        if (informative_external_apriori)
          this->_external_apriori->addConditioningApriori(idset, N_ij);
        if (informative_score_internal_apriori)
          this->_score_internal_apriori->addConditioningApriori(idset, N_ij);

        const std::size_t conditioning_domsize = N_ij.size();
        const std::size_t target_domsize = N_ijk.size() / conditioning_domsize;

#  ifdef GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
        // check that all conditioning nodes have strictly positive counts
        for (std::size_t j = std::size_t(0); j < conditioning_domsize; ++j) {
          if (N_ij[j] == 0) {
            // get the domain sizes of the conditioning nodes
            const std::size_t  cond_nb = conditioning_nodes.size();
            std::vector< Idx > cond_domsize(cond_nb);

            const auto& node2cols = this->_counter.nodeId2Columns();
            const auto& database = this->_counter.database();
            if (node2cols.empty()) {
              for (std::size_t i = std::size_t(0); i < cond_nb; ++i) {
                cond_domsize[i] = database.domainSize(conditioning_nodes[i]);
              }
            } else {
              for (std::size_t i = std::size_t(0); i < cond_nb; ++i) {
                cond_domsize[i] =
                   database.domainSize(node2cols.second(conditioning_nodes[i]));
              }
            }

            // determine the value of each conditioning variable in N_ij[j]
            std::vector< Idx > offsets(cond_nb);
            Idx                offset = 1;
            std::size_t        i;
            for (i = std::size_t(0); i < cond_nb; ++i) {
              offsets[i] = offset;
              offset *= cond_domsize[i];
            }
            std::vector< Idx > values(cond_nb);
            i = 0;
            offset = j;
            for (Idx jj = cond_nb - 1; i < cond_nb; ++i, --jj) {
              values[jj] = offset / offsets[jj];
              offset %= offsets[jj];
            }

            // create the error message
            std::stringstream str;
            str << "The conditioning set <";
            bool deja = true;
            for (i = std::size_t(0); i < cond_nb; ++i) {
              if (deja)
                str << ", ";
              else
                deja = true;
              std::size_t col = node2cols.empty()
                                   ? conditioning_nodes[i]
                                   : node2cols.second(conditioning_nodes[i]);
              const DiscreteVariable& var =
                 dynamic_cast< const DiscreteVariable& >(database.variable(col));
              str << var.name() << "=" << var.labels()[values[i]];
            }
            auto target_col =
               node2cols.empty() ? target_node : node2cols.second(target_node);
            const Variable& var = database.variable(target_col);
            str << "> for target node " << var.name()
                << " never appears in the database. Please consider using "
                << "priors such as smoothing.";

            GUM_ERROR(DatabaseError, str.str());
          }
        }
#  endif   // GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL

        // normalize the counts
        for (std::size_t j = std::size_t(0), k = std::size_t(0);
             j < conditioning_domsize;
             ++j) {
          for (std::size_t i = std::size_t(0); i < target_domsize; ++i, ++k) {
#  ifdef GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
            N_ijk[k] /= N_ij[j];
#  else    // GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
            if (N_ij[j] != 0) {
              N_ijk[k] /= N_ij[j];
            } else {
              N_ijk[k] = 1.0 / target_domsize;
            }
#  endif   // GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
          }
        }
      } else {
        // here, there are no conditioning nodes. Hence N_ijk is the marginal
        // probability distribution over the target node. To normalize it, it
        // is sufficient to divide each cell by the sum over all the cells
        double sum = 0;
        for (const double n_ijk : N_ijk)
          sum += n_ijk;

        if (sum != 0) {
          for (double& n_ijk : N_ijk)
            n_ijk /= sum;
        } else {
#  ifdef GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
          std::stringstream str;

          const auto& node2cols = this->_counter.nodeId2Columns();
          const auto& database = this->_counter.database();
          auto        target_col =
             node2cols.empty() ? target_node : node2cols.second(target_node);
          const Variable& var = database.variable(target_col);
          str << "No data for target node " << var.name()
              << ". It is impossible to estimate the parameters by maximum "
                 "likelihood";
          GUM_ERROR(DatabaseError, str.str());
#  else    // GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
          for (double& n_ijk : N_ijk)
            n_ijk = 1.0 / N_ijk.size();
#  endif   // GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
        }
      }

      return N_ijk;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
