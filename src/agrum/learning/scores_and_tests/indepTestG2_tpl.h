
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
 * @brief the class for computing G2 scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndepTestG2< ALLOC >::IndepTestG2(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                            nodeId2columns,
       const typename IndepTestG2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        __domain_sizes(parser.database().domainSizes()), __chi2(__domain_sizes) {
      GUM_CONSTRUCTOR(IndepTestG2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndepTestG2< ALLOC >::IndepTestG2(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                            nodeId2columns,
       const typename IndepTestG2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(parser, apriori, nodeId2columns, alloc),
        __domain_sizes(parser.database().domainSizes()), __chi2(__domain_sizes) {
      GUM_CONSTRUCTOR(IndepTestG2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IndepTestG2< ALLOC >::IndepTestG2(
       const IndepTestG2< ALLOC >&                          from,
       const typename IndepTestG2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(from, alloc),
        __chi2(__domain_sizes) {
      GUM_CONS_CPY(IndepTestG2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IndepTestG2< ALLOC >::IndepTestG2(const IndepTestG2< ALLOC >& from) :
        IndepTestG2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IndepTestG2< ALLOC >::IndepTestG2(
       IndepTestG2< ALLOC >&&                               from,
       const typename IndepTestG2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(std::move(from), alloc),
        __domain_sizes(from.__domain_sizes), __chi2(__domain_sizes) {
      GUM_CONS_MOV(IndepTestG2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IndepTestG2< ALLOC >::IndepTestG2(IndepTestG2< ALLOC >&& from) :
        IndepTestG2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    IndepTestG2< ALLOC >* IndepTestG2< ALLOC >::clone(
       const typename IndepTestG2< ALLOC >::allocator_type& alloc) const {
      ALLOC< IndepTestG2< ALLOC > > allocator(alloc);
      IndepTestG2< ALLOC >*         new_score = allocator.allocate(1);
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
    IndepTestG2< ALLOC >* IndepTestG2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    IndepTestG2< ALLOC >::~IndepTestG2() {
      GUM_DESTRUCTOR(IndepTestG2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    IndepTestG2< ALLOC >& IndepTestG2< ALLOC >::
                          operator=(const IndepTestG2< ALLOC >& from) {
      if (this != &from) {
        IndependenceTest< ALLOC >::operator=(from);
        //__chi2 = from.__chi2;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    IndepTestG2< ALLOC >& IndepTestG2< ALLOC >::
                          operator=(IndepTestG2< ALLOC >&& from) {
      if (this != &from) {
        IndependenceTest< ALLOC >::operator=(std::move(from));
        //__chi2 = std::move(from.__chi2);
      }
      return *this;
    }

    /// returns the pair <statistics,pvalue> corresponding to a given IdSet
    template < template < typename > class ALLOC >
    std::pair< double, double > IndepTestG2< ALLOC >::statistics(
       NodeId                                        var1,
       NodeId                                        var2,
       const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids) {
      return _statistics(IdSet< ALLOC >(var1, var2, rhs_ids, false));
    }

    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    std::pair< double, double >
       IndepTestG2< ALLOC >::_statistics(const IdSet< ALLOC >& idset) {
      // get the countings
      std::vector< double, ALLOC< double > > N_xyz(
         this->_counter.counts(idset, true));
      const bool informative_external_apriori = this->_apriori->isInformative();
      if (informative_external_apriori)
        this->_apriori->addAllApriori(idset, N_xyz);
      const std::size_t all_size = (N_xyz.size());

      // compute the domain sizes of X and Y
      const auto& nodeId2cols = this->_counter.nodeId2Columns();
      const auto& database = this->_counter.database();
      Idx         var_x, var_y;
      if (nodeId2cols.empty()) {
        var_x = idset[0];
        var_y = idset[1];
      } else {
        var_x = nodeId2cols.second(idset[0]);
        var_y = nodeId2cols.second(idset[1]);
      }

      const std::size_t X_size = database.domainSize(var_x);
      const std::size_t Y_size = database.domainSize(var_y);

      double cumulStat = 0.0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        const std::size_t Z_size = all_size / (X_size * Y_size);

        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_xz =
           this->_marginalize(std::size_t(1), X_size, Y_size, Z_size, N_xyz);
        std::vector< double, ALLOC< double > > N_yz =
           this->_marginalize(std::size_t(0), X_size, Y_size, Z_size, N_xyz);
        std::vector< double, ALLOC< double > > N_z =
           this->_marginalize(std::size_t(2), X_size, Y_size, Z_size, N_xyz);

        // indicate to the chi2 distribution the set of conditioning nodes
        std::vector< Idx > cond_nodes;
        cond_nodes.reserve(idset.nbRHSIds());
        {
          const auto cond_idset = idset.conditionalIdSet().ids();
          if (nodeId2cols.empty()) {
            for (const auto node : cond_idset)
              cond_nodes.push_back(node);
          } else {
            for (const auto node : cond_idset)
              cond_nodes.push_back(nodeId2cols.second(node));
          }
        }
        __chi2.setConditioningNodes(cond_nodes);


        // now, perform :
        // sum_X sum_Y sum_Z #XYZ * log ( ( #XYZ * #Z ) / ( #XZ * #YZ ) )
        for (std::size_t z = std::size_t(0),
                         beg_xz = std::size_t(0),
                         beg_yz = std::size_t(0),
                         xyz = std::size_t(0);
             z < Z_size;
             ++z, beg_xz += X_size, beg_yz += Y_size) {
          if (N_z[z] > 0) {
            for (std::size_t y = std::size_t(0), yz = beg_yz; y < Y_size;
                 ++yz, ++y) {
              for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size;
                   ++xz, ++x, ++xyz) {
                const double tmp1 = N_xyz[xyz] * N_z[z];
                const double tmp2 = N_yz[yz] * N_xz[xz];
                if ((tmp1 != 0.0) && (tmp2 != 0.0)) {
                  cumulStat += N_xyz[xyz] * std::log(tmp1 / tmp2);
                }
              }
            }
          } else {   // moving xyz out of the loops x,y when if N_z[z]==0
            xyz += X_size * Y_size;
          }
        }
      } else {
        // here, there is no conditioning set

        // indicate to the chi2 distribution the set of conditioning nodes
        __chi2.setConditioningNodes(__empty_set);

        // now, perform sum_X sum_Y #XY * log ( ( #XY * N ) / ( #X * #Y ) )

        // get the counts for all the targets and for the conditioning nodes
        std::vector< double, ALLOC< double > > N_x = this->_marginalize(
           std::size_t(1), X_size, Y_size, std::size_t(1), N_xyz);
        std::vector< double, ALLOC< double > > N_y = this->_marginalize(
           std::size_t(0), X_size, Y_size, std::size_t(1), N_xyz);

        // count N
        double N = 0.0;
        for (auto n_x : N_x)
          N += n_x;

        for (std::size_t y = std::size_t(0), xy = 0; y < Y_size; ++y) {
          const double tmp_Ny = N_y[y];
          for (std::size_t x = 0; x < X_size; ++x, ++xy) {
            const double tmp = (tmp_Ny * N_x[x]);
            if ((tmp != 0.0) && (N_xyz[xy] != 0.0)) {
              cumulStat += N_xyz[xy] * std::log((N_xyz[xy] * N) / tmp);
            }
          }
        }
      }

      // used to make the G test formula asymptotically equivalent
      // to the Pearson's chi-squared test formula
      cumulStat *= 2;

      Size   df = __chi2.degreesOfFreedom(var_x, var_y);
      double pValue = __chi2.probaChi2(cumulStat, df);
      return std::pair< double, double >(cumulStat, pValue);
    }

    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double IndepTestG2< ALLOC >::_score(const IdSet< ALLOC >& idset) {
      // compute the domain sizes of X and Y
      const auto& nodeId2cols = this->_counter.nodeId2Columns();
      Idx         var_x, var_y;
      if (nodeId2cols.empty()) {
        var_x = idset[0];
        var_y = idset[1];
      } else {
        var_x = nodeId2cols.second(idset[0]);
        var_y = nodeId2cols.second(idset[1]);
      }

      auto   stat = _statistics(idset);
      double score = stat.first;

      // ok, here, score contains the value of the chi2 formula.
      // To get a meaningful score, we shall compute the critical values
      // for the Chi2 distribution and assign as the score of
      // (score - alpha ) / alpha, where alpha is the critical value
      const double alpha = __chi2.criticalValue(var_x, var_y);
      score = (score - alpha) / alpha;

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
