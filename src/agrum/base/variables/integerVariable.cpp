/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <agrum/base/variables/integerVariable.h>

#include <agrum/base/core/math/math_utils.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/variables/integerVariable_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  /// constructor assigning a domain to the variable
  IntegerVariable::IntegerVariable(const std::string&        aName,
                                   const std::string&        aDesc,
                                   const std::vector< int >& domain) :
      DiscreteVariable(aName, aDesc) {
    // get the values in increasing order
    for (const auto value: domain) {
      if (!gum::isfinite< double >(value)) {
        GUM_ERROR(DefaultInLabel,
                  "Value '" << value << "' is not allowed for variable " << toString())
      }
      if (!isValue(value)) { _domain_.push_back(value); }
    }
    std::sort(_domain_.begin(), _domain_.end());

    // for debugging purposes
    GUM_CONSTRUCTOR(IntegerVariable)
  }

  /// constructor assigning a domain to the variable
  IntegerVariable::IntegerVariable(const std::string& aName,
                                   const std::string& aDesc,
                                   int                first,
                                   int                last,
                                   Size               nb) : DiscreteVariable(aName, aDesc) {
    // store the sorted values into a sequence
    if (nb < 2) GUM_ERROR(ArgumentError, "The size of the domain must be >2 (here :" << nb << ").")
    if (first >= last)
      GUM_ERROR(ArgumentError, "first (here :" << first << " must be <last (here :" << last << ").")

    double step = (last - first) / double(nb - 1);
    if (step <= 1)
      GUM_ERROR(ArgumentError,
                "With nb=" << nb << ", increment is less (or equal) than 1 ! (" << step << ")")

    _domain_.resize(nb);
    _domain_.clear();

    _domain_.push_back(first);
    double current = first;
    for (Idx i = 1; i < nb - 1; i++) {
      current += step;
      _domain_.push_back(int(current));
    }
    _domain_.push_back(last);

    std::sort(_domain_.begin(), _domain_.end());

    // for debugging purposes
    GUM_CONSTRUCTOR(IntegerVariable)
  }

  /// Returns the domain as a string
  std::string IntegerVariable::domain() const {
    std::stringstream s;
    const Size        size = domainSize();

    s << "{";
    if (size > 0) {
      s << _domain_[0];
      for (Idx i = 1; i < size; ++i)
        s << '|' << _domain_[i];
    }
    s << "}";
    return s.str();
  }
}   // namespace gum
