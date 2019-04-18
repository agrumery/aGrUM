
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
 * @brief the class for computing K2 scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/learning/scores_and_tests/scoreK2.h>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreK2< ALLOC >::ScoreK2(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                        nodeId2columns,
       const typename ScoreK2< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreK2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreK2< ALLOC >::ScoreK2(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                        nodeId2columns,
       const typename ScoreK2< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(parser, apriori, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreK2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreK2< ALLOC >::ScoreK2(
       const ScoreK2< ALLOC >&                          from,
       const typename ScoreK2< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(from, alloc),
        __internal_apriori(from.__internal_apriori, alloc),
        __gammalog2(from.__gammalog2) {
      GUM_CONS_CPY(ScoreK2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScoreK2< ALLOC >::ScoreK2(const ScoreK2< ALLOC >& from) :
        ScoreK2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreK2< ALLOC >::ScoreK2(
       ScoreK2< ALLOC >&&                               from,
       const typename ScoreK2< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(std::move(from), alloc),
        __internal_apriori(std::move(from.__internal_apriori), alloc),
        __gammalog2(std::move(from.__gammalog2)) {
      GUM_CONS_MOV(ScoreK2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreK2< ALLOC >::ScoreK2(ScoreK2< ALLOC >&& from) :
        ScoreK2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScoreK2< ALLOC >* ScoreK2< ALLOC >::clone(
       const typename ScoreK2< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScoreK2< ALLOC > > allocator(alloc);
      ScoreK2< ALLOC >*         new_score = allocator.allocate(1);
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
    ScoreK2< ALLOC >* ScoreK2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    ScoreK2< ALLOC >::~ScoreK2() {
      GUM_DESTRUCTOR(ScoreK2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScoreK2< ALLOC >& ScoreK2< ALLOC >::operator=(const ScoreK2< ALLOC >& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(from);
        __internal_apriori = from.__internal_apriori;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScoreK2< ALLOC >& ScoreK2< ALLOC >::operator=(ScoreK2< ALLOC >&& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(std::move(from));
        __internal_apriori = std::move(from.__internal_apriori);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    std::string
       ScoreK2< ALLOC >::isAprioriCompatible(const std::string& apriori_type,
                                             double             weight) {
      // check that the apriori is compatible with the score
      if (apriori_type == AprioriNoAprioriType::type) { return ""; }

      if (weight == 0.0) {
        return "The apriori is currently compatible with the K2 score but "
               "if you change the weight, it will become incompatible.";
      }

      // known incompatible aprioris
      if ((apriori_type == AprioriDirichletType::type)
          || (apriori_type == AprioriSmoothingType::type)) {
        return "The K2 score already contains a different 'implicit' apriori. "
               "Therefore, the learning will probably be biased.";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible os Score K2";
      return msg.str();
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string
           ScoreK2< ALLOC >::isAprioriCompatible(const Apriori< ALLOC >& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string ScoreK2< ALLOC >::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->_apriori));
    }


    /// returns the internal apriori of the score
    template < template < typename > class ALLOC >
    INLINE const Apriori< ALLOC >& ScoreK2< ALLOC >::internalApriori() const {
      return __internal_apriori;
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double ScoreK2< ALLOC >::_score(const IdSet< ALLOC >& idset) {
      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double, ALLOC< double > > N_ijk(
         this->_counter.counts(idset, true));
      const std::size_t all_size = N_ijk.size();
      const bool informative_external_apriori = this->_apriori->isInformative();
      double     score = 0.0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_ij(
           this->_marginalize(idset[0], N_ijk));
        const std::size_t conditioning_size = N_ij.size();
        const double      ri = double(all_size / conditioning_size);

        if (informative_external_apriori) {
          // the score to compute is that of BD with aprioris N'_ijk + 1
          // (the + 1 is here to take into account the internal apriori of K2)
          std::vector< double, ALLOC< double > > N_prime_ijk(all_size, 0.0);
          this->_apriori->addAllApriori(idset, N_prime_ijk);
          std::vector< double, ALLOC< double > > N_prime_ij(N_ij.size(), 0.0);
          this->_apriori->addConditioningApriori(idset, N_prime_ij);

          // the K2 score can be computed as follows:
          // sum_j=1^qi [ gammalog2 ( N'_ij + r_i ) -
          //              gammalog2 ( N_ij + N'_ij + r_i )
          //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk + 1 ) -
          //                             gammalog2 ( N'_ijk + 1 ) } ]
          for (std::size_t j = std::size_t(0); j < conditioning_size; ++j) {
            score += __gammalog2(N_prime_ij[j] + ri)
                     - __gammalog2(N_ij[j] + N_prime_ij[j] + ri);
          }
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + 1.0)
                     - __gammalog2(N_prime_ijk[k] + 1.0);
          }
        } else {
          // the K2 score can be computed as follows:
          // qi log {(ri - 1)!} + sum_j=1^qi [ - log {(N_ij+ri-1)!} +
          //                                   sum_k=1^ri log { N_ijk! } ]
          score = conditioning_size * __gammalog2(ri);

          for (const auto n_ij : N_ij) {
            score -= __gammalog2(n_ij + ri);
          }
          for (const auto n_ijk : N_ijk) {
            score += __gammalog2(n_ijk + 1);
          }
        }
      } else {
        // here, there are no conditioning nodes
        const double ri = double(all_size);

        if (informative_external_apriori) {
          // the score to compute is that of BD with aprioris N'_ijk + 1
          // (the + 1 is here to take into account the internal apriori of K2)

          // the K2 score can be computed as follows:
          // gammalog2 ( N' + r_i ) - gammalog2 ( N + N' + r_i )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i + 1 ) - gammalog2 ( N'_i + 1 )
          // }
          std::vector< double, ALLOC< double > > N_prime_ijk(all_size, 0.0);
          this->_apriori->addAllApriori(idset, N_prime_ijk);

          // the K2 score can be computed as follows:
          double N = 0.0;
          double N_prime = 0.0;
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + 1)
                     - __gammalog2(N_prime_ijk[k] + 1);
            N += N_ijk[k];
            N_prime += N_prime_ijk[k];
          }
          score += __gammalog2(N_prime + ri) - __gammalog2(N + N_prime + ri);
        } else {
          // the K2 score can be computed as follows:
          // log {(ri - 1)!} - log {(N + ri-1)!} + sum_k=1^ri log { N_ijk! } ]
          score = __gammalog2(ri);
          double N = 0;
          for (const auto n_ijk : N_ijk) {
            score += __gammalog2(n_ijk + 1);
            N += n_ijk;
          }
          score -= __gammalog2(N + ri);
        }
      }

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
