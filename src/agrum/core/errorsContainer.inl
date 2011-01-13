/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *   test $Id: $                                                           *
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
 * @authorPierre-Henri WUILLEMIN
 */

//to ease automatic parsers
#include <agrum/core/inline.h>
#include <agrum/core/errorsContainer.h>

namespace gum {
  INLINE
  void ErrorsContainer::Error ( const std::wstring& filename, int line, int col, const wchar_t *s ) {
    add_error ( true, filename, line, col, std::wstring ( s ) );
    error_count++;
  }

  INLINE
  void ErrorsContainer::Warning ( const std::wstring& filename, int line, int col, const wchar_t *s ) {
    add_error ( false, filename, line, col, std::wstring ( s ) );
    warning_count++;
  }

  INLINE
  void ErrorsContainer::Exception ( const std::wstring& filename, const wchar_t* s ) {
    add_error ( true, filename, 0, 0, std::wstring (L"Exception : ") + std::wstring ( s ) );
    error_count++;
  }

  INLINE
  void ErrorsContainer::add_error ( const bool is_error, const std::wstring& filename, int line , int col, const std::wstring& s ) const {
    storer.add ( is_error, filename, s, line, col );
  }

  INLINE
  int ErrorsContainer::count ( void ) const {
    return error_count + warning_count;
  }

  INLINE
  const std::wstring ErrorsContainer::filename ( int i ) const {
    return storer.filename ( i );
  }

  INLINE
  const std::wstring ErrorsContainer::msg ( int i ) const {
    return storer.msg ( i );
  }

  INLINE
  int ErrorsContainer::line ( int i ) const {
    return storer.line ( i );
  }

  INLINE
  int ErrorsContainer::col ( int i ) const {
    return storer.col ( i );
  }

  INLINE
  bool ErrorsContainer::is_error ( int i ) const {
    return storer.is_error ( i );
  }

  INLINE
  void ErrorsContainer::showSyntheticResults() const {
    std::cerr << "Errors : " << error_count << std::endl;
    std::cerr << "Warnings : " << warning_count << std::endl;
  }

} //namespace gum
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
