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
 * @brief the class for computing BDeu scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/learning/scores_and_tests/scoreBDeu2.h>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBDeu2< ALLOC >::ScoreBDeu2(
      const DBRowGeneratorParser< ALLOC >&                                 parser,
      const Apriori2< ALLOC >&                                             apriori,
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ScoreBDeu2< ALLOC >::allocator_type&            alloc) :
        Score2< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBDeu2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBDeu2< ALLOC >::ScoreBDeu2(
      const DBRowGeneratorParser< ALLOC >&                          parser,
      const Apriori2< ALLOC >&                                      apriori,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ScoreBDeu2< ALLOC >::allocator_type&            alloc) :
        Score2< ALLOC >(parser, apriori, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBDeu2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreBDeu2< ALLOC >::ScoreBDeu2(
      const ScoreBDeu2< ALLOC >&                          from,
      const typename ScoreBDeu2< ALLOC >::allocator_type& alloc) :
        Score2< ALLOC >(from, alloc),
      __internal_apriori(from.__internal_apriori, alloc),
      __gammalog2(from.__gammalog2) {
      GUM_CONS_CPY(ScoreBDeu2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBDeu2< ALLOC >::ScoreBDeu2(const ScoreBDeu2< ALLOC >& from) :
        ScoreBDeu2< ALLOC >(from, this->getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreBDeu2< ALLOC >::ScoreBDeu2(
      ScoreBDeu2< ALLOC >&&                               from,
      const typename ScoreBDeu2< ALLOC >::allocator_type& alloc) :
        Score2< ALLOC >(std::move(from), alloc),
      __internal_apriori(std::move(from.__internal_apriori), alloc),
      __gammalog2(std::move(from.__gammalog2)) {
      GUM_CONS_MOV(ScoreBDeu2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBDeu2< ALLOC >::ScoreBDeu2(ScoreBDeu2< ALLOC >&& from) :
        ScoreBDeu2< ALLOC >(std::move(from), this->getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScoreBDeu2< ALLOC >* ScoreBDeu2< ALLOC >::clone(
      const typename ScoreBDeu2< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScoreBDeu2< ALLOC > > allocator(alloc);
      ScoreBDeu2< ALLOC >*         new_score = allocator.allocate(1);
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
    ScoreBDeu2< ALLOC >* ScoreBDeu2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    ScoreBDeu2< ALLOC >::~ScoreBDeu2< ALLOC >() {
      GUM_DESTRUCTOR(ScoreBDeu2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScoreBDeu2< ALLOC >& ScoreBDeu2< ALLOC >::
                        operator=(const ScoreBDeu2< ALLOC >& from) {
      if (this != &from) {
        Score2< ALLOC >::operator=(from);
        __internal_apriori = from.__internal_apriori;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScoreBDeu2< ALLOC >& ScoreBDeu2< ALLOC >::operator=(ScoreBDeu2< ALLOC >&& from) {
      if (this != &from) {
        Score2< ALLOC >::operator=(std::move(from));
        __internal_apriori = std::move(from.__internal_apriori);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    std::string
      ScoreBDeu2< ALLOC >::isAprioriCompatible(const std::string& apriori_type,
                                              double             weight) {
      // check that the apriori is compatible with the score
      if (apriori_type == AprioriNoAprioriType::type) { return ""; }

      if (weight == 0.0) {
        return "The apriori is currently compatible with the BDeu score but "
               "if you change the weight, it will become incompatible.";
      }

      // known incompatible aprioris
      if ((apriori_type == AprioriDirichletType::type)
          || (apriori_type == AprioriSmoothingType::type)) {
        return "The BDeu score already contains a different 'implicit' apriori. "
               "Therefore, the learning will probably be biased.";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible os Score BDeu";
      return msg.str();
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string
           ScoreBDeu2< ALLOC >::isAprioriCompatible(const Apriori2< ALLOC >& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string ScoreBDeu2< ALLOC >::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->_apriori));
    }


    /// returns the internal apriori of the score
    template < template < typename > class ALLOC >
    INLINE const Apriori2< ALLOC >&
                 ScoreBDeu2< ALLOC >::internalApriori() const {
      return __internal_apriori;
    }


    /// sets the effective sample size of the internal apriori
    template < template < typename > class ALLOC >
    INLINE void
      ScoreBDeu2< ALLOC >::setEffectiveSampleSize(double ess) {
      if (ess < 0) {
        GUM_ERROR(OutOfBounds,
                  "The effective sample size of the BDeu's "
                  "internal apriori must be positive");
      } else {
        __internal_apriori.setEffectiveSampleSize(ess);
      }
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double ScoreBDeu2< ALLOC >::_score(const IdSet2< ALLOC >& idset) {
      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double, ALLOC< double > > N_ijk(
        this->_counter.counts(idset, true));
      const double all_size = (N_ijk.size());
      double score = 0.0;
      const double ess = __internal_apriori.weight ();

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_ij =
          this->_counter.counts(idset.conditionalIdSet(), false);
        const double conditioning_size = double(N_ij.size());
        const double ess_qi   = ess / conditioning_size;
        const double ess_riqi = ess / all_size;

        if (ess != 0.0) {
          // the score to compute is that of BD with aprioris
          // N'_ijk + ESS / (r_i * q_i )
          // (the + ESS / (r_i * q_i ) is here to take into account the
          // internal apriori of BDeu)
          std::vector< double, ALLOC< double > > N_prime_ijk ( all_size, 0.0 );
          this->_apriori->addAllApriori(idset, N_prime_ijk);
          std::vector< double, ALLOC< double > > N_prime_ij ( N_ij.size(), 0.0 );
          this->_apriori->addConditioningApriori(idset, N_prime_ij);
        
          // the BDeu score can be computed as follows:
          // sum_j=1^qi [ gammalog2 ( N'_ij + ESS / q_i ) -
          //     gammalog2 ( N_ij + N'_ij + ESS / q_i )
          //     + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk + ESS / (r_i * q_i ) )
          //     - gammalog2 ( N'_ijk + ESS / (r_i * q_i ) ) } ]
          for (std::size_t j = std::size_t(0); j < conditioning_size; ++j) {
            score += __gammalog2(N_prime_ij[j] + ess_qi)
              - __gammalog2(N_ij[j] + N_prime_ij[j] + ess_qi);
          }
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + ess_riqi)
              - __gammalog2(N_prime_ijk[k] + ess_riqi);
          }
        }
        else {
          // the BDeu score can be computed as follows:
          // qi * gammalog2 (ess / qi) - ri * qi * gammalog2 (ess / (ri * qi) )
          // - sum_j=1^qi [ gammalog2 ( N_ij + ess / qi ) ]
          // + sum_j=1^qi sum_k=1^ri log [ gammalog2 ( N_ijk + ess / (ri * qi) )
          // ]
          score = conditioning_size * __gammalog2(ess_qi)
            - all_size * __gammalog2(ess_riqi);

          for (std::size_t j = std::size_t(0); j < conditioning_size; ++j) {
            score -= __gammalog2(N_ij[j] + ess_qi);
          }
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += __gammalog2(N_ijk[k] + ess_riqi);
          }
        }
      } else {
        // here, there are no conditioning nodes
        const double ess_ri = ess / all_size;
        
        if (this->_apriori->weight()) {        
         // the score to compute is that of BD with aprioris
          // N'_ijk + ESS / ( ri * qi )
          // (the + ESS / ( ri * qi ) is here to take into account the
          // internal apriori of K2)
          std::vector< double, ALLOC< double > > N_prime_ijk ( all_size, 0.0 );
          this->_apriori->addAllApriori(idset, N_prime_ijk);
 
          // the BDeu score can be computed as follows:
          // gammalog2 ( N' + ess ) - gammalog2 ( N + N' + ess )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i + ESS / ri)
          //                - gammalog2 ( N'_i + ESS / ri ) }
          double N = 0.0;
          double N_prime = 0.0;
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + ess_ri)
              - __gammalog2(N_prime_ijk[k] + ess_ri);
            N += N_ijk[k];
            N_prime += N_prime_ijk[k];
          }
          score += __gammalog2(N_prime + ess) - __gammalog2(N + N_prime + ess);
        }
        else {
          // the BDeu score can be computed as follows:
          // gammalog2 ( ess ) - ri * gammalog2 ( ess / ri )
          // - gammalog2 ( N + ess )
          // + sum_k=1^ri log [ gammalog2 ( N_ijk + ess / ri ) ]

          score = __gammalog2(ess) - all_size * __gammalog2(ess_ri);
          double N = 0;
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += __gammalog2(N_ijk[k] + ess_ri);
            N += N_ijk[k];
          }
          score -= __gammalog2(N + ess);
        }
      }

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
