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
 * @brief The class representing cells in tabular database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_CELL_H
#define GUM_LEARNING_DB_CELL_H

#include <string>

#include <agrum/config.h>


namespace gum {

  
  namespace learning {


    /** @class DBCell
     * @ingroup learning_group
     * @brief the type of the elements read from the database */
    class DBCell {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBCell ();

      /// copy constructor
      DBCell ( const DBCell& from );

      /// destructor
      ~DBCell ();

      /// @}

      
      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBCell& operator= ( const DBCell& from );

      /// unsafe set operator (assumes that the preceding type is of the same type)
      DBCell& operator= ( unsigned int x ) noexcept;
      
      /// unsafe set operator (assumes that the preceding type is of the same type)
      DBCell& operator= ( int x ) noexcept;

      /// unsafe set operator (assumes that the preceding type is of the same type)
      DBCell& operator= ( float x ) noexcept;

      /// unsafe set operator (assumes that the preceding type is of the same type)
      DBCell& operator= ( double x ) noexcept;

      /// unsafe set operator (assumes that the preceding type is of the same type)
      DBCell& operator= ( char x ) noexcept;

      /// unsafe set operator (assumes that the preceding type is of the same type)
      DBCell& operator= ( const std::string& x ) noexcept;
      
      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the DBcell as an unsigned int (without checking its type)
      /** @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      unsigned int getUint () const noexcept;

      /// returns the DBcell as an int (without checking its type)
      /** @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      int getInt () const noexcept;
      
      /// returns the DBcell as a float (without checking its type)
      /** @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      float getFloat () const noexcept;

      /// returns the DBcell as a double (without checking its type)
      double getDouble () const noexcept;

      /// returns the DBcell as a char (without checking its type)
      /** @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      char getChar () const noexcept;
      
      /// returns the DBcell as a string (without checking its type)
      /** @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      const std::string& getString () const noexcept;
      
      /// returns the DBcell as an unsigned int (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      unsigned int getUintSafe () const;

      /// returns the DBcell as an int (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      int getIntSafe () const;
      
      /// returns the DBcell as a float (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      float getFloatSafe () const;

      /// returns the DBcell as a double (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      double getDoubleSafe () const;

      /// returns the DBcell as a char (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      char getCharSafe () const;
      
      /// returns the DBcell as a string (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      const std::string& getStringSafe () const;

      /// sets the content of the DBCell (safe type checking)
      void setUintSafe ( unsigned int elt );

      /// sets the content of the DBCell (safe type checking)
      void setIntSafe ( int elt );

      /// sets the content of the DBCell (safe type checking)
      void setFloatSafe ( float elt );

      /// sets the content of the DBCell (safe type checking)
      void setDoubleSafe ( double elt );

      /// sets the content of the DBCell (safe type checking)
      void setCharSafe ( char elt );

      /// sets the content of the DBCell (safe type checking)
      void setStringSafe ( const std::string& elt );
      
      /// safely sets the content of the DBCell with the best possible type
      /** This method is used to parse the string passed in argument and
       * estimate which internal type is the most appropriate for the DBCell.
       * Then it fills the DBCell with this type. */
      void setBestTypeSafe ( const std::string& elt );

      /** @brief safely sets the content of the DBCell with the preceding type or,
       * if this is not possible, with the best possible type */
      void setAgainTypeSafe ( const std::string& elt );
      
      /// unsafe set (assumes that the preceding type is of the same type)
      void setUint ( unsigned int x );
      
      /// unsafe set (assumes that the preceding type is of the same type)
      void setInt ( int x );

      /// unsafe set (assumes that the preceding type is of the same type)
      void setFloat ( float x );

      /// unsafe set (assumes that the preceding type is of the same type)
      void setDouble ( double x );

      /// unsafe set (assumes that the preceding type is of the same type)
      void setChar ( char x );

      /// unsafe set (assumes that the preceding type is of the same type)
      void setString ( const std::string& x );

      /// @}

      
    private:
      /// the set of types possibly taken by the last element read
      enum class EltType {
        UINT, INT, FLOAT, DOUBLE, CHAR, STRING
      };

      /// the real type of the last element read from the database
      EltType __type { EltType::UINT };

      /// the element read from the database
      union {
        unsigned int __unsigned_int { 0 };
        int          __int;
        float        __float;
        double       __double;
        char         __char;
        std::string  __string;
      };


      /// sets the content of the DBCell from a string
      /** @throws std::invalid_argument if the string cannot be converted */
      void __setUIntFromStringSafe ( const std::string& str );

      /// sets the content of the DBCell from a string
      /** @throws std::invalid_argument if the string cannot be converted */
      void __setIntFromStringSafe ( const std::string& str );

      /// sets the content of the DBCell from a string
      /** @throws std::invalid_argument if the string cannot be converted */
      void __setFloatFromStringSafe ( const std::string& str );

      /// sets the content of the DBCell from a string
      /** @throws std::invalid_argument if the string cannot be converted */
      void __setDoubleFromStringSafe ( const std::string& str );

      /// sets the content of the DBCell from a string
      /** @throws std::invalid_argument if the string cannot be converted */
      void __setCharFromStringSafe ( const std::string& str );

    };
    

  } /* namespace learning */

  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/DBCell.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_DB_CELL_H */
