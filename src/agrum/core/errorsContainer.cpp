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

#include <agrum/core/errorsContainer.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/core/errorsContainer.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  ErrorsContainer::ErrorsContainer() {
    error_count = 0;
    warning_count = 0;
  }

  void ErrorsContainer::showElegantErrors() const {
    if ( count() == 0 ) return;

    int nb_err = 0;

    int no_line = 1;

    int num_msg = 0;

    std::ifstream ifs ( narrow ( filename ( num_msg ) ).c_str() );

    std::string temp;

    while ( getline ( ifs, temp ) ) {
      if ( nb_err > error_count ) break;

      while ( no_line == line ( num_msg ) ) {
        if ( is_error ( num_msg ) ) {
          std::cerr << narrow ( filename ( num_msg ) ) << ":" << line ( num_msg ) << std::endl;
          std::cerr << temp << std::endl;
          std::cerr << std::string ( col ( num_msg ) - 1, ' ' ) << "^" << " " << narrow ( msg ( num_msg ) ) << std::endl << std::endl;
          nb_err++;
        }

        num_msg++;
      }

      no_line++;
    }
  }

  void ErrorsContainer::showElegantErrorsAndWarnings() const {
    if ( count() == 0 ) return;

    int nb_err = 0;

    int no_line = 1;

    int num_msg = 0;

    std::ifstream ifs ( narrow ( filename ( num_msg ) ).c_str() );

    std::string temp;

    while ( getline ( ifs, temp ) ) {
      if ( nb_err > error_count + warning_count ) break;

      while ( no_line == line ( num_msg ) ) {
        std::cerr << narrow ( filename ( num_msg ) ) << ":" << line ( num_msg ) << std::endl;
        std::cerr << temp << std::endl;
        std::cerr << std::string ( col ( num_msg ) - 1, ' ' ) << "^" << " " << narrow ( msg ( num_msg ) ) << std::endl << std::endl;
        nb_err++;
        num_msg++;
      }

      no_line++;
    }
  }

  void ErrorsContainer::showErrorsAndWarnings() const {
    if ( count() == 0 ) return;

    int nb_err = 0;

    int no_line = 1;

    int num_msg = 0;

    while ( true ) {
      if ( nb_err > error_count + warning_count ) break;

      while ( no_line == line ( num_msg ) ) {
        std::cerr << narrow ( filename ( num_msg ) ) << ":" << line ( num_msg ) << ":";
        std::cerr << ( ( is_error ( num_msg ) ) ? "error" : "warning" ) << ":" << narrow ( msg ( num_msg ) ) << std::endl;
        nb_err++;
        num_msg++;
      }

      no_line++;
    }
  }
}//namespace gum

#endif //DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
