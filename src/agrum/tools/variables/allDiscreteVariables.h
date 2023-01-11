/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#ifndef GUM_ALL_DISCRETE_VARIABLE_H
#define GUM_ALL_DISCRETE_VARIABLE_H
#include <memory>

#include <agrum/tools/variables/rangeVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/integerVariable.h>
#include <agrum/tools/variables/numericalDiscreteVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>

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
  std::unique_ptr< DiscreteVariable > fastVariable(const std::string& var_description,
                                                   Size               default_domain_size = 2);
}   // namespace gum

#include <agrum/tools/variables/allDiscreteVariables_tpl.h>

#endif   // GUM_ALL_DISCRETE_VARIABLE_H