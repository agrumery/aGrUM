
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
 * @brief the class for computing the log2 of the parametric complexity of
 * an r-ary multinomial variable
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/core/math/variableLog2ParamComplexity.h>
#  include <agrum/core/math/gammaLog2.h>
#  include <iostream>
#  include <sstream>


namespace gum {


  /// returns the allocator used by the parameterized complexity class
  template < template < typename > class ALLOC >
  INLINE typename VariableLog2ParamComplexity< ALLOC >::allocator_type
     VariableLog2ParamComplexity< ALLOC >::getAllocator() const {
    return *this;
  }


  /// default constructor
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >::VariableLog2ParamComplexity(
     const allocator_type& alloc) :
      VariableLog2ParamComplexity< ALLOC >::allocator_type(alloc),
      __cache(1000) {
    GUM_CONSTRUCTOR(VariableLog2ParamComplexity);
  }


  /// copy constructor with a given allocator
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >::VariableLog2ParamComplexity(
     const VariableLog2ParamComplexity< ALLOC >&                          from,
     const typename VariableLog2ParamComplexity< ALLOC >::allocator_type& alloc) :
      VariableLog2ParamComplexity< ALLOC >::allocator_type(alloc),
      __use_cache(from.__use_cache), __cache(from.__cache) {
    GUM_CONS_CPY(VariableLog2ParamComplexity);
  }


  /// copy constructor
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >::VariableLog2ParamComplexity(
     const VariableLog2ParamComplexity< ALLOC >& from) :
      VariableLog2ParamComplexity(from, this->getAllocator()) {}


  /// move constructor with a given allocator
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >::VariableLog2ParamComplexity(
     VariableLog2ParamComplexity< ALLOC >&&                               from,
     const typename VariableLog2ParamComplexity< ALLOC >::allocator_type& alloc) :
      VariableLog2ParamComplexity< ALLOC >::allocator_type(alloc),
      __use_cache(from.__use_cache), __cache(std::move(from.__cache)) {
    GUM_CONS_MOV(VariableLog2ParamComplexity);
  }


  /// move constructor
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >::VariableLog2ParamComplexity(
     VariableLog2ParamComplexity< ALLOC >&& from) :
      VariableLog2ParamComplexity(std::move(from), this->getAllocator()) {}


