/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/** @file
 * @brief A DBRowGenerator class that returns exactly the rows it gets in input
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_IDENTITY_H
#define GUM_LEARNING_DBROW_GENERATOR_IDENTITY_H

#include <agrum/agrum.h>
#include <agrum/tools/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    /** @class DBRowGeneratorIdentity
     * @headerfile DBRowGeneratorIdentity.h <agrum/tools/database/DBRowGeneratorIdentity.h>
     * @brief A DBRowGenerator class that returns exactly the rows it gets
     * in input
     *
     * @ingroup learning_database
     *
     * This class is a dummy DBRowGenerator that can be essentially used for
     * debugging purposes. The DBRowGeneratorIdentity class is designed to
     * take as input DBRow instances via its method setInputRow and to ouput
     * them without any additional processing via its method generate. See
     * class DBRowGenerator for further details on how DBRowGenerator works.
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
     * gum::learning::DBRowGeneratorIdentity generator ( col_types );
     *
     * // parse the database and produce output rows
     * for ( auto dbrow : database ) {
     *   generator.setInputRow ( dbrow );
     *   const auto& output_dbrow = generator.generate ();
     *   // do something with the output dbrow
     * }
     * @endcode
     */
    class DBRowGeneratorIdentity: public DBRowGenerator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGeneratorIdentity(const std::vector< DBTranslatedValueType >& column_types);

      /// copy constructor
      DBRowGeneratorIdentity(const DBRowGeneratorIdentity& from);

      /// move constructor
      DBRowGeneratorIdentity(DBRowGeneratorIdentity&& from);

      /// virtual copy constructor
      DBRowGeneratorIdentity* clone() const final;

      /// destructor
      ~DBRowGeneratorIdentity();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGeneratorIdentity& operator=(const DBRowGeneratorIdentity& from);

      /// move operator
      DBRowGeneratorIdentity& operator=(DBRowGeneratorIdentity&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// generates one ouput DBRow for each DBRow passed to method setInputRow
      virtual const DBRow< DBTranslatedValue >& generate() final;

      /// @}


      protected:
      /// computes the rows it will provide as output
      virtual std::size_t computeRows_(const DBRow< DBTranslatedValue >& row) final;


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
#  include <agrum/tools/database/DBRowGeneratorIdentity_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DBROW_GENERATOR_IDENTITY_H */
