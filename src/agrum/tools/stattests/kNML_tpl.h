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


/**
 * @file
 * @brief The class for the NML penalty used in 3off2
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
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
        param_complexity__(alloc) {
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
        param_complexity__(alloc) {
      GUM_CONSTRUCTOR(KNML);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE
       KNML< ALLOC >::KNML(const KNML< ALLOC >&                          from,
                           const typename KNML< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(from, alloc),
        param_complexity__(from.param_complexity__, alloc) {
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
        param_complexity__(std::move(from.param_complexity__), alloc) {
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
        param_complexity__                 = from.param_complexity__;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    KNML< ALLOC >& KNML< ALLOC >::operator=(KNML< ALLOC >&& from) {
      if (this != &from) {
        IndependenceTest< ALLOC >::operator=(std::move(from));
        param_complexity__                 = std::move(from.param_complexity__);
      }
      return *this;
    }


    /// clears all the data structures from memory, including the cache
    template < template < typename > class ALLOC >
    void KNML< ALLOC >::clear() {
      IndependenceTest< ALLOC >::clear();
      param_complexity__.clearCache();
    }


    /// clears the current cache
    template < template < typename > class ALLOC >
    void KNML< ALLOC >::clearCache() {
      IndependenceTest< ALLOC >::clearCache();
      param_complexity__.clearCache();
    }


    /// turn on/off the use of a cache of the previously computed score
    template < template < typename > class ALLOC >
    void KNML< ALLOC >::useCache(const bool on_off) {
      IndependenceTest< ALLOC >::useCache(on_off);
      param_complexity__.useCache(on_off);
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double KNML< ALLOC >::score_(const IdCondSet< ALLOC >& idset) {
      // perform the countings on the database for all the nodes in the idset
      // This will help optimizing the computations of the Nxui, Nyui and Nui
      // that we will be needed subsequently
      this->counter_.counts(idset, true);

      const bool informative_external_apriori = this->apriori_->isInformative();

      // get the domain sizes of X and Y
      const auto& db        = this->database();
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
        IdCondSet< ALLOC > idset_xui = idset;
        idset_xui.erase(idset[1]);
        IdCondSet< ALLOC > idset_yui = idset;
        idset_yui.erase(idset[0]);

        std::vector< double, ALLOC< double > > N_ui
           = this->counter_.counts(idset.conditionalIdCondSet(), false);
        std::vector< double, ALLOC< double > > N_xui
           = this->counter_.counts(idset_xui, false);
        std::vector< double, ALLOC< double > > N_yui
           = this->counter_.counts(idset_yui, false);

        if (informative_external_apriori) {
          this->apriori_->addConditioningApriori(idset, N_ui);
          this->apriori_->addAllApriori(idset, N_xui);
          this->apriori_->addAllApriori(idset, N_yui);
        }


        // the value of kNML is equal to:
        // 0.5 * sum_Z ( sum_X( log( C^(r_y)_#ZX ) ) - log( C^(r_y)_#Z ) +
        // 		 sum_Y( log( C^(r_x)_#ZY ) ) - log( C^(r_x)_#Z ) )
        double score = 0.0;
        for (auto n_xui: N_xui)
          score += param_complexity__.log2Cnr(r_y, n_xui);
        for (auto n_yui: N_yui)
          score += param_complexity__.log2Cnr(r_x, n_yui);
        for (auto n_ui: N_ui) {
          score -= param_complexity__.log2Cnr(r_y, n_ui);
          score -= param_complexity__.log2Cnr(r_x, n_ui);
        }

        score *= 0.5;

        return score;
      } else {
        // here, there is no conditioning set
        // now get the Nxui, Nyui and Nui
        IdCondSet< ALLOC > idset_xui(idset[0], this->empty_ids_, true);
        IdCondSet< ALLOC > idset_yui(idset[1], this->empty_ids_, true);

        std::vector< double, ALLOC< double > > N_xui
           = this->counter_.counts(idset_xui, false);
        std::vector< double, ALLOC< double > > N_yui
           = this->counter_.counts(idset_yui, false);

        if (informative_external_apriori) {
          this->apriori_->addAllApriori(idset, N_xui);
          this->apriori_->addAllApriori(idset, N_yui);
        }


        // so, the formula for kNML is:
        // 0.5 * ( sum_X( log( C^(r_y)_#X ) ) - log( C^(r_y)N_ ) +
        //         sum_Y( log( C^(r_x)_#Y ) ) - log( C^(r_x)N_ ) )
        double N     = 0.0;
        double score = 0.0;
        for (auto n_xui: N_xui) {
          score += param_complexity__.log2Cnr(r_y, n_xui);
          N += n_xui;
        }
        for (auto n_yui: N_yui)
          score += param_complexity__.log2Cnr(r_x, n_yui);
        score -= param_complexity__.log2Cnr(r_y, N);
        score -= param_complexity__.log2Cnr(r_x, N);

        score *= 0.5;

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
