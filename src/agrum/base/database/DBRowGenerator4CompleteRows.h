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
 * @brief A DBRowGenerator class that returns the rows that are complete
 * (fully observed) w.r.t. the nodes of interest
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_4_COMPLETE_ROWS_H
#define GUM_LEARNING_DBROW_GENERATOR_4_COMPLETE_ROWS_H

#include <agrum/agrum.h>

#include <agrum/base/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    /** @class DBRowGenerator4CompleteRows
     * @headerfile DBRowGenerator4CompleteRows.h <agrum/base/database/DBRowGenerator4CompleteRows.h>
     * @brief A DBRowGenerator class that returns the rows that are complete
     * (fully observed) w.r.t. the nodes of interest
     *
     * @ingroup learning_database
     *
     * This class is a DBRowGenerator that returns all the rows that are
     * complete (i.e., fully observed) w.r.t. the nodes of interest. In other
     * words, whenever the values of the nodes of interest are observed in a
     * row, this one is returned, but if at least one node is unobserved, the
     * generator does not return anything. This class is useful for bootstraping
     * EM algorithms.
     *
     * The standard usage of a DBRowGenerator is the following:
     * @code
     * // create a DatabaseTable and fill it
     * gum::learning::DBTranslatorSet set;
     * for ( int i = 0; i < 10; ++i )
     *   set.insertTranslator(gum::learning::DBTranslator4LabelizedVariable(),i);
     * gum::learning::DatabaseTable database ( set );
     * // fill the database
     *
     * // keep in a vector the types of the columns in the database
     * const std::vector<gum::learning::DBTranslatedValueType>
     *   column_types ( 10, gum::learning::DBTranslatedValueType::DISCRETE );
     *
     * // create the generator
     * gum::learning::DBRowGenerator4CompleteRows generator ( col_types );
     *
     * // parse the database and produce output rows
     * for ( auto dbrow : database ) {
     *   generator.setInputRow ( dbrow );
     *   if ( generator.hasRows() ) {
     *     const auto& output_dbrow = generator.generate ();
     *     // do something with the output dbrow
     *   }
     * }
     * @endcode
     */
    class DBRowGenerator4CompleteRows: public DBRowGenerator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGenerator4CompleteRows(const std::vector< DBTranslatedValueType >& column_types);

      /// copy constructor
      DBRowGenerator4CompleteRows(const DBRowGenerator4CompleteRows& from);

      /// move constructor
      DBRowGenerator4CompleteRows(DBRowGenerator4CompleteRows&& from);

      /// virtual copy constructor
      DBRowGenerator4CompleteRows* clone() const final;

      /// destructor
      ~DBRowGenerator4CompleteRows();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGenerator4CompleteRows& operator=(const DBRowGenerator4CompleteRows& from);

      /// move operator
      DBRowGenerator4CompleteRows& operator=(DBRowGenerator4CompleteRows&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// generates one output DBRow for each DBRow passed to method setInputRow
      /** @warning if this method is applied while the row it should return is
       * incomplete w.r.t. the nodes of interest, its behavior is uncertain
       * and will certainly result in a segmentation fault */
      const DBRow< DBTranslatedValue >& generate() final;

      /// @}


      protected:
      /// computes the rows it will provide as output
      std::size_t computeRows_(const DBRow< DBTranslatedValue >& row) final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the row used as input to generate the output DBRows
      const DBRow< DBTranslatedValue >* _input_row_{nullptr};

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/base/database/DBRowGenerator4CompleteRows_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DBROW_GENERATOR_4_COMPLETE_ROWS_H */
