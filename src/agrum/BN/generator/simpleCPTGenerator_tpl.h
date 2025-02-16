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


/** @file
 * @brief Source implementation of SimpleCPTGenerator.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI and Ariele-Paolo MAESANO
 *
 */
#include <ctime>

#include <agrum/BN/generator/simpleCPTGenerator.h>

namespace gum {

  // Default constructor.
  template < typename GUM_SCALAR >
  INLINE SimpleCPTGenerator< GUM_SCALAR >::SimpleCPTGenerator() : ICPTGenerator< GUM_SCALAR >() {
    GUM_CONSTRUCTOR(SimpleCPTGenerator);
  }

  // Destructor.
  template < typename GUM_SCALAR >
  INLINE SimpleCPTGenerator< GUM_SCALAR >::~SimpleCPTGenerator() {
    GUM_DESTRUCTOR(SimpleCPTGenerator);
  }

  // Generates a CPT using floats.
  // @param varID The variable id of the CPT owner.
  // @param cpt A reference on the CPT to fill.

  template < typename GUM_SCALAR >
  void SimpleCPTGenerator< GUM_SCALAR >::generateCPT(const Idx&                     varId,
                                                     const Potential< GUM_SCALAR >& cpt) const {
    cpt.random().normalizeAsCPT(varId);
  }

} /* namespace gum */
