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
 * @brief the class for computing Chi2 scores
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/stattests/idCondSet.h>

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndepTestChi2< ALLOC >::IndepTestChi2(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                              nodeId2columns,
       const typename IndepTestChi2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
         _domain_sizes_(parser.database().domainSizes()),  _chi2_( _domain_sizes_) {
      GUM_CONSTRUCTOR(IndepTestChi2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndepTestChi2< ALLOC >::IndepTestChi2(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                              nodeId2columns,
       const typename IndepTestChi2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(parser, apriori, nodeId2columns, alloc),
         _domain_sizes_(parser.database().domainSizes()),  _chi2_( _domain_sizes_) {
      GUM_CONSTRUCTOR(IndepTestChi2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IndepTestChi2< ALLOC >::IndepTestChi2(
       const IndepTestChi2< ALLOC >&                          from,
       const typename IndepTestChi2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(from, alloc),
         _domain_sizes_(from. _domain_sizes_),  _chi2_( _domain_sizes_) {
      GUM_CONS_CPY(IndepTestChi2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE
       IndepTestChi2< ALLOC >::IndepTestChi2(const IndepTestChi2< ALLOC >& from) :
        IndepTestChi2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IndepTestChi2< ALLOC >::IndepTestChi2(
       IndepTestChi2< ALLOC >&&                               from,
       const typename IndepTestChi2< ALLOC >::allocator_type& alloc) :
        IndependenceTest< ALLOC >(std::move(from), alloc),
         _domain_sizes_(from. _domain_sizes_),  _chi2_( _domain_sizes_) {
      GUM_CONS_MOV(IndepTestChi2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IndepTestChi2< ALLOC >::IndepTestChi2(IndepTestChi2< ALLOC >&& from) :
        IndepTestChi2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    IndepTestChi2< ALLOC >* IndepTestChi2< ALLOC >::clone(
       const typename IndepTestChi2< ALLOC >::allocator_type& alloc) const {
      ALLOC< IndepTestChi2< ALLOC > > allocator(alloc);
      IndepTestChi2< ALLOC >*         new_score = allocator.allocate(1);
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
    IndepTestChi2< ALLOC >* IndepTestChi2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    IndepTestChi2< ALLOC >::~IndepTestChi2() {
      GUM_DESTRUCTOR(IndepTestChi2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    IndepTestChi2< ALLOC >&
       IndepTestChi2< ALLOC >::operator=(const IndepTestChi2< ALLOC >& from) {
      if (this != &from) {
        IndependenceTest< ALLOC >::operator=(from);
        // __chi2 = from. _chi2_;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    IndepTestChi2< ALLOC >&
       IndepTestChi2< ALLOC >::operator=(IndepTestChi2< ALLOC >&& from) {
      if (this != &from) {
        IndependenceTest< ALLOC >::operator=(std::move(from));
        // __chi2 = std::move(from. _chi2_);
      }
      return *this;
    }

    /// returns the pair <statistics,pvalue> corresponding to a given IdCondSet
    template < template < typename > class ALLOC >
    std::pair< double, double > IndepTestChi2< ALLOC >::statistics(
       NodeId                                        var1,
       NodeId                                        var2,
       const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids) {
      return statistics_(IdCondSet< ALLOC >(var1, var2, rhs_ids, false));
    }

    /// returns the pair <statistics,pvalue> corresponding to a given IdCondSet
    template < template < typename > class ALLOC >
    std::pair< double, double >
       IndepTestChi2< ALLOC >::statistics_(const IdCondSet< ALLOC >& idset) {
      // get the countings
      std::vector< double, ALLOC< double > > N_xyz(
         this->counter_.counts(idset, true));
      const bool informative_external_apriori = this->apriori_->isInformative();
      if (informative_external_apriori)
        this->apriori_->addAllApriori(idset, N_xyz);
      const std::size_t all_size = (N_xyz.size());

      // compute the domain sizes of X and Y
      const auto& nodeId2cols = this->counter_.nodeId2Columns();
      const auto& database    = this->counter_.database();
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

      double cumulStat = 0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        const std::size_t Z_size = all_size / (X_size * Y_size);

        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_xz
           = this->marginalize_(std::size_t(1), X_size, Y_size, Z_size, N_xyz);
        std::vector< double, ALLOC< double > > N_yz
           = this->marginalize_(std::size_t(0), X_size, Y_size, Z_size, N_xyz);
        std::vector< double, ALLOC< double > > N_z
           = this->marginalize_(std::size_t(2), X_size, Y_size, Z_size, N_xyz);

        // indicate to the chi2 distribution the set of conditioning nodes
        std::vector< Idx > cond_nodes;
        cond_nodes.reserve(idset.nbRHSIds());
        {
          const auto cond_idset = idset.conditionalIdCondSet().ids();
          if (nodeId2cols.empty()) {
            for (const auto node: cond_idset)
              cond_nodes.push_back(node);
          } else {
            for (const auto node: cond_idset)
              cond_nodes.push_back(nodeId2cols.second(node));
          }
        }
         _chi2_.setConditioningNodes(cond_nodes);


        // now, perform sum_X sum_Y sum_Z ( #ZYX - #ZX * #ZY / #Z )^2 /
        // (#ZX * #ZY / #Z )
        for (std::size_t z      = std::size_t(0),
                         beg_xz = std::size_t(0),
                         beg_yz = std::size_t(0),
                         xyz    = std::size_t(0);
             z < Z_size;
             ++z, beg_xz += X_size, beg_yz += Y_size) {
          if (N_z[z] > 0) {
            for (std::size_t y = std::size_t(0), yz = beg_yz; y < Y_size;
                 ++yz, ++y) {
              for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size;
                   ++xz, ++x, ++xyz) {
                const double tmp1 = (N_yz[yz] * N_xz[xz]) / N_z[z];
                if (tmp1 != 0.0) {
                  const double tmp2 = N_xyz[xyz] - tmp1;
                  cumulStat += (tmp2 * tmp2) / tmp1;
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
         _chi2_.setConditioningNodes( _empty_set_);

        // now, perform sum_X sum_Y ( #XY - (#X * #Y) / N )^2 / (#X * #Y )/N

        // get the counts for all the targets and for the conditioning nodes
        std::vector< double, ALLOC< double > > N_x
           = this->marginalize_(std::size_t(1),
                                X_size,
                                Y_size,
                                std::size_t(1),
                                N_xyz);
        std::vector< double, ALLOC< double > > N_y
           = this->marginalize_(std::size_t(0),
                                X_size,
                                Y_size,
                                std::size_t(1),
                                N_xyz);

        // count N
        double N = 0.0;
        for (const auto n_x: N_x)
          N += n_x;

        for (std::size_t y = std::size_t(0), xy = 0; y < Y_size; ++y) {
          const double tmp_Ny = N_y[y];
          for (std::size_t x = 0; x < X_size; ++x, ++xy) {
            const double tmp1 = (tmp_Ny * N_x[x]) / N;
            if (tmp1 != 0.0) {
              const double tmp2 = N_xyz[xy] - tmp1;
              cumulStat += (tmp2 * tmp2) / tmp1;
            }
          }
        }
      }

      Size   df     =  _chi2_.degreesOfFreedom(var_x, var_y);
      double pValue =  _chi2_.probaChi2(cumulStat, df);
      return std::pair< double, double >(cumulStat, pValue);
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    inline double IndepTestChi2< ALLOC >::score_(const IdCondSet< ALLOC >& idset) {
      const auto& nodeId2cols = this->counter_.nodeId2Columns();
      Idx         var_x, var_y;
      if (nodeId2cols.empty()) {
        var_x = idset[0];
        var_y = idset[1];
      } else {
        var_x = nodeId2cols.second(idset[0]);
        var_y = nodeId2cols.second(idset[1]);
      }

      auto   stat  = statistics_(idset);   // stat contains pair(Chi2stat,pValue)
      double score = stat.first;

      // ok, here, score contains the value of the chi2 formula.
      // To get a meaningful score, we shall compute the critical values
      // for the Chi2 distribution and assign as the score of
      // (score - alpha ) / alpha, where alpha is the critical value
      const double alpha =  _chi2_.criticalValue(var_x, var_y);
      score              = (score - alpha) / alpha;

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
