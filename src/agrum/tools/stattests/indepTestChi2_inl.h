/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/stattests/idCondSet.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE IndepTestChi2::IndepTestChi2(
       const DBRowGeneratorParser&                                 parser,
       const Prior&                                                prior,
       const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
       const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        IndependenceTest(parser, prior, ranges, nodeId2columns),
        _domain_sizes_(parser.database().domainSizes()), _chi2_(_domain_sizes_) {
      GUM_CONSTRUCTOR(IndepTestChi2);
    }


    /// default constructor
    INLINE IndepTestChi2::IndepTestChi2(const DBRowGeneratorParser&             parser,
                                        const Prior&                            prior,
                                        const Bijection< NodeId, std::size_t >& nodeId2columns) :
        IndependenceTest(parser, prior, nodeId2columns),
        _domain_sizes_(parser.database().domainSizes()), _chi2_(_domain_sizes_) {
      GUM_CONSTRUCTOR(IndepTestChi2);
    }


    /// copy constructor
    INLINE IndepTestChi2::IndepTestChi2(const IndepTestChi2& from) :
        IndependenceTest(from), _domain_sizes_(from._domain_sizes_), _chi2_(_domain_sizes_) {
      GUM_CONS_CPY(IndepTestChi2);
    }


    /// move constructor
    INLINE IndepTestChi2::IndepTestChi2(IndepTestChi2&& from) :
        IndependenceTest(std::move(from)), _domain_sizes_(from._domain_sizes_),
        _chi2_(_domain_sizes_) {
      GUM_CONS_MOV(IndepTestChi2);
    }


    /// virtual copy constructor
    INLINE IndepTestChi2* IndepTestChi2::clone() const { return new IndepTestChi2(*this); }


    /// destructor
    INLINE IndepTestChi2::~IndepTestChi2() { GUM_DESTRUCTOR(IndepTestChi2); }


    /// returns the pair <statistics,pvalue> corresponding to a given IdCondSet
    INLINE std::pair< double, double >
       IndepTestChi2::statistics(NodeId var1, NodeId var2, const std::vector< NodeId >& rhs_ids) {
      return statistics_(IdCondSet(var1, var2, rhs_ids, false));
    }


    /// returns the score corresponding to a given nodeset
    INLINE double IndepTestChi2::score_(const IdCondSet& idset) {
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
      const double alpha = _chi2_.criticalValue(var_x, var_y);
      score              = (score - alpha) / alpha;

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
