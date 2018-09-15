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
 * @brief the class for computing fNML scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/learning/scores_and_tests/scorefNML2.h>
#  include <agrum/learning/scores_and_tests/scorefNMLCTable.h>
#  include <agrum/core/math/gammaLog2.h>
#  include <iostream>
#  include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::CTable::CTable() {
      GUM_CONSTRUCTOR(CTable);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::CTable::CTable(
      const typename ScorefNML2< ALLOC >::CTable& from) {
      GUM_CONS_CPY(CTable);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::CTable::CTable(
      typename ScorefNML2< ALLOC >::CTable&& from) {
      GUM_CONS_MOV(CTable);
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    typename ScorefNML2< ALLOC >::CTable* ScorefNML2< ALLOC >::CTable::clone(
      const typename ScorefNML2< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScorefNML2< ALLOC >::CTable >  allocator(alloc);
      typename ScorefNML2< ALLOC >::CTable* table = allocator.allocate(1);
      try {
        allocator.construct(table, *this);
      } catch (...) {
        allocator.deallocate(table, 1);
        throw;
      }
      return table;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    typename ScorefNML2< ALLOC >::CTable*
      ScorefNML2< ALLOC >::CTable::clone() const {
      return clone(ALLOC< NodeId >());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::CTable::~CTable() {
      GUM_DESTRUCTOR(CTable);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE typename ScorefNML2< ALLOC >::CTable& ScorefNML2< ALLOC >::CTable::
                                                 operator=(const typename ScorefNML2< ALLOC >::CTable& from) {
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE typename ScorefNML2< ALLOC >::CTable& ScorefNML2< ALLOC >::CTable::
                                                 operator=(typename ScorefNML2< ALLOC >::CTable&& from) {
      return *this;
    }


    /// returns the value of the log in base 2 of Cnr
    template < template < typename > class ALLOC >
    double ScorefNML2< ALLOC >::CTable::log2Cnr(const std::size_t r,
                                                const double      n) {
      // we know that c_n^1 = 1 for all values of n
      // in addition, c_0^r = 1 for all values of r
      // finally, it is easy to see that c_1^r = r for all r
      if (r == std::size_t(1)) return 0.0;         // log2(1)
      if (n == 0.0) return 0.0;                    // log2(1)
      if (n == 1.0) return std::log2((double)r);   // log2(r)

      if (n < 0.0) {
        GUM_ERROR(OutOfBounds,
                  "In the penalty of the fNML score, n must be greater "
                    << "than or equal to 0. But, here, n = " << n);
      }

      if (n < ScorefNMLCTableNSize) {
        // check if we can find the value we look for in precomputed table
        // ScorefNMLCTable
        std::size_t xn = (std::size_t)n;
        if (r - 2 < ScorefNMLCTableRSize) {
          return ScorefNMLCTable[r - 2][xn];
        } else {
          // use Equation (13) of the paper to compute the value of cnr:
          // C_n^r = C_n^{r-1} + (n / (r-2)) C_n^{r-2}
          // as we handle only log2's of C_n^r, we have the following:
          // let k_r be such that C_n^{r-2} = k_r * C_n^{r-1}
          // log2 ( C_n^r ) = log2 ( C_n^{r-1} + k_r * (n/(r-2)) * C_n^{r-1} )
          //                = log2 ( C_n^{r-1} ) + log2 ( 1 + k_r * (n/(r-2)) )
          // as  k_r = C_n^{r-2} / C_n^{r-1}, we have that
          // log2(k_r) = log2 ( C_n^{r-2} ) - log2 ( C_n^{r-1} )
          // so, k_r = exp ( (log2(cn_^{r-2}) - log2(C_n^{r-1})) * log(2) )
          // now, let q_r = 1 + k_r * (n/(r-2)), then
          // C_n^r = C_n^{r-1} * q_r, or, equivalently,
          // log2(C_n^r) = log2(C_n^{r-1}) + log2(q_r)
          // Now, we can use the same method to compute C_n^{r+1}:
          // k_{r+1}   = C_n^{r-1} / C_n^r = 1 / q_r
          // q_{r+1}   = 1 + k_{r+1} * (n/(r-1))
          // C_n^{r+1} = C_n^r * q_{r+1}
          double log2Cnr1 = ScorefNMLCTable[3][xn];   // log(C_n^5)
          double log2Cnr2 = ScorefNMLCTable[2][xn];   // log(C_n^4)
          double log2Cnr = 0.0;
          double k_r = std::exp((log2Cnr2 - log2Cnr1) * M_LN2);
          double q_r = 1.0 + k_r * n / (6.0 - 2.0);   // we first compute C_n^6
          for (std::size_t i = std::size_t(6); i <= r; ++i) {
            log2Cnr = log2Cnr1 + std::log2(q_r);
            log2Cnr1 = log2Cnr;
            k_r = 1.0 / q_r;
            q_r = 1.0 + k_r * (n / (i - 1.0));
          }
          return log2Cnr;
        }
      } else {
        // compute the corrected Szpankowski approximation of cn2 (see the
        // documentation of constants cst1, cst2, cst3 in the ScorefNML header)
        double log2Cnr1 =
          0.5 * std::log2(n) + __cst1 + __cst2 / std::sqrt(n) + __cst3 / n;
        if (r == std::size_t(2)) return log2Cnr1;

        // the value of log2(cn1), which is always equal to 0
        double log2Cnr2 = 0.0;

        // use Equation (13) of the paper to compute the value of cnr
        // (see the detail of the formulas in the above if statement)
        double k_r = std::exp((log2Cnr2 - log2Cnr1) * M_LN2);
        double q_r = 1.0 + k_r * n / (3.0 - 2.0);   // we first compute C_n^3
        double log2Cnr = 0.0;
        for (std::size_t i = std::size_t(3); i <= r; ++i) {
          log2Cnr = log2Cnr1 + std::log2(q_r);
          log2Cnr1 = log2Cnr;
          k_r = 1.0 / q_r;
          q_r = 1.0 + k_r * (n / (i - 1.0));
        }
        return log2Cnr;
      }
    }


    /// the function used to write the cpp file with the values of log2(Cnr)
    template < template < typename > class ALLOC >
    void ScorefNML2< ALLOC >::CTable::CnrToFile(const std::string& filename) {
      // save all the value of cn2
      std::vector< long double > cn2_table(__Szpankowski_threshold);
      cn2_table[0] = 1;
      cn2_table[1] = 2;

      // for every value of n less than Szpankowski_threshold, we compute the
      // value of C_n^2 and write it into cn2_table
      GammaLog2 gamma_log2;
      for (double n = 2; n < __Szpankowski_threshold; ++n) {
        // here, note that, in  Silander, Roos, Kontkanen and Myllymaki (2007)
        // "Factorized Normalized Maximum Likelihood Criterion for Learning
        // Bayesian Network Structures" paper, there is an uppercase N in the
        // formula, but this should be a lowercase n. In addition, we will loop
        // only on h=1 to n-1 and add to 2.0 the value computed to take into
        // account of h=0 and h=n.
        long double cn2 = 2;
        for (double h = 1; h < n; ++h) {
          long double elt =
            (gamma_log2(n + 1) - gamma_log2(h + 1) - gamma_log2((n - h) + 1))
              * M_LN2
            + h * std::log(h / n) + (n - h) * std::log((n - h) / n);
          cn2 += std::exp(elt);
        }

        // const double logCn2 = (double) std::log2 ( cn2 );
        cn2_table[n] = cn2;
      }

      // write the header of the output file
      std::ofstream outfile(filename);
      if (!outfile.is_open()) {
        GUM_ERROR(IOError, "It is impossible to open file " << filename);
      }
      outfile.precision(20);
      outfile << "namespace gum {\n\n";
      outfile << "  namespace learning {\n\n";
      outfile << "    // the size in r of ScorefNMLCTable\n";
      outfile << "    const std::size_t ScorefNMLCTableRSize = 4;\n\n";
      outfile << "    // the size in n of ScorefNMLCTable\n";
      outfile << "    const std::size_t ScorefNMLCTableNSize = "
              << __Szpankowski_threshold << ";\n\n";
      outfile << "    // the CTable for log2(Cnr), n < " << __Szpankowski_threshold
              << " and r in {2,3,4,5}\n";
      outfile << "    const double ScorefNMLCTable[4][" << __Szpankowski_threshold
              << "] = {\n";

      // write the values of Cn2:
      outfile << "      { ";
      bool first = true;
      for (const auto cn2 : cn2_table) {
        if (first)
          first = false;
        else
          outfile << ",\n        ";
        const double logCn2 = (double)std::log2(cn2);
        outfile << logCn2;
      }
      outfile << " },\n";

      // write the values of cn3, which are equal to cn2 + n
      outfile << "      { ";
      for (std::size_t i = std::size_t(0); i < __Szpankowski_threshold; ++i) {
        if (i > std::size_t(0)) outfile << ",\n        ";
        const double logCn3 = (double)std::log2(cn2_table[i] + i);
        outfile << logCn3;
      }
      outfile << " },\n";

      // write the values of cn4, which are equal to cn2 * (1 + n/2) + n
      outfile << "      { ";
      for (std::size_t i = std::size_t(0); i < __Szpankowski_threshold; ++i) {
        if (i > std::size_t(0)) outfile << ",\n        ";
        const double logCn4 =
          (double)std::log2(cn2_table[i] * (1.0 + i / 2.0) + i);
        outfile << logCn4;
      }
      outfile << " },\n";

      // write the values of cn5, which are equal to cn2 * (1 + 5n/6) + n + n^2/3
      outfile << "      { ";
      for (std::size_t i = std::size_t(0); i < __Szpankowski_threshold; ++i) {
        if (i > std::size_t(0)) outfile << ",\n        ";
        const double logCn5 = (double)std::log2(
          cn2_table[i] * (1.0 + 5.0 * i / 6.0) + i + i * i / 3.0);
        outfile << logCn5;
      }
      outfile << " }\n";

      // write the footer and close the file
      outfile << "    };\n\n";
      outfile << "  } /* namespace learning */\n\n";
      outfile << "} /* namespace gum */\n";
      outfile.close();
    }


    /// ==========================================================================
    /// ==========================================================================


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::ScorefNML2(
      const DBRowGeneratorParser< ALLOC >&                                 parser,
      const Apriori2< ALLOC >&                                             apriori,
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ScorefNML2< ALLOC >::allocator_type&           alloc) :
        Score2< ALLOC >(parser, apriori, ranges, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScorefNML2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::ScorefNML2(
      const DBRowGeneratorParser< ALLOC >&                          parser,
      const Apriori2< ALLOC >&                                      apriori,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ScorefNML2< ALLOC >::allocator_type&           alloc) :
        Score2< ALLOC >(parser, apriori, nodeId2columns, alloc),
        __internal_apriori(parser.database(), nodeId2columns) {
      GUM_CONSTRUCTOR(ScorefNML2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::ScorefNML2(
      const ScorefNML2< ALLOC >&                          from,
      const typename ScorefNML2< ALLOC >::allocator_type& alloc) :
        Score2< ALLOC >(from, alloc),
        __internal_apriori(from.__internal_apriori, alloc) {
      GUM_CONS_CPY(ScorefNML2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::ScorefNML2(const ScorefNML2< ALLOC >& from) :
        ScorefNML2< ALLOC >(from, this->getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::ScorefNML2(
      ScorefNML2< ALLOC >&&                               from,
      const typename ScorefNML2< ALLOC >::allocator_type& alloc) :
        Score2< ALLOC >(std::move(from), alloc),
        __internal_apriori(std::move(from.__internal_apriori), alloc) {
      GUM_CONS_MOV(ScorefNML2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScorefNML2< ALLOC >::ScorefNML2(ScorefNML2< ALLOC >&& from) :
        ScorefNML2< ALLOC >(std::move(from), this->getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScorefNML2< ALLOC >* ScorefNML2< ALLOC >::clone(
      const typename ScorefNML2< ALLOC >::allocator_type& alloc) const {
      ALLOC< ScorefNML2< ALLOC > > allocator(alloc);
      ScorefNML2< ALLOC >*         new_score = allocator.allocate(1);
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
    ScorefNML2< ALLOC >* ScorefNML2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    ScorefNML2< ALLOC >::~ScorefNML2< ALLOC >() {
      GUM_DESTRUCTOR(ScorefNML2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScorefNML2< ALLOC >& ScorefNML2< ALLOC >::
                         operator=(const ScorefNML2< ALLOC >& from) {
      if (this != &from) {
        Score2< ALLOC >::operator=(from);
        __internal_apriori = from.__internal_apriori;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScorefNML2< ALLOC >& ScorefNML2< ALLOC >::
                         operator=(ScorefNML2< ALLOC >&& from) {
      if (this != &from) {
        Score2< ALLOC >::operator=(std::move(from));
        __internal_apriori = std::move(from.__internal_apriori);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    std::string
      ScorefNML2< ALLOC >::isAprioriCompatible(const std::string& apriori_type,
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
           ScorefNML2< ALLOC >::isAprioriCompatible(const Apriori2< ALLOC >& apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    template < template < typename > class ALLOC >
    INLINE std::string ScorefNML2< ALLOC >::isAprioriCompatible() const {
      return isAprioriCompatible(*(this->_apriori));
    }


    /// returns the internal apriori of the score
    template < template < typename > class ALLOC >
    INLINE const Apriori2< ALLOC >& ScorefNML2< ALLOC >::internalApriori() const {
      return __internal_apriori;
    }


    /// returns the score corresponding to a given nodeset
    template < template < typename > class ALLOC >
    double ScorefNML2< ALLOC >::_score(const IdSet2< ALLOC >& idset) {
      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double, ALLOC< double > > N_ijk(
        this->_counter.counts(idset, true));
      const bool informative_external_apriori = this->_apriori->isInformative();
      if (informative_external_apriori)
        this->_apriori->addAllApriori(idset, N_ijk);
      const double all_size = (N_ijk.size());

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double, ALLOC< double > > N_ij =
          this->_counter.counts(idset.conditionalIdSet(), false);
        if (informative_external_apriori)
          this->_apriori->addConditioningApriori(idset, N_ij);
        const double      conditioning_size = double(N_ij.size());
        const std::size_t target_domsize = all_size / conditioning_size;

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
