/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_EM_H
#define GUM_LEARNING_DBROW_GENERATOR_EM_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/database/DBRowGeneratorWithBN.h>

namespace gum {

  namespace learning {

    /** @class DBRowGeneratorEM
     * @headerfile DBRowGeneratorEM.h <agrum/base/database/DBRowGeneratorEM.h>
     * @brief A DBRowGenerator class that returns incomplete rows as EM would do
     *
     * @ingroup learning_database
     *
     * This class is a DBRowGenerator that fills the unobserved values of the
     * nodes of interest as the EM algorithm does, i.e., by returning all the
     * possible completed rows with a weight corresponding to the probability
     * of the completion.
     * The standard usage of a DBRowGenerator is the following:
     * @code
     * // create a DatabaseTable and fill it
     * gum::learning::DBTranslatorSet set;
     * for ( int i = 0; i < 10; ++i )
     *   set.insertTranslator(gum::learning::DBTranslator4LabelizedVariable(),i);
     * gum::learning::DatabaseTable<> database ( set );
     * // fill the database
     *
     * // keep in a vector the types of the columns in the database
     * const std::vector<gum::learning::DBTranslatedValueType>
     *   column_types ( 10, gum::learning::DBTranslatedValueType::DISCRETE );
     *
     * // create the generator
     * gum::learning::DBRowGeneratorEM<> generator ( col_types );
     *
     * // parse the database and produce output rows
     * for ( auto dbrow : database ) {
     *   generator.setInputRow ( dbrow );
     *   while ( generator.hasRows() ) {
     *     const auto& output_dbrow = generator.generate ();
     *     // do something with the output dbrow
     *   }
     * }
     * @endcode
     */
    template < typename GUM_SCALAR = double >
    class DBRowGeneratorEM: public DBRowGeneratorWithBN< GUM_SCALAR > {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGeneratorEM(const std::vector< DBTranslatedValueType >& column_types,
                       const BayesNet< GUM_SCALAR >&               bn,
                       const Bijection< NodeId, std::size_t >&     nodeId2columns
                       = Bijection< NodeId, std::size_t >());

      /// copy constructor
      DBRowGeneratorEM(const DBRowGeneratorEM< GUM_SCALAR >& from);

      /// move constructor
      DBRowGeneratorEM(DBRowGeneratorEM< GUM_SCALAR >&& from);

      /// virtual copy constructor
      DBRowGeneratorEM< GUM_SCALAR >* clone() const final;

      /// destructor
      ~DBRowGeneratorEM();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGeneratorEM< GUM_SCALAR >& operator=(const DBRowGeneratorEM< GUM_SCALAR >& from);

      /// move operator
      DBRowGeneratorEM< GUM_SCALAR >& operator=(DBRowGeneratorEM< GUM_SCALAR >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// generates one ouput DBRow for each DBRow passed to method setInputRow
      virtual const DBRow< DBTranslatedValue >& generate() override final;

      /// assign a new Bayes net to the generator
      virtual void setBayesNet(const BayesNet< GUM_SCALAR >& new_bn) override final;

      /// @}


      protected:
      /// computes the rows it will provide as output
      virtual std::size_t computeRows_(const DBRow< DBTranslatedValue >& row) override final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the row used as input to generate the output DBRows
      const DBRow< DBTranslatedValue >* _input_row_{nullptr};

      /// the set of missing columns of the current row
      std::vector< std::size_t > _missing_cols_;

      /// the number of missing values in the current row
      std::size_t _nb_miss_;

      /// the joint probability of the missing variables of the current row
      Tensor< GUM_SCALAR > _joint_proba_;

      /// an instantiation over the joint proba
      Instantiation* _joint_inst_{nullptr};

      /// the row that we return if there are missing values
      /** DBRowGeneratorSets produce new rows in advance. So, when they
       * return a row, they have already computed the next row. To cope with
       * this, the DBRowGeneratorEM should have 2 filled_row: one that will
       * be used for the first row returned and one for the next one. Hence
       *  _filled_row1_ and _filled_row2_, which are filled alternatively by
       * the content of the probabilities computed. */
      DBRow< DBTranslatedValue > _filled_row1_;

      /// the row that we return if there are missing values
      DBRow< DBTranslatedValue > _filled_row2_;

      /// indicates whether we should return filled_row1 or filled_row2
      bool _use_filled_row1_{true};

      /// the weight of the original input row
      double _original_weight_;


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/base/database/DBRowGeneratorEM_tpl.h>

#endif /* GUM_LEARNING_DBROW_GENERATOR_EM_H */
