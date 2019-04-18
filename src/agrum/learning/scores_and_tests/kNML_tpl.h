
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


/**
 * @file
 * @brief The class for the NML penalty used in 3off2
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE KNML< ALLOC >::KNML(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                     nodeId2columns,
       const typename KNML< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        __param_complexity(alloc) {
      GUM_CONSTRUCTOR(KNML);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE KNML< ALLOC >::KNML(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                     nodeId2columns,
       const typename KNML< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(parser, apriori, nodeId2columns, alloc),
        __param_complexity(alloc) {
      GUM_CONSTRUCTOR(KNML);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE
       KNML< ALLOC >::KNML(const KNML< ALLOC >&                          from,
                           const typename KNML< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(from, alloc),
        __param_complexity(from.__param_complexity, alloc) {
      GUM_CONS_CPY(KNML);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE KNML< ALLOC >::KNML(const KNML< ALLOC >& from) :
        KNML< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE
       KNML< ALLOC >::KNML(KNML< ALLOC >&&                               from,
                           const typename KNML< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(std::move(from), alloc),
        __param_complexity(std::move(from.__param_complexity), alloc) {
      GUM_CONS_MOV(KNML);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE KNML< ALLOC >::KNML(KNML< ALLOC >&& from) :
        KNML< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    KNML< ALLOC >* KNML< ALLOC >::clone(
       const typename KNML< ALLOC >::allocator_type& alloc) const {
      ALLOC< KNML< ALLOC > > allocator(alloc);
      KNML< ALLOC >*         new_score = allocator.allocate(1);
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
    KNML< ALLOC >* KNML< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    KNML< ALLOC >::~KNML() {
      GUM_DESTRUCTOR(KNML);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    KNML< ALLOC >& KNML< ALLOC >::operator=(const KNML< ALLOC >& from) {
      if (this != &from) {
        IndependenceTest< ALLOC >::operator=(from);
        __param_complexity = from.__param_complexity;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    KNML< ALLOC >& KNML< ALLOC >::operator=(KNML< ALLOC >&& from) {
      if (this != &from) {
        IndependenceTest< ALLOC >::operator=(std::move(from));
        __param_complexity = std::move(from.__param_complexity);
      }
      return *this;
    }


    /// clears all the data structures from memory, including the cache
    template < template < typename > class ALLOC >
    void KNML< ALLOC >::clear() {
      IndependenceTest< ALLOC >::clear();
      __param_complexity.clearCache();
    }


    /// clears the current cache
    template < template < typename > class ALLOC >
    void KNML< ALLOC >::clearCache() {
      IndependenceTest< ALLOC >::clearCache();
      __param_complexity.clearCache();
    }


    /// turn on/off the use of a cache of the previously computed score
    template < template < typename > class ALLOC >
    void KNML< ALLOC >::useCache(const bool on_off) {
      IndependenceTest< ALLOC >::useCache(on_off);
      __param_complexity.useCache(on_off);
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double KNML< ALLOC >::_score(const IdSet< ALLOC >& idset) {
      // perform the countings on the database for all the nodes in the idset
      // This will help optimizing the computations of the Nxui, Nyui and Nui
      // that we will be needed subsequently
      this->_counter.counts(idset, true);

      const bool informative_external_apriori = this->_apriori->isInformative();

      // get the domain sizes of X and Y
      const auto& db = this->database();
      const auto& node2cols = this->nodeId2Columns();
      std::size_t r_x, r_y;
      if (!node2cols.empty()) {
        r_x = db.domainSize(node2cols.second(idset[0]));
        r_y = db.domainSize(node2cols.second(idset[1]));
      } else {
        r_x = db.domainSize(idset[0]);
        r_y = db.domainSize(idset[1]);
      }


      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // now get the Nxui, Nyui and Nui
        IdSet< ALLOC > idset_xui = idset;
        idset_xui.erase(idset[1]);
        IdSet< ALLOC > idset_yui = idset;
        idset_yui.erase(idset[0]);

        std::vector< double, ALLOC< double > > N_ui =
           this->_counter.counts(idset.conditionalIdSet(), false);
        std::vector< double, ALLOC< double > > N_xui =
           this->_counter.counts(idset_xui, false);
        std::vector< double, ALLOC< double > > N_yui =
           this->_counter.counts(idset_yui, false);

        if (informative_external_apriori) {
          this->_apriori->addConditioningApriori(idset, N_ui);
          this->_apriori->addAllApriori(idset, N_xui);
          this->_apriori->addAllApriori(idset, N_yui);
        }


        // the value of kNML is equal to:
        // 0.5 * sum_Z ( sum_X( log( C^(r_y)_#ZX ) ) - log( C^(r_y)_#Z ) +
        // 		 sum_Y( log( C^(r_x)_#ZY ) ) - log( C^(r_x)_#Z ) )
        double score = 0.0;
        for (auto n_xui : N_xui)
          score += __param_complexity.log2Cnr(r_y, n_xui);
        for (auto n_yui : N_yui)
          score += __param_complexity.log2Cnr(r_x, n_yui);
        for (auto n_ui : N_ui) {
          score -= __param_complexity.log2Cnr(r_y, n_ui);
          score -= __param_complexity.log2Cnr(r_x, n_ui);
        }

        score *= 0.5;

        return score;
      } else {
        // here, there is no conditioning set
        // now get the Nxui, Nyui and Nui
        IdSet< ALLOC > idset_xui(idset[0], this->_empty_ids, true);
        IdSet< ALLOC > idset_yui(idset[1], this->_empty_ids, true);

        std::vector< double, ALLOC< double > > N_xui =
           this->_counter.counts(idset_xui, false);
        std::vector< double, ALLOC< double > > N_yui =
           this->_counter.counts(idset_yui, false);

        if (informative_external_apriori) {
          this->_apriori->addAllApriori(idset, N_xui);
          this->_apriori->addAllApriori(idset, N_yui);
        }


        // so, the formula for kNML is:
        // 0.5 * ( sum_X( log( C^(r_y)_#X ) ) - log( C^(r_y)_N ) +
        //         sum_Y( log( C^(r_x)_#Y ) ) - log( C^(r_x)_N ) )
        double N = 0.0;
        double score = 0.0;
        for (auto n_xui : N_xui) {
          score += __param_complexity.log2Cnr(r_y, n_xui);
          N += n_xui;
        }
        for (auto n_yui : N_yui)
          score += __param_complexity.log2Cnr(r_x, n_yui);
        score -= __param_complexity.log2Cnr(r_y, N);
        score -= __param_complexity.log2Cnr(r_x, N);

        score *= 0.5;

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
