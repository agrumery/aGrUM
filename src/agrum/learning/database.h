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
 * @brief Class for storing and quickly parsing (small) databases
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_DATABASE_H
#define GUM_DATABASE_H

#include <string>
#include <vector>
#include <agrum/core/utils.h>
#include <agrum/core/list.h>
#include <agrum/core/hashTable.h>


namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  class Database;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} /* namespace gum */


int  GUM_PARSE_XML_CSV_parse( gum::Database& );
int  GUM_PARSE_XML_CSV_error( gum::Database&, const char* );
bool GUM_PARSE_XML_CSV_parser( gum::Database&, const std::string& );


namespace gum {


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                   CLASS FOR PARSING DATABASE QUERIES                 === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class DatabaseIterator
   * @brief iterator class for parsing quickly databases
   * @ingroup learning_group
   */
  /* ============================================================================ */

  class DatabaseIterator {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

      // ============================================================================
      /// creates an iterator pointing toward nothing
      // ============================================================================
      DatabaseIterator();

      // ============================================================================
      /// basic constructor points toward the nb_caseth case of the database
      // ============================================================================
      DatabaseIterator( const Database& data, unsigned int nb_case );

      // ============================================================================
      /// copy constructor
      // ============================================================================
      DatabaseIterator( const DatabaseIterator& from );

      // ============================================================================
      /// destructor
      // ============================================================================
      ~DatabaseIterator();

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{

      // ============================================================================
      /// detach the iterator from the database it points to
      // ============================================================================
      void clear();

      // ============================================================================
      /// makes the iterator point toward the next case (safe, \sa operator++)
      /** @throws OutOfBound is thrown when we are already at the end of the
       * database or when the database is empty. */
      // ============================================================================
      DatabaseIterator& nextCase();

      // ============================================================================
      /// makes the iterator point toward the preceding case (safe, \sa operator--)
      /** @throws OutOfBound is thrown when we are already at the beginning of the
       * database or when the database is empty. */
      // ============================================================================
      DatabaseIterator& prevCase();

      // ============================================================================
      /// returns the value of the ith node in the current case (safe, \sa operator[])
      /** In a Database, values of the fields/variables are actually stored as
       * integers: 0 for the first value, 1 for the second and so on. Method at
       * returns the index (the integer) corresponding to the value of the ith
       * variable of the database of the case pointed to by the DatabaseIterator.
       * @throws NotFound exception is raised if the database is empty or if i is
       * greater than or equal to the number of cases in the database. */
      // ============================================================================
      unsigned int at( unsigned int i ) const;

      /// @}


      // ############################################################################
      /// @name Operators
      // ############################################################################
      /// @{

      // ============================================================================
      /// copy operator
      // ============================================================================
      DatabaseIterator& operator= ( const DatabaseIterator& from );

      // ============================================================================
      /// makes the iterator point toward the next case (fast but unsafe)
      // ============================================================================
      DatabaseIterator& operator++ ();

      // ============================================================================
      /// makes the iterator point toward the previous case (fast but unsafe)
      // ============================================================================
      DatabaseIterator& operator-- ();

      // ============================================================================
      /// check whether two iterators point toward the same case
      // ============================================================================
      bool operator== ( const DatabaseIterator& ) const throw();

      // ============================================================================
      /// check whether two iterators point toward different cases
      // ============================================================================
      bool operator!= ( const DatabaseIterator& ) const throw();

      // ============================================================================
      /// returns the value of the ith node/index in the current case (fast but unsafe)
      /** In a Database, values of the fields/variables are actually stored as
       * integers: 0 for the first value, 1 for the second and so on. Operator[]
       * returns the index (the integer) corresponding to the value of the ith
       * variable of the database of the case pointed to by the DatabaseIterator. */
      // ============================================================================
      unsigned int operator[]( unsigned int i ) const;

      /// @}


    private:
      /// type used to create the begin/rbegin/end/rend iterators
      enum IteratorLocation {
        GUM_DATABASE_ITER_BEGIN,
        GUM_DATABASE_ITER_RBEGIN,
        GUM_DATABASE_ITER_END,
        GUM_DATABASE_ITER_REND
      };

      /// the database the iterator points to
      const Database *database;

      /// a pointer to the case the iterator points to
      const unsigned int *current_case;

      /// the offset by which we ++ or -- in the database cases vector
      unsigned int offset;

      /// Database should be able to update the iterators

      friend class Database;

      // ============================================================================
      /// initializers used for begin/rbegin/end/rend
      // ============================================================================
      void initializeIterator( const Database& data, enum IteratorLocation );
  };






  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                  CLASS FOR MANAGING DATABASE QUERIES                 === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class Database
   * Database
   * @ingroup learning_group
   */
  /* ============================================================================ */

