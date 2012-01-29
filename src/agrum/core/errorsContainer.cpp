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
 * @brief Errors container (at least) for parser
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/core/exceptions.h>
#include <agrum/core/errorsContainer.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/core/errorsContainer.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  ParseError::ParseError ( bool is_error, const string& msg, int line ) :
      is_error ( is_error ), line ( line ), colomn ( -1 ), msg ( msg ), filename ( "" ), code ( "" ) {}

  ParseError::ParseError ( bool is_error, const string& msg, const string& filename, int line, int col ) :
      is_error ( is_error ), line ( line ), colomn ( col ), msg ( msg ), filename ( filename ), code ( "" ) {}

  ParseError::ParseError ( bool is_error, const string& msg, const string& filename, const string& code, int line, int col ) :
      is_error ( is_error ), line ( line ), colomn ( col ), msg ( msg ), filename ( filename ), code ( code ) {}

  ///
  string ParseError::toString() const {
    ostringstream s;

    if ( ! filename.empty() )
      s << filename << ":" << line << ": ";
    else
      s << line << " : ";

    s << ( is_error ? "error" : "warning" ) << " : " << msg;

    return s.str();
  }

  ///
  string ParseError::toElegantString() const {
    if ( filename.empty() )
      return toString();

    if ( code.empty() ) {
      ifstream ifs ( filename.c_str() );

      for ( int i = 0 ; i < line ; i++ )
        getline ( ifs, code );
    }

    ostringstream s;

    s << filename << ":" << line << "\n";
    s << code << "\n";

    if ( colomn > 0 )
      s << string ( colomn - 1,' ' ) << "^";

    s << ( is_error ? "error" : "warning" ) << " : " << msg << "\n";

    return s.str();
  }

  /// Return the i-th error.
  /// May throw an exception if i >= count().
  ParseError ErrorsContainer::error ( int i ) const {
    if ( count() > i )
      return errors[i]; // May throw an error if i >= count().
    else {
      GUM_ERROR ( OutOfBounds, "Index out of bound." );
    }
  }

  ParseError ErrorsContainer::last() const {
    if ( count() > 0 )
      return errors[count() - 1];
    else {
      GUM_ERROR ( OutOfBounds, "Index out of bound." );
    }
  }

  /* ************************************************************************** */

  ErrorsContainer::ErrorsContainer() {
    error_count = 0;
    warning_count = 0;
  }

  ErrorsContainer::ErrorsContainer ( const ErrorsContainer & cont ) {

    error_count = cont.error_count;
    warning_count = cont.warning_count;
    errors.clear();
    errors = cont.errors;
  }

  ErrorsContainer ErrorsContainer::operator+ ( const ErrorsContainer & cont ) const {
    ErrorsContainer newCont;

    newCont.error_count = this->error_count + cont.error_count;
    newCont.warning_count = this->warning_count + cont.warning_count;
    std::copy ( this->errors.begin(), this->errors.end(), newCont.errors.begin() );
    std::copy ( cont.errors.begin(), cont.errors.end(), newCont.errors.end() );

    return newCont;
  }

  ErrorsContainer ErrorsContainer::operator= ( const ErrorsContainer & cont ) {
    error_count = cont.error_count;
    warning_count = cont.warning_count;
    errors.clear();
    errors = cont.errors;

    return *this;
  }

  ErrorsContainer ErrorsContainer::operator+= ( const ErrorsContainer & cont ) {
    error_count += cont.error_count;
    warning_count += cont.warning_count;

    for ( int i = 0 ; i < cont.count() ; i++ )
      errors.push_back ( cont.error ( i ) );

    return *this;
  }

  void ErrorsContainer::showErrors() const {
    if ( count() == 0 )
      return;

    for ( int i = 0 ; i < count() ; i++ ) {
      if ( error ( i ).is_error )
        cerr << error ( i ).toString() << endl;
    }
  }


  void ErrorsContainer::showErrorsAndWarnings() const {
    if ( count() == 0 )
      return;

    for ( int i = 0 ; i < count() ; i++ )
      cerr << error ( i ).toString() << endl;
  }


  void ErrorsContainer::showElegantErrors() const {
    if ( count() == 0 )
      return;

    for ( int i = 0 ; i < count() ; i++ ) {
      if ( error ( i ).is_error ) {
        cerr << error ( i ).toElegantString();
        cerr << endl;
      }
    }
  }

  void ErrorsContainer::showElegantErrorsAndWarnings() const {
    if ( count() == 0 )
      return;

    for ( int i = 0 ; i < count() ; i++ ) {
      cerr << error ( i ).toElegantString();
      cerr << endl;
    }
  }

}//namespace gum

#endif //DOXYGEN_SHOULD_SKIP_THIS

