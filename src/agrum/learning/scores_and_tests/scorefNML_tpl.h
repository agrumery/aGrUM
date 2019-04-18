
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
 * @brief the class for computing fNML scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/learning/scores_and_tests/scorefNML.h>

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML< ALLOC >::ScorefNML(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                          nodeId2columns,
       const typename ScorefNML< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScorefNML);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML< ALLOC >::ScorefNML(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                          nodeId2columns,
       const typename ScorefNML< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(parser, apriori, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScorefNML);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScorefNML< ALLOC >::ScorefNML(
       const ScorefNML< ALLOC >&                          from,
       const typename ScorefNML< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(from, alloc),
        __internal_apriori(from.__internal_apriori, alloc) {
      GUM_CONS_CPY(ScorefNML);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML< ALLOC >::ScorefNML(const ScorefNML< ALLOC >& from) :
        ScorefNML< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScorefNML< ALLOC >::ScorefNML(
       ScorefNML< ALLOC >&&                               from,
       const typename ScorefNML< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(std::move(from), alloc),
        __internal_apriori(std::move(from.__internal_apriori), alloc) {
      GUM_CONS_MOV(ScorefNML);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML< ALLOC >::ScorefNML(ScorefNML< ALLOC >&& from) :
        ScorefNML< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScorefNML< ALLOC >* ScorefNML< ALLOC >::clone(
       const typename ScorefNML< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScorefNML< ALLOC > > allocator(alloc);
      ScorefNML< ALLOC >*         new_score = allocator.allocate(1);
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
    ScorefNML< ALLOC >* ScorefNML< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    ScorefNML< ALLOC >::~ScorefNML() {
      GUM_DESTRUCTOR(ScorefNML);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScorefNML< ALLOC >& ScorefNML< ALLOC >::
                        operator=(const ScorefNML< ALLOC >& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(from);
        __internal_apriori = from.__internal_apriori;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScorefNML< ALLOC >& ScorefNML< ALLOC >::operator=(ScorefNML< ALLOC >&& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(std::move(from));
        __internal_apriori = std::move(from.__internal_apriori);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    std::string
       ScorefNML< ALLOC >::isAprioriCompatible(const std::string& apriori_type,
                                               double             weight) {
      // check that the apriori is compatible with the score
      if ((apriori_type == AprioriDirichletType::type)
          || (apriori_type == AprioriSmoothingType::type)
          || (apriori_type == AprioriNoAprioriType::type)) {
        return "";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible os Score fNML";
      return msg.str();
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string
           ScorefNML< ALLOC >::isAprioriCompatible(const Apriori< ALLOC >& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string ScorefNML< ALLOC >::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->_apriori));
    }


    /// returns the internal apriori of the score
    template < template < typename > class ALLOC >
    INLINE const Apriori< ALLOC >& ScorefNML< ALLOC >::internalApriori() const {
      return __internal_apriori;
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double ScorefNML< ALLOC >::_score(const IdSet< ALLOC >& idset) {
      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double, ALLOC< double > > N_ijk(
         this->_counter.counts(idset, true));
      const bool informative_external_apriori = this->_apriori->isInformative();
      if (informative_external_apriori)
        this->_apriori->addAllApriori(idset, N_ijk);
      const std::size_t all_size = N_ijk.size();

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_ij(
           this->_marginalize(idset[0], N_ijk));
        const std::size_t target_domsize = all_size / N_ij.size();

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i sum_j=1^q_i N_ijk log (N_ijk / N_ij), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - sum_j=1^q_i N_ij log N_ij
        double score = 0.0;
        for (const auto n_ijk : N_ijk) {
          if (n_ijk) { score += n_ijk * std::log(n_ijk); }
        }
        for (const auto n_ij : N_ij) {
          if (n_ij) { score -= n_ij * std::log(n_ij); }
        }

        // divide by log(2), since the log likelihood uses log_2
        score *= this->_1log2;

        // finally, remove the penalty
        double penalty = 0.0;
        for (const auto n_ij : N_ij) {
          penalty += __ctable.log2Cnr(target_domsize, n_ij);
        }

        score -= penalty;

        return score;
      } else {
        // here, there are no conditioning nodes

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i N_ijk log (N_ijk / N), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - N log N
        double N = 0.0;
        double score = 0.0;
        for (const auto n_ijk : N_ijk) {
          if (n_ijk) {
            score += n_ijk * std::log(n_ijk);
            N += n_ijk;
          }
        }
        score -= N * std::log(N);

        // divide by log(2), since the log likelihood uses log_2
        score *= this->_1log2;

        // finally, remove the penalty
        score -= __ctable.log2Cnr(all_size, N);

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
