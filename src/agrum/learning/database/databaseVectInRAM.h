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
 * @brief The class representing a tabular database as a vector of DBRows stored
 * in RAM
 *
 * Class DatabaseVector is a helper for tabular databases that store in RAM all
 * their content into a vector a DBRows. It is not intended to be used as is but
 * rather through inheritance of other classes such as DatabaseFromCSV.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DATABASE_VECT_IN_RAM_H
#define GUM_DATABASE_VECT_IN_RAM_H


#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/DBHandler.h>


namespace gum {

  
  namespace learning {

    
    /** @class DatabaseVectInRAM
     * @brief The base class representing a tabular database as a vector of DBRows
     * @ingroup learning_group
     *
     * Class DatabaseVectInRAM is a helper for tabular databases that store in RAM
     * all their content into a vector a DBRows. It is not intended to be used as
     * is but rather through inheritance of other classes such as DatabaseFromCSV.
     */
    class DatabaseVectInRAM {
    public:
      
      /** @class DatabaseVectInRAM::Handler
       * @brief the handler of tabular databases represented  as vectors of DBRows
       * @ingroup learning_group
       */
      class Handler : public DBHandler {
      public:
        // ########################################################################
        /// @name Constructors / Destructors
        // ########################################################################
        /// @{

        /// default constructor
        Handler ( const DatabaseVectInRAM& db );

        /// copy constructor
        Handler ( const Handler& h );

        /// move constructor
        Handler ( Handler&& h );

        /// destructor
        virtual ~Handler ();

        /// @}

        
        // ########################################################################
        /// @name Operators
        // ########################################################################
        /// @{

        /// copy operator
        Handler& operator= ( const Handler& );

        /// move operator
        Handler& operator= ( Handler&& );
        
        /// @}


        // ########################################################################
        /// @name Accessors / Modifiers
        // ########################################################################
        /// @{
       
        /// returns the number of rows managed by the handler
        /** A handler needs not necessarily handle all the rows of the database.
         * For instance, RecordCounters cut the database into several pieces and
         * assign each piece to a handler. Then each handler is used to perform
         * countings only on a subset of the database */
        unsigned long size () const noexcept;

        /// return the number of rows of the whole database
        unsigned long DBSize () const noexcept;

        /// returns the current row pointed to by the handler
        /** @throws OutOfBounds if the handler points to the end of its area */
        const DBRow& row () const;
        
        /// makes the handler point to the next row
        void nextRow () noexcept;

        /// indicates whether the handler has reached its end or not
        bool hasRows () const noexcept;

        /// puts the handler to the beginning of the database area it handles
        void reset () noexcept;

        /// sets the area in the database the handler will handle
        void setRange ( unsigned long begin, unsigned long end ) noexcept;

        /// returns the current range of the handler
        std::pair<unsigned long, unsigned long> range () const noexcept;

        /// returns the names of the variables
        const std::vector<std::string>& variableNames () const noexcept;
        
        /// @}

      private:
        /// a reference on the whole database, including variable names
        const DatabaseVectInRAM* __db;
        
        /// a reference on the database
        const std::vector<DBRow>* __row;

        /// the index of the row currently pointed to by the handler
        unsigned long __index       { 0 };

        /// the first row managed by the handler
        unsigned long __begin_index { 0 };

        /// the row just after the last one managed by the handler
        unsigned long __end_index   { 0 };
        
      };



      
      // ##########################################################################
      /// @name Destructors
      // ##########################################################################
      /// @{

      /// destructor
      ~DatabaseVectInRAM ();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the content of the database
      const std::vector<DBRow>& content () const noexcept;

      /// returns a new handler on the database
      Handler handler () const;

      /// returns the variable names for all the columns
      const std::vector<std::string>& variableNames () const noexcept;
      
      /// @}
      
      
    protected:
      // ##########################################################################
      /// @name Constructors
      // ##########################################################################
      /// @{

      /// default constructor
      DatabaseVectInRAM ();

      /// copy constructor
      DatabaseVectInRAM ( const DatabaseVectInRAM& );

      /// move constructor
      DatabaseVectInRAM ( DatabaseVectInRAM&& );

      /// @}

      
      /// returns the content of the database
      std::vector<DBRow>& _content () noexcept;

      /// returns the variable names for all the columns
      std::vector<std::string>& _variableNames () noexcept;

      
    private:
      /// the vector of DBRows
      std::vector<DBRow> __data;

      /// the names of the variables for each column
      std::vector<std::string> __variable_names;
      
    };


  } /* namespace learning */


} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/databaseVectInRAM.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_DATABASE_VECT_IN_RAM_H */

