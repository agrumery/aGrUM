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
 * @brief the class for computing the log2 of the parametric complexity of
 * an r-ary multinomial variable
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_VARIABLE_LOG2_PARAM_COMPLEXITY_H
#define GUM_VARIABLE_LOG2_PARAM_COMPLEXITY_H

#include <cstddef>
#include <string>
#include <fstream>

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/hashTable.h>

namespace gum {


  // the CTable cache for log2(C_n^r), with n in {0,...,999} and r in {2,3,4,5}
  extern const double VariableLog2ParamComplexityCTable[4][1000];

  // the size in r of the CTable cache
  constexpr std::size_t VariableLog2ParamComplexityCTableRSize{std::size_t(4)};

  // the size in n of the CTable cache
  constexpr std::size_t VariableLog2ParamComplexityCTableNSize{std::size_t(1000)};


  /** @class VariableLog2ParamComplexity
   * @brief the class for computing the log2 of the parametric complexity
   * of an r-ary multinomial variable
   * @headerfile VariableLog2ParamComplexity.h <agrum/tools/core/math/VariableLog2ParamComplexity.h>
   * @ingroup math_group
   *
   * This class enables to compute the log in base 2 of the parametric
   * complexity of a single r-ary multinomial variable, i.e., the log in
   * base 2 of the C_N^r term used by NML scores in Bayesian network
   * structure learning algorithm (see, e.g., Silander, Roos,
   * Kontkanen and Myllymaki (2007) "Factorized Normalized Maximum "
   * Likelihood Criterion for Learning Bayesian network Structures)"
   */
  template < template < typename > class ALLOC = std::allocator >
  class VariableLog2ParamComplexity: private ALLOC< double > {
    public:
    /// type for the allocators passed in arguments of methods
    using allocator_type = ALLOC< double >;

    // ########################################################################
    /// @name Constructors / Destructors
    // ########################################################################
    /// @{

    /// default constructor
    VariableLog2ParamComplexity(const allocator_type& alloc = allocator_type());

    /// copy constructor
    VariableLog2ParamComplexity(const VariableLog2ParamComplexity& from);

    /// copy constructor with a given allocator
    VariableLog2ParamComplexity(const VariableLog2ParamComplexity& from,
                                const allocator_type&              alloc);

    /// move constructor
    VariableLog2ParamComplexity(VariableLog2ParamComplexity&& from);

    /// move constructor with a given allocator
    VariableLog2ParamComplexity(VariableLog2ParamComplexity&& from,
                                const allocator_type&         alloc);

    /// virtual copy constructor
    virtual VariableLog2ParamComplexity* clone() const;

    /// virtual copy constructor with a given allocator
    virtual VariableLog2ParamComplexity* clone(const allocator_type& alloc) const;

    /// destructor
    virtual ~VariableLog2ParamComplexity();

    /// @}


    // ########################################################################
    /// @name Operators
    // ########################################################################
    /// @{

    /// copy operator
    VariableLog2ParamComplexity&
       operator=(const VariableLog2ParamComplexity& from);

    /// move operator
    VariableLog2ParamComplexity& operator=(VariableLog2ParamComplexity&& from);

    /// @}


    // ########################################################################
    /// @name Accessors / Modifiers
    // ########################################################################
    /// @{

    /// returns the value of the log in base 2 of Cnr
    double log2Cnr(const std::size_t r, const double n);

    /// the function used to write the cpp file with the values of log2(Cnr)
    void CnrToFile(const std::string& filename);

    /// indicates whether we wish to use a cache for the Cnr
    void useCache(const bool on_off);

    /// clears the current cache
    void clearCache();

    /// returns the allocator used by the parameterized complexity class
    allocator_type getAllocator() const;


    /// @}

    private:
    /// the value of N above which we should use Szpankowski's approximation
    const double Szpankowski_threshold__{VariableLog2ParamComplexityCTableNSize};

    // constants used to speed-up the computation of the Szpankowski
    // approximation.
    // The formula for the approximation given in Silander, Roos,
    // Kontkanen and Myllymaki (2007) "Factorized Normalized Maximum "
    // Likelihood Criterion for Learning Bayesian network Structures" paper
    // is incorrect. However, the one in Kontkanen, Buntine, Myllymaki,
    // Rissanen and Tirri (2003) "Efficient Computation of Stochastic
    // Complexity" is correct. So we use the latter and simplify it. Thus,
    // the approximation of log2(Cnr) is equal to:
    // 0.5 log2(n) - 0.5 + log2(sqrt(pi)) + (sqrt(2/pi)/3) / sqrt(n) +
    // (3/36 - 4/(9*pi)) / n.
    // So, given the constants below, it is equal to:
    // 0.5 * std::log2 (n) + cst1__ + cst2__ / std::sqrt(n) + cst3__ / n
    const double cst1__ = -0.5 + std::log2(std::sqrt(M_PI));
    const double cst2__ = std::sqrt(2.0 / M_PI) / 3.0;
    const double cst3__ = 3.0 / 36.0 - 4.0 / (9.0 * M_PI);

    // indicates whether we should use a cache or not
    bool use_cache__{true};

    // the cache used, eventually, to store the log2Cnr values
    HashTable< std::pair< std::size_t, double >, double > cache__;
  };

} /* namespace gum */


// always include the template implementation
#include <agrum/tools/core/math/variableLog2ParamComplexity_tpl.h>

#endif /* GUM_VARIABLE_LOG2_PARAM_COMPLEXITY_H */
