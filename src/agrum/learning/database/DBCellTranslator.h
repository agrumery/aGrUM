/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief The base class for all the tabular databases' cell translators
 *
 * This file contains the basis for interacting with a tabular database as
 * a translator. Every translator should derive from this class.
 *
 * To parse databases, learning algorithms use row filters that translate
 * the content of the database into an integer-encoded array. This translation
 * is performed by cell translators. Each one is specialized to perform a
 * given translation: this can be an identity translator that, given x, returns
 * x, but this can also be a discretization of x or a more complex
 * transformation like the addition of two columns of the database. To
 * simplify the design to cell translators, all those should derive from
 * DBCellTranslator. By Doing so, implementing a new cell translator
 * essentially amounts to create method "translate", which will translate the
 * content of the database, accessible through method "in" or "inputRow", into
 * an output vector accessible through method "out" or "outputRow". For
 * instance, here is how an Indentity translator over 1 column can be
 * implemented:
 * @code
 * class CellIdentity :
 *   public DBCellTranslator<1,1> { // there are 1 col in input and 1 in output
 *   public:
 *   void translate () {    // the method called to translate the column
 *     out( 0 ) = in ( 0 ); // the first element (0) in input is put into the
 *                          // the first element (0) in the output vector
 * };
 * @endcode
 * A similar way of specifying the operations to perform is through the use
 * of methods inputRow, outputRow, inputCols and outputCols:
 * @code
 * class CellIdentity :
 *   public DBCellTranslator<1,1> { // there are 1 col in input and 1 in output
 *   public:
 *   void translate () { // the method called to translate the column
 *     outputRow() [outputCols() [0]] = inputRow() [inputCols() [0]];
 * };
 * @endcode
 * The template parameters Nb_inputs and Nb_outputs indicate how many columns
 * are read from the database by the cell translator and how many it will
 * produce in the output vector.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_CELL_TRANSLATOR_H
#define GUM_LEARNING_DB_CELL_TRANSLATOR_H

#include <cstring>
#include <iostream>
#include <vector>
#include <type_traits>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/filteredRow.h>

namespace gum {

  namespace learning {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    // This empty class is used to test whether a given filter is a cell
    // translator or not, without having to take into account the template
    // parameters of class DBCellTranslator.
    struct BaseDBCellTranslator {};

#endif  // DOXYGEN_SHOULD_SKIP_THIS

    /// metaprogramming for checking whether a type is a CellTranslator
    /** To check at compile time that a type T is a DBCellTranslator, use
     * gum::isDBCellTranslator<T>::value which will be a Boolean True or
     * False */
    template<typename Type>
    struct isDBCellTranslator :
      public std::is_base_of<BaseDBCellTranslator,Type> {};
    
    
    /** @class DBCellTranslator
     * @ingroup learning_group
     * @brief The base class for all the tabular database cell translators
     *
     * To parse databases, learning algorithms use row filters that translate
     * the content of the database into an integer-encoded array. This
     *translation
     * is performed by cell translators. Each one is specialized to perform a
     * given translation: this can be an identity translator that, given x,
     *returns
     * x, but this can also be a discretization of x or a more complex
     * transformation like the addition of two columns of the database. To
     * simplify the design to cell translators, all those should derive from
     * DBCellTranslator. By Doing so, implementing a new cell translator
     * essentially amounts to create method "translate", which will translate
     *the
     * content of the database, accessible through method "in" or "inputRow",
     *into
     * an output vector accessible through method "out" or "outputRow". For
     * instance, here is how an Indentity translator over 1 column can be
     * implemented:
     * @code
     * class CellIdentity :
     *   public DBCellTranslator<1,1> { // there are 1 col in input and 1 in
     *output
     *   public:
     *   void translate () {    // the method called to translate the column
     *     out( 0 ) = in ( 0 ); // the first element (0) in input is put into
     *the
     *                          // the first element (0) in the output vector
     *   }
     * };
     * @endcode
     * A similar way of specifying the operations to perform is through the use
     * of methods inputRow, outputRow, inputCols and outputCols:
     * @code
     * class CellIdentity :
     *   public DBCellTranslator<1,1> { // there are 1 col in input and 1 in
     *output
     *   public:
     *   void translate () { // the method called to translate the column
     *     outputRow() [outputCols() [0]] = inputRow() [inputCols() [0]];
     *   }
     * };
     * @endcode
     * The template parameters Nb_inputs and Nb_outputs indicate how many
     *columns
     * are read from the database by the cell translator and how many it will
     * produce in the output vector.
     */
    template <int Nb_inputs, int Nb_outputs>
    class DBCellTranslator : public BaseDBCellTranslator {
      public:
      /// the number of DBRow cells read by the translator
      static constexpr Size input_size = Nb_inputs;

      /// the number of FilteredRow cells generated as output by the translator
      static constexpr Size output_size = Nb_outputs;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBCellTranslator() noexcept;

      /// copy constructor
      DBCellTranslator( const DBCellTranslator<Nb_inputs, Nb_outputs>& ) noexcept;

      /// move constructor
      DBCellTranslator( DBCellTranslator<Nb_inputs, Nb_outputs>&& ) noexcept;

      /// virtual copy constructor
      virtual DBCellTranslator<Nb_inputs, Nb_outputs>* copyFactory() = 0;

      /// destructor
      virtual ~DBCellTranslator() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBCellTranslator<Nb_inputs, Nb_outputs>&
      operator=( const DBCellTranslator<Nb_inputs, Nb_outputs>& from );

      /// move operator
      DBCellTranslator<Nb_inputs, Nb_outputs>&
      operator=( DBCellTranslator<Nb_inputs, Nb_outputs>&& from );

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// sets a new DBRow to translate
      /** DBRow is the type of the data read from the database */
      void setInputRow( const DBRow& row ) noexcept;

      /// sets a new FilteredRow to which the translator will write its output
      /** Every DBCellTranslator transforms a DBRow into a FilteredRow. */
      void setOutputRow( FilteredRow& row ) noexcept;

      /// sets the input DBRow's columns read by the translator
      void setInputCols( const std::vector<Idx>& cols );

      /// sets the input DBRow's columns read by the translator
      void setInputCols( Idx start ) noexcept;
      
      /// sets the output FilteredRow's columns written by the translator
      /** If the DBCellTranslator outputs N columns, then those will be written
       * in
       * the output vector at indices start, start+1, ..., start+N-1. */
      void setOutputCols( Idx start ) noexcept;

      /// returns the current input DBRow
      const DBRow& inputRow() const noexcept;

      /// returns the current output FilteredRow
      FilteredRow& outputFilteredRow() noexcept;

      /// returns the row of unsigned int of the current output FilteredRow
      std::vector<float>& outputRow() noexcept;

      /// returns the set of input DBRow's columns used by the translator
      const Idx* inputCols() const noexcept;

      /// returns the set of output FilteredRow's columns written by the
      /// translator
      const Idx* outputCols() const noexcept;

      /// returns the DBCell read at the ith input column of translator
      const DBCell& in( Idx i ) const noexcept;

      /// returns the FilteredRow cell corresponding to the ith output column
      float& out( Idx i ) noexcept;

      /// performs a translation
      virtual void translate() = 0;

      /// performs the initialization of the cell filter
      /** Upon initialization of the learning algorithms, the database is read
       * once and this reading allows to initialize the content of the
       * cellfilter. Method initialize is called for each row of the database
       * used for initialization. */
      virtual void initialize() = 0;

      /// performs a post initialization after the database parsing
      virtual void postInitialize() = 0;

      /** @brief indicates whether the translator needs an initial parsing of
       * the
       * database to initialize itself */
      virtual bool requiresInitialization() const noexcept = 0;

      /// push back the number of modalities of the variables of the output
      /// columns
      /** After initialization, most cell translators know exactly all the
       * values
       * found in the columns they translate. They can thus push back the
       * numbers
       * of such values into vector "modals". */
      virtual void modalities( std::vector<Size>& domain_sizes ) const = 0;

      /// back-translate a given output (i.e., returns its input value)
      /** @param col the column in _output_cols corresponding to the translated
       * value
       * @param translated_val the value in _output_cols of which we want to
       * know the original value (that which was actually read from the
       * database) */
      virtual std::string translateBack( Idx col, float translated_val ) const = 0;

      /// returns the size of the input for this cell translator
      Size inputSize() const noexcept;

      /// returns the size of the output for this cell translator
      Size outputSize() const noexcept;

      virtual std::string toString() const noexcept { return std::string(); }

      /// @}

      protected:
      /// the DBRow which is read as input by the translator
      const DBRow* _input_row{nullptr};

      /// the columns of the input DBRow that will be read by the translator
      Idx _input_cols[Nb_inputs];

      /// the FilteredRow to which the translator will write its output
      FilteredRow* _output_row{nullptr};

      /** @brief the columns in the output FilteredRow that will be written by
       * the translator */
      Idx _output_cols[Nb_outputs];
    };

    /// for friendly displaying the content of translators
    template <int Nb_inputs, int Nb_outputs>
    std::ostream& operator<<( std::ostream&,
                              const DBCellTranslator<Nb_inputs, Nb_outputs>& );

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/database/DBCellTranslator_tpl.h>

#endif /* GUM_LEARNING_DB_CELL_TRANSLATOR_H */
