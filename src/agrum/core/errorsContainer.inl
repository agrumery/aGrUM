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
 * @brief Inlined implementation of the basic methods of ErrorsContainer
 *
 * @author Pierre-Henri WUILLEMIN
 */

// to ease automatic parsers
#include <agrum/core/errorsContainer.h>

namespace gum {

  INLINE
  void ErrorsContainer::Error( const std::wstring& filename,
                               Idx                 line,
                               Idx                 col,
                               const wchar_t*      msg ) {
    add( ParseError(
        true, narrow( std::wstring( msg ) ), narrow( filename ), line, col ) );
  }

  INLINE
  void ErrorsContainer::Warning( const std::wstring& filename,
                                 Idx                 line,
                                 Idx                 col,
                                 const wchar_t*      msg ) {
    add( ParseError(
        false, narrow( std::wstring( msg ) ), narrow( filename ), line, col ) );
  }

  INLINE
  void ErrorsContainer::Exception( const std::wstring& filename,
                                   const wchar_t*      msg ) {
    add( ParseError( true,
                     "Exception : " + narrow( std::wstring( msg ) ),
                     narrow( filename ),
                     0,
                     0 ) );
  }

  INLINE
  void ErrorsContainer::add( ParseError error ) {
    errors.push_back( error );

    if ( error.is_error )
      error_count++;
    else
      warning_count++;
  }

  INLINE
  void ErrorsContainer::addError( const std::string& msg,
                                  const std::string& filename,
                                  Idx                line,
                                  Idx                col ) {
    add( ParseError( true, msg, filename, line, col ) );
  }

  INLINE
  void ErrorsContainer::addWarning( const std::string& msg,
                                    const std::string& filename,
                                    Idx                line,
                                    Idx                col ) {
    add( ParseError( false, msg, filename, line, col ) );
  }

  INLINE
  void ErrorsContainer::addException( const std::string& msg,
                                      const std::string& filename ) {
    add( ParseError( true, msg, filename, 0, 0 ) );
  }

  INLINE
  Size ErrorsContainer::count( void ) const { return error_count + warning_count; }

  INLINE
  void ErrorsContainer::syntheticResults( std::ostream& o ) const {
    o << "Errors : " << error_count << std::endl;
    o << "Warnings : " << warning_count << std::endl;
  }

}  // namespace gum
