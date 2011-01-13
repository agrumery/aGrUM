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
#ifndef GUM_ERRORS_CONTAINERS_H
#define GUM_ERRORS_CONTAINERS_H

#include <iostream>
#include <fstream>
#include <vector>

#include <agrum/core/cocoR/common.h>

namespace gum {

  class ErrorsContainer {

      class Storing {

        private:
          std::vector<bool> is_errors;
          std::vector<std::wstring> filenames;
          std::vector<std::wstring> msgs;
          std::vector<unsigned int> cols;
          std::vector<unsigned int> lines;

        public:
          Storing() {};

          void add ( const bool is_error, const std::wstring& filename, const std::wstring& msg, int line, int col ) {
            is_errors.push_back ( is_error );
            filenames.push_back ( filename );
            msgs.push_back ( msg );
            lines.push_back ( line );
            cols.push_back ( col );
          }

          const std::wstring filename ( unsigned int i ) const {
            return ( i < filenames.size() ) ? filenames[i] : L"No filename";
          };

          const std::wstring msg ( unsigned int i ) const {
            return ( i < msgs.size() ) ? msgs[i] : L"No error";
          };

          unsigned int line ( unsigned int i ) const {
            return ( i < lines.size() ) ? lines[i] : -1;
          }

          unsigned int col ( unsigned int i ) const {
            return ( i < cols.size() ) ? cols[i] : -1;
          }

          bool is_error ( unsigned int i ) const {
            return ( i < is_errors.size() ) ? is_errors[i] : -1;
          }
      };

    private:
      mutable Storing storer;

    public:
      int error_count;      // number of errors detected
      int warning_count;


      ErrorsContainer();
      void Error ( const std::wstring& filename, int line, int col, const wchar_t *s );
      void Warning ( const std::wstring& filename, int line, int col, const wchar_t *s );
      void Exception ( const std::wstring& filename, const wchar_t *s );
      void add_error ( const bool is_error, const std::wstring& filename, int lig, int col, const std::wstring& s ) const;

      int count ( void ) const;
      const std::wstring filename ( int i ) const;
      const std::wstring msg ( int i ) const;
      int line ( int i ) const;
      int col ( int i ) const;
      bool is_error ( int i ) const;
      
      void showSyntheticResults() const;
      void showElegantErrors() const;
      void showElegantErrorsAndWarnings() const;
      void showErrorsAndWarnings() const;
  }; // Errors

}//namespace gum

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/core/errorsContainer.inl>
#endif /* GUM_NO_INLINE */

#endif //GUM_ERRORS_CONTAINERS_H
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
