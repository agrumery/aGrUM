
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
 * @brief the class for computing BD scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/learning/scores_and_tests/scoreBD.h>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBD< ALLOC >::ScoreBD(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                        nodeId2columns,
       const typename ScoreBD< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBD);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBD< ALLOC >::ScoreBD(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                        nodeId2columns,
       const typename ScoreBD< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(parser, apriori, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreBD);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreBD< ALLOC >::ScoreBD(
       const ScoreBD< ALLOC >&                          from,
       const typename ScoreBD< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(from, alloc),
        __internal_apriori(from.__internal_apriori, alloc),
        __gammalog2(from.__gammalog2) {
      GUM_CONS_CPY(ScoreBD);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBD< ALLOC >::ScoreBD(const ScoreBD< ALLOC >& from) :
        ScoreBD< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreBD< ALLOC >::ScoreBD(
       ScoreBD< ALLOC >&&                               from,
       const typename ScoreBD< ALLOC >::allocator_type& alloc) :
        Score< ALLOC >(std::move(from), alloc),
        __internal_apriori(std::move(from.__internal_apriori), alloc),
        __gammalog2(std::move(from.__gammalog2)) {
      GUM_CONS_MOV(ScoreBD);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreBD< ALLOC >::ScoreBD(ScoreBD< ALLOC >&& from) :
        ScoreBD< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScoreBD< ALLOC >* ScoreBD< ALLOC >::clone(
       const typename ScoreBD< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScoreBD< ALLOC > > allocator(alloc);
      ScoreBD< ALLOC >*         new_score = allocator.allocate(1);
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
    ScoreBD< ALLOC >* ScoreBD< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    ScoreBD< ALLOC >::~ScoreBD() {
      GUM_DESTRUCTOR(ScoreBD);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScoreBD< ALLOC >& ScoreBD< ALLOC >::operator=(const ScoreBD< ALLOC >& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(from);
        __internal_apriori = from.__internal_apriori;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScoreBD< ALLOC >& ScoreBD< ALLOC >::operator=(ScoreBD< ALLOC >&& from) {
      if (this != &from) {
        Score< ALLOC >::operator=(std::move(from));
        __internal_apriori = std::move(from.__internal_apriori);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    std::string
       ScoreBD< ALLOC >::isAprioriCompatible(const std::string& apriori_type,
                                             double             weight) {
      if (apriori_type == AprioriNoAprioriType::type) {
        return "The BD score requires an apriori";
      }

      if (weight != 0.0) {
        return "The apriori is currently compatible with the BD score but if "
               "you change the weight, it may become biased";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible";
      return msg.str();
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string
           ScoreBD< ALLOC >::isAprioriCompatible(const Apriori< ALLOC >& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string ScoreBD< ALLOC >::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->_apriori));
    }


    /// returns the internal apriori of the score
    template < template < typename > class ALLOC >
    INLINE const Apriori< ALLOC >& ScoreBD< ALLOC >::internalApriori() const {
      return __internal_apriori;
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double ScoreBD< ALLOC >::_score(const IdSet< ALLOC >& idset) {
      // if the weight of the apriori is 0, then gammaLog2 will fail
      if (!this->_apriori->isInformative()) {
        GUM_ERROR(OutOfBounds,
                  "The BD score requires its external apriori to "
                     << "be strictly positive");
      }

      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double, ALLOC< double > > N_ijk(
         this->_counter.counts(idset, true));
      const std::size_t                      all_size = N_ijk.size();
      std::vector< double, ALLOC< double > > N_prime_ijk(all_size, 0.0);
      this->_apriori->addAllApriori(idset, N_prime_ijk);

      double score = 0.0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_ij(
           this->_marginalize(idset[0], N_ijk));
        const std::size_t conditioning_size = N_ij.size();

        std::vector< double, ALLOC< double > > N_prime_ij(N_ij.size(), 0.0);
        this->_apriori->addConditioningApriori(idset, N_prime_ij);

        // the BD score can be computed as follows:
        // sum_j=1^qi [ gammalog2 ( N'_ij ) - gammalog2 ( N_ij + N'_ij )
        //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk ) -
        //                             gammalog2 ( N'_ijk ) } ]
        for (std::size_t j = std::size_t(0); j < conditioning_size; ++j) {
          score +=
             __gammalog2(N_prime_ij[j]) - __gammalog2(N_ij[j] + N_prime_ij[j]);
        }
        for (std::size_t k = std::size_t(0); k < all_size; ++k) {
          score +=
             __gammalog2(N_ijk[k] + N_prime_ijk[k]) - __gammalog2(N_prime_ijk[k]);
        }
      } else {
        // the BD score can be computed as follows:
        // gammalog2 ( N' ) - gammalog2 ( N + N' )
        // + sum_k=1^ri { gammlog2 ( N_i + N'_i ) - gammalog2 ( N'_i ) }
        double N = 0.0;
        double N_prime = 0.0;
        for (std::size_t k = std::size_t(0); k < all_size; ++k) {
          score +=
             __gammalog2(N_ijk[k] + N_prime_ijk[k]) - __gammalog2(N_prime_ijk[k]);
          N += N_ijk[k];
          N_prime += N_prime_ijk[k];
        }
        score += __gammalog2(N_prime) - __gammalog2(N + N_prime);
      }

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
