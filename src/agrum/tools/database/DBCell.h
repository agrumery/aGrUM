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
 * @brief The class representing the original values of the cells of databases
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_CELL_H
#define GUM_LEARNING_DB_CELL_H

#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <agrum/agrum.h>
#include <agrum/tools/core/bijection.h>

namespace gum {

  namespace learning {

    /** @class DBCell
     * @headerfile DBCell.h <agrum/tools/database/DBCell.h>
     * @brief The class representing the original values of the cells
     * of databases
     *
     * Learning algorithms read input data, the so-called datasets, and
     * infer their models. To be fast, they should not parse directly
     * the original datasets, which are often databases of strings, but rather
     * they should parse preprocessed data with types more suited for fast
     * analysis. To do so, in aGrUM, learning algorithms parse datasets of
     * DBTranslatedValue instances. However, sometimes, these algorithms make
     * some decisions which, afterwards, they may realize were not so good and
     * should be modified. For instance, when coping with continuous variables,
     * they may use a discretization for fast learning the structure of a hybrid
     * Bayesian network. But, given the structure found, they may try to find
     * a better discretization. With a new discretization, the original dataset
     * should be parsed again and mapped into new DBTranslatedValue instances.
     * Unfortunately, reparsing, say, a CSV file or a SQL database, is time
     * consuming. Sometimes, it is faster to read such datasets once and store
     * them in an appropriate form that can be mapped easily into
     * DBTranslatedValue instances. The RawDatabaseTable class is made precisely
     * for this purpose and can be thought of as a 2-dimensional table, the
     * elements of which are DBCell instances. The latter can actually encode
     * compactly integers, real numbers, strings and even missing values. So they
     * are very well suited to be the type of the cells of the RawDatabaseTable
     * class.
     *
     * @ingroup learning_database */
    class DBCell {
      public:
      /// the set of types possibly taken by the last element read
      enum class EltType : unsigned char {
        REAL,
        INTEGER,
        STRING,
        MISSING
      };

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor (ontains a missing value)
      DBCell();

      /// constructor for a real number
      DBCell(const float nb);

      /// constructor for an integer number
      DBCell(const int nb);

      /// constructor for a string
      DBCell(const std::string& str);

      /// copy constructor
      DBCell(const DBCell& from);

      /// move constructor
      DBCell(DBCell&& from);

      /// destructor
      ~DBCell();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBCell& operator=(const DBCell& from);

      /// move operator
      DBCell& operator=(DBCell&& from);

      /// assignment operator
      DBCell& operator=(const float x);

      /// assignment operator
      DBCell& operator=(const int x);

      /// assignment operator
      DBCell& operator=(const std::string& x);

      /// test of equality
      bool operator==(const DBCell& from) const;

      /// test of inequality
      bool operator!=(const DBCell& from) const;

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the current type of the DBCell
      EltType type() const noexcept;

      /// try to convert the content of the DBCell into another type
      /** @return true if the conversion has been successfully performed. */
      bool convertType(const EltType newtype);

      /// returns the DBcell as a real number
      /** @warning if the cell is not of type REAL, the dbcell will not try to
       * convert its content into a real number, it will raise a TypeError
       * exception.
       * @return the content of the DBCell (if this is a real number)
       * @throw TypeError if the DBCell is not labelled as type REAL */
      float real() const;

      /// sets the content of the DBCell
      void setReal(const float x);

      /// sets the content of the DBCell from a string
      /** @throw TypeError if the string does not correspond to a real number */
      void setReal(const std::string& elt);

      /// returns the DBcell as an integer
      /** @warning if the cell is not of type INTEGER, the dbcell will not try
       * to convert its content into a float, it will raise a TypeError
       * exception.
       * @throw TypeError if the DBCell is not labelled as type INTEGER */
      int integer() const;

      /// sets the content of the DBCell
      void setInteger(const int x);

      /// sets the content of the DBCell from a string
      /** @throw TypeError if the string does not correspond to an integer */
      void setInteger(const std::string& elt);

      /// returns the DBcell as a string
      /** @warning if the cell is not of type STRING, the dbcell will not try to
       * convert its content into a string, it will raise a TypeError exception.
       * @throw TypeError if the DBCell is not labelled as type STRING */
      const std::string& string() const;

      /** @brief returns the DBcell as the index of a string in a static
       * bijection
       *
       * All strings referenced by DBCells are stored in a bijection. This method
       * returns the index of the string referenced by the current DBCell.
       * @throw TypeError if the DBCell is not labelled as type STRING */
      int stringIndex() const;

      /// sets the content of the DBCell
      void setString(const std::string& elt);

      /// sets the DBCell as a missing element
      void setMissingState();

      /// indicates whether the cell contains a missing value
      bool isMissing() const;

      /// @}


      // ##########################################################################
      /// @name Public Static Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// strings are stored into a static bijection. Get its ith string
      /** @throw UndefinedElement if the index does not correspond to
       * any string */
      static const std::string& string(const int index);

      /// returns the best type to store a given element encoded as a string
      /** @param str the string to convert into a DBCell
       * @param missingVals a vector containing the set of strings that should
       * be interpreted as missing values. Whenever str matches one these strings,
       * the returned EltType represents a missing value. */
      static EltType bestType(const std::string&                str,
                              const std::vector< std::string >& missingVals);

      /// returns the DBCell with the best type for an element encoded as a string
      /** @param str the string to convert into a DBCell
       * @param missingVals a vector containing the set of strings that should
       * be interpreted as missing values. Whenever str matches one these strings,
       * the returned DBCell represents a missing value. */
      static DBCell bestDBCell(const std::string&                str,
                               const std::vector< std::string >& missingVals);

      /// returns the content of the DBCell as a string, whatever its type
      /** @throw UndefinedElement is raised if the DBCell corresponds to a
       * missing value but the set of missing values passed in argument is
       * empty. */
      std::string toString(const std::vector< std::string >& missingVals) const;

      /// determines whether a string corresponds precisely to an integer
      static bool isInteger(const std::string& str);

      /// determine whether a string corresponds precisely to a real number
      static bool isReal(const std::string& str);

      /// checks whether a string correspond to a missing value
      static bool isMissing(const std::string& str, const std::vector< std::string >& missingVals);

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the real type of the last element read from the database
      EltType _type_{EltType::MISSING};

      // the element read from the database
      union {
        int   _val_index_;     // stores string indices. Basically, it should have
        int   _val_integer_;   // been an Idx, but int are shorter than Idx.
        float _val_real_;
      };


      // determine the longest type of the union. This is used for fast
      // copying/moving DBCells
      using UnionType = typename std::conditional< sizeof(int) < sizeof(float), float, int >::type;

      // raises an appropriate exception when encountering a type error
      std::string _typeErrorMsg_(const std::string& real_type) const;


      // a bijection assigning to each string index its corresponding string
      static Bijection< std::string, int >& _strings_();

      // the last index used so far
      static int _string_max_index_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBCell_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_CELL_H */
