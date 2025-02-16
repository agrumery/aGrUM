/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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


#ifndef GUM_ALL_DISCRETE_VARIABLE_H
#define GUM_ALL_DISCRETE_VARIABLE_H
#include <memory>

#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/integerVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/numericalDiscreteVariable.h>
#include <agrum/base/variables/rangeVariable.h>

namespace gum {
  /**
   * Create a pointer on a Discrete Variable from a "fast" syntax
   *
   * @param var_description : the string in "fast" syntax
   * @param default_domain_size  : the domain size if the description does not give this
   * information. Note that if default_domain_size=1, a variable with a domain Size=1 is not an
   * error.
   * @return an unique_ptr on the new variable.
   */
  template < typename GUM_SCALAR = double >
  std::unique_ptr< DiscreteVariable > fastVariable(std::string var_description,
                                                   Size        default_domain_size);
  /**
   * Create a pointer on a Discrete Variable from a "fast" syntax
   *
   * @param var_description : the string in "fast" syntax
   * @param default_domain_size  : the domain size if the description does not give this
   * information. Note that if default_domain_size=1, a variable with a domain Size=1 is not an
   * error.
   * @return an unique_ptr on the new variable.
   */
  template < typename GUM_SCALAR = double >
  std::unique_ptr< DiscreteVariable > fastVariable(std::string        var_description,
                                                   const std::string& default_domain = "[2]");
}   // namespace gum

#include <agrum/base/variables/allDiscreteVariables_tpl.h>

#endif   // GUM_ALL_DISCRETE_VARIABLE_H
