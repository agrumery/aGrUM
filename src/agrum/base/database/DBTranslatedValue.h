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
 * @brief The union class for storing the translated values in learning databases
 *
 * When learning models from databases, it is necessary to preprocess the
 * data, i.e., transform them in such a way that their analysis can be
 * performed as quickly as possible. The purpose of the DBTranslatedValue type
 * is to enable such a fast analysis.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_TRANSLATED_VALUE_H
#define GUM_LEARNING_DB_TRANSLATED_VALUE_H

#include <cstddef>

#include <agrum/agrum.h>

namespace gum {

  namespace learning {

    /** @union DBTranslatedValue
     * @headerfile DBTranslatedValue.h <agrum/base/database/DBTranslatedValue.h>
     * @brief The union class for storing the translated values in learning
     * databases
     *
     * In aGrUM, learning is performed on datasets that are preprocessed, i.e.,
     * their values are encoded in such a way that learning is fast. More
     * precisely, the values of discrete random variables are encoded as
     * integers ranging from 0 to the domain size minus 1 of the variable, and
     * the values of continuous random variables are encoded as floating point
     * numbers. Missing values are encoded as std::numeric_limits<>::max() over
     * these types. The DBTranslatedValue class is the representation of these
     * types.
     *
     * @par Usage example:
     * @code
     * // create a discrete value equal to 3 and a continuous value equal to 4.5
     * gum::learning::DBTranslatedValue x1 { std::size_t(3) };
     * gum::learning::DBTranslatedValue x2 { 4.5f };
     *
     * // access these elements and display them
     * std::cout << x1.discr_val << "  " << x2.cont_val << std::endl;
     * @endcode
     *
     * @ingroup learning_database */
    union DBTranslatedValue {
      /// the field for storing discrete values
      std::size_t discr_val;

      /// the field for storing continuous values
      float cont_val;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor: stores discrete value 0
      constexpr DBTranslatedValue() : discr_val{std::size_t(0)} {}

      /// the constructor for storing a continuous value
      constexpr DBTranslatedValue(const float& val) : cont_val{val} {}

      /// the constructor for storing a discrete value
      constexpr DBTranslatedValue(const std::size_t& val) : discr_val{val} {}

      /// destructor
      ~DBTranslatedValue() {};

      /// @}
    };


    /** @enum DBTranslatedValueType
     * @headerfile DBTranslatedValue.h <agrum/base/database/DBTranslatedValue.h>
     * @brief The nature of the elements handled by translators (discrete,
     * continuous).
     *
     * @code
     * #include <agrum/base/database/DBTranslatedValue.h>
     * @endcode
     *
     * Currently, a DBTranslatorType is one of:
     * - DBTranslatedValueType::DISCRETE
     * - DBTranslatedValueType::CONTINUOUS
     *
     * Each DBTranslator has a DBTranslatedValueType that indicates whether it
     * handles a discrete or a continuous variable. As a result, when the
     * DBTranslator translates a string into a DBTranslatedValue, to access the
     * content of the latter, you should use either the discr_val field of the
     * DBTranslatedValue if the DBTranslatedValueType of the translator is
     * DISCRETE or the cont_val field if the translator is CONTINUOUS.
     *
     * @ingroup learning_database
     */
    enum class DBTranslatedValueType : char { DISCRETE, CONTINUOUS };


  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_DB_TRANSLATED_VALUE_H */
