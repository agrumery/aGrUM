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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
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
#ifndef GUM_DB_TRANSLATOR_UTILS_H
#define GUM_DB_TRANSLATOR_UTILS_H

#include <agrum/agrum.h>

#include <agrum/base/database/DBTranslator4ContinuousVariable.h>
#include <agrum/base/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/base/database/DBTranslator4IntegerVariable.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslator4NumericalDiscreteVariable.h>
#include <agrum/base/database/DBTranslator4RangeVariable.h>

namespace gum {

  namespace learning {

    namespace DBTranslators {

      /// named constructor
      /**
       * @param var the variable that the translator represents. This will
       * implicitly select the appropriate translator to be created
       * (e.g., DBTranslator4RangeVariable).
       * @param editable_dictionary For those translators that can
       * enable/disable the update of their dictionary during the reading of
       * databases (e.g., DBTranslator4LabelizedVariable), this indicates whether
       * we allow or not such updates. For DBTranslator4ContinuousVariable, this
       * corresponds to the fit_range constructor's parameter.
       * @param max_dico_entries For translators that store explicitly their
       * dictionary in memory, this parameter specifies the max number of entries
       * in this dictionary
       * @return a DBTranslator specifically designed for the variable in argument.
       * @warning DBTranslator is an abstract class. To construct one, you should
       * usually construct one of its inherited classes (e.g.,
       * DBTranslator4LabelizedVariable, DBTranslator4RangeVariable, etc.). The
       * named constructor create will construct the inherited class that suits
       * the variable passed in argument.
       * @ingroup learning_database
       */
      DBTranslator* create(const Variable& var,
                           const bool      editable_dictionary = false,
                           std::size_t     max_dico_entries
                           = std::numeric_limits< std::size_t >::max());

      /// named constructor
      /**
       * @param var the variable that the translator represents. This will
       * implicitly select the appropriate translator to be created
       * (e.g., DBTranslator4RangeVariable).
       * @param missing_symbols the set of symbols that the translator should
       * consider as corresponding to missing values in the database
       * @param editable_dictionary For those translators that can
       * enable/disable the update of their dictionary during the reading of
       * databases (e.g., DBTranslator4LabelizedVariable), this indicates whether
       * we allow or not such updates. For DBTranslator4ContinuousVariable, this
       * corresponds to the fit_range constructor's parameter.
       * @param max_dico_entries For translators that store explicitly their
       * dictionary in memory, this parameter specifies the max number of entries
       * in this dictionary
       * @return a DBTranslator specifically designed for the variable in argument.
       * @warning DBTranslator is an abstract class. To construct one, you should
       * usually construct one of its inherited classes (e.g.,
       * DBTranslator4LabelizedVariable, DBTranslator4RangeVariable, etc.). The
       * named constructor create will construct the inherited class that suits
       * the variable passed in argument.
       * @ingroup learning_database
       */
      DBTranslator* create(const Variable&                   var,
                           const std::vector< std::string >& missing_symbols,
                           const bool                        editable_dictionary = false,
                           std::size_t                       max_dico_entries
                           = std::numeric_limits< std::size_t >::max());

    } /* namespace DBTranslators */

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/database/DBTranslatorUtils_inl.h>
#endif   /* GUM_NO_INLINE */

#endif   // GUM_DB_TRANSLATOR_UTILS_H
