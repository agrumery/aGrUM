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
 * @brief The databases' cell translators for discretized variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <sstream>
#include <utility>
#include <vector>

#include <agrum/base/database/DBCell.h>
#include <agrum/base/database/DBTranslator4DiscretizedVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor with a discretized variable as translator
    template < typename GUM_SCALAR >
    DBTranslator4DiscretizedVariable::DBTranslator4DiscretizedVariable(
        const DiscretizedVariable< GUM_SCALAR >& var,
        const std::vector< std::string >&        missing_symbols,
        std::size_t                              max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE,
                     false,
                     missing_symbols,
                     false,
                     max_dico_entries),
        _variable_(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // copy the ticks of var into our internal variable
      const auto& ticks = var.ticks();
      for (const auto tick: ticks) {
        _variable_.addTick((float)tick);
      }

      // the the bounds of the discretized variable
      const auto lower_bound = (float)ticks[0];
      const auto upper_bound = (float)ticks.back();

      // remove all the missing symbols corresponding to a number between
      // lower_bound and upper_bound
      for (auto iter = this->missing_symbols_.beginSafe(); iter != this->missing_symbols_.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) {
          const float missing_val = std::stof(*iter);
          if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
            this->missing_symbols_.erase(iter);
          }
        }
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        // if the label corresponds to a missing value, then remove it from
        // the set of missing symbols. If, in addition, it has already
        // been entered into the back_dictionary, then, this has been done
        // because the label corresponded to a missing value, so we should
        // remove the label as well from the back_dictionary.
        if (this->missing_symbols_.exists(label)) { this->missing_symbols_.erase(label); }

        this->back_dico_.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }

    /// default constructor with a discretized variable as translator
    template < typename GUM_SCALAR >
    DBTranslator4DiscretizedVariable::DBTranslator4DiscretizedVariable(
        const DiscretizedVariable< GUM_SCALAR >& var,
        std::size_t                              max_dico_entries) :
        DBTranslator(DBTranslatedValueType::DISCRETE, false, false, max_dico_entries),
        _variable_(var.name(), var.description()) {
      // check that the variable has not too many entries
      if (var.domainSize() > max_dico_entries) {
        GUM_ERROR(SizeError, "the dictionary induced by the variable is too large")
      }

      // copy the ticks of var into our internal variable
      const auto& ticks = var.ticks();
      for (const auto tick: ticks) {
        _variable_.addTick((float)tick);
      }

      // add the content of the variable into the back dictionary
      std::size_t size = 0;
      for (const auto& label: var.labels()) {
        this->back_dico_.insert(size, label);
        ++size;
      }

      // store a copy of the variable, that should be used by method variable ()
      _real_variable_ = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4DiscretizedVariable);
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
