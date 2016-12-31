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
 * @brief The class representing cells in tabular databases
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_CELL_H
#define GUM_LEARNING_DB_CELL_H

#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <agrum/config.h>
#include <agrum/core/bijection.h>

namespace gum {

  namespace learning {

    /** @class DBCell
     * @ingroup learning_group
     * @brief the type of the elements read from the database */
    class DBCell {
      public:
      /// the set of types possibly taken by the last element read
      enum EltType { REAL, STRING, MISSING };

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBCell();

      /// constructor for a number
      DBCell( double nb );

      /// constructor for a string
      DBCell( const std::string& str );

      /// copy constructor
      DBCell( const DBCell& from );

      /// move constructor
      DBCell( DBCell&& from );

      /// destructor
      ~DBCell();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBCell& operator=( const DBCell& from );

      /// move operator
      DBCell& operator=( DBCell&& from );

      /// unsafe set operator (assumes that the preceding type is of the same
      /// type)
      DBCell& operator=( double x ) noexcept;

      /// unsafe set operator (assumes that the preceding type is of the same
      /// type)
      DBCell& operator=( const std::string& x ) noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the DBcell as a double (without checking its type)
      /** @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      double getReal() const noexcept;

      /// returns the DBcell as a double (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      double getRealSafe() const;

      /// unsafe set (assumes that the preceding type is of the same type)
      void setReal( double x );

      /// sets the content of the DBCell (safe type checking)
      void setRealSafe( double elt );

      /// returns the DBcell as a string (without checking its type)
      /** @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      const std::string& getString() const noexcept;

      /// returns the DBcell as a string (safe with type checking)
      /** @throw TypeError if the DBCell does not contain this type */
      const std::string& getStringSafe() const;

      /** @brief returns the DBcell as the index of a string in a static
       *bijection
       * (without checking its type)
       *
       * @warning this method is unsafe: it assumes that you know the
       * correct type of the element in the DBCell */
      int getStringIndex() const noexcept;

      /// returns the DBcell as the index of a string in a static bijection
      int getStringIndexSafe() const;

      /// strings are stored into a static bijection. Get its ith string
      static const std::string& getString( Idx index );

      /// unsafe set (assumes that the preceding type is of the same type)
      void setString( const std::string& x );

      /// sets the content of the DBCell (safe type checking)
      void setStringSafe( const std::string& elt );

      /// sets the DBCell as a missing element
      void setMissingSafe();

      /// safely sets the content of the DBCell with the best possible type
      /** This method is used to parse the string passed in argument and
       * estimate which internal type is the most appropriate for the DBCell.
       * Then it fills the DBCell with this type. */
      void setBestTypeSafe( const std::string& elt );

      /** @brief safely sets the content of the DBCell with the preceding type
       * or,
       * if this is not possible, with the best possible type */
      void setAgainTypeSafe( const std::string& elt );

      /// returns the current type of the DBCell
      EltType type() const noexcept;

      /// try to convert the content of the DBCell into another type
      bool convertType( EltType );

      /// @}

      private:
      using Real = double;
      // typename std::conditional<sizeof( double ) >= sizeof( int
      // ),double,double>::type;

      /// the real type of the last element read from the database
      EltType __type{EltType::REAL};

      /// the element read from the database
      union {
        Idx  __index;  // stores string indices
        Real __value{0.0};
      };

      /// a bijection assigning to each string index its corresponding string
      static Bijection<std::string, int>& __strings();

      /// the last index used so far
      static int __string_max_index;

      /// sets the content of the DBCell from a string
      /** @throws std::invalid_argument if the string cannot be converted */
      void __setRealFromStringSafe( const std::string& str );
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/DBCell_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_CELL_H */