  /// virtual copy constructor
  template < template < typename > class ALLOC >
  VariableLog2ParamComplexity< ALLOC >*
     VariableLog2ParamComplexity< ALLOC >::clone(
        const typename VariableLog2ParamComplexity< ALLOC >::allocator_type& alloc)
        const {
    ALLOC< VariableLog2ParamComplexity< ALLOC > > allocator(alloc);
    VariableLog2ParamComplexity< ALLOC >*         table = allocator.allocate(1);
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
  VariableLog2ParamComplexity< ALLOC >*
     VariableLog2ParamComplexity< ALLOC >::clone() const {
    return clone(this->getAllocator());
  }


  /// destructor
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >::~VariableLog2ParamComplexity() {
    GUM_DESTRUCTOR(VariableLog2ParamComplexity);
  }


  /// copy operator
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >&
         VariableLog2ParamComplexity< ALLOC >::
         operator=(const VariableLog2ParamComplexity< ALLOC >& from) {
    return *this;
  }


  /// move operator
  template < template < typename > class ALLOC >
  INLINE VariableLog2ParamComplexity< ALLOC >&
         VariableLog2ParamComplexity< ALLOC >::
         operator=(VariableLog2ParamComplexity< ALLOC >&& from) {
    return *this;
  }


  /// indicates whether we wish to use a cache for the Cnr
  template < template < typename > class ALLOC >
  INLINE void VariableLog2ParamComplexity< ALLOC >::useCache(const bool on_off) {
    __use_cache = on_off;
  }


  /// clears the current cache
  template < template < typename > class ALLOC >
  INLINE void VariableLog2ParamComplexity< ALLOC >::clearCache() {
    __cache.clear();
  }


  /// returns the value of the log in base 2 of Cnr
  template < template < typename > class ALLOC >
  double VariableLog2ParamComplexity< ALLOC >::log2Cnr(const std::size_t r,
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

    if (n < VariableLog2ParamComplexityCTableNSize) {
      // check if we can find the value we look for in precomputed table
      // ScorefNMLVariableLog2ParamComplexity
      std::size_t xn = (std::size_t)n;
      if (r - 2 < VariableLog2ParamComplexityCTableRSize) {
        return VariableLog2ParamComplexityCTable[r - 2][xn];
      } else {
        // try to find the value in the cache
        if (__use_cache) {
          try {
            return __cache[std::pair< std::size_t, double >{r, n}];
          } catch (NotFound&) {}
        }

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
        double log2Cnr1 = VariableLog2ParamComplexityCTable[3][xn];   // log(C_n^5)
        double log2Cnr2 = VariableLog2ParamComplexityCTable[2][xn];   // log(C_n^4)
        double log2Cnr = 0.0;
        double k_r = std::exp((log2Cnr2 - log2Cnr1) * M_LN2);
        double q_r = 1.0 + k_r * n / (6.0 - 2.0);   // we first compute C_n^6
        for (std::size_t i = std::size_t(6); i <= r; ++i) {
          log2Cnr = log2Cnr1 + std::log2(q_r);
          log2Cnr1 = log2Cnr;
          k_r = 1.0 / q_r;
          q_r = 1.0 + k_r * (n / (i - 1.0));
        }

        // if we use a cache, update it
        if (__use_cache) {
          __cache.insert(std::pair< std::size_t, double >{r, n}, log2Cnr);
        }

        return log2Cnr;
      }
    } else {
      // try to find the value in the cache
      if (__use_cache) {
        try {
          return __cache[std::pair< std::size_t, double >{r, n}];
        } catch (NotFound&) {}
      }

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

      // if we use a cache, update it
      if (__use_cache) {
        __cache.insert(std::pair< std::size_t, double >{r, n}, log2Cnr);
      }

      return log2Cnr;
    }
  }


  /// the function used to write the cpp file with the values of log2(Cnr)
  template < template < typename > class ALLOC >
  void
     VariableLog2ParamComplexity< ALLOC >::CnrToFile(const std::string& filename) {
    // save all the value of cn2
    std::vector< long double > cn2_table(VariableLog2ParamComplexityCTableNSize);
    cn2_table[0] = 1;
    cn2_table[1] = 2;

    // for every value of n less than Szpankowski_threshold, we compute the
    // value of C_n^2 and write it into cn2_table
    GammaLog2 gamma_log2;
    for (double n = 2; n < VariableLog2ParamComplexityCTableNSize; ++n) {
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
    /*
      outfile << "  // the size in r of VariableLog2ParamComplexityCTable\n";
      outfile << "  const std::size_t VariableLog2ParamComplexityCTableRSize = "
      << "4;\n\n";
      outfile << "  // the size in n of VariableLog2ParamComplexityCTable\n";
      outfile << "  const std::size_t VariableLog2ParamComplexityCTableNSize = "
      << VariableLog2ParamComplexityCTableNSize << ";\n\n";
    */
    outfile << "  // the CTable cache for log2(Cnr), n < "
            << VariableLog2ParamComplexityCTableNSize << " and r in {2,3,4,5}\n";
    outfile << "  const double VariableLog2ParamComplexityCTable[4]["
            << VariableLog2ParamComplexityCTableNSize << "] = {\n";

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
    for (std::size_t i = std::size_t(0);
         i < VariableLog2ParamComplexityCTableNSize;
         ++i) {
      if (i > std::size_t(0)) outfile << ",\n        ";
      const double logCn3 = (double)std::log2(cn2_table[i] + i);
      outfile << logCn3;
    }
    outfile << " },\n";

    // write the values of cn4, which are equal to cn2 * (1 + n/2) + n
    outfile << "      { ";
    for (std::size_t i = std::size_t(0);
         i < VariableLog2ParamComplexityCTableNSize;
         ++i) {
      if (i > std::size_t(0)) outfile << ",\n        ";
      const double logCn4 = (double)std::log2(cn2_table[i] * (1.0 + i / 2.0) + i);
      outfile << logCn4;
    }
    outfile << " },\n";

    // write the values of cn5, which are equal to cn2 * (1 + 5n/6) + n + n^2/3
    outfile << "      { ";
    for (std::size_t i = std::size_t(0);
         i < VariableLog2ParamComplexityCTableNSize;
         ++i) {
      if (i > std::size_t(0)) outfile << ",\n        ";
      const double logCn5 =
         (double)std::log2(cn2_table[i] * (1.0 + 5.0 * i / 6.0) + i + i * i / 3.0);
      outfile << logCn5;
    }
    outfile << " }\n";

    // write the footer and close the file
    outfile << "  };\n\n";
    outfile << "} /* namespace gum */\n";
    outfile.close();
  }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
