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
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {


    /// default constructor
    INLINE DBCell::DBCell () {
      GUM_CONSTRUCTOR ( DBCell );
    }

    
    /// destructor
    INLINE DBCell::~DBCell () {
      GUM_DESTRUCTOR ( DBCell );
      if ( __type == EltType::STRING )
        __string.std::string::~string ();
    }

    
    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell& DBCell::operator= ( unsigned int x ) noexcept {
      __unsigned_int = x;
      return *this;
    }
    

    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell& DBCell::operator= ( int x ) noexcept {
      __int = x;
      return *this;
    }


    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell& DBCell::operator= ( float x ) noexcept {
      __float = x;
      return *this;
    }


    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell& DBCell::operator= ( double x ) noexcept {
      __double = x;
      return *this;
    }


    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell& DBCell::operator= ( char x ) noexcept {
      __char = x;
      return *this;
    }


    /// unsafe set operator (assumes that the preceding type is of the same type)
    INLINE DBCell& DBCell::operator= ( const std::string& x ) noexcept {
      __string = x;
      return *this;
    }
      

    /// returns the DBcell as an unsigned int (without checking its type)
    INLINE  unsigned int DBCell::getUint () const noexcept {
      return __unsigned_int;
    }


    /// returns the DBcell as an int (without checking its type)
    INLINE int DBCell::getInt () const noexcept {
      return __int;
    }
      

    /// returns the DBcell as a float (without checking its type)
    INLINE float DBCell::getFloat () const noexcept {
      return __float;
    }


    /// returns the DBcell as a double (without checking its type)
    INLINE double DBCell::getDouble () const noexcept {
      return __double;
    }

    
    /// returns the DBcell as a char (without checking its type)
    INLINE char DBCell::getChar () const noexcept {
      return __char;
    }
      

    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string& DBCell::getString () const noexcept {
      return __string;
    }
      

    /// returns the DBcell as an unsigned int (safe with type checking)
    INLINE unsigned int DBCell::getUintSafe () const {
      if ( __type == EltType::UINT ) return __unsigned_int;
      else GUM_ERROR ( TypeError, "the DBCell does not contain an unsigned int" );
    }

    
    /// returns the DBcell as an int (safe with type checking)
    INLINE int DBCell::getIntSafe () const {
      if ( __type == EltType::INT ) return __int;
      else GUM_ERROR ( TypeError, "the DBCell does not contain an int" );
    }

    
    /// returns the DBcell as a float (safe with type checking)
    INLINE float DBCell::getFloatSafe () const {
      if ( __type == EltType::FLOAT ) return __float;
      else GUM_ERROR ( TypeError, "the DBCell does not contain a float" );
    }


    /// returns the DBcell as a double (safe with type checking)
    INLINE double DBCell::getDoubleSafe () const {
      if ( __type == EltType::DOUBLE ) return __double;
      else GUM_ERROR ( TypeError, "the DBCell does not contain a double" );
    }

    
    /// returns the DBcell as a char (safe with type checking)
    INLINE char DBCell::getCharSafe () const {
      if ( __type == EltType::CHAR ) return __char;
      else GUM_ERROR ( TypeError, "the DBCell does not contain a char" );
    }
      

    /// returns the DBcell as a string (safe with type checking)
    INLINE const std::string& DBCell::getStringSafe () const {
      if ( __type == EltType::STRING ) return __string;
      else GUM_ERROR ( TypeError, "the DBCell does not contain a string" );
    }


    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setUintSafe ( unsigned int elt ) {
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __type = EltType::UINT;
      __unsigned_int = elt;
    }

    
    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setIntSafe ( int elt ) {
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __type = EltType::INT;
      __int = elt;
    }


    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setFloatSafe ( float elt ) {
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __type = EltType::FLOAT;
      __float = elt;
    }

    
    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setDoubleSafe ( double elt ) {
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __type = EltType::DOUBLE;
      __double = elt;
    }


    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setCharSafe ( char elt ) {
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __type = EltType::CHAR;
      __char = elt;
    }


    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setStringSafe ( const std::string& elt ) {
      if ( __type == EltType::STRING )
        __string = elt;
      else {
        __type = EltType::STRING;
        new (&__string) std::string { elt };
      }
    }

      
    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setUint ( unsigned int x ) {
      __unsigned_int = x;
    }
      

    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setInt ( int x ) {
      __int = x;
    }


    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setFloat ( float x ) {
      __float = x;
    }


    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setDouble ( double x ) {
      __double = x;
    }


    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setChar ( char x ) {
      __char = x;
    }


    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setString ( const std::string& x ) {
      __string = x;
    }

    
    /// sets the content of the DBCell from a string
    INLINE void DBCell::__setUIntFromStringSafe ( const std::string& elt ) {
      unsigned int new_elt = stoul ( elt );
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __unsigned_int = new_elt;
      __type = EltType::UINT;
    }

    
    /// sets the content of the DBCell from a string
    INLINE void DBCell::__setIntFromStringSafe ( const std::string& elt ) {
      int new_elt = stoi ( elt );
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __int = new_elt;
      __type = EltType::INT;
    }
    

    /// sets the content of the DBCell from a string
    INLINE void DBCell::__setFloatFromStringSafe ( const std::string& elt ) {
      float new_elt = stof ( elt );
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __float = new_elt;
      __type = EltType::FLOAT;
    }

    
    /// sets the content of the DBCell from a string
    INLINE void DBCell::__setDoubleFromStringSafe ( const std::string& elt ) {
      double new_elt = stod ( elt );
      if ( __type == EltType::STRING ) __string.std::string::~string ();
      __double = new_elt;
      __type = EltType::DOUBLE;
    }

    
    /// sets the content of the DBCell from a string
    INLINE void DBCell::__setCharFromStringSafe ( const std::string& elt ) {
      if ( elt.size () == 1 ) {
        if ( __type == EltType::STRING ) __string.std::string::~string ();
        __char = elt[0];
        __type = EltType::CHAR;
      }
      else {
        throw std::invalid_argument ("char -> DBCell conversion impossible");
      }
    }
    

  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
