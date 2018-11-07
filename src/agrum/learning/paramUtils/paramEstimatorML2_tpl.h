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
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML2< ALLOC >::ParamEstimatorML2(
      const DBRowGeneratorParser< ALLOC >& parser,
      const Apriori2< ALLOC >& external_apriori,
      const Apriori2< ALLOC >& score_internal_apriori,
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ParamEstimatorML2< ALLOC >::allocator_type& alloc) :
      ParamEstimator2< ALLOC >(parser, external_apriori, score_internal_apriori,
                               ranges, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(ParamEstimatorML2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML2< ALLOC >::ParamEstimatorML2(
      const DBRowGeneratorParser< ALLOC >&                          parser,
      const Apriori2< ALLOC >& external_apriori,
      const Apriori2< ALLOC >& score_internal_apriori,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ParamEstimatorML2< ALLOC >::allocator_type&    alloc) :
      ParamEstimator2< ALLOC >(parser, external_apriori, score_internal_apriori,
                               nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(ParamEstimatorML2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML2< ALLOC >::ParamEstimatorML2(
      const ParamEstimatorML2< ALLOC >&                          from,
      const typename ParamEstimatorML2< ALLOC >::allocator_type& alloc) :
      ParamEstimator2< ALLOC >(from, alloc) {
      GUM_CONS_CPY(ParamEstimatorML2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML2< ALLOC >::ParamEstimatorML2(
        const ParamEstimatorML2< ALLOC >& from) :
        ParamEstimatorML2< ALLOC >(from, this->getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML2< ALLOC >::ParamEstimatorML2(
      ParamEstimatorML2< ALLOC >&&                               from,
      const typename ParamEstimatorML2< ALLOC >::allocator_type& alloc) :
      ParamEstimator2< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(ParamEstimatorML2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimatorML2< ALLOC >::ParamEstimatorML2(
        ParamEstimatorML2< ALLOC >&& from) :
        ParamEstimatorML2< ALLOC >(std::move(from), this->getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ParamEstimatorML2< ALLOC >* ParamEstimatorML2< ALLOC >::clone(
      const typename ParamEstimatorML2< ALLOC >::allocator_type& alloc) const {
      ALLOC< ParamEstimatorML2< ALLOC > > allocator(alloc);
      ParamEstimatorML2< ALLOC >*         new_score = allocator.allocate(1);
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
    ParamEstimatorML2< ALLOC >* ParamEstimatorML2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }

    
    /// destructor
    template < template < typename > class ALLOC >
    ParamEstimatorML2< ALLOC >::~ParamEstimatorML2() {
      GUM_DESTRUCTOR(ParamEstimatorML2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ParamEstimatorML2< ALLOC >& ParamEstimatorML2< ALLOC >::
                        operator=(const ParamEstimatorML2< ALLOC >& from) {
      ParamEstimator2< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ParamEstimatorML2< ALLOC >&
    ParamEstimatorML2< ALLOC >::operator=(ParamEstimatorML2< ALLOC >&& from) {
      ParamEstimator2< ALLOC >::operator=(std::move(from));
      return *this;
    }

    
    /// returns the CPT's parameters corresponding to a given set of nodes
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > >
    ParamEstimatorML2< ALLOC >::parameters(
           const NodeId target_node,
           const std::vector< NodeId, ALLOC< NodeId> >& conditioning_nodes ) {
      // create an idset that contains all the nodes in the following order:
      // first, the target node, then all the conditioning nodes
      IdSet2< ALLOC > idset ( target_node, conditioning_nodes, true );

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

        const std::size_t conditioning_domsize = N_ij.size ();
        const std::size_t target_domsize = N_ijk.size() / conditioning_domsize;

        // check that all conditioning nodes have strictly positive counts
        for (std::size_t j = std::size_t(0); j < conditioning_domsize; ++j) {
          if (!N_ij[j]) {
            // get the domain sizes of the conditioning nodes
            const std::size_t cond_nb = conditioning_nodes.size();
            std::vector< Idx > cond_domsize (cond_nb);

            const auto& node2cols = this->_counter.nodeId2Columns();
            const auto& database  = this->_counter.database();
            if ( node2cols.empty () ) {
              for ( std::size_t i = std::size_t(0); i < cond_nb; ++i ) {
                cond_domsize[i] = database.domainSize(conditioning_nodes[i]);
              }
            }
            else {
              for ( std::size_t i = std::size_t(0); i < cond_nb; ++i ) {
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
              std::size_t col = node2cols.empty () ? conditioning_nodes[i] :
                node2cols.second(conditioning_nodes[i]);
              const DiscreteVariable& var =
                dynamic_cast< const DiscreteVariable& >(database.variable(col));
              str << var.name() << "=" << var.labels()[values[i]];
            }
            auto target_col = node2cols.empty () ? target_node :
              node2cols.second(target_node);
            const Variable& var = database.variable(target_col);
            str << "> for target node " << var.name()
                << " never appears in the database. Please consider using "
                << "priors such as smoothing.";

            GUM_ERROR(DatabaseError, str.str());
          }
        }

        // normalize the counts
        for (std::size_t i = std::size_t(0), k = std::size_t(0);
             i < target_domsize; ++i) {
          for (std::size_t j=std::size_t(0); j < conditioning_domsize; ++k, ++j) {
            N_ijk[k] /= N_ij[j];
          }
        }
      }
      else {
        // here, there are no conditioning nodes. Hence N_ijk is the marginal
        // probability distribution over the target node. To normalize it, it
        // is sufficient to divide each cell by the sum over all the cells
        double sum = 0;
        for ( const double n_ijk : N_ijk )
          sum += n_ijk;
        
        if (sum) {
          for ( double& n_ijk : N_ijk )
            n_ijk /= sum;
        } else {
          GUM_ERROR(DatabaseError,
                    "The database being empty, it is impossible "
                    "to estimate the parameters by maximum likelihood");
        }
      }

      return N_ijk;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
