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
 * @brief The class representing a tabular database as a vector of DBRows
 *
 * Class DatabaseVector is a helper for tabular databases that store all their
 * content into a vector a DBRows. It is not intended to be used as is but
 * rather through inheritance of other classes such as DatabaseCSV.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DATABASE_VECTOR_H
#define GUM_DATABASE_VECTOR_H


#include <vector>

#include <agrum/config.h>
#include <agrum/learning/DBRow.h>

#define CACHE_SIZE 1024


namespace gum {

  
  namespace learning {

    
    /** @class DatabaseVector
     * @brief The base class representing a tabular database as a vector of DBRows
     * @ingroup learning_group
     *
     * Class DatabaseVector is a helper for tabular databases that store all their
     * content into a vector a DBRows. It is not intended to be used as is but
     * rather through inheritance of other classes such as DatabaseCSV.
     */
    class DatabaseVector {
    public:
      
      /** @class DatabaseVector::Handler
       * @brief the handler of tabular databases represented  as vectors of DBRows
       * @ingroup learning_group
       */
      class Handler {
      public:
        // ########################################################################
        /// @name Constructors
        // ########################################################################
        /// @{

        /// default constructor
        Handler ( const DatabaseVector& db );

        /// copy constructor
        Handler ( const Handler& h );

        /// destructor
        virtual ~Handler ();

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
        
        /// @}

      private:
        /// a reference on the database
        const std::vector<DBRow>& __row;

        /// the index of the row currently pointed to by the handler
        unsigned long __index       { 0 };

        /// the first row managed by the handler
        unsigned long __begin_index { 0 };

        /// the row just after the last one managed by the handler
        unsigned long __end_index   { 0 };

        /// a buffer to avoid cacheline problems due to parallelism
        char __align[CACHE_SIZE];
        
      } ;



      
      // ##########################################################################
      /// @name Destructors
      // ##########################################################################
      /// @{

      /// destructor
      ~DatabaseVector ();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the content of the database
      const std::vector<DBRow>& content () const noexcept;

      /// returns a new handler on the database
      Handler handler () const;
      
      /// @}
      
      
    protected:
      // ##########################################################################
      /// @name Constructors
      // ##########################################################################
      /// @{

      /// default constructor
      DatabaseVector ();

      /// copy constructor
      DatabaseVector ( const DatabaseVector& );

      /// move constructor
      DatabaseVector ( DatabaseVector&& );

      /// @}

      
      /// returns the content of the database
      std::vector<DBRow>& _content () noexcept;

      
    private:
      /// the vector of DBRows
      std::vector<DBRow> __data;
      
    };


  } /* namespace learning */


} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/databaseVector.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_DATABASE_VECTOR_H */

