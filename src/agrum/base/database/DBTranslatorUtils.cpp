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


/** @file
 * @brief Utilities for DBTranslators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/database/DBTranslatorUtils.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/DBTranslatorUtils_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    namespace DBTranslators {

      /// named constructor using dynamic allocation
      DBTranslator* create(const Variable&                   var,
                           const std::vector< std::string >& missing_symbols,
                           const bool                        editable_dictionary,
                           std::size_t                       max_dico_entries) {
        switch (var.varType()) {
          case VarType::LABELIZED : {
            const LabelizedVariable& xvar = static_cast< const LabelizedVariable& >(var);
            return new DBTranslator4LabelizedVariable(xvar,
                                                      missing_symbols,
                                                      editable_dictionary,
                                                      max_dico_entries);
          }

          case VarType::INTEGER : {
            const auto& xvar = static_cast< const IntegerVariable& >(var);

            return new DBTranslator4IntegerVariable(xvar, missing_symbols, max_dico_entries);
          }

          case VarType::DISCRETIZED : {
            const auto& xvar = static_cast< const IDiscretizedVariable& >(var);
            return new DBTranslator4DiscretizedVariable(xvar, missing_symbols, max_dico_entries);
          }

          case VarType::RANGE : {
            const auto& xvar = static_cast< const RangeVariable& >(var);
            return new DBTranslator4RangeVariable(xvar,
                                                  missing_symbols,
                                                  editable_dictionary,
                                                  max_dico_entries);
          }

          case VarType::CONTINUOUS : {
            const auto& xvar = static_cast< const IContinuousVariable& >(var);
            return new DBTranslator4ContinuousVariable(xvar, missing_symbols, editable_dictionary);
          }

          case VarType::NUMERICAL : {
            const auto& xvar = static_cast< const NumericalDiscreteVariable& >(var);
            return new DBTranslator4NumericalDiscreteVariable(xvar,
                                                              missing_symbols,
                                                              max_dico_entries);
          }
        }

        GUM_ERROR(NotImplementedYet,
                  "The translator for Variable "
                      << var.name()
                      << " cannot yet be created yet by named constructor CreatePtr.");
      }

    } /* namespace DBTranslators */

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
