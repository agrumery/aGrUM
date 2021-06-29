/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief the class for computing AIC scores
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/scores_and_tests/scoreAIC.h>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreAIC< ALLOC >::ScoreAIC(
       const DBRowGeneratorParser< ALLOC >&                                 parser,
       const Apriori< ALLOC >&                                              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&        nodeId2columns,
       const typename ScoreAIC< ALLOC >::allocator_type&                    alloc) :
        Score< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreAIC);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreAIC< ALLOC >::ScoreAIC(
       const DBRowGeneratorParser< ALLOC >&                          parser,
       const Apriori< ALLOC >&                                       apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
       const typename ScoreAIC< ALLOC >::allocator_type&             alloc) :
        Score< ALLOC >(parser, apriori, nodeId2columns, alloc),
        _internal_apriori_(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreAIC);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreAIC< ALLOC >::ScoreAIC(const ScoreAIC< ALLOC >&                          from,
                                       const typename ScoreAIC< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(from, alloc),
        _internal_apriori_(from._internal_apriori_, alloc) {
      GUM_CONS_CPY(ScoreAIC);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScoreAIC< ALLOC >::ScoreAIC(const ScoreAIC< ALLOC >& from) :
        ScoreAIC< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreAIC< ALLOC >::ScoreAIC(ScoreAIC< ALLOC >&&                               from,
                                       const typename ScoreAIC< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(std::move(from), alloc),
        _internal_apriori_(std::move(from._internal_apriori_), alloc) {
      GUM_CONS_MOV(ScoreAIC);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreAIC< ALLOC >::ScoreAIC(ScoreAIC< ALLOC >&& from) :
        ScoreAIC< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScoreAIC< ALLOC >*
       ScoreAIC< ALLOC >::clone(const typename ScoreAIC< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScoreAIC< ALLOC > > allocator(alloc);
      ScoreAIC< ALLOC >*         new_score = allocator.allocate(1);
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
    ScoreAIC< ALLOC >* ScoreAIC< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    ScoreAIC< ALLOC >::~ScoreAIC() {
      GUM_DESTRUCTOR(ScoreAIC);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScoreAIC< ALLOC >& ScoreAIC< ALLOC >::operator=(const ScoreAIC< ALLOC >& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(from);
        _internal_apriori_      = from._internal_apriori_;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScoreAIC< ALLOC >& ScoreAIC< ALLOC >::operator=(ScoreAIC< ALLOC >&& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(std::move(from));
        _internal_apriori_      = std::move(from._internal_apriori_);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    std::string ScoreAIC< ALLOC >::isAprioriCompatible(const std::string& apriori_type,
                                                       double             weight) {
      // check that the apriori is compatible with the score
      if ((apriori_type == AprioriDirichletType::type)
          || (apriori_type == AprioriSmoothingType::type)
          || (apriori_type == AprioriNoAprioriType::type)) {
        return "";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type << "' is not yet compatible with the score 'AIC'.";
      return msg.str();
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string ScoreAIC< ALLOC >::isAprioriCompatible(const Apriori< ALLOC >& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string ScoreAIC< ALLOC >::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->apriori_));
    }


    /// returns the internal apriori of the score
    template < template < typename > class ALLOC >
    INLINE const Apriori< ALLOC >& ScoreAIC< ALLOC >::internalApriori() const {
      return _internal_apriori_;
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double ScoreAIC< ALLOC >::score_(const IdCondSet< ALLOC >& idset) {
      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double, ALLOC< double > > N_ijk(this->counter_.counts(idset, true));
      const bool informative_external_apriori = this->apriori_->isInformative();
      if (informative_external_apriori) this->apriori_->addAllApriori(idset, N_ijk);
      const std::size_t all_size = N_ijk.size();

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_ij(this->marginalize_(idset[0], N_ijk));
        const std::size_t                      conditioning_size = N_ij.size();

        // initialize the score: this should be the penalty of the AIC score,
        // i.e., -(ri-1 ) * qi
        const std::size_t target_domsize = all_size / conditioning_size;
        const double      penalty = conditioning_size * double(target_domsize - std::size_t(1));

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i sum_j=1^q_i N_ijk log (N_ijk / N_ij), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - sum_j=1^q_i N_ij log N_ij
        double score = 0.0;
        for (const auto n_ijk: N_ijk) {
          if (n_ijk) { score += n_ijk * std::log(n_ijk); }
        }
        for (const auto n_ij: N_ij) {
          if (n_ij) { score -= n_ij * std::log(n_ij); }
        }

        // divide by log(2), since the log likelihood uses log_2
        score *= this->one_log2_;

        // finally, remove the penalty
        score -= penalty;

        return score;
      } else {
        // here, there are no conditioning nodes

        // initialize the score: this should be the penalty of the AIC score,
        // i.e., -(ri-1 )
        const double penalty = double(all_size - std::size_t(1));

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i N_ijk log (N_ijk / N), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - N log N
        double N     = 0.0;
        double score = 0.0;
        for (const auto n_ijk: N_ijk) {
          if (n_ijk) {
            score += n_ijk * std::log(n_ijk);
            N += n_ijk;
          }
        }
        score -= N * std::log(N);

        // divide by log(2), since the log likelihood uses log_2
        score *= this->one_log2_;

        // finally, remove the penalty
        score -= penalty;

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