  class Database {
    public:
      typedef DatabaseIterator iterator;

      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

      // ============================================================================
      /// copy constructor
      // ============================================================================
      Database( const Database& from );

      // ============================================================================
      /// copy operator
      // ============================================================================
      Database& operator= ( const Database& from );

      // ============================================================================
      /// destructor
      // ============================================================================
      ~Database();

      // ============================================================================
      /// creates a new database from a pure CSV file
      /** The assumed format of the CSV file is the following: the first line contains
       * the names of the nodes/fields, and the following lines contain the content
       * of the database.
       *
       * Missing values are just empty fields ",,"
       * @warning Missing values are represented by a label "?". Thus, if there are missing
      * values, the number of modalities take into account this "?" label.
       */
      // ============================================================================
      static Database createFromCSV( const std::string& filename,
                                     char field_separator = ';',
                                     char field_delimiter = '"',
                                     char escape_char = '\\' );

      // ============================================================================
      /// creates a new database from a mixed XML/CSV file
      /** The format of the file is the following:
       * - the whole file is enclosed in tags <DATABASE></DATABASE>
       * - then variables are given as <VARIABLE name="xxx"></VARIABLE>
       * - whithin <VARIABLE>'s tags, modalities are specified as:
       *   <MODALITY name="xxx" id="integer" /> or <MODALITY name="xxx"/>
       *   for each modality, it is compulsory to fill its name.
       *   However, you can avoid the id tag. In this case, an id is generated as
       *   1 + preceding id if it exists, else 0.
       * - finally, cases of the database are specified within <DATA></DATA>.
       * - Each case is given within as a list of numbers separated by blanks within
       *   <CASE></CASE> tags. These numbers correspond to the id's of the modalities
       *   of the variables. The first id given correspond to the first variable
       *   specified within <VARIABLE></VARIABLE> tags, the second id to the second
       *   variable and so on. For instance:
       *   @code
       *   <DATABASE>
       *     <VARIABLE name="xxx">
       *       <MODALITY name="toto" id="4" />
       *       <MODALITY name="titi" id="6"/>
       *     </VARIABLE>
       *     <VARIABLE name="yyy">
       *       <MODALITY name="a" id="1" />
       *       <MODALITY name="b"/>
       *     </VARIABLE>
       *     <VARIABLE name="zzz">
       *       <MODALITY name="cc" id="20" />
       *       <MODALITY name="dd" id="15"/>
       *     </VARIABLE>
       *     <DATA>
       *     <CASE>4 2 20</CASE>
       *     <CASE>6 1 15</CASE>
       *     </DATA>
       *   </DATABASE>
       *   @endcode
       *   corresponds to a database with 2 rows (cases) and 3 columns (variables).
       *   In the first line, variable "xxx"'s value is "toto", variable "yyy"'s value
       *   is "b" and variable "zzz"'s value is "cc".
			 *
			 *   Missing value are represented by "?"
			 */
      // ============================================================================
      static Database createFromXmlCSV( const std::string& filename );

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{

      // ============================================================================
      /// get the ncaseth case in the database
      /** the first case is in position 0. A case is encoded as an array of
       * getNbrNodes() integers. These correspond to the values of the nodes/fields
       * of the database.
       * @throw NotFound exception is thrown if the case cannot be found
       * (ncase is too big). */
      // ============================================================================
      const unsigned int* line( unsigned int ncase ) const throw( NotFound );

      // ============================================================================
      /// get the value of the ith node in the nth case of the database
      /** the first case is in position 0.
       * @throw NotFound exception is thrown if the value cannot be found,
       * i.e., if n and/or i is/are too big. */
      // ============================================================================
      unsigned int value( unsigned int n, unsigned int i )
      const throw( NotFound );

      // ============================================================================
      /// returns the name of the kth node in the database
      // ============================================================================
      const std::string& nodeName( unsigned int k ) const throw( NotFound );

      // ============================================================================
      /// returns the names of all the nodes
      // ============================================================================
      const std::vector<std::string>& nodeNames() const;

      // ============================================================================
      /// returns the names of all the modalities of a given node
      // ============================================================================
      const std::vector<std::string>& modalitiesNames( unsigned int k )
      const throw( NotFound );

      // ============================================================================
      /// returns the kth modality name of the ith node
      // ============================================================================
      const std::string& modalityName( unsigned int k, unsigned int i )
      const throw( NotFound );

      // ============================================================================
      /// get the number of modalities of the kth node
			/// @warning : including "?" label if there are missing values
      // ============================================================================
      unsigned int nbrModalities( unsigned int k ) const throw( NotFound );

      // ============================================================================
      /// get the number of modalities of all the nodes
      // ============================================================================
      const std::vector<unsigned int>& nbrModalities() const throw();

      // ============================================================================
      /// get the number of cases in the database
      // ============================================================================
      unsigned int nbrLines() const throw();

      // ============================================================================
      /// get the number of nodes in the database
      // ============================================================================
      unsigned int nbrNodes() const throw();

      /// is there any missing value in the database
      bool hasMissingValue() const;

      /// is there any missing value for this row
      bool hasMissingValue( unsigned int k ) const;

      /// @}


      // ############################################################################
      /// @name Iterators
      // ############################################################################
      /// @{

      // ============================================================================
      /// returns an iterator pointing to the beginning of the database
      // ============================================================================
      const DatabaseIterator& begin() const throw();

      // ============================================================================
      /// returns an iterator pointing just after the last element of the database
      // ============================================================================
      const DatabaseIterator& end() const throw();

      // ============================================================================
      /// returns an iterator pointing to the last element of the database
      // ============================================================================
      const DatabaseIterator& rbegin() const throw();

      // ============================================================================
      /// returns an iterator pointing just before the first element of the database
      // ============================================================================
      const DatabaseIterator& rend() const throw();

      /// @}


    protected:
      // ============================================================================
      /// basic constructor
      /** the idea is not to use this constructor but rather the "named constructors"
       * whoch know about the formats of the input data */
      // ============================================================================
      Database();


    private:
      /// the number of nodes
      unsigned int __nb_nodes;

      /// the number of cases in the database
      unsigned int __nb_cases;

      /// the list of nodes of the database
      std::vector<std::string> __node_names;

      /// a hashtable indicating for each node name its field index (ID)
      HashTable<std::string,unsigned int> __node_name_per_id;

      /// the number of modalities of the nodes
      std::vector<unsigned int> __nb_modalities;

      /// a flag for missing values for each node
      std::vector<bool> __missing_value;


      /// modalities_names[i][j] = name of the jth modality of the ith node
      std::vector< std::vector<std::string> > __modalities_names;

      /** @brief the cases stored into the database. They are stored in the order
       * the values are read */
      unsigned int *__cases;

      /// pseudo iterators used to speed-up parsings
      DatabaseIterator __iter_begin;
      DatabaseIterator __iter_end;
      DatabaseIterator __iter_rbegin;
      DatabaseIterator __iter_rend;

      /// the list of iterators pointing toward the current database
      mutable List<DatabaseIterator*> __iterators;

      /// iterators should be able to access the internals of the database

      friend class DatabaseIterator;

      /// enable the XML_CSV parser to fill the Database structure
      friend int  ::GUM_PARSE_XML_CSV_parse( Database& );
      friend int  ::GUM_PARSE_XML_CSV_error( Database&, const char* );
      friend bool ::GUM_PARSE_XML_CSV_parser( Database&, const std::string& );
  };


} /* namespace gum */


/* ============================================================================== */
/* ============================================================================== */
/* ===                             IMPLEMENTATIONS                            === */
/* ============================================================================== */
/* ============================================================================== */
#ifndef GUM_NO_INLINE
#include <agrum/learning/database.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_DATABASE_H */
